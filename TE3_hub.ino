//-------------------------------------------------------
// TE3_hub.ino
//-------------------------------------------------------
// USB Serial & Audio device
// Runs on a teensy4.0 with a RevD audio board above it.
// Communicates with TE3 via MIDI Serial data.
// The audio device is a teensyQuad (AudioIn/OutI2SQuad).

#include <Audio.h>
#include <Wire.h>
#include <myDebug.h>
#include <teMIDI.h>
#include <teCommon.h>
#include <sgtl5000midi.h>
#include "src/sgtl5000.h"


#define	dbg_audio	0
#define dbg_sine	1


#define USB_SERIAL_PORT			Serial
#define MIDI_SERIAL_PORT		Serial1

void tehub_dumpCCValues(const char *where);
	// forward


//---------------------------------------
// compile options
//---------------------------------------

#define DEBUG_TO_USB_SERIAL		0
#define DEBUG_TO_MIDI_SERIAL	1
#define HOW_DEBUG_OUTPUT		DEBUG_TO_MIDI_SERIAL
	// The default is that debugging goes to the MIDI Serial port
	// (TE3) and is forwarded to the laptop from there.  If I need
	// to hook up directly to the TE3_hub, it means it's connected
	// to the laptop for unusual debugging.

#define PIN_HUB_ALIVE	13
	// Set this to a pin to flash a heartbeat during loop()
	// The teensy4.x onboard LED is pin 13

#define WITH_MIXERS		1
	// The production setting is currently WITH_MIXERS=1
	// Set this to zero for a minimal USB pass through device
	// 		without involving the rPi andw ith no volume controls,
	//		where i2s_in -> usb_out && usb_in --> i2s_out
	// If 1, a pair of mixers are added for the final i2s_out
	//		and the device is responsive to midi MIX_XXX commands.
	//		including the LOOP volume

#define WITH_SINE	0
	// if defined, will include an input mixer to the USB out
	//		allowing me to send a defined sine wave pattern to it.
	// and if WITH_MIXERS it will also be sent to the MIX_AUX channel.


#define DEFAULT_VOLUME_IN		0		// listen to the raw LINE_IN signal
#define DEFAULT_VOLUME_USB		100		// listen to the returned USB
#define DEFAULT_VOLUME_LOOP		0		// currently all mixing is done on the Looper ... more later
#define DEFAULT_VOLUME_AUX		0		// unused

#define DEFAULT_VOLUME_IN_USB	100		// if WITH_SINE amount of i2s_in --> usb_out
#define DEFAULT_VOLUME_IN_SINE	0		// if WITH_SINE amount of sine -> usb_out


//-------------------------------------------
// audio system setup
//-------------------------------------------

#if WITH_SINE
	#define NUM_MIXER_CHANNELS	6
#else
	#define NUM_MIXER_CHANNELS	4
#endif


#define MIX_CHANNEL_IN			0		// the original i2s_in that is sent to the iPad (USB out)
#define MIX_CHANNEL_USB			1		// the sound returned from the iPad and sent to rpi Looper
#define MIX_CHANNEL_LOOP  		2		// the sound returned from the rPi Looper
#define MIX_CHANNEL_AUX			3		// if WITH_SINE, monitor the sine directly

#define MIX_CHANNEL_IN_USB  	4		// amount of i2s_in->usb_out;  channel 0 from perspective of in_mixers
#define MIX_CHANNEL_IN_SINE		5		// amount of sine->usb_out;    channel 1 from perspective of in_mixers

// audio vars


uint8_t mix_level[NUM_MIXER_CHANNELS];

SGTL5000 sgtl5000;

AudioInputI2SQuad       i2s_in;
AudioOutputI2SQuad      i2s_out;
AudioInputUSB   		usb_in;
AudioOutputUSB  		usb_out;
#if WITH_MIXERS
	AudioMixer4			mixer_L;
	AudioMixer4			mixer_R;
#endif
#if WITH_SINE
	AudioMixer4			in_mix_L;
	AudioMixer4			in_mix_R;
	AudioSynthWaveformSine  sine;
#endif


#if WITH_MIXERS

	AudioConnection	c_i1(i2s_in,  0, mixer_L, MIX_CHANNEL_IN);			// SGTL5000 LINE_IN --> out_mixer(0)
	AudioConnection	c_i2(i2s_in,  1, mixer_R, MIX_CHANNEL_IN);

	#if !WITH_SINE
		AudioConnection c_in1(i2s_in, 0, usb_out, 0);					// STGTL5000 LINE_IN --> USB_out
		AudioConnection c_in2(i2s_in, 1, usb_out, 1);
	#else
		AudioConnection c_in1(i2s_in, 0, in_mix_L, 0);					// STGTL5000 LINE_IN --> in_mixer(0)
		AudioConnection c_in2(i2s_in, 1, in_mix_R, 0);

		AudioConnection c_usb1(in_mix_L, 0, usb_out, 0);				// in_mixers --> usb_out
		AudioConnection c_usb2(in_mix_R, 0, usb_out, 1);

		AudioConnection c_sine1(sine, 0, mixer_L, MIX_CHANNEL_AUX);		// sine --> out_mixer(3)
		AudioConnection c_sine2(sine, 0, mixer_R, MIX_CHANNEL_AUX);
		AudioConnection c_sine3(sine, 0, in_mix_L, 1);					// sine --> in_mixer(1)
		AudioConnection c_sine4(sine, 0, in_mix_R, 1);
	#endif


	AudioConnection	c_ul(usb_in,  0, mixer_L, MIX_CHANNEL_USB);			// USB_in --> out_mixer(1)
	AudioConnection	c_ur(usb_in,  1, mixer_R, MIX_CHANNEL_USB);
	AudioConnection c_q1(usb_in,  0, i2s_out, 2);						// USB_in --> Looper
	AudioConnection c_q2(usb_in,  1, i2s_out, 3);
	AudioConnection c_q3(i2s_in,  2, mixer_L, MIX_CHANNEL_LOOP);		// Looper --> out_mixer(2)
	AudioConnection c_q4(i2s_in,  3, mixer_R, MIX_CHANNEL_LOOP);
	AudioConnection c_o1(mixer_L, 0, i2s_out, 0);						// out_mixers --> SGTL5000
	AudioConnection c_o2(mixer_R, 0, i2s_out, 1);

#else	// no mixers; no looper; stripped version

	#if !WITH_SINE

		AudioConnection	c_i1(i2s_in, 0, usb_out, 0);					// SGTL5000 LINE_IN --> usb_out
		AudioConnection	c_i2(i2s_in, 1, usb_out, 1);

	#else

		AudioConnection	c_i1(i2s_in, 	0, in_mix_L, 0);				// SGTL5000 LINE_IN --> in_mixer(0)
		AudioConnection	c_i2(i2s_in, 	1, in_mix_R, 0);
		AudioConnection c_s1(sine, 	 	0, in_mix_L, 1);				// sine --> in_mixer(1)
		AudioConnection c_s2(sine, 	 	0, in_mix_R, 1);
		AudioConnection	c_u1(in_mix_L,	0, usb_out, 0);					// in_mixers --> usb_out
		AudioConnection	c_u2(in_mix_R,  0, usb_out, 1);

	#endif

	AudioConnection c_o1(usb_in, 0, i2s_out, 0);						// usb_in --> SGTL5000
	AudioConnection c_o2(usb_in, 1, i2s_out, 1);

#endif



bool setMixLevel(uint8_t channel, uint8_t val)
{
	display(dbg_audio,"setMixLevel(%d,%d)",channel,val);
	if (val > 127) val = 127;
	float vol = val;
	vol = vol/100;

	#if WITH_SINE
		if (channel >= MIX_CHANNEL_IN_USB && channel <= MIX_CHANNEL_IN_SINE)
		{
			uint8_t in_channel = channel - MIX_CHANNEL_IN_USB;
			in_mix_L.gain(in_channel,vol);
			in_mix_R.gain(in_channel,vol);
			mix_level[channel] = val;
			return true;
		}
	#endif

	#if WITH_MIXERS
		if (channel >= MIX_CHANNEL_IN && channel <= MIX_CHANNEL_AUX)
		{
			mixer_L.gain(channel, vol);
			mixer_R.gain(channel, vol);
			mix_level[channel] = val;
			return true;
		}
	#endif

	my_error("unimplmented mix_channel(%d)",channel);
	return false;
}



//----------------------------------------------
// sine stuff
//-----------------------------------------------

#if WITH_SINE

	#define SINE_VOL		0.20	// 0..127
	#define SINE_FREQ		440		// 69 = A440

	#define SINE_ATTACK		1		// seconds
	#define SINE_DUR		1		// seconds
	#define SINE_DECAY		24		// seconds
	#define SINE_OFF		4		// seconds

	// state machine that moves through attack, dur, decay, and off stages

	#define SINE_PHASE_OFF		0
	#define SINE_PHASE_ATTACK	1
	#define SINE_PHASE_DUR		2
	#define SINE_PHASE_DECAY	3

	uint8_t sine_phase;
	volatile uint32_t sine_phase_time;

	void initSine()
	{
		display(0,"initSine vol(%0.3f) freq(%d) attack(%d) dur(%d) decay(%d) off(%d)",
			SINE_VOL,
			SINE_FREQ,
			SINE_ATTACK,
			SINE_DUR,
			SINE_DECAY,
			SINE_OFF);

		sine.amplitude(0.00);
		sine.frequency(SINE_FREQ);
		sine_phase = SINE_PHASE_OFF;
		sine_phase_time = millis();
	}


	float calc_pct(uint32_t now, uint32_t secs)
	{
		float num = (now - sine_phase_time);
		float denom = 1000 * secs;
		float pct = num/denom;
		if (pct > 1) pct = 1;
		return pct;
	}

	void handleSine()
	{
		uint32_t now = millis();
		switch (sine_phase)
		{
			case SINE_PHASE_OFF:
				if (!SINE_OFF || (now - sine_phase_time > (1000 * SINE_OFF)))
				{
					sine_phase = SINE_PHASE_ATTACK;
					sine_phase_time = now;
					display(dbg_sine,"sine_attack",0);
				}
				break;
			case SINE_PHASE_ATTACK:
				if (!SINE_ATTACK || (now - sine_phase_time > (1000 * SINE_ATTACK)))
				{
					sine_phase = SINE_PHASE_DUR;
					sine_phase_time = now;
					display(dbg_sine,"sine_dur",0);
				}
				else
				{
					sine.amplitude(SINE_VOL * calc_pct(now,SINE_ATTACK));
				}
				break;
			case SINE_PHASE_DUR:
				if (!SINE_DUR || (now - sine_phase_time > (1000 * SINE_DUR)))
				{
					sine_phase = SINE_PHASE_DECAY;
					sine_phase_time = now;
					display(dbg_sine,"sine_decay",0);
				}
				break;
			case SINE_PHASE_DECAY:
				if (!SINE_DECAY || (now - sine_phase_time > (1000 * SINE_DECAY)))
				{
					sine_phase = SINE_PHASE_OFF;
					sine_phase_time = now;
					display(dbg_sine,"sine_off",0);
					sine.amplitude(0.00);
				}
				else
				{
					sine.amplitude(SINE_VOL * (1.0 - calc_pct(now,SINE_DECAY)));
				}
				break;
		}
	}	// handleSine()

#endif	// WITH_SINE





//=================================================
// setup()
//=================================================

extern "C" {
    extern void my_usb_init();          	 // in usb.c
	extern const char *getUSBSerialNum();	// _usbNames.c
}



void reboot_teensy()
{
	warning(0,"REBOOTING TE_HUB!",0);
	delay(300);
	SCB_AIRCR = 0x05FA0004;
	SCB_AIRCR = 0x05FA0004;
	while (1) ;
}



void setup()
{
	#if PIN_HUB_ALIVE
		pinMode(PIN_HUB_ALIVE,OUTPUT);
		digitalWrite(PIN_HUB_ALIVE,1);
	#endif

	//-----------------------------------------
	// initialize MIDI_SERIAL_PORT
	//-----------------------------------------

	setColorString(COLOR_CONST_DEFAULT, "\033[94m");	// bright blue
        // TE3_hub's normal display color is bright blue
        // TE3's normal (default) display color is green
        // Looper's normal display color, is cyan, I think

	MIDI_SERIAL_PORT.begin(115200);		// Serial1
	#if HOW_DEBUG_OUTPUT == DEBUG_TO_MIDI_SERIAL
		delay(500);
		dbgSerial = &MIDI_SERIAL_PORT;
		display(0,"TE3_hub.ino setup() started on MIDI_SERIAL_PORT",0);
	#endif

	//-----------------------
	// initialize usb
	//-----------------------

    delay(500);
	my_usb_init();

	#if PIN_HUB_ALIVE
		digitalWrite(PIN_HUB_ALIVE,0);
	#endif

	//---------------------------------
	// initialize USB_SERIAL_PORT
	//---------------------------------

	#if HOW_DEBUG_OUTPUT == DEBUG_TO_USB_SERIAL
		delay(500);
		USB_SERIAL_PORT.begin(115200);		// Serial.begin()
		delay(500);
		display(0,"TE3_hub.ino setup() started on USB_SERIAL_PORT",0);
	#endif

	//-----------------------------------
	// initialize the audio system
	//-----------------------------------

	delay(500);
	display(0,"initializing audio system",0);

	AudioMemory(100);
	delay(250);

	sgtl5000.enable();
	sgtl5000.setDefaults();

	// tehub_dumpCCValues("in TE_hub::setup()");
		// see heavy duty notes in sgtl5000midi.h

	#if WITH_MIXERS
		setMixLevel(MIX_CHANNEL_IN, 	DEFAULT_VOLUME_IN);
		setMixLevel(MIX_CHANNEL_USB, 	DEFAULT_VOLUME_USB);
		setMixLevel(MIX_CHANNEL_LOOP,	DEFAULT_VOLUME_LOOP);
		setMixLevel(MIX_CHANNEL_AUX, 	DEFAULT_VOLUME_AUX);
	#endif
	
	#if WITH_SINE
		setMixLevel(MIX_CHANNEL_IN_USB, 	DEFAULT_VOLUME_IN_USB);
		setMixLevel(MIX_CHANNEL_IN_SINE, 	DEFAULT_VOLUME_IN_SINE);
		initSine();
	#endif


	//--------------------------------
	// setup finished
	//--------------------------------

	#if PIN_HUB_ALIVE
		digitalWrite(PIN_HUB_ALIVE,1);
	#endif

	tehub_dumpCCValues("from dump_tehub command"); 

	display(0,"TE3_hub.ino setup() finished",0);

}	// setup()





//=============================================
// loop()
//=============================================

void loop()
{
	// trying to debug USB audio glitches (pops and hiccups).
	// I don't know if these available global vars are indicators or not.
	//
	// I need to understand what these signify, and possibly how to address them.
	//
	// I always get an underrun of about 120-135 to begin with
	//		and I have not seen it change after that.
	//
	// I always get regular, 1-2 per second, overrruns after a while.
	// It is not clear if I get the overruns sooner, or more frequently,
	//		based on the complexity (WITH_MIXERS, WITH_SINE) of my code.

	#if 0
		extern volatile uint32_t usb_audio_underrun_count;
		extern volatile uint32_t usb_audio_overrun_count;

		static uint32_t show_usb_time = 0;
		static uint32_t last_underrun = 0;
		static uint32_t last_overrun = 0;

		if	(millis() - show_usb_time > 30)
		{
			if (last_underrun != usb_audio_underrun_count ||
				last_overrun != usb_audio_overrun_count)
			{
				last_underrun = usb_audio_underrun_count;
				last_overrun = usb_audio_overrun_count;

				display(0,"USB Audio over(%d) under(%d)",
					usb_audio_overrun_count,
					usb_audio_underrun_count);
			}
			show_usb_time = millis();
		}
	#endif

	#if PIN_HUB_ALIVE
		static bool flash_on = 0;
		static uint32_t flash_last = 0;
		if (millis() - flash_last > 1000)
		{
			flash_last = millis();
			flash_on = !flash_on;
			digitalWrite(PIN_HUB_ALIVE,flash_on);
	    }
	#endif // PIN_HUB_ALIVE


	//-----------------------------
	// set SGTL5000 from USB
	//-----------------------------
	// interesting possibility to set levels from iPad

	#if 0
		float vol_float = usb_in.volume();  		// read PC volume setting
		uint8_t vol = vol_float * 127;
		uint8_t left = sgtl5000.getHeadphoneVolumeLeft();
		if (vol != left)
		{
			display(0,"USB VOL(%0.3f)=%d left=%d",vol_float,vol,left);
			sgtl5000.setHeadphoneVolume(vol);
		}
	#endif

	//------------------------------------------------------
	// Normal Processing
	//------------------------------------------------------
	// handleSerialMidi() && SGTL5000 eq automation

	#if 1
		handleSerialMidi();
	#endif

	#if 1
		sgtl5000.loop();
	#endif

	#if WITH_SINE
		handleSine();
	#endif

}	// loop()




//==============================================================
// Serial MIDI handler
//==============================================================
// #define TEHUB_CC_IN_MIX_USB			37
// #define TEHUB_CC_IN_MIX_SINE			38



#define dbg_sm  0
#define dbg_dispatch	0

int tehub_getCC(uint8_t cc)
{
	switch (cc)
	{
		case TEHUB_CC_DUMP				: return 255;
		case TEHUB_CC_REBOOT			: return 255;
		case TEHUB_CC_RESET				: return 255;

		#if WITH_MIXERS
			case TEHUB_CC_MIX_IN		: return mix_level[MIX_CHANNEL_IN];
			case TEHUB_CC_MIX_USB		: return mix_level[MIX_CHANNEL_USB];
			case TEHUB_CC_MIX_LOOP		: return mix_level[MIX_CHANNEL_LOOP];
			case TEHUB_CC_MIX_AUX		: return mix_level[MIX_CHANNEL_AUX];
		#endif

		#if WITH_SINE
			case TEHUB_CC_IN_MIX_USB	: return mix_level[MIX_CHANNEL_IN_USB];
			case TEHUB_CC_IN_MIX_SINE	: return mix_level[MIX_CHANNEL_IN_SINE];
		#endif
	}

	return -1;		// unimplmented CC
}


void tehub_dumpCCValues(const char *where)
{
	display(0,"tehub CC values %s",where);
	int num_dumped = 0;
	
	proc_entry();
	for (uint8_t cc=TEHUB_CC_BASE; cc<=TEHUB_CC_MAX; cc++)
	{
		if (!tehub_writeOnlyCC(cc))
		{
			int val = tehub_getCC(cc);
			if (val != -1)
			{
				num_dumped++;
				display(0,"TEHUB_CC(%-2d) = %-4d  %-19s max=%d",cc,val,tehub_getCCName(cc),tehub_getCCMax(cc));
			}
		}
	}
	if (!num_dumped)
		my_error("There are no CC's implemented by define in te_hub!",0);

	proc_leave();
}


bool tehub_dispatchCC(uint8_t cc, uint8_t val)
{
	display(dbg_dispatch,"tehub CC(%d) %s <= %d",cc,tehub_getCCName(cc),val);

	switch (cc)
	{
		case TEHUB_CC_DUMP		: tehub_dumpCCValues("from dump_tehub command"); return 1;
		case TEHUB_CC_REBOOT	: reboot_teensy();
		case TEHUB_CC_RESET		: display(0,"TEHUB_RESET not implemented yet",0); return 1;

		#if WITH_MIXERS
			case TEHUB_CC_MIX_IN	: return setMixLevel(MIX_CHANNEL_IN,   val);
			case TEHUB_CC_MIX_USB	: return setMixLevel(MIX_CHANNEL_USB,  val);
			case TEHUB_CC_MIX_LOOP	: return setMixLevel(MIX_CHANNEL_LOOP, val);
			case TEHUB_CC_MIX_AUX	: return setMixLevel(MIX_CHANNEL_AUX,  val);
		#endif

		#if WITH_SINE
			case TEHUB_CC_IN_MIX_USB	: return setMixLevel(MIX_CHANNEL_IN_USB, val);
			case TEHUB_CC_IN_MIX_SINE	: return setMixLevel(MIX_CHANNEL_IN_SINE, val);
		#endif
	}

	my_error("unknown dispatchCC(%d,%d)",cc,val);
	return false;
}



#define isCC(byte)				(((byte) & 0x0f) == MIDI_TYPE_CC)
#define knownCable(byte)		(((byte >> 4) == SGTL5000_CABLE) || ((byte >> 4) == TEHUB_CABLE))
#define knownCableCC(byte)		(isCC(byte) && knownCable(byte))


void handleSerialMidi()
{
	static uint32_t msg32 = 0;
	static uint8_t *buf = (uint8_t *)&msg32;
	static uint8_t len = 0;

	// code expects a leading SERIAL_MIDI byte that
	// has a known cable. The redundant message 'type'
	// in the leading byte is not checked.
	//
	// If an unknown cable is detected, it will report an
	// error and skip the byte in an attempt to try to
	// synchronize to the MIDI stream.  Otherwise it buffers
	// four bytes for handling.

	while (MIDI_SERIAL_PORT.available())
	{
		uint8_t byte = MIDI_SERIAL_PORT.read();

		if (len == 0)
		{
			if (knownCableCC(byte))
			{
				buf[len++] = byte;
			}
			else
			{
				my_error("TE3_hub: unexpected cable/type in MIDI byte0(0x%02x)",byte);
			}
		}
		else
		{
			buf[len++] = byte;
			if (len == 4)
			{
				display(dbg_sm,"<-- %08x",msg32);

				msgUnion msg(msg32);

				if (msg.cable() == SGTL5000_CABLE &&
					msg.channel() == SGTL5000_CHANNEL &&
					msg.type() == MIDI_TYPE_CC)
				{
					sgtl5000.dispatchCC(msg.param1(),msg.param2());
				}
				else if (msg.cable() == TEHUB_CABLE &&
						 msg.channel() == TEHUB_CHANNEL &&
						 msg.type() == MIDI_TYPE_CC)
				{
					tehub_dispatchCC(msg.param1(),msg.param2());
				}

				else
				{
					my_error("TE3_hub: unexpected serial midi(0x%08x)",msg32);
				}

				len = 0;
			}
		}
	}

}




