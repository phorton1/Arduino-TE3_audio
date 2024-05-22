#pragma once

#define WITH_MIDI_HOST  0
    // The MIDI host can be compiled out if I'm paranoid
    // about something not working, but otherwise, there's
    // no reason to ever set this define to zero.

#if WITH_MIDI_HOST

    #include <Arduino.h>
    #include <USBHost_t36.h>

    class midiHost : public MIDIDevice
        // requires slightly modified USBHost_t36.h
    {
        public:

            midiHost();
            void init();
            virtual void rx_data(const Transfer_t *transfer);

    };


    extern midiHost midi_host;

#endif

