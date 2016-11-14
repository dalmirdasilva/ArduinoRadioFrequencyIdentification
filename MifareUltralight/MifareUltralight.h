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

    virtual unsigned int getSize() = 0;

    virtual bool readBlock(unsigned char address, unsigned char *buf);

    virtual bool writeBlock(unsigned char address, unsigned char *buf);

    virtual bool readBlockSlice(unsigned char address, unsigned char from, unsigned char len, unsigned char *buf);

    virtual bool writeBlockSlice(unsigned char address, unsigned char from, unsigned char len, unsigned char *buf);

    virtual int readByte(unsigned char address, unsigned char pos);

    virtual bool writeByte(unsigned char address, unsigned char pos, unsigned char value);
};

#endif // __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MIFARE_ULTRALIGHT_H__
