# audio_bug.md

On initial audio test there was mostly success.

- If MIX_IN is set to 100, you hear the guitar.
- If the iPad is hooked up, AudioBus and ToneStack are started
  and MIX_USB is set to 100 (MIX_IN set to zero), then you
  hear the guitar effects from ToneStack.
- The looper is hearing the USB_IN-->i2sQuad(3,4) output
  as evidenced by its VU meters, and appears to be recording
  correctly, which is good because it means the pin soldered
  to the bottom of the teensy4.0 is working.

However, if MIX_LOOP is set to 100 (IN and USB set to zero), we
don't hear the sound from the Looper.

The rotaries are not working consistently.


## FIX ROUND1

Discovered a carnival of errors on the initial printed PCB
Added DEBUG_AUDIO_LEVELS define to TE3_audio.ino to see the
levels every 250ms (had to increase console.pm ctrl-E window
to upload kernels over the serial, not usb, cable).

- Had the I2S_RXB from the rPI_I2sTX going to the wrong pin
  on the RevD audio shield. It was going to gpio5 (abs pin 7)
  and should have been going to gpio6 (abs pin 8).
  Once I corrected this, I started getting sound from the
  Looper, although only on the Left (i2s_in(2)) channel.
  The problem persists.
- Completely fucked up with the audio connector, had a bad
  kicad symbol from looking at the boards upside down, I was
  putting the audiio GND between the LINE_IN pins, and not
  between the LINE_OUT_PINS!! However, even after resoldering
  this it did not help the situation with the Looper (bcm_pcm
  teensyQuad inputs(0,1)) getting only one of the two channels.

I now believe there is something wrong with the LRCLK (FCLK)
in the bcm_pcm.  I noticed that the CHANNEL_WIDTH in
output_teensy_quad.cpp was set to 15, when 16 would be
more logical, tried changing that, to no avail.

It is (hopefully) a bad trace from the RevD board to the
rPI on the LRCLK (FCLK) pin.  If not, it is quickly going
to get exceedingly complicated trying to figure out what's
wrong with the bcm_pcm, probably requiring a logic analyzer
and many hours of dicking around.

I've already taken the motherboard out, and modified it,
twice, today, to try to get the audio completely working.
It's now 4:30pm (started at 7:30am) and I think I may need
a break.

One thing I can say for sure.  I'm glad I have the ability to mill
and test these PCB's before committing 3-5 weeks to getting another
round from china.  My traces are big and fat and I have no problem
modifying the shit out of the PCB using a mini-drill-grinder bit and
exacto knife.

I added a TEST_RPI_WITHOUT_IPAD define to TE3_audio.ino so that
the i2s(0,1) inputs will be sent directly to the looper, as well
as a MONO_GUITAR_INPUT define that uses i2s(0) for both sides
wherever needed.

If I'm gonna uss the logic analyzer, I feel a need to implement
a LOOPER command to set an arbitrary TRIGGER_PIN low/high to
use the logic_analyzer, though I might be able to get away
with the BCLK pin with a big enough window.


## Results of hooking up the logic analyzer

After adding a TEST_CONFIGURATION define to TE3_audio.ino that
generates a sine wave and sends it directly to the Looper,
bypassing the need for the iPad and USB audio io to AudioBus,
I hooked up the logic analyzer and quickly realized that
the "channel width" of the i2s data is 32, not 16 bits, and
that, after setting that in the call to bcm_pcm.static_init()
in output_teensyQuad.cpp I started getting data (and presumably
sound) into and out of the 2nd Looper channel.


Much clarity has been achieved.


(a) I suspect that my implementation of SGTL5000 is setting
	the channel width to 32 whereas Paul's may set it to 16,
	and perhaps the '15' parameter to bcm_pcm.static_init()
	was correct.

(b) I think the idea that the Looper audio.cpp currently
    has many defines for various audio configurations is
	misleading.  The CS42448 is used in Looper1/2, the
	WM8731 *might* be used in a pinch, but otherwise, there
	is ONLY TE3.

(c) I think _prh/audio/input|output_teensyQuad should be
	renamed and/or perhaps eliminated.  It *might* be
	feasable (and better) to use the "standard" i2s devices,
	and call bcm_pcm.static_init() and bcm_pcm.start() DIRECTLY
	from Looper::audio.cpp than to imply that there is a
	meaningful pair of devices called a teensyQuad.

	At best, if teesyQuad devices continue to exist in _prh/audio,
	then they should actually be paired with the SGTL5000 control
	device and turn the (dupont wire connected RevB or RevD) audioshield
	into a "true" i2s_quad device ALA how Paul does it with his
	SGTL5000 device.

(d) I think I need to review my SGTL5000 implementation in some
	detail to see if it is the "culprit" in this issue.

I am now going to put the box back together, complete the initial
audio testing (hopefully) and check everything in before attempting
to clean up any of this code.







