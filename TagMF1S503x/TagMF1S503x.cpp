#include "TagMF1S503x.h"
#include <Reader.h>

TagMF1S503x::TagMF1S503x(Reader *reader)
        : TagMifareClassic(reader) {
}

bool TagMF1S503x::writeBlock(unsigned char address, unsigned char *buf) {
    if (getTagType() != Tag::MIFARE_1K) {
        return false;
    }
    return TagMifareClassic::writeBlock(address, buf);
}

unsigned int TagMF1S503x::getSize() {
    return TAG_MF1S503X_SIZE;
}

unsigned char TagMF1S503x::getSectorCount() {
    return TAG_MF1S503X_SECTOR_COUNT;
}

unsigned int TagMF1S503x::getBlockCount() {
    return TAG_MF1S503X_BLOCK_COUNT;
}

unsigned char TagMF1S503x::getBlockCountInSector(unsigned char sector) {
    return TAG_MF1S503X_BLOCK_COUNT_IN_SECTOR;
}

unsigned int TagMF1S503x::getSectorSize(unsigned char sector) {
    return TAG_MF1S503X_SECTOR_SIZE;
}

unsigned char TagMF1S503x::addressToSector(unsigned char address) {
    return address / TAG_MF1S503X_BLOCK_COUNT_IN_SECTOR;
}

unsigned char TagMF1S503x::addressToBlock(unsigned char address) {
    return address % TAG_MF1S503X_BLOCK_COUNT_IN_SECTOR;
}

unsigned char TagMF1S503x::isAddressSectorTrailer(unsigned char address) {
    return ((address % TAG_MF1S503X_BLOCK_COUNT_IN_SECTOR) == (TAG_MF1S503X_BLOCK_COUNT_IN_SECTOR - 1));
}

unsigned char TagMF1S503x::getSectorTrailerAddress(unsigned char sector) {
    return (sector * TAG_MF1S503X_BLOCK_COUNT_IN_SECTOR) + (TAG_MF1S503X_BLOCK_COUNT_IN_SECTOR - 1);
}
