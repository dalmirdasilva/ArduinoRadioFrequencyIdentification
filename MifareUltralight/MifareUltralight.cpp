#include "../MifareUltralight/MifareUltralight.h"

#include <Reader.h>

MifareUltralight::MifareUltralight(Reader *reader)
        : Tag(reader) {
}

bool MifareUltralight::readBlock(unsigned char address, unsigned char *buf) {
    return false;
}

bool MifareUltralight::writeBlock(unsigned char address, unsigned char *buf) {
    return false;
}

bool MifareUltralight::readBlockSlice(unsigned char address, unsigned char from, unsigned char len, unsigned char *buf) {
    return false;
}

bool MifareUltralight::writeBlockSlice(unsigned char address, unsigned char from, unsigned char len, unsigned char *buf) {
    return false;
}

int MifareUltralight::readByte(unsigned char address, unsigned char pos) {
    return false;
}

bool MifareUltralight::writeByte(unsigned char address, unsigned char pos, unsigned char value) {
    return false;
}
