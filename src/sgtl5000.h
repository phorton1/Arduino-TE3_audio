//-------------------------------------------------------------------------
// sgtl5000.h
//-------------------------------------------------------------------------
// Audio Library for Teensy 3.X
// Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
// please see ../LICENSE.TXT.

#pragma once

#include <AudioStream.h>
#include "AudioControl.h"

// SGTL5000-specific defines for headphones
#define AUDIO_HEADPHONE_DAC 0
#define AUDIO_HEADPHONE_LINEIN 1

//For Filter Type: 0 = LPF, 1 = HPF, 2 = BPF, 3 = NOTCH, 4 = PeakingEQ, 5 = LowShelf, 6 = HighShelf
#define FILTER_LOPASS 0
#define FILTER_HIPASS 1
#define FILTER_BANDPASS 2
#define FILTER_NOTCH 3
#define FILTER_PARAEQ 4
#define FILTER_LOSHELF 5
#define FILTER_HISHELF 6

//For frequency adjustment
#define FLAT_FREQUENCY 0
#define PARAMETRIC_EQUALIZER 1
#define TONE_CONTROLS 2
#define GRAPHIC_EQUALIZER 3




class SGTL5000 : public AudioControl
	// Paul depends on the AudioDesignTool to document these methods,
	// but the tool does not include all function calls.
	// The API would be better if the terminology matched the SGTL5000 flowchart.
	// Using single words like "volume()" is misleading.
	// Glomming together flowchart blocks (Mic & Analog Gain) and making
	// assumptions about default gain levels is not helpful.
	// I think I'm gonna rewrite this entire API.
	//
	// FLOWCHART:
	//
	//				 (bypass)
	//					+---------------------------------------------------------------------------------+
	//                  |                                                                                 |--> [HP_VOLUME] --> HP_OUT
	//   LINE_IN -------+--------+                                                                        |
	//                           |---> [ANALOG_GAIN] --> ADC --> [SWITCH] --> [DAC_VOLUME] --> DAC ---+---+
	//   MIC ---> [MIC_GAIN] ----+                                 |  ^                               |
	//															   v  |							      +------> [LINEOUT_VOL] --> LINE_OUT
	//                                                          [DSP BLOCK]


{
public:
	SGTL5000(void) : i2c_addr(0x0A) { }
	void setAddress(uint8_t level);
	bool enable(void);	// OVERRIDE		//For Teensy LC the SGTL acts as master, for all other Teensys as slave.
	bool enable(const unsigned extMCLK, const uint32_t pllFreq = (4096.0l * AUDIO_SAMPLE_RATE_EXACT) ); //With extMCLK > 0, the SGTL acts as Master
	bool disable(void) /* OVERRIDE */ { return false; }

	// This API is unclear, largely because the SGTL500 is very complicated
	// but also because this file is more or less totally uncommented.
	// and it needs to (barely) support an orthogonal base class API.

	bool volume(float n)	// OVERRIDE
	{
			return volumeInteger(n * 129 + 0.499f);
	}
	bool volume(float left, float right);
		// Adjusts the HEADPHONE amplifier from 0.0 to 1.0 as a float
		// Does not change the LineOut levels.
		// The single param version, calls volumeInteger with a number from 0..129
		//		It then does automatic muting of the headphone if the value is zero,
		// 		The values in the CHIP_ANA_HP_CTRL reg are from 0x7f..0x00 for -51.5dB to +12dB
		// on the other the LR version does not call volumeInteger and merely sets the
		//		CHIP_ANA_HP_CTRL to the inverted 0..0x7F directly from the floats.

	bool inputLevel(float n)	// OVERRIDE
	{
		// Note that inputLevel(float) does nothing!
		return false;
	}

	bool muteHeadphone(void) { return write(0x0024, ana_ctrl | (1<<4)); }
	bool unmuteHeadphone(void) { return write(0x0024, ana_ctrl & ~(1<<4)); }
	bool muteLineout(void) { return write(0x0024, ana_ctrl | (1<<8)); }
	bool unmuteLineout(void) { return write(0x0024, ana_ctrl & ~(1<<8)); }
			// These single bit modifiers do what they say.

	bool inputSelect(int n)		// OVERRIDE
		// Paul picks default levels when inputSelect() is called.
		// There is a "Mic Gain" and an "Analog Gain" in the flowchart.
		// For AUDIO_INPUT_LINEIN he sets the Analog gain to arbitrary
		// 		+7.5db (on 31 enum scale from 0..22.5db).
		// For AUDIO_INPUT_MIC, he sets the Mic as loud as possible
		//	    at 40dB of gain, and an aribrary Analog gain of +12db
		// AUDIO_INPUT_MIC also makes assumptions about bias voltage
		//		and impedance.
		// Personally I would prefer that (a) initialization sets all
		// gains to their minimums, (b) selecting an input does not
		// change any gains, and (c) let (the user) decide what
		// the startup defaults should be with appropriate ramp ups
		// in the implementation.
	{
		if (n == AUDIO_INPUT_LINEIN)
		{
			return write(0x0020, 0x055)	// +7.5dB gain (1.3Vp-p full scale)
			 && write(0x0024, ana_ctrl | (1<<2)); // enable linein
		}
		else if (n == AUDIO_INPUT_MIC)
		{
			return write(0x002A, 0x0173) // mic preamp gain = +40dB
				// 0x003 = gain 40DB
				// 0x070 = max 3.0V Mic Bias voltage
				// 0x100 = BIAS Reister 2.0 kOhm
			 && write(0x0020, 0x088)     // input gain +12dB (is this enough?)
			 && write(0x0024, ana_ctrl & ~(1<<2)); // enable mic
		} else {
			return false;
		}
	}

	bool headphoneSelect(int n)
		// The headphone amplifier can be connected to the DAC (normal)
		// or "bypass mode" can be entered that routes the LINE directly
		// to the headphone amp.
	{
		if (n == AUDIO_HEADPHONE_DAC)
		{
			return write(0x0024, ana_ctrl | (1<<6));
				// one line comment does not help: "route DAC to headphones out"
				// This is the normal operating mode for the headphones

		}
		else if (n == AUDIO_HEADPHONE_LINEIN)
		{
			// This is essentially the bypass mode.
			// Line-In can be connected directly to the headphone amplifier

			return write(0x0024, ana_ctrl & ~(1<<6)); // route linein to headphones out
		}
		else
		{
			return false;
		}
	}

	bool micGain(unsigned int dB);	// 0..63
		// This is NOT just the micGain, but a glom of changing
		// both the "Mic Gain" and the "Analog Gain" to
		// achieve an "overall gain" of the "integer dB"
		// from 0..63db, assuming, but not checking, that you
		// have chosen the AUDIO_INPUT_MIC


	bool lineInLevel(uint8_t left, uint8_t right);
	bool lineInLevel(uint8_t n)		// 0..15
		// Sets the "Analog Gain" independent of "Mic Gain".
		// The "Analog Gain" is represented as
		// "senstivity" from 3.12v p-p to 0.26v p-p
	{
		return lineInLevel(n, n);
	}



	unsigned short lineOutLevel(uint8_t n);		// 13..31, backwards, sheesh
	unsigned short lineOutLevel(uint8_t left, uint8_t right);
		// Weird API.
		// BACKWARDS?!?! from 13..31, where apparently
		// 0..12  "have clipping", and cannot be passed in
		// but 13..31 are "appropriate", and
		// 29: 1.29 Volts p-p is the default

	unsigned short dacVolume(float n);	// 0.0-1.0; default 1.0
	unsigned short dacVolume(float left, float right);
		// allows to attenuate (turn down) the digital signal before the DAC
		// the default is no attenuation (what comes into the DAC goes out as a voltage).

	bool dacVolumeRamp();
	bool dacVolumeRampLinear();
		// these return true on success
	bool dacVolumeRampDisable();
		// will return false on success


	unsigned short adcHighPassFilterEnable(void);
	unsigned short adcHighPassFilterFreeze(void);
	unsigned short adcHighPassFilterDisable(void);
		// "don't touch unless you do" ... disabling the High Pass
		// filter may give better bass response, but allow DC noise in.
		// Freezing it would be weird as it is an ongoing process.


	//------------------------------------------
	// DSP Blocks
	//------------------------------------------
	// all this stuff is for the DSP block and gruesomely complicated
	//
	// switch -> MIX --> AVC --> SURROUND --> BASS_ENHANCE --> TONE_CONTROL --> switch
	//           +6db    +12b                 +6db             +12db
	//
	// I don't see any API for the MIX block, nor do I understand it very well)

	unsigned short audioPreProcessorEnable(void);
	unsigned short audioPostProcessorEnable(void);
	unsigned short audioProcessorDisable(void);
		// Paul says Pre is before I2S and Post is after I2S
		// Does the whole thing move?
		// "It is good practice to mute the outputs before enabling or disabling the Audio Processor,
		// to avoid clicks or thumps."

	// AVC
	unsigned short autoVolumeControl(uint8_t maxGain, uint8_t lbiResponse, uint8_t hardLimit, float threshold, float attack, float decay);
		// Configures the auto volume control, which is implemented as a
		// compressor/expander or hard limiter. maxGain is the maximum gain that
		// can be applied for expanding, and can take one of three values:
		// 0 (0dB), 1 (6.0dB) and 2 (12dB). Values greater than 2 are treated as 2.
		// response controls the integration time for the compressor and can take four values:
		// 0 (0ms), 1 (25ms), 2 (50ms) or 3 (100ms).
		// Larger values average the volume over a longer time, allowing short-term peaks through.
		//
		// If hardLimit is 0, a 'soft knee' compressor is used to progressively compress louder
		// values which are near to or above the threashold (the louder they are,
		// the greater the compression). If it is 1, a hard compressor is used
		// (all values above the threashold are the same loudness). The threashold is
		// specified as a float in the range 0dBFS to -96dBFS, where -18dBFS is a typical value.
		// attack is a float controlling the rate of decrease in gain when the signal is over threashold,
		// in dB/s. decay controls how fast gain is restored once the level drops below threashold, again in dB/s.
		// It is typically set to a longer value than attack.
	unsigned short autoVolumeEnable(void);
	unsigned short autoVolumeDisable(void);
		// limiter/compressor stage

	// SURROUND
	unsigned short surroundSound(uint8_t width);
	unsigned short surroundSound(uint8_t width, uint8_t select);
	unsigned short surroundSoundEnable(void);
	unsigned short surroundSoundDisable(void);
		// Configures virtual surround width from 0 (mono) to 7 (widest).
		// select may be set to 1 (disable), 2 (mono input) or 3 (stereo input).

	// BASS_ENHANCE
	unsigned short enhanceBassEnable(void);
	unsigned short enhanceBassDisable(void);
	unsigned short enhanceBass(float lr_lev, float bass_lev);
	unsigned short enhanceBass(float lr_lev, float bass_lev, uint8_t hpf_bypass, uint8_t cutoff);
		// Configures the bass enhancement by setting the levels of the original stereo
		// signal and the bass-enhanced mono level which will be mixed together.
		// The high-pass filter may be enabled (0) or bypassed (1).
		// The cutoff frequency is specified as follows:
		//		value  frequency
		//		0      80Hz
		//		1     100Hz
		//		2     125Hz
		//		3     150Hz
		//		4     175Hz
		//		5     200Hz
		//		6     225Hz


	// TONE_CONTROL
	unsigned short eqSelect(uint8_t n);
		// Selects the type of frequency control, where n is one of
		// FLAT_FREQUENCY (0) Equalizers and tone controls disabled, flat frequency response.
		// PARAMETRIC_EQUALIZER (1) Enables the 7-band parametric equalizer
			void eqFilter(uint8_t filterNum, int *filterParameters);
				// Configurs the parametric equalizer. The number of filters (1 to 7) is
				// specified along with a pointer to an array of filter coefficients.
				// The parametric equalizer is implemented using 7 cascaded, second order
				// bi-quad filters whose frequencies, gain, and Q may be freely configured,
				// but each filter can only be specified as a set of filter coefficients.
			unsigned short eqFilterCount(uint8_t n);
				// Enables zero or more of the already enabled parametric filters.
			void calcBiquad(uint8_t filtertype, float fC, float dB_Gain, float Q, uint32_t quantization_unit, uint32_t fS, int *coef);
				// Helper method to build filter parameters
		// TONE_CONTROLS (2) Enables bass and treble tone controls
			void eqBands(float bass, float treble);
				// When changing bass or treble level, call this function repeatedly to ramp up
				// or down the level in steps of 0.04 (=0.5dB) or so, to avoid pops.
		// GRAPHIC_EQUALIZER (3) Enables the five-band graphic equalizer
			void eqBands(float bass, float mid_bass, float midrange, float mid_treble, float treble);
				// Configures the graphic equalizer. It is implemented by five parallel, second order biquad filters with
				// fixed frequencies of 115Hz, 330Hz, 990Hz, 3kHz, and 9.9kHz. Each band has a range of adjustment
				// from 1.00 (+12dB) to -1.00 (-11.75dB).
			unsigned short eqBand(uint8_t bandNum, float n);
				// Configures the gain or cut on one band in the graphic equalizer.
				// bandnum can range from 1 to 5; n is a float in the range 1.00 to -1.00.
				// When changing a band, call this function repeatedly to ramp up the gain in steps of 0.5dB, to avoid pops.

	// Paul has his own built-in automation system for ramping
	// EQ settings.  It is not generally used for main blocks

	void killAutomation(void) { semi_automated=false; }
	void setMasterMode(uint32_t freqMCLK_in);

protected:

	bool muted;
	bool volumeInteger(unsigned int n); // range: 0x00 to 0x80
	uint16_t ana_ctrl;
	uint8_t i2c_addr;
	unsigned char calcVol(float n, unsigned char range);
	unsigned int read(unsigned int reg);
	bool write(unsigned int reg, unsigned int val);
	unsigned int modify(unsigned int reg, unsigned int val, unsigned int iMask);
		// returns the val if read/write() succeeds
		// hence, sigh, dacVolumeRampDisable(), returns false if it succeeds.
	unsigned short dap_audio_eq_band(uint8_t bandNum, float n);
		// dunno
private:
	bool semi_automated;
	void automate(uint8_t dap, uint8_t eq);
	void automate(uint8_t dap, uint8_t eq, uint8_t filterCount);
};



