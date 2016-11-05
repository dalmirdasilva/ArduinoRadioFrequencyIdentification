#include <Reader.h>

Reader::Reader() : lastError(NO_ERROR) {
}

Reader::~Reader() {
}

unsigned char Reader::getLastError() {
    return lastError;
}

void Reader::clearLastError() {
    lastError = NO_ERROR;
}
