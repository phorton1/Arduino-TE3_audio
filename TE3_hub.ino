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


#define USB_SERIAL_PORT		Serial
#define MIDI_SERIAL_PORT	Serial1
#define DEBUG_SERIAL_PORT	Serial3
#define USE_DEBUG_PORT		MIDI_SERIAL_PORT


#define WITH_SERIAL3	1
	// must set this to use DEBUG_SERIAL_PORT for debugging

#define TRIGGER_PIN		0		// 13
	// set this to a pin to trigger logic analyzer
#define FLASH_PIN		13
	// set this to a pin to flash during loop

#define TONE_SWEEP_TEST	0
	// set to 1 to do simple autdio tone out test
#define WITH_USB_AUDIO	1
	// set to 1 to route audio from SGTL through the USB Audio IO

#define WITH_MIDI_HOST	1
	// set to 1 to include my midiHost class


#if WITH_MIDI_HOST
	#include "midiHost.h"
#endif



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
		AudioInputUSB       usb_in;
		AudioOutputUSB      usb_out;

		AudioConnection     c1(i2s_in, 0, usb_out, 0);
		AudioConnection     c2(i2s_in, 1, usb_out, 1);
		AudioConnection     c7(usb_in, 0, i2s_out, 0);
		AudioConnection     c8(usb_in, 1, i2s_out, 1);

	#else

		AudioConnection     c1(i2s_in, 0, i2s_out, 0);
		AudioConnection     c2(i2s_in, 1, i2s_out, 1);

	#endif

	// AudioMixer4			mixer_l;
	// AudioMixer4			mixer_r;

#endif


AudioControlSGTL5000 sgtl5000;



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

	sgtl5000.enable();
	sgtl5000.inputSelect(AUDIO_INPUT_LINEIN);
	sgtl5000.lineInLevel(4);

	sgtl5000.unmuteLineout();
	sgtl5000.unmuteHeadphone();
	sgtl5000.volume(0.8); 		//headphone volume

	// mixer_l.gain(0,0.90);
	// mixer_r.gain(0,0.90);
	// mixer_l.gain(1,0.70);
	// mixer_r.gain(1,0.70);


	#if WITH_MIDI_HOST
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
    if (now > flash_last + 250)
    {
        flash_last = now;
        flash_on = !flash_on;
        digitalWrite(FLASH_PIN,flash_on);

		#if 1
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
}



