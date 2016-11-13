#include "TagMF1ICS70.h"
#include <Reader.h>

TagMF1ICS70::TagMF1ICS70(Reader *reader)
        : Tag(reader) {
}

bool TagMF1ICS70::writeBlock(unsigned char address, unsigned char *buf) {
    if (getTagType() != MIFARE_4K) {
        return false;
    }
    return Tag::writeBlock(address, buf);
}

unsigned char TagMF1ICS70::getSectorSize(unsigned char sector) {
    unsigned char size = TAG_MF1ICS70_LOW_SECTOR_SIZE;
    if (sector >= TAG_MF1ICS70_LOW_SECTOR_COUNT) {
        size = TAG_MF1ICS70_HIGH_SECTOR_SIZE;
    }
    return size;
}

unsigned char TagMF1ICS70::addressToSector(unsigned char address) {
    unsigned char sector = address / TAG_MF1ICS70_LOW_SECTOR_SIZE;
    if (address >= TAG_MF1ICS70_LOW_MEMORY_SIZE) {
        address -= TAG_MF1ICS70_LOW_MEMORY_SIZE;
        sector = TAG_MF1ICS70_LOW_SECTOR_COUNT + (address / TAG_MF1ICS70_HIGH_SECTOR_SIZE);
    }
    return sector;
}

unsigned char TagMF1ICS70::addressToBlock(unsigned char address) {
    unsigned char block = address % TAG_MF1ICS70_LOW_SECTOR_SIZE;
    if (address >= TAG_MF1ICS70_LOW_MEMORY_SIZE) {
        address -= TAG_MF1ICS70_LOW_MEMORY_SIZE;

        // For the first 32 sectors (first 2 Kbytes of NV-memory) the access conditions can be set individually for a data area sized one block.
        // For the last 8 sectors (upper 2 Kbytes of NV-memory) access conditions can be set individually for a data area sized 5 blocks.
        block = (address % TAG_MF1ICS70_HIGH_SECTOR_SIZE) / 5;
    }
    return block;
}

unsigned char TagMF1ICS70::isAddressSectorTrailer(unsigned char address) {
    unsigned char sectorSize = getSectorSize(addressToSector(address));
    if (address >= TAG_MF1ICS70_LOW_MEMORY_SIZE) {
        address -= TAG_MF1ICS70_LOW_MEMORY_SIZE;
    }
    return ((address % sectorSize) == (sectorSize - 1));
}

unsigned char TagMF1ICS70::getSectorTrailerAddress(unsigned char sector) {
    unsigned char sectorSize, offset = 0;
    sectorSize = getSectorSize(sector);
    if (sector >= TAG_MF1ICS70_LOW_SECTOR_COUNT) {
        offset = TAG_MF1ICS70_LOW_MEMORY_SIZE;
        sector -= TAG_MF1ICS70_LOW_SECTOR_COUNT;
    }
    return offset + (sector * sectorSize) + (sectorSize - 1);
}
