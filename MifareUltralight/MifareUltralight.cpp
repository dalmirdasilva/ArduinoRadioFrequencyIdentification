#include "MifareUltralight.h"
#include <Reader.h>
#include <Tag.h>

MifareUltralight::MifareUltralight(Reader *reader)
        : Tag(reader), subType(ULTRALIBGHT), error(UNDEFINED), lowPagesProtected(true) {
}

bool MifareUltralight::readPage(unsigned char address, unsigned char *buf) {
    unsigned char buffer[18] = { READ, address };
    if (transceive(buffer, buffer, 2, true) != 18) {
        setError(TRANSCEIVE_LENGTH_NOT_MATCH);
        return false;
    }
    memcpy(buf, buffer, 4);
    return true;
}

bool MifareUltralight::writePage(unsigned char address, unsigned char *buf) {
    unsigned char buffer[6] = { WRITE, address };
    if (isAddressAtLowPages(address) && lowPagesProtected) {
        setError(LOW_PAGES_WRITE_ATTEMPT_DENIED);
        return false;
    }
    transceive(buffer, buffer, 2);
    if (reader->getLastError() == Reader::NACK) {
        setError(TRANSCEIVE_NACK);
        return false;
    }
    memcpy(buffer, buf, 4);
    transceive(buffer, buffer, 4);
    if (reader->getLastError() == Reader::NACK) {
        setError(TRANSCEIVE_NACK);
        return false;
    }
    return true;
}

bool MifareUltralight::readPageSlice(unsigned char address, unsigned char from, unsigned char len, unsigned char *buf) {
    unsigned char buffer[18];
    if (len == 0 || from + len > MIFARE_ULTRALIGHT_PAGE_SIZE) {
        setError(INDEX_OUT_OF_BOUNDS);
        return false;
    }
    if (!readPage(address, buffer)) {
        return false;
    }
    memcpy(buf, &buffer[from], len);
    return true;
}

bool MifareUltralight::writePageSlice(unsigned char address, unsigned char from, unsigned char len, unsigned char *buf) {
    unsigned char buffer[MIFARE_ULTRALIGHT_PAGE_SIZE_PLUS_CRC];
    if (len == 0 || from + len > MIFARE_ULTRALIGHT_PAGE_SIZE) {
        setError(INDEX_OUT_OF_BOUNDS);
        return false;
    }

    // We only need to read the block if we are not setting the entire block.
    if ((from != 0 || len != MIFARE_ULTRALIGHT_PAGE_SIZE) && !readPage(address, buffer)) {
        return false;
    }
    memcpy(&buffer[from], buf, len);
    return writePage(address, buffer);
}

bool MifareUltralight::readLockBytes(unsigned char *buf) {
    return readPageSlice(MIFARE_ULTRALIGHT_LOCK_BYTES_PAGE_ADDRESS, MIFARE_ULTRALIGHT_LOCK_BYTES_POS, MIFARE_ULTRALIGHT_LOCK_BYTES_SIZE, buf);
}

bool MifareUltralight::writeLockBytes(unsigned char *buf) {
    return writePageSlice(MIFARE_ULTRALIGHT_LOCK_BYTES_PAGE_ADDRESS, MIFARE_ULTRALIGHT_LOCK_BYTES_POS, MIFARE_ULTRALIGHT_LOCK_BYTES_SIZE, buf);
}

int MifareUltralight::readByte(unsigned char address, unsigned char pos) {
    unsigned char value;
    if (!readPageSlice(address, pos, 1, &value)) {
        return -1;
    }
    return value;
}

bool MifareUltralight::writeByte(unsigned char address, unsigned char pos, unsigned char value) {
    return writePageSlice(address, pos, 1, &value);
}

bool MifareUltralight::halt() {
    unsigned char buffer[4] = { HALT };
    transceive(buffer, buffer, 2);
    setState(HALT_STATE);

    // Passive (implicit) ACKnowledge without PICC answer.
    return reader->getLastError() == Reader::TIMEOUT_ERROR;
}

void MifareUltralight::setLowPagesProtected(bool protect) {
    lowPagesProtected = protect;
}

bool MifareUltralight::isAddressAtLowPages(unsigned char address) {
    return address < MIFARE_ULTRALIGHT_LOW_PAGES_COUNT;
}

void MifareUltralight::setError(Error error) {
    this->error = error;
}

MifareUltralight::Error MifareUltralight::getError() {
    return error;
}
