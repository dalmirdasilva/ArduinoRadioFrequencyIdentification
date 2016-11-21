#include "MifareClassic.h"
#include <Reader.h>

MifareClassic::MifareClassic(Reader *reader)
        : Tag(reader), keyType(KEY_A), error(UNDEFINED), key(NULL), sectorTrailerProtected(true) {
}

bool MifareClassic::authenticate(unsigned char address, KeyType type, unsigned char *key) {
    unsigned char authWith = (type == KEY_A) ? AUTH_KEY_A : AUTH_KEY_B;
    unsigned char buffer[12] = { authWith, address };
    if (getState() != ACTIVE_STATE) {
        setError(DEVICE_NOT_ACTIVE_FOR_AUTH);
        return false;
    }
    memcpy(&buffer[2], key, MIFARE_CLASSIC_KEY_SIZE);
    memcpy(&buffer[8], uid.uid, MIFARE_CLASSIC_UID_NEED_FOR_AUTH_LEN);
    return reader->authenticate(buffer) >= 0;
}

bool MifareClassic::readBlock(unsigned char address, unsigned char *buf) {
    unsigned char buffer[MIFARE_CLASSIC_BLOCK_SIZE_AND_CRC] = { READ, address };
    if (key != NULL && !authenticate(address, keyType, key)) {
        setError(AUTO_AUTHENTICATION_FAILED);
        return false;
    }
    if (transceive(buffer, buffer, 2, true) != MIFARE_CLASSIC_BLOCK_SIZE_AND_CRC) {
        setError(TRANSCEIVE_LENGTH_NOT_MATCH);
        return false;
    }
    memcpy(buf, buffer, MIFARE_CLASSIC_BLOCK_SIZE);
    return true;
}

bool MifareClassic::writeBlock(unsigned char address, unsigned char *buf) {
    unsigned char buffer[MIFARE_CLASSIC_BLOCK_SIZE_AND_CRC] = { WRITE, address };
    if (isAddressSectorTrailer(address) && sectorTrailerProtected) {
        setError(SECTOR_TRAILER_WRITE_ATTEMPT_DENIED);
        return false;
    }
    if (key != NULL && !authenticate(address, keyType, key)) {
        setError(AUTO_AUTHENTICATION_FAILED);
        return false;
    }
    transceive(buffer, buffer, 2);
    if (reader->getLastError() == Reader::NACK) {
        setError(TRANSCEIVE_NACK);
        return false;
    }
    memcpy(buffer, buf, MIFARE_CLASSIC_BLOCK_SIZE);
    transceive(buffer, buffer, MIFARE_CLASSIC_BLOCK_SIZE);
    if (reader->getLastError() == Reader::NACK) {
        setError(TRANSCEIVE_NACK);
        return false;
    }
    return true;
}

bool MifareClassic::readBlockSlice(unsigned char address, unsigned char from, unsigned char len, unsigned char *buf) {
    unsigned char buffer[MIFARE_CLASSIC_BLOCK_SIZE_AND_CRC];
    if (len == 0 || from + len > MIFARE_CLASSIC_BLOCK_SIZE) {
        setError(INDEX_OUT_OF_BOUNDS);
        return false;
    }
    if (!readBlock(address, buffer)) {
        return false;
    }
    memcpy(buf, &buffer[from], len);
    return true;
}

bool MifareClassic::writeBlockSlice(unsigned char address, unsigned char from, unsigned char len, unsigned char *buf) {
    unsigned char buffer[MIFARE_CLASSIC_BLOCK_SIZE_AND_CRC];
    if (len == 0 || from + len > MIFARE_CLASSIC_BLOCK_SIZE) {
        setError(INDEX_OUT_OF_BOUNDS);
        return false;
    }

    // We only need to read the block if we are not setting the entire block.
    if ((from != 0 || len != MIFARE_CLASSIC_BLOCK_SIZE) && !readBlock(address, buffer)) {
        return false;
    }
    memcpy(&buffer[from], buf, len);
    return writeBlock(address, buffer);
}

int MifareClassic::readByte(unsigned char address, unsigned char pos) {
    unsigned char buffer[MIFARE_CLASSIC_BLOCK_SIZE_AND_CRC];
    if (!readBlock(address, buffer)) {
        return -1;
    }
    return buffer[pos];
}

bool MifareClassic::writeByte(unsigned char address, unsigned char pos, unsigned char value) {
    unsigned char buffer[MIFARE_CLASSIC_BLOCK_SIZE_AND_CRC];
    if (!readBlock(address, buffer)) {
        return false;
    }
    buffer[pos] = value;
    return writeBlock(address, buffer);
}

bool MifareClassic::increment(unsigned char address, uint32_t delta) {
    return arithmeticOperation(INCREMENT, address, delta);
}

bool MifareClassic::decrement(unsigned char address, uint32_t delta) {
    return arithmeticOperation(DECREMENT, address, delta);
}

bool MifareClassic::restore(unsigned char address) {
    return arithmeticOperation(RESTORE, address, 0);
}

bool MifareClassic::arithmeticOperation(unsigned char operation, unsigned char address, uint32_t delta) {
    unsigned char buffer[6] = { operation, address };
    if (key != NULL && !authenticate(address, keyType, key)) {
        setError(AUTO_AUTHENTICATION_FAILED);
        return false;
    }
    transceive(buffer, buffer, 2);
    if (reader->getLastError() == Reader::NACK) {
        setError(TRANSCEIVE_NACK);
        return false;
    }
    memcpy(&buffer[0], &delta, 4);
    transceive(buffer, buffer, 4);

    /**
     * The MIFARE Increment, Decrement, and Restore command part 2 does not
     * provide an acknowledgement, so the regular time-out has to be used instead.
     */
    return reader->getLastError() != Reader::NACK;
}

bool MifareClassic::transfer(unsigned char address) {
    unsigned char buffer[4] = { TRANSFER, address };
    if (key != NULL && !authenticate(address, keyType, key)) {
        setError(AUTO_AUTHENTICATION_FAILED);
        return false;
    }
    transceive(buffer, buffer, 2);
    return reader->getLastError() == Reader::NO_ERROR;
}

bool MifareClassic::createValueBlock(unsigned char address, uint32_t value, uint8_t addr) {
    Access access;
    unsigned char buffer[MIFARE_CLASSIC_BLOCK_SIZE_AND_CRC];
    if (!getAccessCondition(address, &access) || (access != CONDITION_1 && access != CONDITION_6)) {
        setError(OPERATION_DENIED_BY_ACCESS_BITS);
        return false;
    }
    fillValueBlock(buffer, value, addr);
    return writeBlock(address, buffer);
}

bool MifareClassic::readAccessBits(unsigned char sector, unsigned char *accessBits) {
    return readBlockSlice(getSectorTrailerAddress(sector), MIFARE_CLASSIC_ACCESS_BITS_POSITION, MIFARE_CLASSIC_ACCESS_BITS_SIZE, accessBits);
}

bool MifareClassic::writeAccessBits(unsigned char sector, unsigned char *accessBits, unsigned char *keyA, unsigned char *keyB) {
    unsigned char buffer[MIFARE_CLASSIC_BLOCK_SIZE_AND_CRC];
    if (!isAccessBitsCorrect(accessBits)) {
        setError(WRONG_ACCESS_BITS_LAYOUT);
        return false;
    }
    memcpy(&buffer[MIFARE_CLASSIC_KEY_TYPE_TO_POS(KEY_A)], keyA, MIFARE_CLASSIC_KEY_SIZE);
    memcpy(&buffer[MIFARE_CLASSIC_ACCESS_BITS_POSITION], accessBits, MIFARE_CLASSIC_ACCESS_BITS_SIZE);
    memcpy(&buffer[MIFARE_CLASSIC_KEY_TYPE_TO_POS(KEY_B)], keyB, MIFARE_CLASSIC_KEY_SIZE);
    return writeBlockSlice(getSectorTrailerAddress(sector), 0, MIFARE_CLASSIC_BLOCK_SIZE, buffer);
}

bool MifareClassic::setAccessCondition(unsigned char sector, unsigned char block, Access access, unsigned char *keyA, unsigned char *keyB) {
    unsigned char mask, c1, c2, c3, accessBits[4];
    if (!readAccessBits(sector, accessBits)) {
        return false;
    }
    mask = 0x01 << (block & 0x03);
    unpackAccessBits(accessBits, &c1, &c2, &c3);
    c1 = (c1 & ~mask) | ((access & 0x04) ? mask : 0);
    c2 = (c2 & ~mask) | ((access & 0x02) ? mask : 0);
    c3 = (c3 & ~mask) | ((access & 0x01) ? mask : 0);
    packAccessBits(accessBits, c1, c2, c3);
    return writeAccessBits(sector, accessBits, keyA, keyB);
}

bool MifareClassic::getAccessCondition(unsigned char address, Access *access) {
    unsigned char block, mask, c1, c2, c3, accessBits[4];
    if (!readAccessBits(addressToSector(address), accessBits)) {
        return false;
    }
    block = addressToBlock(address) & 0x03;
    mask = 0x01 << block;
    unpackAccessBits(accessBits, &c1, &c2, &c3);
    *access = (Access) ((((c1 & mask) ? 0x04 : 0) | ((c2 & mask) ? 0x02 : 0) | ((c3 & mask) ? 0x01 : 0)) & 0x07);
    return true;
}

bool MifareClassic::writeKey(unsigned char sector, KeyType type, unsigned char *keyA, unsigned char *keyB) {
    unsigned char buffer[MIFARE_CLASSIC_BLOCK_SIZE_AND_CRC];
    unsigned address = getSectorTrailerAddress(sector);
    if (!readBlock(address, buffer)) {
        return false;
    }
    memcpy(&buffer[MIFARE_CLASSIC_KEY_TYPE_TO_POS(KEY_A)], keyA, MIFARE_CLASSIC_KEY_SIZE);
    memcpy(&buffer[MIFARE_CLASSIC_KEY_TYPE_TO_POS(KEY_B)], keyB, MIFARE_CLASSIC_KEY_SIZE);
    return writeBlock(address, buffer);
}

bool MifareClassic::readKey(unsigned char sector, KeyType type, unsigned char *key) {
    unsigned from = MIFARE_CLASSIC_KEY_TYPE_TO_POS(type);
    return readBlockSlice(getSectorTrailerAddress(sector), from, MIFARE_CLASSIC_KEY_SIZE, key);
}

void MifareClassic::setupAuthenticationKey(KeyType keyType, unsigned char *key) {
    this->keyType = keyType;
    this->key = key;
}

bool MifareClassic::isAccessBitsCorrect(unsigned char *accessBits) {
    unsigned char *c = accessBits;
    return (~(c[1] >> 4) & 0x0f) == (c[0] & 0x0f) && (~(c[2]) & 0x0f) == ((c[0] >> 4) & 0x0f) && (~(c[2] >> 4) & 0x0f) == (c[1] & 0x0f);
}

void MifareClassic::packAccessBits(unsigned char *accessBits, unsigned char c1, unsigned char c2, unsigned char c3) {
    accessBits[0] = (~(c2 << 4) & 0xf0) | ((~c1) & 0x0f);
    accessBits[1] = ((c1 << 4) & 0xf0) | ((~c3) & 0x0f);
    accessBits[2] = ((c3 << 4) & 0xf0) | (c2 & 0x0f);
}

void MifareClassic::unpackAccessBits(unsigned char *accessBits, unsigned char *c1, unsigned char *c2, unsigned char *c3) {
    *c1 = (accessBits[1] >> 4) & 0x0f;
    *c2 = accessBits[2] & 0x0f;
    *c3 = (accessBits[2] >> 4) & 0x0f;
}

void MifareClassic::setSectorTrailerProtected(bool protect) {
    sectorTrailerProtected = protect;
}

unsigned char MifareClassic::getSectorCount() {
    unsigned char sectorCount = MIFARE_CLASSIC_LOW_SECTOR_COUNT;
    if (size == SIZE_MINI) {
        sectorCount = MIFARE_CLASSIC_MINI_SECTOR_COUT;
    } else if (size == SIZE_4K) {
        sectorCount += MIFARE_CLASSIC_HIGH_SECTOR_COUNT;
    }
    return sectorCount;
}

unsigned int MifareClassic::getBlockCount() {
    return size / MIFARE_CLASSIC_BLOCK_SIZE;
}

unsigned char MifareClassic::getBlockCountInSector(unsigned char sector) {
    return (sector < MIFARE_CLASSIC_LOW_SECTOR_COUNT) ? MIFARE_CLASSIC_LOW_BLOCK_COUNT_IN_SECTOR : MIFARE_CLASSIC_HIGH_BLOCK_COUNT_IN_SECTOR;
}

unsigned int MifareClassic::getSectorSize(unsigned char sector) {
    return (sector < MIFARE_CLASSIC_LOW_SECTOR_COUNT) ? MIFARE_CLASSIC_LOW_SECTOR_SIZE : MIFARE_CLASSIC_HIGH_SECTOR_SIZE;
}

unsigned char MifareClassic::addressToSector(unsigned char address) {
    unsigned char sector = address / MIFARE_CLASSIC_LOW_BLOCK_COUNT_IN_SECTOR;
    if (address >= MIFARE_CLASSIC_LOW_BLOCK_COUNT) {
        address -= MIFARE_CLASSIC_LOW_BLOCK_COUNT;
        sector = MIFARE_CLASSIC_LOW_SECTOR_COUNT + (address / MIFARE_CLASSIC_HIGH_BLOCK_COUNT_IN_SECTOR);
    }
    return sector;
}

unsigned char MifareClassic::addressToBlock(unsigned char address) {
    unsigned char block = address % MIFARE_CLASSIC_LOW_SECTOR_COUNT;
    if (address >= MIFARE_CLASSIC_LOW_BLOCK_COUNT) {
        address -= MIFARE_CLASSIC_LOW_BLOCK_COUNT;

        // For the first 32 sectors (first 2K bytes of NV-memory) the access conditions can be set individually for a data area sized one block.
        // For the last 8 sectors (upper 2K bytes of NV-memory) access conditions can be set individually for a data area sized 5 blocks.
        block = (address % MIFARE_CLASSIC_HIGH_SECTOR_COUNT) / 5;
    }
    return block;
}

unsigned char MifareClassic::isAddressSectorTrailer(unsigned char address) {
    unsigned char blockCount = getBlockCountInSector(addressToSector(address));
    if (address >= MIFARE_CLASSIC_LOW_BLOCK_COUNT) {
        address -= MIFARE_CLASSIC_LOW_BLOCK_COUNT;
    }
    return ((address % blockCount) == (blockCount - 1));
}

unsigned char MifareClassic::getSectorTrailerAddress(unsigned char sector) {
    unsigned char blockCount, offset = 0;
    blockCount = getBlockCountInSector(sector);
    if (sector >= MIFARE_CLASSIC_LOW_SECTOR_COUNT) {
        offset = MIFARE_CLASSIC_LOW_BLOCK_COUNT;
        sector -= MIFARE_CLASSIC_LOW_SECTOR_COUNT;
    }
    return offset + (sector * blockCount) + (blockCount - 1);
}

bool MifareClassic::halt() {
    unsigned char buffer[4] = { HLT_A };
    reader->stopCrypto();
    transceive(buffer, buffer, 2);
    setState(HALT_STATE);

    // If the PICC responds with any modulation during a period of 1 ms after the end of the frame containing the
    // HLTA command, this response shall be interpreted as 'not acknowledge'.
    return reader->getLastError() == Reader::TIMEOUT_ERROR;
}

void MifareClassic::setError(Error error) {
    this->error = error;
}

MifareClassic::Error MifareClassic::getError() {
    return error;
}

void MifareClassic::fillValueBlock(unsigned char *buf, uint32_t value, uint8_t addr) {
    memcpy(&buf[0], &value, 4);
    memcpy(&buf[8], &value, 4);
    value = ~value;
    memcpy(&buf[4], &value, 4);
    buf[12] = addr;
    buf[13] = ~addr;
    buf[14] = addr;
    buf[15] = ~addr;
}
