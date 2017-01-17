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

#define MIFARE_ULTRALIGHT_PAGE_COUNT                16
#define MIFARE_ULTRALIGHT_CRC_SIZE                  2
#define MIFARE_ULTRALIGHT_PAGE_SIZE                 4
#define MIFARE_ULTRALIGHT_PAGE_SIZE_PLUS_CRC        MIFARE_ULTRALIGHT_PAGE_SIZE + MIFARE_ULTRALIGHT_CRC_SIZE
#define MIFARE_ULTRALIGHT_LOW_PAGES_COUNT           4
#define MIFARE_ULTRALIGHT_LOCK_BYTES_PAGE_ADDRESS   2
#define MIFARE_ULTRALIGHT_LOCK_BYTES_POS            2
#define MIFARE_ULTRALIGHT_LOCK_BYTES_SIZE           2
#define MIFARE_ULTRALIGHT_OTP_AREA_ADDRESS         3
#define MIFARE_ULTRALIGHT_OTP_AREA_SIZE            4

class MifareUltralight: public Tag {

public:

    enum SubType {
        ULTRALIBGHT,
        ULTRALIBGHT_C,
    };

    enum SpecialLockPosition {
        BL_OTP,
        BL_9_TO_4,
        BL_15_TO_10,
        L_OTP
    };

    enum Command {
        READ = 0x30,
        HALT = 0x50,
        WRITE = 0xa2
    };

    enum Error {
        UNDEFINED,
        INDEX_OUT_OF_BOUNDS,
        GENERIC_ERROR,
        LOW_PAGES_WRITE_ATTEMPT_DENIED,
        TRANSCEIVE_NACK,
        TRANSCEIVE_LENGTH_NOT_MATCH
    };

    MifareUltralight(Reader *reader);

    /**
     * The READ command needs the page address as a parameter. Only addresses 00h to 0Fh
     * are decoded. The MF0ICU1 returns a NAK for higher addresses.
     *
     * @param   address             Page address.
     * @param   buf                 4-byte-long buffer.
     * @return  bool                Successfulness.
     */
    bool readPage(unsigned char address, unsigned char *buf);

    /**
     * The WRITE command is used to program the lock bytes in page 02h, the OTP bytes in
     * page 03h and the data bytes in pages 04h to 0Fh. A WRITE command is performed
     * page-wise, programming 4 bytes in a row.
     *
     * @param   address             Page address.
     * @param   buf                 4-byte long buffer to be written into the given page.
     * @return  bool                Successfulness.
     */
    bool writePage(unsigned char address, unsigned char *buf);

    bool readPageSlice(unsigned char address, unsigned char from, unsigned char len, unsigned char *buf);

    bool writePageSlice(unsigned char address, unsigned char from, unsigned char len, unsigned char *buf);

    /**
     * Read the lock bytes.
     */
    bool readLockBytes(unsigned char *buf);

    /**
     * The locking and block-locking bits are set by a WRITE command to page 2. Bytes 2 and 3
     * of the WRITE command, and the contents of the lock bytes are bitwise OR’ed and the
     * result then becomes the new contents of the lock bytes. This process is irreversible if a bit
     * is set to logic 1, it cannot be changed back to logic 0.
     * The contents of bytes 0 and 1 of page 2 are unaffected by the corresponding data bytes of the WRITE command.
     */
    bool writeLockBytes(unsigned char *buf);

    int readByte(unsigned char address, unsigned char pos);

    bool writeByte(unsigned char address, unsigned char pos, unsigned char value);

    bool lockPage(unsigned char pageAddress);

    /**
     * Page 03h is the OTP page and it is preset so that all bits are set to logic 0 after production.
     * These bytes can be bitwise modified using the WRITE command.
     * The WRITE command bytes and the current contents of the OTP bytes are bitwise OR’ed.
     * The result is the new OTP byte contents. This process is irreversible and if a bit is set to
     * logic 1, it cannot be changed back to logic 0.
     */
    bool readOneTimeProgrammableArea(unsigned char *buf);

    bool writeOneTimeProgrammableArea(unsigned char *buf);

    /**
     * The HALT command is used to set the MF0ICU1 ICs into a different wait state (halt
     * instead of idle), enabling devices whose UIDs are already known because they have
     * passed the anticollision procedure, to be separated from devices yet to be identified by
     * their UIDs. This mechanism is a very efficient way of finding all contactless devices in the PCD field.
     */
    bool halt();

    void setLowPagesProtected(bool protect);

    bool areLowPagesProtected();

    bool isAddressAtLowPages(unsigned char address);

    bool checkWriteAtLowPages(unsigned char address);

    inline void setError(Error error);

    inline Error getError();

protected:

    SubType subType;

    Error error;

    bool lowPagesProtected;

    void setupTagType();
};

#endif // __ARDUINO_RADIO_FREQUENCY_IDENTIFICATION_TAG_MIFARE_ULTRALIGHT_H__
