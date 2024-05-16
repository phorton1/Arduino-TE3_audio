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
#include "src/midiHost.h"


#define USB_SERIAL_PORT		Serial
#define MIDI_SERIAL_PORT	Serial1
#define DEBUG_SERIAL_PORT	Serial3
#define USE_DEBUG_PORT		USB_SERIAL_PORT


#define WITH_SERIAL3	0
	// must set this to use DEBUG_SERIAL_PORT for debugging

#define TRIGGER_PIN		0		// 13
	// set this to a pin to trigger logic analyzer
#define FLASH_PIN		13
	// set this to a pin to flash during loop

#define TONE_SWEEP_TEST	0
	// set to 1 to do simple autdio tone out test
#define WITH_USB_AUDIO	0
	// set to 1 to route audio from SGTL through the USB Audio IO
#define WITH_MIXERS		0
	// allows to monitor I2S_IN and mix it with USB_IN to I2S_OUT






#if TONE_SWEEP_TEST

	AudioSynthToneSweep tone_sweep;
	AudioOutputI2S    	i2s_out;
	AudioConnection 	c1(tone_sweep, 0, i2s_out, 0);
	AudioConnection 	c2(tone_sweep, 0, i2s_out, 1);

	float t_ampx = 0.8;
	int t_lox = 10;
	int t_hix = 4000;
	float t_timex = 10;

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

	#endif


#endif


SGTL5000 sgtl5000;



void setup()
{
	Serial.begin(115200);
	delay(100);
	Serial1.begin(115200);
	delay(100);
	#if WITH_SERIAL3
		Serial3.begin(115200);
		delay(100);
	#endif

	setColorString(COLOR_CONST_DEFAULT, "\033[94m");	// bright blue
        // TE3_hub's normal display color is bright blue
        // TE3's normal (default) display color is green
        // Looper's normal display color, is cyan, I think
	dbgSerial = &USE_DEBUG_PORT;

	delay(250);
	display(0,"TE3_hub.ino setup() started",0);

	pinMode(FLASH_PIN,OUTPUT);

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
	
	#if 1
		sgtl5000.setDefaultGains();
	#else
		sgtl5000.dispatchCC(SGTL_CC_SET_DEFAULT_GAINS,0);
	#endif

	// try various DAP effects

	#if 1
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

	
	sgtl5000.dumpCCValues("after my init");

	#if WITH_USB_AUDIO
		#if WITH_MIXERS
			mixer_l.gain(0, 1.0);
			mixer_l.gain(1, 1.0);
			mixer_r.gain(0, 1.0);
			mixer_r.gain(1, 1.0);
		#endif
	#endif
	

	#if WITH_MIDI_HOST
		// defined in midiHost.h
		display(0,"initilizing midiHost",0);
		midi_host.init();
	#endif

	display(0,"TE3_hub.ino setup() finished",0);

}



void loop()
{
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
			// Serial1.print("counter=");
			// Serial1.println(counter);

			#if 1
				// send an arbitray midi message

				uint32_t val = counter % 128;
				val <<= 24;
				uint32_t msg = 0x0022b70b | val;
				usb_midi_write_packed(msg);
			#endif

		#endif
    }


	#if TONE_SWEEP_TEST
		if (!tone_sweep.play(t_ampx,t_lox,t_hix,t_timex))
		{
			my_error("tone_sweep.play() failed",0);
			delay(10000);
			return;
		}
		while (tone_sweep.isPlaying());
	#endif

	#if 0	// vestigal code to set SGTL5000 volume from USB device
		float vol = usb_in.volume();  		// read PC volume setting
		display(0,"USB VOL=(%0.3f)",vol);
		sgtl5000.volume(vol); 				// set headphone volume
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


	//------------------------------------------------------
	// a lack of any delay here may cause problems
	//------------------------------------------------------
	// for USB and rPi Audio processing?

	delay(50);

	// handle SGTL5000 eq automation

	sgtl5000.loop();
}



