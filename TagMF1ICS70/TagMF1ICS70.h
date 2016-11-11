/**
 * Arduino - Radio Frequency Identification MFRC522
 *
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */

#ifndef __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MF1ICS70_H__
#define __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MF1ICS70_H__ 1

#include <Arduino.h>
#include <Reader.h>
#include <Tag.h>

/**
 *
 * NOT IMPLEMENTED
 *
 *
 * MIFARE Ultralight contactless single-ticket IC
 */
class TagMF1ICS70: public Tag {

public:

    // All MIFARE Classic commands use the MIFARE Crypto1 and require an authentication.
    enum Command {
    };

    TagMF1ICS70(Reader *reader);

    bool detect(unsigned char command);

    bool request();

    bool wakeUp();

    bool activate();

    bool select();

    bool halt();

    bool authenticate(unsigned char address, KeyType type, unsigned char *key);

    bool readBlock(unsigned char address, unsigned char *buf);

    bool writeBlock(unsigned char address, unsigned char *buf);

    bool readBlockSlice(unsigned char address, unsigned char from, unsigned char to, unsigned char *buf);

    bool writeBlockSlice(unsigned char address, unsigned char from, unsigned char to, unsigned char *buf);

    int readByte(unsigned char address, unsigned char pos);

    bool writeByte(unsigned char address, unsigned char pos, unsigned char value);

    bool decrement();

    bool increment();

    bool restore();

    bool transfer();

    bool setBlockType(unsigned char address, BlockType type);

    bool readAccessBits(unsigned char sector, unsigned char *buf);

    bool writeAccessBits(unsigned char sector, unsigned char *buf);

    bool setBlockPermission(unsigned char address, unsigned char permission);

    bool writeKey(unsigned char sector, KeyType type, unsigned char *key);

    bool readKey(unsigned char sector, KeyType type, unsigned char *key);

    void setupAuthenticationKey(KeyType keyType, unsigned char *key);

    void setAllowSectorTrailerWrite(bool allow);
};

#endif // __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MF1ICS70_H__
