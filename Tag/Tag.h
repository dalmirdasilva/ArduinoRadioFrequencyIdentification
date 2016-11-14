/**
 * Arduino - Radio Frequency Identification
 *
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */

#ifndef __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_H__
#define __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_H__ 1

#include <Reader.h>
#include <stdint.h>

#define TAG_SAK_BIT                             0x20
#define TAG_ATQA_ANTICOLLISION_BIT              0x04

#define TAG_BLOCK_SIZE                          0x10
#define TAG_KEY_SIZE                            0x06
#define TAG_ACCESS_BITS_SIZE                    0x04
#define TAG_ACCESS_POSITION                     0x06
#define TAG_DEFAULT_SECTOR_SIZE                 0x04

#define TAG_KEY_TYPE_TO_POS(type)               (((type) == KEY_A) ? 0 : 10)

class Tag {

public:

    enum State {
        POWER_OFF = 0x00,
        IDLE = 0x01,
        READY = 0x02,
        ACTIVE = 0x03,
        HALT = 0x04
    };

    enum TagType {
        MIFARE_UNKNOWN = 0X00,
        MIFARE_MINI = 0X01,
        MIFARE_1K = 0X02,
        MIFARE_4K = 0X03,
        MIFARE_UL = 0X04,
        MIFARE_PLUS = 0x06,
        MIFARE_NOT_COMPLETE = 0xff
    };

    enum KeyType {
        KEY_A = 0x00,
        KEY_B = 0x01
    };

    enum BlockType {
        DATA_BLOCK = 0x00,
        VALUE_BLOCK = 0x01,
    };

    enum Access {
        CONDITION_0 = 0x00,
        CONDITION_1 = 0x01,
        CONDITION_2 = 0x02,
        CONDITION_3 = 0x03,
        CONDITION_4 = 0x04,
        CONDITION_5 = 0x05,
        CONDITION_6 = 0x06,
        CONDITION_7 = 0x07
    };

    // All MIFARE Classic commands use the MIFARE Crypto1 and require an authentication.
    enum Command {

        // REQuest command, Type A. Invites PICCs in state IDLE to go to READY and prepare for anticollision or selection. 7 bit frame.
        REQUEST = 0x26,

        // Wake-UP command, Type A. Invites PICCs in state IDLE and HALT to go to READY(*) and prepare for anticollision or selection. 7 bit frame.
        WAKE_UP = 0x52,

        // Anti collision/Select, Cascade Level 1
        SEL_CL1 = 0x93,

        // Anti collision/Select, Cascade Level 2
        SEL_CL2 = 0x95,

        // Anti collision/Select, Cascade Level 3
        SEL_CL3 = 0x97,

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

    struct Uid {

        // Number of bytes in the UID. 4, 7 or 10.
        unsigned char size;

        unsigned char uid[10];

        // The SAK (Select acknowledge) byte returned from the tag after successful selection.
        unsigned char sak;
    };

    Tag(Reader *reader);

    virtual ~Tag();


protected:

    Reader *reader;
};

#endif // __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_H__
