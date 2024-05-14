//-------------------------------------------------------------------------
// mySGTL5000.h
//-------------------------------------------------------------------------
// Audio Library for Teensy 3.X
// Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
// please see ../LICENSE.TXT.
//
// This API has been reworked for clarity and control by MIDI.
// It is envisioned that client code will have some way of persistently
// storing the SGTL5000 "configuration" and will initialize all gains,
// senstivities, filters, and DSP blocks for the given application.

#pragma once

#include <AudioStream.h>
#include "AudioControl.h"


#define HEADPHONE_NORMAL	0	// normal mode
#define HEADPHONE_LINEIN	1	// bypass mode
	// param to headphoneSelect()

#define DAC_VOLUME_RAMP_NORMAL  	0
#define DAC_VOLUME_RAMP_LINEAR  	1
#define DAC_VOLUME_RAMP_DISABLE  	2
	// param to dacVolumeRamp()

#define ADC_HIGH_PASS_ENABLE		0
#define ADC_HIGH_PASS_FREEZE		1
#define ADC_HIGH_PASS_DISABLE		2
	// param to adcHighPassFilter()

#define DAP_DISABLE					0
#define DAP_ENABLE_PRE				1
#define DAP_ENABLE_POST				2
	// param to dspEnable()

#define EQ_FLAT 					0
#define EQ_PARAM_EQ 				1
#define EQ_TONE_BASS_AND_TREBLE		2
#define EQ_GRAPHIC_EQ_5CH			3
	// param to eqSelect()

#define FILTER_LOPASS 				0
#define FILTER_HIPASS 				1
#define FILTER_BANDPASS 			2
#define FILTER_NOTCH 				3
#define FILTER_PARAEQ 				4
#define FILTER_LOSHELF 				5
#define FILTER_HISHELF 				6
	// params to PEQ method





class mySGTL5000 : public AudioControl
	// Client may call setDefaultGains() for a quick seetup of reasonable initial values.
	// Otherwise, client may call the the methods associated with the [bracketed] blocks.
	//
	//				 (bypass)
	//					+----------------------------------------------------------------------------------+
	//                  |                                                                                  |--> [HP_VOLUME] --> HP_OUT
	//   LINE_IN -------+--------+                                                                         |
	//                           |---> [LINEIN_LEVEL] --> ADC --> [SWITCH] --> [DAC_VOLUME] --> DAC ---+---+
	//   MIC ---> [MIC_GAIN] ----+      (ANALOG_GAIN)               |  ^                               |
	//												 			    v  |							   +------> [LINEOUT_VOL] --> LINE_OUT
	//                                                           [DSP BLOCK]
{
public:

	mySGTL5000(void) : i2c_addr(0x0A) { }
	void setAddress(uint8_t level);

	bool enable(void) override;
	bool enable(const unsigned extMCLK, const uint32_t pllFreq = (4096.0l * AUDIO_SAMPLE_RATE_EXACT) );
		// sets lineOutLevel(2) = "approx 1.3v p-p"  on my scale

	// unimplmented orthogonal base class control API

	bool disable(void) override 		{ return false; }
	bool inputLevel(float n) override 	{ return false; }
	bool volume(float n) override 		{ return false; }
	bool inputSelect(int n) override 	{ return false; };

	// additional sensible API
	// returning bools on these methods is not properly implemented
	// as they actually return a register value cast to a bool which
	// in some cases may be zero.  I think they all need to be changed
	// to "functional booleans".  Also note that we (probably) need
	// "getters" for all of these to support a full MIDI API.

	bool setDefaultGains();
		// Unmutes everything, turns off all DSP blocks and sets all
		// senstivities and gains to reasonable levels so that you are
		// likely to hear something for testing.
		//
		// micGain(3) 			= 40db (maximum)
		// lineInLevel(5) 		= 7.5db (1.33 Volts p-p measured)
		// headphoneVolume(90) 	= -3.5db


	//----------------------------------------------------
	// control API from left to right
	//----------------------------------------------------
	// These will be represented by incrementing Midi CC numbers,
	// and all will take uint8_t parameters in the range 0..127.

	bool setInput(uint8_t val);
		// Uses teeensy audio system constants.
		//		0 = AUDIO_INPUT_LINEIN
		//		1 = AUDIO_INPUT_MIC
		// Does not set or change any gains or other characteristics

	bool micGain(uint8_t val);	// 0..3
		// 0 = 0db
		// 1 = 20db
		// 2 = 30db
		// 3 = 40db

	bool lineInLevel(uint8_t left, uint8_t right);	// 0..15
	bool lineInLevel(uint8_t n)	 { return lineInLevel(n, n); }
		// Sets the ANALOG_GAIN independent of the MIC_GAIN.
		// in 1.5db steps from 0 to 22.5db. See implementation
		// for table of measured p-p voltages. Note the register
		// supports a 6db attenuation bit to make it go from
		// -6.0db to 16.5db if needed.

	unsigned short dacVolume(uint8_t val);	// 0..127 INVERTED!
	unsigned short dacVolume(uint8_t left, uint8_t right);
		// ATTENUATE (turn down) the digital signal before the DAC
		// in 0.5db steps, from 0 to -63db, or mute it entirely.
		//
		// 		0 = 0 db (no attentuation)
		// 		1 = -0.5 db
		// 		2 = -1.0 db
		//		...
		//		125 = -62.5db
		//		126 = -63db
		//		127 = mute
		//
		// Note that to accomodate the 0..127 midi CC value range, we do not
		// access the full SGTL5000 range of -90db available for this registers.
		// IMO it is unlikely anyone will ever want to turn the DAC down by more than 63db.
		// This is set to default of 0 on chip reset.
	bool dacVolumeRamp(uint8_t val);	// 0..2
		// control ramping of dac
		// 0 = normal, 1=linear, 2=disabled

	unsigned short lineOutLevel(uint8_t val);		// 0..18 (see note)
	unsigned short lineOutLevel(uint8_t left, uint8_t right);
		// Sets the LINE_OUT volume in 0.5db steps
		//		from some arbitrary starting point.
		// This register is complicated.
		// At some point Paul measured the device and created a useful scale.
		// Paul's original uint8_t API had wacky allowed values of 13..31,
		//  	and was was, I believe, "backwards" where higher
		//      numbers result in less p-p output voltage with 13=3.3V p-p
		//		and 31=1.66V p-p
		// I use his scale, but with, I believe, with the  "correct"" sense
		//		where bigger numbers result in bigger output p-p voltages.
		// On my scale, 0=1.66v p-p and 18=3.16v p-p
		// On my scale enable() defaults to lineOutLevel(2) == 1.29v p-p

	bool headphoneSelect(uint8_t val);
		// The headphone amplifier can be connected to the DAC (HEADPHONE_NORMAL)
		// or "bypass mode" (HEADPHONE_LINEIN) that routes the LINE_IN directly
		// to the headphone amp.

	bool headphoneVolume(uint8_t val);	// 0..127; mutes headphones at volume 0
	bool headphoneVolume(uint8_t left, uint8_t right);
		// Adjusts the HEADPHONE amplifier from -51.5 db to +12db
		// in 0.5db steps. headPhoneVolume(97) = 0db
		// Does not change the LineOut levels.

	bool muteHeadphone(uint8_t mute);
	bool muteLineout(uint8_t mute);
		// These single bit modifiers do what they say.

	unsigned short adcHighPassFilter(uint8_t val);	// 0..2
		// oddball API.
		// 		0 = ADC_HIGH_PASS_ENABLE
		// 		1 = ADC_HIGH_PASS_FREEZE
		// 		2 = ADC_HIGH_PASS_DISABLE
		// Disabling the High Pass filter may give better bass response,
		// but allow DC noise in. Freezing it would be weird as it is an
		// ongoing process.



	//------------------------------------------
	// DAP Blocks
	//------------------------------------------
	// all this stuff is for the DAP block and gruesomely complicated
	//
	// switch -> MIX --> AVC --> SURROUND --> BASS_ENHANCE --> TONE_CONTROL --> switch
	//           +6db    +12b                 +6db             +12db
	//
	// The MIX is generally disabled in this implementation.

	unsigned short dapEnable(uint8_t val);
		// Disable the DAP, or enable it pre or post I2S
		// 		0 = DAP_DISABLE
		// 		1 = DAP_ENABLE_PRE before I2S_OUT
		// 		2 = DAP_ENABLE_POST after I2S_IN
		// "It is good practice to mute the outputs before enabling or disabling the Audio Processor,
		// to avoid clicks or thumps."



	// prh - API below here needs work
	//
	// Above everything scrunched into 0..17 midi uint8_t params.
	// Below we start running into problems where there are full 16 bit registers,
	// 		and the API uses floats extensively.
	// I may not be able to sensibly represent all blocks as midi parameterized devices.
	// I will probably need to figuire out some common use cases that CAN be represented
	// 		by 7bit midi values and implement those in terms of the below API's.

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
		// prh note: mid for just this one?
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
	// prh - I can probably midify the bass/trebl TONE and GRAPHIC EQ to
	//	   midi with a value of 63 for flat and some appropriate range
	//     for the gain/cut.

	unsigned short eqSelect(uint8_t n);	// 0..3
		// Selects the type of frequency control, 
		// FLAT_FREQUENCY (0) Equalizers and tone controls disabled, flat frequency response.
		// PARAMETRIC_EQUALIZER (1) Enables the 7-band parametric equalizer
			void eqFilter(uint8_t filterNum, int *filterParameters);
				// Configurs the parametric equalizer. The number of filters (1 to 7) is
				// specified along with a pointer to an array of filter coefficients.
				// The parametric equalizer is implemented using 7 cascaded, second order
				// bi-quad filters whose frequencies, gain, and Q may be freely configured,
				// but each filter can only be specified as a set of filter coefficients.
			unsigned short eqFilterCount(uint8_t n);
				// Enables zero or more of the already configured parametric filters.
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

	// utilties

	void killAutomation(void) { semi_automated=false; }
	void setMasterMode(uint32_t freqMCLK_in);

protected:

	bool muted;
	uint16_t ana_ctrl;
	uint8_t i2c_addr;

	unsigned int read(unsigned int reg);
	bool write(unsigned int reg, unsigned int val);
	unsigned int modify(unsigned int reg, unsigned int val, unsigned int iMask);
		// returns the val if read/write() succeeds
		// hence, sigh, dacVolumeRampDisable(), returns false if it succeeds.

	// utilities

	unsigned short dap_audio_eq_band(uint8_t bandNum, float n);
	unsigned char calcVol(float n, unsigned char range);

private:
	bool semi_automated;
	void automate(uint8_t dap, uint8_t eq);
	void automate(uint8_t dap, uint8_t eq, uint8_t filterCount);
};



