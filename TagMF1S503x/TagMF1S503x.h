/**
 * Arduino - Radio Frequency Identification MFRC522
 *
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */
#ifndef __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MF1S503X_H__
#define __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MF1S503X_H__ 1

#include <Arduino.h>
#include <Reader.h>
#include <Tag.h>

#define TAG_MF1S503X_KEY_SIZE               0x06
#define MF1S503X_ATQA_ANTICOLLISION_BIT     0x04

class TagMF1S503x: public Tag {

    Reader *reader;

public:

    enum Permission {
        LEVEL_0 = 0x00,
        LEVEL_1 = 0x01,
        LEVEL_2 = 0x02,
        LEVEL_3 = 0x03,
        LEVEL_4 = 0x04,
        LEVEL_5 = 0x05,
        LEVEL_6 = 0x06,
        LEVEL_7 = 0x07
    };

    // All MIFARE Classic commands use the MIFARE Crypto1 and require an authentication.
    enum Command {

        // REQuest command, Type A. Invites PICCs in state IDLE to go to READY and prepare for anticollision or selection. 7 bit frame.
        REQUEST = 0x26,

        // Wake-UP command, Type A. Invites PICCs in state IDLE and HALT to go to READY(*) and prepare for anticollision or selection. 7 bit frame.
        WAKE_UP = 0x52,

        // Anti collision/Select, Cascade Level 1
        ANTI_COLLISION_CL1 = 0x93,

        // HaLT command, Type A. Instructs an ACTIVE PICC to go to state HALT.
        HLT_A = 0x50,

        // Perform authentication with Key A.
        AUTH_KEY_A = 0x60,

        // Perform authentication with Key B.
        AUTH_KEY_B = 0x61,

        // Reads one 16 byte block from the authenticated sector of the PICC. Also used for MIFARE Ultralight.
        READ = 0x30,

        // Writes one 16 byte block to the authenticated sector of the PICC. Called "COMPATIBILITY WRITE" for MIFARE Ultralight.
        WRITE = 0xa0,

        // Decrements the contents of a block and stores the result in the internal data register.
        DECREMENT = 0xc0,

        // Increments the contents of a block and stores the result in the internal data register.
        INCREMENT = 0xc1,

        // Reads the contents of a block into the internal data register.
        RESTORE = 0xc2,

        // Writes the contents of the internal data register to a block.
        TRANSFER = 0xb0,
    };

    enum KeyType {
        KEY_A = 0x00,
        KEY_B = 0x01,
    };

    TagMF1S503x(Reader *reader);

    bool detect(unsigned char command);

    bool request();

    bool wakeUp();

    bool antiCollision();

    bool select();

    bool halt();

    bool authenticate(unsigned char keyType, unsigned char blockAddress, unsigned char *key, Uid *uid);

    bool readBlock(unsigned char blockAddress, unsigned char *buf);

    bool writeBlock(unsigned char blockAddress, unsigned char *buf);

    bool decrement();

    bool increment();

    bool restore();

    bool transfer();

    bool setBlockType(unsigned char blockAddress, BlockType type);

    bool readAccessBits(unsigned char blockAddress, unsigned char *buf);

    bool writeAccessBits(unsigned char blockAddress, unsigned char *buf);

    bool setBlockPermission(unsigned char blockAddress, unsigned char permission);

    bool writeKey(unsigned char blockAddress, unsigned char blockType, unsigned char *key);
};

#endif // __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MF1S503X_H__
