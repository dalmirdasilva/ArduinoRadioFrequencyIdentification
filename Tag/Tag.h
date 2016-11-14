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

    Uid getUid();

    bool hasAnticollisionSupport();

    TagType getTagType();

    void setState(State state);

    State getState();

    bool detect(unsigned char command);

    bool activate();

    bool activateWakeUp();

    bool request();

    bool wakeUp();

    bool select();

    bool halt();

protected:

    Reader *reader;

    TagType tagType;

    Uid uid;

    bool supportsAnticollision;

    State state;

    void computeTagType();

    unsigned char computeNvb(unsigned char collisionPos);
};

#endif // __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_H__
