//-----------------------------------------
// midiHost.cpp
//-----------------------------------------
// simple one uni-directional midi host
// forwards everything from the HOST port
// to the 1st USB midi port

#include "midiHost.h"
#include <myDebug.h>
// #include "midiQueue.h"
// #include "theSystem.h"

#if WITH_MIDI_HOST

    USBHost myusb;
    midiHost midi_host;


    midiHost::midiHost() : MIDIDevice(myusb) {}

    void midiHost::init()
    {
        myusb.begin();
    }



    void midiHost::rx_data(const Transfer_t *transfer)
        // made virtual in USBHost_t36.h
    {
        uint32_t len = (transfer->length - ((transfer->qtd.token >> 16) & 0x7FFF)) >> 2;
        if (len)
        {
            bool any = 0;
            for (uint32_t i=0; i < len; i++)
            {
                uint32_t msg32 = rx_buffer[i];
                if (msg32)
                {
                    any = 1;
                    usb_midi_write_packed(msg32);

                    #if DEBUG_MIDI_HOST
                        // debugging withink the irq DEFINITELY mucks things up
                            display(0,"host:  0x%08x",msg32);
                    #endif

                    // Not implemented yet

                    // port comes in as 0x00 or 0x10
                    // we bump it to MIDI_PORT_HOST1 = 0x40 or
                    // MIDI_PORT_HOST2 = 0x50;

                    // enqueueMidi(false, MIDI_PORT_HOST1 | (msg32 & MIDI_PORT_NUM_MASK), msg32);

                }
            }

            if (any)
                usb_midi_flush_output();
        }

        queue_Data_Transfer(rxpipe, rx_buffer, rx_size, this);
    }

#endif  // WITH_MIDI_HOST


//-----------------------------------------------
// not implemented yet
//-----------------------------------------------

#if 0   //  not compiled yet

    bool passFilter(uint32_t iii)
    {
        // don't pass em if filter is not on

        msgUnion msg(iii);
        int type = msg.getMsgType();
        if (getPref8(PREF_PERF_FILTER))        // filter is on
        {
            // only accept messages from cable 0

            if (msg.isCable1())
                return 0;

            // only accept note on, note off, or pitchbends if the pref is cleared

            bool filter_bends = getPref8(PREF_PERF_FILTER_BENDS);
            if (type!=0x08 && type!=0x09 && (type!=0x0E || filter_bends))
                return 0;
        }

        // prh 2020-08-02 - removed initial hack of layer filters

        // send it to the teensyduino

        usb_midi_write_packed(msg.i);
        theSystem.midiActivity(INDEX_MASK_OUTPUT);
            // it IS port one, cable 0

        // if "monitor performanc" pref is set
        // enqueue it for display as PORT_INDEX_DUINO_OUTPUT0
        // with the PORT_MASK_PERFORM flag to display it differently

        if (getPref8(PREF_MONITOR_PERFORMANCE))
        {
            msg.i &= ~PORT_MASK;                            // clear the old port
            msg.i |= PORT_MASK_OUTPUT | PORT_MASK_PERFORM;  // output to teensyDuino0
            enqueueProcess(msg.i);
        }

        return 1;   // flush the usb_midi buffer
    }
#endif



//-----------------------------------------------
// obsolete junk
//-----------------------------------------------
// 2024-05-13 - spent a lot of time trying the newest USBHost_t36 library
// so I am keeping this code for future reference

#if 0   // obsolete junk


    #if HOW_HOST < HOW_HOST_DOIT_ALL_IN_IRQ


        uint32_t midiHost::myRead()
            // alternative streamlined read() method
            // All we need is access to the rx queue ...
            // This seems workable and still takes advantage of queuing in
            // the presumably very important USB rx_data() method.
        {
            uint32_t n, head, tail, avail;

            __disable_irq();
            bool packet_queued = rx_packet_queued;
            head = rx_head;
            tail = rx_tail;
            __enable_irq();

            if (head == tail)
                return 0;

            if (++tail >= rx_queue_size)
                tail = 0;

            n = rx_queue[tail];
            rx_tail = tail;

            if (!packet_queued && rxpipe)
            {
                avail = (head < tail) ? tail - head - 1 : rx_queue_size - 1 - head + tail;
                if (avail >= (uint32_t)(rx_size>>2))
                {
                    rx_packet_queued = true;
                    queue_Data_Transfer(rxpipe, rx_buffer, rx_size, this);
                }
            }

            return n;
        }

    #endif


    #if HOW_HOST == HOW_HOST_CALL_BASE_CLASS

        // Just call the base class directly.
        // read() or myRead() required

        void midiHost::rx_data(const Transfer_t *transfer)
        {
            MIDIDevice::tx_data(transfer);
        }

    #elif HOW_HOST >= HOW_HOST_RE_IMPLEMENTED

        // I saved off my fork, cloned Paul's latest library
        // made my minor API changes to get at this stuff, and
        // copied the new rx_data() method here.

        #define DEBUG_RXDATA    0
            // throws a single display() call into the irq method
            // which DEFINITELY mucks things up.

        #define print   USBHost::print_
        #define println USBHost::println_

        void midiHost::rx_data(const Transfer_t *transfer)
        {
            println("MIDIDevice Receive");
            print("  MIDI Data: ");

            // put transfer bytes into the circular queue
            // note there is no overflow check.

            uint32_t len = (transfer->length - ((transfer->qtd.token >> 16) & 0x7FFF)) >> 2;
            print_hexbytes(transfer->buffer, len * 4);

            #if HOW_HOST >= HOW_HOST_DOIT_ALL_IN_IRQ
                bool any = 0;
                    // a flag indicating if we need to flush the main midi output buffer
            #else
                uint32_t head = rx_head;
                uint32_t tail = rx_tail;
                    // we will be queuing messages and they
                    // must be removed with read() or myRead()
            #endif

            for (uint32_t i=0; i < len; i++)
            {
                uint32_t msg = rx_buffer[i];
                if (msg)
                {
                    #if HOW_HOST >= HOW_HOST_DOIT_ALL_IN_IRQ

                        // THIS WORKS AGAINST OLD LIBRARY!
                        // debugging, if any, here, since there is no other place

                        #if DEBUG_RXDATA
                            display(0,"rxdata(%08x)",msg);
                        #endif

                        any = 1;
                        usb_midi_write_packed(msg);

                    #else
                        // add it to the queue without overflow check
                        if (++head >= rx_queue_size) head = 0;
                        rx_queue[head] = msg;
                    #endif
                }
            }

            // if we are doing it all in the irq, then
            // we just flush the output midi buffer if needed
            // and call the queue_Data_Transfer(rxpipe, rx_buffer, rx_size, this);
            // magic method

            #if HOW_HOST >= HOW_HOST_DOIT_ALL_IN_IRQ

                if (any)
                    usb_midi_flush_output();
                queue_Data_Transfer(rxpipe, rx_buffer, rx_size, this);

            #else   // otherwise, do the enqueueing code from the base class

                rx_head = head;
                rx_tail = tail;

                uint32_t avail = (head < tail) ? tail - head - 1 : rx_queue_size - 1 - head + tail;
                //println("rx_size = ", rx_size);
                println("avail = ", avail);
                if (avail >= (uint32_t)(rx_size>>2))
                {
                    // enough space to accept another full packet
                    println("queue another receive packet");
                    queue_Data_Transfer(rxpipe, rx_buffer, rx_size, this);
                    rx_packet_queued = true;
                }
                else
                {
                    // queue can't accept another packet's data, so leave
                    // the data waiting on the device until we can accept it
                    println("wait to receive more packets");
                    rx_packet_queued = false;
                }
            #endif  // enqueing code from base class

        }

    #endif  // HOW_HOST >= HOW_HOST_RE_IMPLEMENTED

#endif  // obsolete junk
