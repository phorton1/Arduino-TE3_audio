//-------------------------------------------------------
// TE3_hub.ino
//-------------------------------------------------------
// USB Serial, Audio, and MIDI device with USB Host
// Does not yet interface to rPi via I2S.
//
// Serial Ports
//
//		Serial = USB Serial Port	(and/or debug output)
//		Serial1 = Serial Midi Port 	(and/or debug output)
//		Serial3 = Debug Serial Port

#include <Audio.h>
#include <Wire.h>
#include <myDebug.h>
#include <teSGTL5000.h>
#include <teMidi.h>
#include "src/midiHost.h"


#define USB_SERIAL_PORT			Serial
#define MIDI_SERIAL_PORT		Serial1
#define ALT_SERIAL_PORT			0	// Serial3


#define DEBUG_TO_USB_SERIAL		0
#define DEBUG_TO_MIDI_SERIAL	1
#define DEBUG_TO_ALT_SERIAL		2

#define HOW_DEBUG_OUTPUT		DEBUG_TO_MIDI_SERIAL



#define USE_DEBUG_PORT			USB_SERIAL_PORT
#define USE_SERIAL_MIDI_PORT	MIDI_SERIAL_PORT


#define SPOOF_FTP  0
	// requires WITH_MIDI_HOST in midiHost.h
	// however, initial tests reveal that an active midiHost
	// causes severely noticable buzz on the teensy audio shield,
	// which I *doubt* is fixable. Might be important enough to
	// try various methods of isolating the SGTL5000, but more
	// important are basic audio tests with the guitar, USB Audio
	// and the Looper, so this issue is on hold.
	//
	// Current plan is to go back to a teensy 4.1 for TE3, and
	// try to ram the HOST via Serial Midi to the TE3_hub fast,
	// and quiet, enough for usage.




#define TRIGGER_PIN		0		// 13
	// set this to a pin to trigger logic analyzer
#define FLASH_PIN		13
	// set this to a pin to flash during loop



//-------------------------------------------
// audio system setup
//-------------------------------------------

#define TONE_SWEEP_TEST	0
	// set to 1 to do simple autdio tone out test
#define WITH_USB_AUDIO	0
	// set to 1 to route audio from SGTL through the USB Audio IO
#define WITH_MIXERS		0
	// allows to monitor I2S_IN and mix it with USB_IN to I2S_OUT


SGTL5000 sgtl5000;


#if TONE_SWEEP_TEST

	AudioSynthToneSweep tone_sweep;
	AudioOutputI2S    	i2s_out;
	AudioConnection 	c1(tone_sweep, 0, i2s_out, 0);
	AudioConnection 	c2(tone_sweep, 0, i2s_out, 1);

	float t_ampx = 0.8;
	int t_lox = 10;
	int t_hix = 4000;
	float t_timex = 10;

	void toneSweepTest()
	{
		if (!tone_sweep.play(t_ampx,t_lox,t_hix,t_timex))
		{
			my_error("tone_sweep.play() failed",0);
			delay(10000);
			return;
		}
		while (tone_sweep.isPlaying());
	}

#else

	AudioInputI2S       i2s_in;
	AudioOutputI2S      i2s_out;

	#if WITH_USB_AUDIO

		// WITH_USB_AUDO has an option of having mixers
		// the default is to route I2S_IN->USB_OUT .. USB_IN->IS2_OUT
		// but if the teensy is plugged into the laptop, no sound will
		// be heard because there is no connection between USB_OUT
		// and USB_IN on the laptop.
		//
		// Therefore, the option is to have a set of mixers that
		// mix allow the ability to "monitor" the I2S_IN directly
		// to I2S_OUT, and to mix whatever is coming over the USB_OUT
		// in with that.

		AudioInputUSB       usb_in;
		AudioOutputUSB      usb_out;

		#if WITH_MIXERS

			AudioMixer4			mixer_l;
			AudioMixer4			mixer_r;

			AudioConnection     c1(i2s_in, 	0, usb_out, 0);
			AudioConnection     c2(i2s_in, 	0, mixer_l, 0);
			AudioConnection     c3(usb_in, 	0, mixer_l, 1);
			AudioConnection     c4(mixer_l, 0, i2s_out, 0);

			AudioConnection     c5(i2s_in, 	1, usb_out, 1);
			AudioConnection     c6(i2s_in, 	1, mixer_r, 0);
			AudioConnection     c7(usb_in, 	1, mixer_r, 1);
			AudioConnection     c8(mixer_l, 0, i2s_out, 1);

		#else
			AudioConnection     c1(i2s_in, 0, usb_out, 0);
			AudioConnection     c2(i2s_in, 1, usb_out, 1);
			AudioConnection     c7(usb_in, 0, i2s_out, 0);
			AudioConnection     c8(usb_in, 1, i2s_out, 1);
		#endif

	#else

		// the default routing has no mixers
		// and just connects I2S_IN->I2S_OUT

		AudioConnection     c1(i2s_in, 0, i2s_out, 0);
		AudioConnection     c2(i2s_in, 1, i2s_out, 1);

	#endif	// !WITH_USB_AUDIO

#endif	// !TONE_SWEEP_TEST


void audioExperiments()
{
	// Chain from left to right

	#if 0
		sgtl5000.setHeadphoneSelect(0);
			// bypass routes LINE_IN directly to HP_OUT
			// without going through the LINE_IN (ANALOG_IN) amplifier
			// and is not affected by any other settings except
			// setHeadphoneVolume() and setMuteHeadphone().
			// This even includes setInput(), which is ignored
			// in bypass mode.
	#endif

	#if 0
		sgtl5000.setInput(SGTL_INPUT_MIC);		// mic == 1
			// The input selection is the first thing in the
			// chain.  I have not tested the mic input at this
			// point.
		sgtl5000.setMicGain(1);		// 0..3=max 40db
			// even without anything connected I get definite
			// noise at 40db max gain.
	#endif

	#if 0	// these are now the defaults in setDefaultGains()

		sgtl5000.setLineInLevel(7);		// max 15
			// I definitely have the sense correct.
			// 7 or 8 seem like good working defaults
		sgtl5000.setHeadphoneVolume(97);	// max 127
			// Note that if we mute before setHeadphoneVolume(>0),
			// it will turn off the mute.  Driving the little
			// desktop speakers is not the best ultimate test,
			// but at full volume on those, setHeadphoneVolume(<=80)
			// seems to be necessary.  With them turned down, a
			// default volume of 97 seems reasonable.
		sgtl5000.setLineOutLevel(13);	// 0..18 (test max 31)
			// I have the sense correct.
			// It does not affect the sound as much as I expected.
			// I grabbed 13 out of a hat.
		sgtl5000.setMuteLineOut(0);
		sgtl5000.setMuteHeadphone(0);
			// Once again, setMuteHeadphone(1) would need
			// to be called AFTER setHeadphoneVolume(>=0).
	#endif
	
	// try various DAP effects

	#if 0
		sgtl5000.setMuteHeadphone(1);
		sgtl5000.setDapEnable(DAP_ENABLE_POST);

		#if 1
			sgtl5000.setSurroundEnable(SURROUND_STEREO);
			sgtl5000.setSurroundWidth(5);
		#endif

		#if 0
			// I'm not sure how this works
			sgtl5000.setEnableBassEnhance(1);			// 0..1
			sgtl5000.setEnableBassEnhanceCutoff(0);		// 0..1
			sgtl5000.setBassEnhanceCutoff(3);			// 0..6
				// 0 =  80Hz
				// 1 = 100Hz
				// 2 = 125Hz
				// 3 = 150Hz
				// 4 = 175Hz
				// 5 = 200Hz
				// 6 = 225Hz
			sgtl5000.setBassEnhanceBoost(0x40);			// 0..0x7f
				// sets amount of harmonics boost.
				// default = 0x60
			sgtl5000.setBassEnhanceVolume(50);		// 0..0x3f
				// default = 58 on my scale
				// blows out speakers
		#endif

		#if 0
			sgtl5000.setEqSelect(EQ_GRAPHIC_EQ_5CH);

			// 47 (0x2f) = 0 db; max=95 (0x5F)

			sgtl5000.setEqBand(EQ_BASS_BAND0,   60);
			sgtl5000.setEqBand(EQ_BAND1,        47);
			sgtl5000.setEqBand(EQ_BAND1,        47);
			sgtl5000.setEqBand(EQ_BAND1,        47);
			sgtl5000.setEqBand(EQ_TREBLE_BAND4, 42);
		#endif


		sgtl5000.setMuteHeadphone(0);
	#endif

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
	// initialize hardware serial ports
	//-----------------------------------------
	// most important being the MIDI_SERIAL_PORT

	setColorString(COLOR_CONST_DEFAULT, "\033[94m");	// bright blue
        // TE3_hub's normal display color is bright blue
        // TE3's normal (default) display color is green
        // Looper's normal display color, is cyan, I think

	MIDI_SERIAL_PORT.begin(115200);		// Serial1
	delay(100);
	#if HOW_DEBUG_OUTPUT == DEBUG_TO_MIDI_SERIAL
		dbgSerial = &MIDI_SERIAL_PORT;
		display(0,"TE3_hub.ino setup() started on MIDI_SERIAL_PORT",0);
	#endif

	#if ALT_SERIAL_PORT
		ALT_SERIAL_PORT.begin(115200);	// Serial3
		delay(100);
		#if HOW_DEBUG_OUTPUT == DEBUG_TO_ALT_SERIAL
			dbgSerial = &ALT_SERIAL_PORT;
			display(0,"TE3_hub.ino setup() started on ALT_SERIAL_PORT",0);
		#endif
	#endif


	//-------------
	// init usb
	//-------------

	#if SPOOF_FTP
		setFTPDescriptors();
    #endif

	my_usb_init();
    delay(500);

	#if FLASH_PIN
		digitalWrite(FLASH_PIN,0);
	#endif

	//---------------------------------
	// initialize USB_SERIAL_PORT
	//---------------------------------

	USB_SERIAL_PORT.begin(115200);		// Serial.begin()
	delay(100);
	#if HOW_DEBUG_OUTPUT == DEBUG_TO_ALT_SERIAL
		display(0,"TE3_hub.ino setup() started on USB_SERIAL_PORT",0);
	#endif


	//-----------------------------------
	// initialize the audio system
	//-----------------------------------

	delay(250);
	display(0,"initializing audio system",0);

	#if TRIGGER_PIN
		pinMode(TRIGGER_PIN,OUTPUT);
		digitalWrite(TRIGGER_PIN,0);
	#endif

	AudioMemory(100);
	delay(250);

	#if TRIGGER_PIN
		digitalWrite(TRIGGER_PIN,1);
	#endif

	// Note: the artisan Pifi seems to put out about the same signal
	// via either the HP jack or the line out plugs.

	sgtl5000.enable();
	sgtl5000.setInput(SGTL_INPUT_LINEIN);		// line_in == 0
	// audioExperiments();

	#if 1
		sgtl5000.setDefaultGains();
		sgtl5000.dumpCCValues("after setDefaultGains()");
	#endif


	#if WITH_USB_AUDIO
		#if WITH_MIXERS
			mixer_l.gain(0, 1.0);
			mixer_l.gain(1, 1.0);
			mixer_r.gain(0, 1.0);
			mixer_r.gain(1, 1.0);
		#endif
	#endif


	//--------------------------------------
	// midiHost
	//--------------------------------------

	#if WITH_MIDI_HOST
		// defined in midiHost.h
		display(0,"initilizing midiHost",0);
		midi_host.init();
	#endif

	// setup finished

	#if FLASH_PIN
		digitalWrite(FLASH_PIN,1);
	#endif

	display(0,"TE3_hub.ino setup() finished",0);

}




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


	#if TONE_SWEEP_TEST
		toneSweepTest();
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


	//--------------------------------------------------------
	// MIDI HOST processing outside of the rx_data() irq
	//--------------------------------------------------------
	// None of this is needed.
	//
	//	myusb.Task();  // does not seem to be needed
	//	uint32_t msg = midi_host.myRead();
	// 	if (midi_host.read())
	//		display(0,"HOST(%02x,%02x,%02x)",midi_host.getType(), midi_host.getData1(), midi_host.getData2());

	#if WITH_MIDI_HOST && SPOOF_FTP
		uint32_t msg32 = usb_midi_read_message();  // read from device
		if (msg32)
		{
			// display(0,"midi(0x%08x",msg32);
			midi_host.write_packed(msg32);
		}
	#endif


	//------------------------------------------------------
	// Normal Processing
	//------------------------------------------------------

	#if 0
		delay(50);
	#endif

	handleSerialMidi();

	#if 0
		// handle SGTL5000 eq automation
		sgtl5000.loop();
	#endif

}


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




