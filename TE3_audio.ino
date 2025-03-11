//-------------------------------------------------------
// TE3_audio.ino
//-------------------------------------------------------
// USB Serial & Audio device
// Runs on a teensy4.0 with a RevD audio board above it.
// Communicates with TE3 via MIDI Serial data.
// The audio device is a teensyQuad (AudioIn/OutI2SQuad).
// See kicad designs for full teensy4.0 pin usage
//
//------------------------------------------------------------
// I2S Pin Usage
//------------------------------------------------------------
// This table best summarizes the I2S pin usages between the
// teensy audio shields, the teensy MPU's, and the rPi
//
// teensy
// audio		revB			revD
// diagram		teensy			teensy	teensy					quad
// label		3.2		3.6		4.x		fxn			rPi			chan		notes
// ----------------------------------------------------------------------------------------------------
// MCLCK		11				23		MCLCK								generated by teensy
// BLCK			9				21		BCLK		18_BCLK					generated by teensy
// LRCLK		23				20		LRCLCK		19_FCLK					generated by teensy
// SGTL_DOUT	13				8		I2S_RXA					in(0,1)		audio inputs --> SGTL --> teensy
// SGTL_DIN		22				7		I2S_TXA					out(0,1)	teensy --> SGTL --> audio outputs
// 				15				32*		I2S_TXB		20_RPI_RXD	out(2,3)	teensy --> Looper
// 				30*		38		6		I2S_RXB		21_RPI_TXD	in(2,3)		Looper --> teensy
//
// * pins on bottom of teensy3.2 and 4.0
//   on my first modified teensy4.0 this is the GREY wire in the 3 pin connector
//
//-----------------------------------------------------------------
// Notes on my modified teensy 4.0's
//-----------------------------------------------------------------
// Since we are using a teensy 4.0, we need to bring pin32 off the bottom
// of the teensy 4.0 for I2S_TXB.  The 1st teensy4.0 that I modified has a
// three pin JST connector:
//
//			GREY = pin32, I2S_TXB
//			green = usb host D-
// 			yello = usb host D+
//
// There are 'old' teensy4.0's and 'new' ones. The photo I use in the
// inventory is an 'old' one and has two separate dual diodes with
// two traces coming off the bottom one like a sideways 'V'
// to the 3.3V and VBat "end" pins, respectively.  In the "new" teensy4.0's,
// the two diodes are replaced by a 6pin IC and there are horizontal
// traces going to the 3.3V and VBat end pins.
//
// This is relevant *if*  I choose to CUT the VBat trace and route pin32
// to the VBat end pin right next to it, which *may* become a design decision,
// especially to the degree that I will *probably* want to solder a header or
// pins there to bring the teensy 4.0 prog button out somehow.
//
// Unforunately, there is a 'used' 4.0 in a packet marked
// "Bad Mod Beware", where not only did I cut the 3.3V and VBat lines
// to the end pins, but it looks like I also cut the trace to the all
// important pin32, so that one teensy 4.0 is NOT TO BE USED FOR the
// TE3_audio device.

#include <Audio.h>
#include <Wire.h>
#include <myDebug.h>
#include <teMIDI.h>
#include <teCommon.h>
#include <sgtl5000midi.h>
#include "src/sgtl5000.h"

#define	dbg_audio	0



#define TEST_CONFIGURATION		0
	// Whatever I need at the current time.
	// As of this writing, the Looper is not getting anytning on the right channel,
	// and the PCB's are out of the box on my desk.  I am using this to generate
	// a sine wave instead of using the i2s_in(0,1) or MONO_GUITAR_INPUT to generate
	// a signal and send it directly to the Looper on i2s_out(2,3), entirely bypassing
	// the USB in-out to the iPad.



#define MONO_GUITAR_INPUT		1
	// if this is defined, i2s_in0 will be used as a
	// MONO channel for the guitar
#define MONO_GUITAR_I2S_IN		0
	// which of the two input ports to use for
	// the mono guitar channel

#define USB_SERIAL_PORT			Serial
#define MIDI_SERIAL_PORT		Serial1

#define DEBUG_TO_USB_SERIAL		0
#define DEBUG_TO_MIDI_SERIAL	1
#define HOW_DEBUG_OUTPUT		DEBUG_TO_MIDI_SERIAL
	// The default is that debugging goes to the MIDI Serial port
	// (TE3) and is forwarded to the laptop from there.  If I need
	// to hook up directly to the TE3_audio, it means it's connected
	// to the laptop for unusual debugging.

#define PIN_AUDIO_ALIVE	13
	// Onboard Heartbeat LED
#define PIN_AUDIO_BUSY  2
	// Onboard Heartbeat LED + Serial Data Busy indicator

static bool flash_on = 0;
static uint32_t flash_last = 0;
static uint32_t audio_busy_led_time = 0;


//===============================================================================
// Audio Setup
//===============================================================================
// Over the course of development, I had a variety of options for
// building and debugging this device, including defines to build a
// straight USB pass through and to include SINE wave injection.
// Now that it has stabilized a bit, I have removed most compile options.
//
// The i2s_in and i2s_out device are each four channel devices (0..3):
//
// - i2s_in(0,1) receives data directly from the (LINE_IN of the) SGTL5000.
// - i2s_out(0,1) sends data directly to the (LINE_OUT of the) SGTL5000.
// - i2s_out(3,4) sends data to the rPi looper
// - i2s_in(3,4) recives data from the rPi looper
//
// The normal production routing is:
//
// 	      SGTL               iPad         iPad          Looper       Looper                SGTL
// LINE_IN-->i2s_in(0,1)-->usb_out(0,1) usb_in(0,1)-->i2s_out(3,4) i2s_in(3,4)-->i2s_out(0,1)-->LINE_OUT
//
//
// As I integrate the new Looper, there is still some flexibility
// for testing, by the inclusion of an output mixer.
// The fourth channel on the output mixers is currently unused
//
// 	       SGTL              iPad       iPad        Looper     Looper                            SGTL
// LINE_IN_L-->i2s_in(0)+->usb_out(0) usb_in(0)+->i2s_out(3) i2s_in(3)-> mixerL(2) -+
//                      |                      |                                    |
//                      |                      +-----------------------> mixerL(1) -+-> i2s_out(0)-->LINE_OUT_L
//                      |                                                           |
//                      +----------------------------------------------> mixerL(0) -+
//
// LINE_IN_R-->i2s_in(1)+->usb_out(1) usb_in(1)+->i2s_out(4) i2s_in(4)-> mixerR(2) -+
//                      |                      |                                    |
//                      |                      +-----------------------> mixerR(1) -+-> i2s_out(`)-->LINE_OUT_R
//                      |                                                           |
//                      +----------------------------------------------> mixerR(0) -+

#define NUM_MIXER_CHANNELS		4

#define MIX_CHANNEL_IN			0		// the original i2s_in that is sent to the iPad (USB out)
#define MIX_CHANNEL_USB			1		// the sound returned from the iPad and sent to rpi Looper
#define MIX_CHANNEL_LOOP  		2		// the sound returned from the rPi Looper
#define MIX_CHANNEL_AUX			3		// if WITH_SINE, monitor the sine directly

#define DEFAULT_VOLUME_IN		0		// output the raw LINE_IN signal
#define DEFAULT_VOLUME_USB		0		// output returned USB (production=0)
#if TEST_CONFIGURATION
	#define DEFAULT_VOLUME_LOOP		0		// turn the Looper up if you want to hear it
#else
	#define DEFAULT_VOLUME_LOOP		100		// output the Looper (production=100)
#endif

#define DEFAULT_VOLUME_AUX		0		// unused


SGTL5000 sgtl5000;

AudioInputI2SQuad   i2s_in;
AudioOutputI2SQuad  i2s_out;
AudioInputUSB   	usb_in;
AudioOutputUSB  	usb_out;
AudioMixer4			mixer_L;
AudioMixer4			mixer_R;


#if TEST_CONFIGURATION

	#define SINE_FREQ				440
	#define SINE_AMPLITUDE			0.5
	
	AudioSynthWaveformSine  sine;

	AudioConnection	c_i1(sine,    0, mixer_L, MIX_CHANNEL_IN);			// sine --> out_mixer(0)
	AudioConnection	c_i2(sine,    0, mixer_R, MIX_CHANNEL_IN);
	AudioConnection c_q1(sine,    0, i2s_out, 2);						// sine --> Looper
	AudioConnection c_q2(sine,    0, i2s_out, 3);
	AudioConnection c_q3(i2s_in,  2, mixer_L, MIX_CHANNEL_LOOP);		// Looper --> out_mixer(2)
	AudioConnection c_q4(i2s_in,  3, mixer_R, MIX_CHANNEL_LOOP);
	AudioConnection c_o1(mixer_L, 0, i2s_out, 0);						// out_mixers --> SGTL5000
	AudioConnection c_o2(mixer_R, 0, i2s_out, 1);

#else

	#if MONO_GUITAR_INPUT
		AudioConnection	c_i1(i2s_in,  MONO_GUITAR_I2S_IN, mixer_L, MIX_CHANNEL_IN);			// SGTL5000 LINE_IN --> out_mixer(0)
		AudioConnection	c_i2(i2s_in,  MONO_GUITAR_I2S_IN, mixer_R, MIX_CHANNEL_IN);
		AudioConnection c_in1(i2s_in, MONO_GUITAR_I2S_IN, usb_out, 0);						// SGTL5000 LINE_IN --> USB_out
		AudioConnection c_in2(i2s_in, MONO_GUITAR_I2S_IN, usb_out, 1);
	#else
		AudioConnection	c_i1(i2s_in,  0, mixer_L, MIX_CHANNEL_IN);							// SGTL5000 LINE_IN --> out_mixer(0)
		AudioConnection	c_i2(i2s_in,  1, mixer_R, MIX_CHANNEL_IN);
		AudioConnection c_in1(i2s_in, 0, usb_out, 0);										// SGTL5000 LINE_IN --> USB_out
		AudioConnection c_in2(i2s_in, 1, usb_out, 1);
	#endif

	AudioConnection	c_ul(usb_in,  0, mixer_L, MIX_CHANNEL_USB);			// USB_in --> out_mixer(1)
	AudioConnection	c_ur(usb_in,  1, mixer_R, MIX_CHANNEL_USB);
	AudioConnection c_q1(usb_in,  0, i2s_out, 2);						// USB_in --> Looper
	AudioConnection c_q2(usb_in,  1, i2s_out, 3);
	AudioConnection c_q3(i2s_in,  2, mixer_L, MIX_CHANNEL_LOOP);		// Looper --> out_mixer(2)
	AudioConnection c_q4(i2s_in,  3, mixer_R, MIX_CHANNEL_LOOP);
	AudioConnection c_o1(mixer_L, 0, i2s_out, 0);						// out_mixers --> SGTL5000
	AudioConnection c_o2(mixer_R, 0, i2s_out, 1);

#endif



uint8_t mix_level[NUM_MIXER_CHANNELS];


bool setMixLevel(uint8_t channel, uint8_t val)
{
	display(dbg_audio,"setMixLevel(%d,%d)",channel,val);
	if (val > 127) val = 127;
	float vol = val;
	vol = vol/100;

	if (channel >= MIX_CHANNEL_IN && channel <= MIX_CHANNEL_AUX)
	{
		mixer_L.gain(channel, vol);
		mixer_R.gain(channel, vol);
		mix_level[channel] = val;
		return true;
	}

	my_error("unimplmented mix_channel(%d)",channel);
	return false;
}


//----------------------------------------------------
// DEBUG_AUDIO_LEVELS
//----------------------------------------------------

#define DEBUG_AUDIO_LEVELS	0	// 2000
	// If set, this is the number of milliseconds
	// to sample the four input channels using analyze_peak
	// audio devices.

#if DEBUG_AUDIO_LEVELS

	AudioAnalyzePeak peak0;
	AudioAnalyzePeak peak1;
	AudioAnalyzePeak peak2;
	AudioAnalyzePeak peak3;
	AudioAnalyzePeak peak4;
	AudioAnalyzePeak peak5;
	AudioAnalyzePeak peak6;
	AudioAnalyzePeak peak7;

#if TEST_CONFIGURATION
	AudioConnection	cp0(sine,  0, peak0,  0);
	AudioConnection	cp1(sine,  0, peak1,  0);
#elif MONO_GUITAR_INPUT
	AudioConnection	cp0(i2s_in,  MONO_GUITAR_I2S_IN, peak0,  0);
	AudioConnection	cp1(i2s_in,  MONO_GUITAR_I2S_IN, peak1,  0);
#else
	AudioConnection	cp0(i2s_in,  0, peak0,  0);
	AudioConnection	cp1(i2s_in,  1, peak1,  0);
#endif

	AudioConnection	cp2(usb_in,  0, peak2,  0);
	AudioConnection	cp3(usb_in,  1, peak3,  0);
	AudioConnection	cp4(i2s_in,  2, peak4,  0);
	AudioConnection	cp5(i2s_in,  3, peak5,  0);
	AudioConnection	cp6(mixer_L, 0, peak6,  0);
	AudioConnection	cp7(mixer_R, 0, peak7,  0);

	static void debug_audio_levels()
	{
		static uint32_t last_debug = 0;
		uint32_t now = millis();

		if (now - last_debug >= DEBUG_AUDIO_LEVELS)
		{
			last_debug = now;
			if (peak0.available())
			{
				display(0,"LEVEL IN(%0.2f,%0.2f) USB(%0.2f,%0.2f) LOOP(%0.2f,%0.2f) FIN(%0.2f,%0.2f)",
					peak0.read(),
					peak1.read(),
					peak2.read(),
					peak3.read(),
					peak4.read(),
					peak5.read(),
					peak6.read(),
					peak7.read());
			}

		}
	}

#endif	// DEBUG_AUDO_LEVELS


//=================================================
// setup()
//=================================================
// I am still using my copied _usb.c and _usbdesc.c, although
// I *could* get away with just using the official _usbNames.c
// approach.  My _usb.c was introduced to allow me to change the
// device descriptors via preferences, mostly to spoof the FTP,
// by deferring Paul's static usb_init() call to a runtime call.
//
// The only thing it actually does in this incarnation is to
// copy the actual teensy serial number into my serial number
// descriptor before initializing the USB devie, so that each
// device has a unique TE3xxxx serial number.

extern "C" {
    extern void my_usb_init();          	// in usb.c
    // extern void setFTPDescriptors();    	// commented out in _usbNames.c
	extern const char *getUSBSerialNum();	// in _usbNames.c
}


void audio_dumpCCValues(const char *where);
	// forward


void setup()
{
	pinMode(PIN_AUDIO_ALIVE,OUTPUT);
	pinMode(PIN_AUDIO_BUSY,OUTPUT);
	digitalWrite(PIN_AUDIO_ALIVE,1);
	digitalWrite(PIN_AUDIO_BUSY,1);

	for (int i=0; i<23; i++)
	{
		digitalWrite(PIN_AUDIO_ALIVE,i&1);
		digitalWrite(PIN_AUDIO_BUSY,i&1);
		delay(40);
	}

	//-----------------------------------------
	// initialize MIDI_SERIAL_PORT
	//-----------------------------------------

	setColorString(COLOR_CONST_DEFAULT, "\033[94m");	// bright blue
        // TE3_audio's normal display color is bright blue
        // TE3's normal (default) display color is green
        // Looper's normal display color, is cyan, I think

	MIDI_SERIAL_PORT.begin(115200);		// Serial1
	#if HOW_DEBUG_OUTPUT == DEBUG_TO_MIDI_SERIAL
		delay(500);
		dbgSerial = &MIDI_SERIAL_PORT;
		display(0,"TE3_audio.ino setup() started on MIDI_SERIAL_PORT",0);
	#endif

	//-----------------------
	// initialize usb
	//-----------------------

    delay(500);
	my_usb_init();

	digitalWrite(PIN_AUDIO_ALIVE,0);
	digitalWrite(PIN_AUDIO_BUSY,0);

	//---------------------------------
	// initialize USB_SERIAL_PORT
	//---------------------------------

	#if HOW_DEBUG_OUTPUT == DEBUG_TO_USB_SERIAL
		delay(500);
		USB_SERIAL_PORT.begin(115200);		// Serial.begin()
		delay(500);
		display(0,"TE3_audio.ino setup() started on USB_SERIAL_PORT",0);
	#endif

	display(0,"TE3_audio serial_number = %s",getUSBSerialNum());

	//-----------------------------------
	// initialize the audio system
	//-----------------------------------

	digitalWrite(PIN_AUDIO_ALIVE,1);
	digitalWrite(PIN_AUDIO_BUSY,1);

	delay(500);
	display(0,"initializing audio system",0);

	#if TEST_CONFIGURATION
		sine.frequency(SINE_FREQ);
		sine.amplitude(SINE_AMPLITUDE);
	#endif

	AudioMemory(100);
	delay(250);

	sgtl5000.enable();
	sgtl5000.setDefaults();

	// audio_dumpCCValues("in TE3_audio::setup()");
		// see heavy duty notes in sgtl5000midi.h

	setMixLevel(MIX_CHANNEL_IN, 	DEFAULT_VOLUME_IN);
	setMixLevel(MIX_CHANNEL_USB, 	DEFAULT_VOLUME_USB);
	setMixLevel(MIX_CHANNEL_LOOP,	DEFAULT_VOLUME_LOOP);
	setMixLevel(MIX_CHANNEL_AUX, 	DEFAULT_VOLUME_AUX);


	//--------------------------------
	// setup finished
	//--------------------------------

	digitalWrite(PIN_AUDIO_ALIVE,0);
	digitalWrite(PIN_AUDIO_BUSY,0);

	audio_dumpCCValues("from dump_audio command"); 

	display(0,"TE3_audio.ino setup() finished",0);

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

	uint32_t led_delay = flash_on ? 20 : 1980;
	uint32_t flash_now = millis();
	if (flash_now - flash_last > led_delay)
	{
		flash_last = flash_now;
		flash_on = !flash_on;
		digitalWrite(PIN_AUDIO_ALIVE,flash_on);
		digitalWrite(PIN_AUDIO_BUSY,flash_on);
	}

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

	handleSerialMidi();
	sgtl5000.loop();

	#if DEBUG_AUDIO_LEVELS
		debug_audio_levels();
	#endif
	
}	// loop()



//----------------------------
// utilities
//----------------------------


void reboot_teensy()
{
	warning(0,"REBOOTING TE_audio device!",0);
	delay(300);
	SCB_AIRCR = 0x05FA0004;
	SCB_AIRCR = 0x05FA0004;
	while (1) ;
}




//==============================================================
// Serial MIDI handler
//==============================================================

#define dbg_sm  0
#define dbg_dispatch	0

static void setAudioBusy()
{
	digitalWrite(PIN_AUDIO_BUSY,1);
	audio_busy_led_time = millis();
}
static void clearAudioBusy()
{
	if (!flash_on && audio_busy_led_time && millis() - audio_busy_led_time > 200)
	{
		audio_busy_led_time = 0;
		digitalWrite(PIN_AUDIO_BUSY,0);
	}
}


int audio_getCC(uint8_t cc)
{
	switch (cc)
	{
		case AUDIO_CC_DUMP				: return 255;
		case AUDIO_CC_REBOOT			: return 255;
		case AUDIO_CC_RESET				: return 255;

		case AUDIO_CC_MIX_IN		: return mix_level[MIX_CHANNEL_IN];
		case AUDIO_CC_MIX_USB		: return mix_level[MIX_CHANNEL_USB];
		case AUDIO_CC_MIX_LOOP		: return mix_level[MIX_CHANNEL_LOOP];
		case AUDIO_CC_MIX_AUX		: return mix_level[MIX_CHANNEL_AUX];
	}

	return -1;		// unimplmented CC
}


void audio_dumpCCValues(const char *where)
{
	display(0,"audio CC values %s",where);
	int num_dumped = 0;
	
	proc_entry();
	for (uint8_t cc=AUDIO_CC_BASE; cc<=AUDIO_CC_MAX; cc++)
	{
		if (!audio_writeOnlyCC(cc))
		{
			int val = audio_getCC(cc);
			if (val != -1)
			{
				num_dumped++;
				display(0,"AUDIO_CC(%-2d) = %-4d  %-19s max=%d",cc,val,audio_getCCName(cc),audio_getCCMax(cc));
			}
		}
	}
	if (!num_dumped)
		my_error("There are no CC's implemented by define in the TE3_audio device!",0);

	proc_leave();
}


bool audio_dispatchCC(uint8_t cc, uint8_t val)
{
	display(dbg_dispatch,"audio CC(%d) %s <= %d",cc,audio_getCCName(cc),val);

	switch (cc)
	{
		case AUDIO_CC_DUMP		: audio_dumpCCValues("from dump_audio command"); return 1;
		case AUDIO_CC_REBOOT	: reboot_teensy();
		case AUDIO_CC_RESET		: display(0,"AUDIO_RESET not implemented yet",0); return 1;

		case AUDIO_CC_MIX_IN	: return setMixLevel(MIX_CHANNEL_IN,   val);
		case AUDIO_CC_MIX_USB	: return setMixLevel(MIX_CHANNEL_USB,  val);
		case AUDIO_CC_MIX_LOOP	: return setMixLevel(MIX_CHANNEL_LOOP, val);
		case AUDIO_CC_MIX_AUX	: return setMixLevel(MIX_CHANNEL_AUX,  val);
	}

	my_error("unknown dispatchCC(%d,%d)",cc,val);
	return false;
}



#define isCC(byte)				(((byte) & 0x0f) == MIDI_TYPE_CC)
#define knownCable(byte)		(((byte >> 4) == SGTL5000_CABLE) || ((byte >> 4) == AUDIO_CABLE))
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
		setAudioBusy();

		if (len == 0)
		{
			if (knownCableCC(byte))
			{
				buf[len++] = byte;
			}
			else
			{
				my_error("TE3_audio: unexpected cable/type in MIDI byte0(0x%02x)",byte);
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
				else if (msg.cable() == AUDIO_CABLE &&
						 msg.channel() == AUDIO_CHANNEL &&
						 msg.type() == MIDI_TYPE_CC)
				{
					audio_dispatchCC(msg.param1(),msg.param2());
				}

				else
				{
					my_error("TE3_audio: unexpected serial midi(0x%08x)",msg32);
				}

				len = 0;
				setAudioBusy();
			}
		}
	}

	clearAudioBusy();

}	// handleSerialMidi()


// end of TE3_audio.ino
