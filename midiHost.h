#pragma once

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

    
