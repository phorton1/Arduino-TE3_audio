//-------------------------------------------------------
// TE3_hub.ino
//-------------------------------------------------------
// USB Serial, Audio, and MIDI device with USB Host

#include <Audio.h>
#include <Wire.h>
#include <myDebug.h>
#include <teSGTL5000.h>
#include <teMidi.h>
#include "src/midiHost.h"


#define	dbg_audio	0

#define USB_SERIAL_PORT			Serial
#define MIDI_SERIAL_PORT		Serial1


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

#define FLASH_PIN		13
	// Set this to a pin to flash a heartbeat during loop()
	// The teensy4.x onboard LED is pin 13

#define USE_TEENSY_QUAD		0
	// Deployment setting will be 1.  Otherwise, for early
	// testing I allowed for using a regular i2s device.
#define SINE_WAVE_TO_USB	0
	// compile time option to send a sine wave to the iPad
	
//-------------------------------------------
// audio system setup
//-------------------------------------------
// All audio system components are statically constructed.
// They are connected during setup().

#define MAX_CONNECTIONS		16

#define MIX_CHANNEL_IN		0		// the original i2s_in (already in MAIN if !USE_TEENSY_QUAD)
#define MIX_CHANNEL_USB		1		// the sound returned from the iPad
#define MIX_CHANNEL_MAIN  	2		// THE FINAL RESULT
#define MIX_CHANNEL_SINE	3		// A sine wave that should always be audible

#define DEFAULT_VOLUME_IN		1.0
#define DEFAULT_VOLUME_USB		1.0
#define DEFAULT_VOLUME_MAIN		1.0
#define DEFAULT_VOLUME_SINE		0.8

// sine wave setup

#define FREQ_SINE1			330
#define FREQ_SINE2			440
#define SINE_TOGGLE_TIME	1000			// ms

// run time options
// if any are set to 1, setNewConnection() will be called from setup()

bool monitorInput = 0;
	// if 1, MIX_CHANNEL_IN will be hooked up
	// will be redundant to MAIN if !USE_TEENSY_QUAD
bool monitorUSB = 1;
	// if 1, MIX_CHANNEL_USB will be hooked up
bool monitorSine  = 0;
	// if 1, MIX_CHANNEL_SINE will be hooked up
bool lastMonitorInput = 0;
bool lastMonitorUSB = 0;
bool lastMonitorSine = 0;
	// only set volumes on state changes


// ALL AUDIO SYSTEM COMPONENTS ARE STATICALLY INITALIZED

SGTL5000 sgtl5000;

#if WITH_TEENSY_QUAD
	AudioInputI2SQuad       i2s_in;
	AudioOutputI2SQuad      i2s_out;
#else
	AudioInputI2S  			i2s_in;
	AudioOutputI2S  		i2s_out;
#endif

AudioInputUSB   		usb_in;
AudioOutputUSB  		usb_out;
AudioSynthWaveformSine  sine1;
AudioSynthWaveformSine  sine2;
AudioMixer4				mixer_L;
AudioMixer4				mixer_R;

// STATICALLY INITIALIZED CONNECTIONS

#if SINE_WAVE_TO_USB
	AudioConnection c_in1(sine1,  0, usb_out, 0);					// sine wave --> USB_out
	AudioConnection c_in2(sine2,  0, usb_out, 1);
#else	// normal setup
	AudioConnection c_in1(i2s_in, 0, usb_out, 0);					// STGTL5000 --> USB_out
	AudioConnection c_in2(i2s_in, 1, usb_out, 1);
#endif

#if WITH_TEENSY_QUAD
	AudioConnection c_q1(usb_in, 0, i2s_out, 2);					// USB_in --> Looper
	AudioConnection c_q2(usb_in, 1, i2s_out, 3);
	AudioConnection c_q3(i2s_in, 2, mixer_L, MIX_CHANNEL_MAIN);		// Looper --> mixer
	AudioConnection c_q4(i2s_in, 3, mixer_R, MIX_CHANNEL_MAIN);
#else
	AudioConnection c_s1(i2s_in, 0, mixer_L, MIX_CHANNEL_MAIN);		// SGTL5000 --> mixer
	AudioConnection c_s2(i2s_in, 1, mixer_R, MIX_CHANNEL_MAIN);
#endif

AudioConnection c_out1(mixer_L, 0, i2s_out, 0);						// mixer --> SGTL5000
AudioConnection c_out2(mixer_R, 0, i2s_out, 1);


// dynamic connections

AudioConnection c_in_L;
AudioConnection c_in_R;
AudioConnection c_usb_L;
AudioConnection c_usb_R;
AudioConnection c_sine_L;
AudioConnection c_sine_R;

bool sine_toggle = 0;
uint32_t next_sine_time = 0;


//----------------------
// setNewConnection
//----------------------

void setNewConnection()
{
	display(dbg_audio,"setNewConnection() old/new input(%d,%d) USB(%d,%d) sine(%d,%d) ",
		lastMonitorInput,
		monitorInput,
		lastMonitorUSB,
		monitorUSB,
		lastMonitorSine,
		monitorSine);
	proc_entry();

	if (lastMonitorInput != monitorInput)
	{
		lastMonitorInput = monitorInput;
		if (monitorInput)
		{
			display(0,"connecting monitorInput",0);
			c_in_L.connect(i2s_in, 0, mixer_L, MIX_CHANNEL_IN);
			c_in_R.connect(i2s_in, 1, mixer_R, MIX_CHANNEL_IN);
			mixer_L.gain(MIX_CHANNEL_IN, DEFAULT_VOLUME_IN);
			mixer_R.gain(MIX_CHANNEL_IN, DEFAULT_VOLUME_IN);
		}
		else
		{
			display(0,"disconnecting monitorInput",0);
			c_in_L.disconnect();
			c_in_R.disconnect();
			mixer_L.gain(MIX_CHANNEL_IN, 0.0);
			mixer_R.gain(MIX_CHANNEL_IN, 0.0);
		}
	}
	if (lastMonitorUSB != monitorUSB)
	{
		lastMonitorInput = monitorUSB;
		if (monitorUSB)
		{
			display(0,"connecting monitorUSB",0);
			c_usb_L.connect(usb_in, 0, mixer_L, MIX_CHANNEL_USB);
			c_usb_R.connect(usb_in, 1, mixer_R, MIX_CHANNEL_USB);
			mixer_L.gain(MIX_CHANNEL_USB, DEFAULT_VOLUME_USB);
			mixer_R.gain(MIX_CHANNEL_USB, DEFAULT_VOLUME_USB);
		}
		else
		{
			display(0,"disconnecting monitorUSB",0);
			c_usb_L.disconnect();
			c_usb_R.disconnect();
			mixer_L.gain(MIX_CHANNEL_USB, 0.0);
			mixer_R.gain(MIX_CHANNEL_USB, 0.0);
		}
	}
	if (lastMonitorSine != monitorSine)
	{
		lastMonitorSine = monitorSine;
		if (monitorSine)
		{
			display(0,"connecting monitorSine",0);
			c_sine_L.connect(sine1, 0, mixer_L, MIX_CHANNEL_SINE);
			c_sine_R.connect(sine2, 0, mixer_R, MIX_CHANNEL_SINE);
			mixer_L.gain(MIX_CHANNEL_SINE, DEFAULT_VOLUME_SINE);
			mixer_R.gain(MIX_CHANNEL_SINE, DEFAULT_VOLUME_SINE);
		}
		else
		{
			display(0,"disconnecting monitorSine",0);
			c_sine_L.disconnect();
			c_sine_R.disconnect();
			mixer_L.gain(MIX_CHANNEL_SINE, 0.0);
			mixer_R.gain(MIX_CHANNEL_SINE, 0.0);
		}
	}
}




//=================================================
// setup()
//=================================================

extern "C" {
    extern void my_usb_init();          	// in usb_dev.c
    extern void setFTPDescriptors();    	// _usbNames.c
	extern const char *getUSBSerialNum();	// _usbNames.c
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

	//-------------
	// init usb
	//-------------

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

	if (monitorInput || monitorUSB || monitorSine)
		setNewConnection();

	AudioMemory(100);
	delay(250);

	sgtl5000.enable();
	sgtl5000.setInput(SGTL_INPUT_LINEIN);		// line_in == 0
	sgtl5000.setDefaultGains();

	mixer_L.gain(MIX_CHANNEL_MAIN,	DEFAULT_VOLUME_MAIN);
	mixer_R.gain(MIX_CHANNEL_MAIN,	DEFAULT_VOLUME_MAIN);

	sine1.frequency(FREQ_SINE1);
	sine2.frequency(FREQ_SINE2);


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

	if (SINE_WAVE_TO_USB || monitorSine)
	{
		if (millis() > next_sine_time)
		{
			next_sine_time = millis() + SINE_TOGGLE_TIME;
			sine_toggle = !sine_toggle;
			sine1.amplitude(sine_toggle ? 0.0 : 0.8);
			sine2.amplitude(sine_toggle ? 0.8 : 0.0);
		}
	}

}	// loop()



//==============================================================
// Serial MIDI handler
//==============================================================

#define dbg_sm  0


#define RAW_SGTL5000_CABLE 	(0x0E << 4)
#define knownCable(byte)	(((byte) & RAW_SGTL5000_CABLE) == RAW_SGTL5000_CABLE)
#define SGTL5000_CC			(RAW_SGTL5000_CABLE | MIDI_TYPE_CC)


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
			if (knownCable(byte))
			{
				buf[len++] = byte;
			}
			else
			{
				my_error("TE3_hub: unexpected cable in MIDI byte0(0x%02x)",byte);
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
				else
				{
					my_error("TE3_hub: unexpeced serial midi(0x%08x)",msg32);
				}

				len = 0;
			}
		}
	}

}




