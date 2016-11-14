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

bool MifareUltralight::writeBlockSlice(unsigned char address, unsigned char from, unsigned char len, unsigned char *buf) {
    unsigned char receive[18];
    if (len == 0 || from + len > TAG_BLOCK_SIZE) {
        return false;
    }
    if (from == 0 && len == TAG_BLOCK_SIZE && !readBlock(address, receive)) {
        return false;
    }
    memcpy(&receive[from], buf, len);
    return writeBlock(address, receive);
}

int MifareUltralight::readByte(unsigned char address, unsigned char pos) {
    unsigned char buf[18];
    if (!readBlock(address, buf)) {
        return -1;
    }
    return buf[pos];
}

bool MifareUltralight::writeByte(unsigned char address, unsigned char pos, unsigned char value) {
    unsigned char buf[18];
    if (!readBlock(address, buf)) {
        return false;
    }
    buf[pos] = value;
    return writeBlock(address, buf);
}
