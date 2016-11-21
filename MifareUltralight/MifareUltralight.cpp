#include "MifareUltralight.h"
#include <Reader.h>
#include <Tag.h>

MifareUltralight::MifareUltralight(Reader *reader)
        : Tag(reader), error(UNDEFINED), lowPagesProtected(true) {
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

bool MifareUltralight::readLockBytes(unsigned char *lockBytes) {

}

bool MifareUltralight::halt() {
    unsigned char buffer[4] = { HALT };
    reader->stopCrypto();
    transceive(buffer, buffer, 2);
    setState(HALT_STATE);

    // If the PICC responds with any modulation during a period of 1 ms after the end of the frame containing the
    // HLTA command, this response shall be interpreted as 'not acknowledge'.
    return reader->getLastError() == Reader::TIMEOUT_ERROR;
}

void MifareUltralight::setLowPagesProtected(bool protect) {
    lowPagesProtected = protect;
}

bool MifareUltralight::isAddressAtLowPages(unsigned char address) {
    return address <= MIFARE_ULTRALIGHT_LOW_PAGES_COUNT;
}

void MifareUltralight::setError(Error error) {
    this->error = error;
}

MifareUltralight::Error MifareUltralight::getError() {
    return error;
}
