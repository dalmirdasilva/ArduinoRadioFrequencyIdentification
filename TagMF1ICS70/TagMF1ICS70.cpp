#include "TagMF1ICS70.h"
#include <Reader.h>

TagMF1ICS70::TagMF1ICS70(Reader *reader)
        : TagMifareClassic(reader) {
}

bool TagMF1ICS70::writeBlock(unsigned char address, unsigned char *buf) {
    if (getTagType() != Tag::MIFARE_4K) {
        return false;
    }
    return TagMifareClassic::writeBlock(address, buf);
}

unsigned int TagMF1ICS70::getSize() {
    return TAG_MF1ICS70_SIZE;
}

unsigned char TagMF1ICS70::getSectorCount() {
    return TAG_MF1ICS70_LOW_SECTOR_COUNT + TAG_MF1ICS70_HIGH_SECTOR_COUNT;
}

unsigned int TagMF1ICS70::getBlockCount() {
    return TAG_MF1ICS70_LOW_BLOCK_COUNT + TAG_MF1ICS70_HIGH_BLOCK_COUNT;
}

unsigned char TagMF1ICS70::getBlockCountInSector(unsigned char sector) {
    return (sector >= TAG_MF1ICS70_LOW_SECTOR_COUNT) ? TAG_MF1ICS70_HIGH_BLOCK_COUNT_IN_SECTOR : TAG_MF1ICS70_LOW_BLOCK_COUNT_IN_SECTOR;
}

unsigned int TagMF1ICS70::getSectorSize(unsigned char sector) {
    return (sector >= TAG_MF1ICS70_LOW_SECTOR_COUNT) ? TAG_MF1ICS70_HIGH_SECTOR_SIZE : TAG_MF1ICS70_LOW_SECTOR_SIZE;
}

unsigned char TagMF1ICS70::addressToSector(unsigned char address) {
    unsigned char sector = address / TAG_MF1ICS70_LOW_BLOCK_COUNT_IN_SECTOR;
    if (address >= TAG_MF1ICS70_LOW_BLOCK_COUNT) {
        address -= TAG_MF1ICS70_LOW_BLOCK_COUNT;
        sector = TAG_MF1ICS70_LOW_SECTOR_COUNT + (address / TAG_MF1ICS70_HIGH_BLOCK_COUNT_IN_SECTOR);
    }
    return sector;
}

unsigned char TagMF1ICS70::addressToBlock(unsigned char address) {
    unsigned char block = address % TAG_MF1ICS70_LOW_SECTOR_COUNT;
    if (address >= TAG_MF1ICS70_LOW_BLOCK_COUNT) {
        address -= TAG_MF1ICS70_LOW_BLOCK_COUNT;

        // For the first 32 sectors (first 2 Kbytes of NV-memory) the access conditions can be set individually for a data area sized one block.
        // For the last 8 sectors (upper 2 Kbytes of NV-memory) access conditions can be set individually for a data area sized 5 blocks.
        block = (address % TAG_MF1ICS70_HIGH_SECTOR_COUNT) / 5;
    }
    return block;
}

unsigned char TagMF1ICS70::isAddressSectorTrailer(unsigned char address) {
    unsigned char blockCount = getBlockCountInSector(addressToSector(address));
    if (address >= TAG_MF1ICS70_LOW_BLOCK_COUNT) {
        address -= TAG_MF1ICS70_LOW_BLOCK_COUNT;
    }
    return ((address % blockCount) == (blockCount - 1));
}

unsigned char TagMF1ICS70::getSectorTrailerAddress(unsigned char sector) {
    unsigned char blockCount, offset = 0;
    blockCount = getBlockCountInSector(sector);
    if (sector >= TAG_MF1ICS70_LOW_SECTOR_COUNT) {
        offset = TAG_MF1ICS70_LOW_BLOCK_COUNT;
        sector -= TAG_MF1ICS70_LOW_SECTOR_COUNT;
    }
    return offset + (sector * blockCount) + (blockCount - 1);
}
