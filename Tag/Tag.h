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
        POWER_OFF_STATE = 0x00,
        IDLE_STATE = 0x01,
        READY_STATE = 0x02,
        ACTIVE_STATE = 0x03,
        HALT_STATE = 0x04
    };

    enum TagType {
        TYPE_UNKNOWN = 0x00,
        TYPE_MINI = 0x01,
        TYPE_CLASSIC = 0x02,
        TYPE_PLUS = 0x03,
        TYPE_ULTRALIGHT = 0x04,
        TYPE_NOT_COMPLETE = 0xff
    };

    enum TagSize
        : unsigned int {
            SIZE_0 = 0,
        SIZE_1K = 1024,
        SIZE_2K = 2048,
        SIZE_4K = 4096,
        SIZE_MINI = 320
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
        SEL_CL3 = 0x97
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

    TagType getType();

    void setType(TagType type);

    TagSize getSize();

    void setSize(TagSize size);

    State getState();

    void setState(State state);

    bool detect(unsigned char command);

    bool activate();

    bool activateWakeUp();

    bool request();

    bool wakeUp();

    bool select();

    virtual bool halt() = 0;

    /**
     * Helper for transceiving data with CRC calculation.
     *
     * @param   send            Send buffer needs to have enough room for add the 2-bytes CRC.
     * @param   receive         Receive buffer.
     * @param   sendLen         How many bytes we are sending, disregarding the CRC part.
     */
    int transceive(unsigned char *send, unsigned char *receive, unsigned char sendLen, bool checkCrc = false);

protected:

    Reader *reader;

    TagType type;

    TagSize size;

    Uid uid;

    bool supportsAnticollision;

    State state;

    void computeTagType();

    unsigned char computeNvb(unsigned char collisionPos);
};

#endif // __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_H__
