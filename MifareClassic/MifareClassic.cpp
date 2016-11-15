#include "MifareClassic.h"
#include <Reader.h>

MifareClassic::MifareClassic(Reader *reader)
        : Tag(reader), keyType(KEY_A), key(NULL), sectorTrailerProtected(true) {
}

MifareClassic::MifareClassic(Reader *reader, TagType type, TagSize size)
        : MifareClassic(reader) {
    this->type = type;
    this->size = size;
}

bool MifareClassic::authenticate(unsigned char address, KeyType type, unsigned char *key) {
    unsigned char buf[12];
    if (getState() != ACTIVE) {
        return false;
    }
    buf[0] = (type == KEY_A) ? AUTH_KEY_A : AUTH_KEY_B;
    buf[1] = address;
    for (unsigned char i = 0; i < MIFARE_CLASSIC_KEY_SIZE; i++) {
        buf[2 + i] = key[i];
    }
    for (unsigned char i = 0; i < 4; i++) {
        buf[8 + i] = uid.uid[i];
    }
    return reader->authenticate(buf) >= 0;
}

bool MifareClassic::readBlock(unsigned char address, unsigned char *buf) {
    if (key != NULL && !authenticate(address, keyType, key)) {
        return false;
    }
    buf[0] = READ;
    buf[1] = address;
    reader->calculateCrc(buf, 2, &buf[2]);
    return reader->tranceive(buf, buf, 4, true) == 18;
}

bool MifareClassic::writeBlock(unsigned char address, unsigned char *buf) {
    unsigned char cmd[4] = { WRITE, address, 0, 0 };
    if (isAddressSectorTrailer(address) && sectorTrailerProtected) {
        return false;
    }
    if (key != NULL && !authenticate(address, keyType, key)) {
        return false;
    }
    reader->calculateCrc(cmd, 2, &cmd[2]);
    reader->tranceive(cmd, cmd, 4);
    if (reader->getLastError() == Reader::NACK) {
        return false;
    }
    reader->calculateCrc(buf, 16, &buf[16]);
    reader->tranceive(buf, buf, 18);
    return reader->getLastError() != Reader::NACK;
}

bool MifareClassic::readBlockSlice(unsigned char address, unsigned char from, unsigned char len, unsigned char *buf) {
    unsigned char receive[18];
    if (len == 0 || from + len > 16) {
        return false;
    }
    if (!readBlock(address, receive)) {
        return false;
    }
    memcpy(buf, &receive[from], len);
    return true;
}

bool MifareClassic::writeBlockSlice(unsigned char address, unsigned char from, unsigned char len, unsigned char *buf) {
    unsigned char receive[18];
    if (len == 0 || from + len > MIFARE_CLASSIC_BLOCK_SIZE) {
        return false;
    }
    if (from == 0 && len == MIFARE_CLASSIC_BLOCK_SIZE && !readBlock(address, receive)) {
        return false;
    }
    memcpy(&receive[from], buf, len);
    return writeBlock(address, receive);
}

int MifareClassic::readByte(unsigned char address, unsigned char pos) {
    unsigned char buf[18];
    if (!readBlock(address, buf)) {
        return -1;
    }
    return buf[pos];
}

bool MifareClassic::writeByte(unsigned char address, unsigned char pos, unsigned char value) {
    unsigned char buf[18];
    if (!readBlock(address, buf)) {
        return false;
    }
    buf[pos] = value;
    return writeBlock(address, buf);
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
    unsigned char cmd[6] = { operation, address, 0, 0, 0, 0 };
    if (key != NULL && !authenticate(address, keyType, key)) {
        return false;
    }
    reader->calculateCrc(cmd, 2, &cmd[2]);
    reader->tranceive(cmd, cmd, 4);
    if (reader->getLastError() == Reader::NACK) {
        return false;
    }
    memcpy(&cmd[0], &delta, 4);
    reader->calculateCrc(cmd, 4, &cmd[4]);
    reader->tranceive(cmd, cmd, 6);
    return reader->getLastError() != Reader::NACK;
}

bool MifareClassic::transfer(unsigned char address) {
    unsigned char cmd[4] = { TRANSFER, address, 0, 0 };
    if (key != NULL && !authenticate(address, keyType, key)) {
        return false;
    }
    reader->calculateCrc(cmd, 2, &cmd[2]);
    reader->tranceive(cmd, cmd, 4);
    return reader->getLastError() == Reader::NO_ERROR;
}

bool MifareClassic::createValueBlock(unsigned char address, uint32_t value, uint8_t addr) {
    Access access;
    unsigned char buf[18];
    if (!getAccessCondition(address, &access) || (access != CONDITION_1 && access != CONDITION_6)) {
        return false;
    }
    fillValueBlock(buf, value, addr);
    return writeBlock(address, buf);
}

bool MifareClassic::readAccessBits(unsigned char sector, unsigned char *accessBits) {
    return readBlockSlice(getSectorTrailerAddress(sector), MIFARE_CLASSIC_ACCESS_BITS_POSITION, MIFARE_CLASSIC_ACCESS_BITS_SIZE, accessBits);
}

bool MifareClassic::writeAccessBits(unsigned char sector, unsigned char *accessBits, unsigned char *keyA, unsigned char *keyB) {
    unsigned char buf[18];
    if (!isAccessBitsCorrect(accessBits)) {
        return false;
    }
    memcpy(&buf[MIFARE_CLASSIC_KEY_TYPE_TO_POS(KEY_A)], keyA, MIFARE_CLASSIC_KEY_SIZE);
    memcpy(&buf[MIFARE_CLASSIC_ACCESS_BITS_POSITION], accessBits, MIFARE_CLASSIC_ACCESS_BITS_SIZE);
    memcpy(&buf[MIFARE_CLASSIC_KEY_TYPE_TO_POS(KEY_B)], keyB, MIFARE_CLASSIC_KEY_SIZE);
    return writeBlockSlice(getSectorTrailerAddress(sector), 0, MIFARE_CLASSIC_BLOCK_SIZE, buf);
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
    unsigned char buf[18];
    unsigned address = getSectorTrailerAddress(sector);
    if (!readBlock(address, buf)) {
        return false;
    }
    memcpy(&buf[MIFARE_CLASSIC_KEY_TYPE_TO_POS(KEY_A)], keyA, MIFARE_CLASSIC_KEY_SIZE);
    memcpy(&buf[MIFARE_CLASSIC_KEY_TYPE_TO_POS(KEY_B)], keyB, MIFARE_CLASSIC_KEY_SIZE);
    return writeBlock(address, buf);
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
    Serial.println(address);
    Serial.println(addressToSector(address));
    Serial.println(getBlockCountInSector(addressToSector(address)));
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
