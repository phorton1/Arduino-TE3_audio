//-------------------------------------------------------
// TE3_hub.ino
//-------------------------------------------------------
// USB Serial, Audio, and MIDI device with USB Host

#include <Audio.h>
#include <Wire.h>
#include <myDebug.h>
#include <teMIDI.h>
#include <teCommon.h>
#include <sgtl5000midi.h>
#include "src/sgtl5000.h"
#include "src/midiHost.h"


#define	dbg_audio	0


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
	// to the laptop, and I can recompile if I wanna change this.

#define SPOOF_FTP  0
	// requires WITH_MIDI_HOST in midiHost.h (which is always on)
	// This works and has been tested, but I know more than I want
	// to know about the FTP already, and I'm probably not gonna
	// implement midi monitoring in TE3, much less TE3_hub.
	// If I need to, I can recompile and insert custom debugging.
	//
	// NOTE REGARDINg midiHost NOISE
	//
	// Initial tests revealed that an active midiHost causes noticable
	// noise on the teensy audio shield. I thought it was a severe
	// problem, requiring an architectural change, but after i hooked
	// the guitar up, got the SGTL5000 levels adjusted, through the PA,
	// I decided it wasn't bad enough to worry about. So the plan is,
	// at this time, to assume the FTP, if any, will be in the midiHost
	// port, and no provision will be coded for having it on an outside
	// hub, or creating a midiHost in the TE3 part of the system.

#define FLASH_PIN	13
	// Set this to a pin to flash a heartbeat during loop()
	// The teensy4.x onboard LED is pin 13

#define SINE_WAVE_TO_USB	0
	// compile time option to send a sine wave to the iPad
#define SINE_WAVE_TO_AUX	0
	// compile time option to send a sine wave to the MIX_CHANNEL_AUX


#define DEFAULT_VOLUME_IN		0		// set to 1.0 in early testing or to bypass USB and Looper
#define DEFAULT_VOLUME_USB		0		// set to 1.0 to hear what comes from the iPad
#define DEFAULT_VOLUME_LOOP		100		// currently all mixing is done on the Looper ... more later
#define DEFAULT_VOLUME_AUX		80		// zero if !SINE_WAVE_TO_USB || SINE_WAVE_TO_AUX
	
#define FREQ_SINE1				330
#define FREQ_SINE2				440
#define SINE_TOGGLE_TIME		1000	// ms
#define SINE_AMPLITUDE			0.8


//-------------------------------------------
// audio system setup
//-------------------------------------------

#define MIX_CHANNEL_IN		0			// the original i2s_in (already in MAIN if !USE_TEENSY_QUAD)
#define MIX_CHANNEL_USB		1			// the sound returned from the iPad and sent to rpi Looper
#define MIX_CHANNEL_LOOP  	2			// the sound returned from the rPi Looper
#define MIX_CHANNEL_AUX		3			// at this time, where the sine wave, if any will be sent

uint8_t mix_level[4];


SGTL5000 sgtl5000;

AudioInputI2SQuad       i2s_in;
AudioOutputI2SQuad      i2s_out;
AudioInputUSB   		usb_in;
AudioOutputUSB  		usb_out;
AudioSynthWaveformSine  sine1;
AudioSynthWaveformSine  sine2;
AudioMixer4				mixer_L;
AudioMixer4				mixer_R;


AudioConnection	c_il(i2s_in,  0, mixer_L, MIX_CHANNEL_IN);			// SGTL5000 --> mixer
AudioConnection	c_ir(i2s_in,  1, mixer_R, MIX_CHANNEL_IN);

#if SINE_WAVE_TO_USB
	AudioConnection c_in1(sine1,  0, usb_out, 0);					// sine wave --> USB_out
	AudioConnection c_in2(sine2,  0, usb_out, 1);
#else
	AudioConnection c_in1(i2s_in, 0, usb_out, 0);					// STGTL5000 --> USB_out
	AudioConnection c_in2(i2s_in, 1, usb_out, 1);
#endif

AudioConnection	c_ul(usb_in,  0, mixer_L, MIX_CHANNEL_USB);			// USB_in -> mixer
AudioConnection	c_ur(usb_in,  1, mixer_R, MIX_CHANNEL_USB);
AudioConnection c_q1(usb_in,  0, i2s_out, 2);						// USB_in --> Looper
AudioConnection c_q2(usb_in,  1, i2s_out, 3);
AudioConnection c_q3(i2s_in,  2, mixer_L, MIX_CHANNEL_LOOP);		// Looper --> mixer
AudioConnection c_q4(i2s_in,  3, mixer_R, MIX_CHANNEL_LOOP);
AudioConnection c_o1(mixer_L, 0, i2s_out, 0);						// mixer --> SGTL5000
AudioConnection c_o2(mixer_R, 0, i2s_out, 1);


#if SINE_WAVE_TO_USB || SINE_WAVE_TO_AUX
	AudioConnection c_sine_L.connect(sine1, 0, mixer_L, MIX_CHANNEL_SINE);
	AudioConnection c_sine_R.connect(sine2, 0, mixer_R, MIX_CHANNEL_SINE);
#endif


bool setMixLevel(uint8_t channel, uint8_t val)
{
	display(dbg_audio,"setMixLevel(%d,%d)",channel,val);
	if (val > 127) val = 127;
	float vol = val/100;
	mixer_L.gain(channel, vol);
	mixer_R.gain(channel, vol);
	mix_level[channel] = val;
	return true;
}



//=================================================
// setup()
//=================================================

extern "C" {
    extern void my_usb_init();          	// in usb_dev.c
    extern void setFTPDescriptors();    	// _usbNames.c
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
	#if FLASH_PIN
		pinMode(FLASH_PIN,OUTPUT);
		digitalWrite(FLASH_PIN,1);
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

	#if SPOOF_FTP
		setFTPDescriptors();
    #endif

    delay(500);
	my_usb_init();

	#if FLASH_PIN
		digitalWrite(FLASH_PIN,0);
	#endif


	//---------------------------------
	// initialize USB_SERIAL_PORT
	//---------------------------------

	delay(500);
	USB_SERIAL_PORT.begin(115200);		// Serial.begin()
	#if HOW_DEBUG_OUTPUT == DEBUG_TO_ALT_SERIAL
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
		// see heavy duty notes in sgtl5000midi.h
		
	setMixLevel(MIX_CHANNEL_IN, 	DEFAULT_VOLUME_IN);
	setMixLevel(MIX_CHANNEL_USB, 	DEFAULT_VOLUME_USB);
	setMixLevel(MIX_CHANNEL_LOOP,	DEFAULT_VOLUME_LOOP);

	#if SINE_WAVE_TO_USB || SINE_WAVE_TO_AUX
		sine1.frequency(FREQ_SINE1);
		sine2.frequency(FREQ_SINE2);
		setMixLevel(MIX_CHANNEL_AUX, 	DEFAULT_VOLUME_AUX);
	#else
		setMixLevel(MIX_CHANNEL_AUX, 	0.0);
	#endif

	tehub_dumpCCValues("in TE_hub::setup()");


	//--------------------------------------
	// initialize midiHost
	//--------------------------------------

	#if WITH_MIDI_HOST   // defined in midiHost.h
		display(0,"initilizing midiHost",0);
		midi_host.init();
	#endif

	// setup finished

	#if FLASH_PIN
		digitalWrite(FLASH_PIN,1);
	#endif

	display(0,"TE3_hub.ino setup() finished",0);

}	// setup()





//=============================================
// loop()
//=============================================

void loop()
{
	#if FLASH_PIN
		static bool flash_on = 0;
		static uint32_t flash_last = 0;
		uint32_t now = millis();
		if (now > flash_last + 1000)
		{
			flash_last = now;
			flash_on = !flash_on;
			digitalWrite(FLASH_PIN,flash_on);

			#if 0
				static int counter = 0;
				display(0,"loop(%d)",counter++);

				#if 0
					// send an arbitray midi message
					uint32_t val = counter % 128;
					val <<= 24;
					uint32_t msg = 0x0022b70b | val;
					usb_midi_write_packed(msg);
				#endif

			#endif
	    }
	#endif // FLASH_PIN


	//--------------------------------------------------------
	// SPOOF_FTP processing
	//--------------------------------------------------------

	#if WITH_MIDI_HOST && SPOOF_FTP
		uint32_t msg32 = usb_midi_read_message();  // read from device
		if (msg32)
		{
			// display(0,"midi(0x%08x",msg32);
			midi_host.write_packed(msg32);
		}
	#endif


	//-----------------------------
	// set SGTL5000 from USB
	//-----------------------------
	// interesting possibility to set levels from iPad

	#if WITH_USB_AUDIO	// vestigal code to set SGTL5000 volume from USB device
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
	// handle SGTL5000 eq automation

	handleSerialMidi();

	sgtl5000.loop();

	#if SINE_WAVE_TO_USB || SINE_WAVE_TO_AUX
		static bool sine_toggle = 0;
		static uint32_t next_sine_time = 0;
		if (millis() > next_sine_time)
		{
			next_sine_time = millis() + SINE_TOGGLE_TIME;
			sine_toggle = !sine_toggle;
			sine1.amplitude(sine_toggle ? 0.0 : SINE_AMPLITUDE);
			sine2.amplitude(sine_toggle ? SINE_AMPLITUDE : 0.0);
		}
	#endif

}	// loop()



//==============================================================
// Serial MIDI handler
//==============================================================

#define dbg_sm  0
#define dbg_dispatch	0

int tehub_getCC(uint8_t cc)
{
	int val = -1;

	switch (cc)
	{
		case TEHUB_CC_DUMP		: val = 255;	break;
		case TEHUB_CC_REBOOT	: val = 255;	break;
		case TEHUB_CC_RESET		: val = 255;	break;
		case TEHUB_CC_MIX_IN	: return mix_level[MIX_CHANNEL_IN];		break;
		case TEHUB_CC_MIX_USB	: return mix_level[MIX_CHANNEL_USB];	break;
		case TEHUB_CC_MIX_LOOP	: return mix_level[MIX_CHANNEL_LOOP];	break;
		case TEHUB_CC_MIX_AUX	: return mix_level[MIX_CHANNEL_AUX];	break;
	}

	return val;
}


void tehub_dumpCCValues(const char *where)
{
	display(0,"tehub CC values %s",where);
	proc_entry();
	for (uint8_t cc=TEHUB_CC_BASE; cc<=TEHUB_CC_MAX; cc++)
	{
		if (!tehub_writeOnlyCC(cc))
		{
			int val = tehub_getCC(cc);
			display(0,"TEHUB_CC(%-2d) = %-4d  %-19s max=%d",cc,val,tehub_getCCName(cc),tehub_getCCMax(cc));
		}
		else if (0)
			display(0,"",0);
	}
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
		case TEHUB_CC_MIX_IN	: return setMixLevel(MIX_CHANNEL_IN,   val);
		case TEHUB_CC_MIX_USB	: return setMixLevel(MIX_CHANNEL_USB,  val);
		case TEHUB_CC_MIX_LOOP	: return setMixLevel(MIX_CHANNEL_LOOP, val);
		case TEHUB_CC_MIX_AUX	: return setMixLevel(MIX_CHANNEL_AUX,  val);
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




