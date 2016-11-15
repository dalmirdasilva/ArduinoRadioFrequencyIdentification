/**
 * Arduino - Radio Frequency Identification MFRC522
 *
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */

#ifndef __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MIFARE_ULTRALIGHT_H__
#define __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MIFARE_ULTRALIGHT_H__ 1

#include <Arduino.h>
#include <Reader.h>
#include <Tag.h>

class MifareUltralight: public Tag {

public:

    MifareUltralight(Reader *reader);

    bool readBlock(unsigned char address, unsigned char *buf);

    bool writeBlock(unsigned char address, unsigned char *buf);

    bool readBlockSlice(unsigned char address, unsigned char from, unsigned char len, unsigned char *buf);

    bool writeBlockSlice(unsigned char address, unsigned char from, unsigned char len, unsigned char *buf);

    int readByte(unsigned char address, unsigned char pos);

    bool writeByte(unsigned char address, unsigned char pos, unsigned char value);
};

#endif // __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MIFARE_ULTRALIGHT_H__
