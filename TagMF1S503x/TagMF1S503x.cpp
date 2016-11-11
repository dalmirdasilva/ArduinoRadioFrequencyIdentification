#include "TagMF1S503x.h"
#include <Reader.h>

TagMF1S503x::TagMF1S503x(Reader *reader)
        : Tag(reader) {
}

unsigned char TagMF1S503x::getSectorSize(unsigned char sector) {
    return TAG_MF1S503X_SECTOR_SIZE;
}

unsigned char TagMF1S503x::addressToSector(unsigned char address) {
    return address / TAG_MF1S503X_SECTOR_SIZE;
}

unsigned char TagMF1S503x::isAddressSectorTrailer(unsigned char address) {
    return ((address % TAG_MF1S503X_SECTOR_SIZE) == (TAG_MF1S503X_SECTOR_SIZE - 1));
}

unsigned char TagMF1S503x::getSectorTrailerAddress(unsigned char sector) {
    return (sector * TAG_MF1S503X_SECTOR_SIZE) + (TAG_MF1S503X_SECTOR_SIZE - 1);
}
