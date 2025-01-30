//-----------------------------------------------------------------------
// This file provides overrides for the USB device names
//-----------------------------------------------------------------------

#include "usb_names.h"
#include <Arduino.h>	// for memcpy()

// as long as I allow enough room in the declaration for any overwrites,
// I can overwrite these at my will.

#define MAX_SERIAL_NUMBER  14


struct usb_string_descriptor_struct usb_string_manufacturer_name = {
	2 + 17 * 2,
	3,
	{'h','a','r','m','o','n','i','c','S','u','b','s','y','t','e','m','s'}
};
struct usb_string_descriptor_struct usb_string_product_name = {
	2 + 18 * 2,
	3,
	{'T','E','_','A','U','D','I','O',  0,0,0,0, 0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0}		// allow 30
};
struct usb_string_descriptor_struct usb_string_serial_number = {
	2 + 4 * 2,
	3,
	{'T','E','3','A', 0,0, 0,0,0,0,0,0,0,0 }			// allow 14
};


// Convert a digit (0-9) to its character representation
#define DIGIT_TO_CHAR(digit) ('0' + digit)

// Macro to create the midi port descriptors
#define MAKE_MIDI_PORT_DESC(a, b) \
    struct usb_string_descriptor_struct usb_string_midi_port_in##a##b = { \
        2 + 20 * 2,  \
        3,           \
        {'T','E','_','A','U','D','I','O','_','I','N',		DIGIT_TO_CHAR(a),DIGIT_TO_CHAR(b), 0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0} \
    }; \
    struct usb_string_descriptor_struct usb_string_midi_port_out##a##b = { \
        2 + 21 * 2,  \
        3,           \
        {'T','E','_','A','U','D','I','O','_','O','U','T', 	DIGIT_TO_CHAR(a),DIGIT_TO_CHAR(b),  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0} \
    }

MAKE_MIDI_PORT_DESC(0,1);
MAKE_MIDI_PORT_DESC(0,2);
//	MAKE_MIDI_PORT_DESC(0,3);
//	MAKE_MIDI_PORT_DESC(0,4);
//	MAKE_MIDI_PORT_DESC(0,5);
//	MAKE_MIDI_PORT_DESC(0,6);
//	MAKE_MIDI_PORT_DESC(0,7);
//	MAKE_MIDI_PORT_DESC(0,8);
//	MAKE_MIDI_PORT_DESC(0,9);
//	MAKE_MIDI_PORT_DESC(1,0);
//	MAKE_MIDI_PORT_DESC(1,1);
//	MAKE_MIDI_PORT_DESC(1,2);
//	MAKE_MIDI_PORT_DESC(1,3);
//	MAKE_MIDI_PORT_DESC(1,4);
//	MAKE_MIDI_PORT_DESC(1,5);
//	MAKE_MIDI_PORT_DESC(1,6);


void setUSBSerialNum()
	// copies the actual teensy serial number to after the TEH
	// in the defined structure
{
	extern struct usb_string_descriptor_struct usb_string_serial_number_default;
	int len = 4; 	// TE3A
	memcpy(&usb_string_serial_number.wString[len],usb_string_serial_number_default.wString,10 * sizeof(uint16_t));
	usb_string_serial_number.bLength = 2 + (len + 10) * 2;
}


const char *getUSBSerialNum()
	// return the USB Serial Number as a null termianted C string
{
	static char buffer[MAX_SERIAL_NUMBER+1];
	char *out = buffer;
	uint16_t *in = &usb_string_serial_number.wString[0];
	int len = usb_string_serial_number.bLength - 4;

	while (len && *in)
	{
		*out++ = *((char *) in++);
	}
	*out = 0;
	return buffer;
}


// end of _usbNames.c
