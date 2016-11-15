#include <Arduino.h>
#include <Tag.h>
#include <MifareClassic.h>

MifareClassic tag(NULL, Tag::TYPE_CLASSIC, Tag::SIZE_4K);

void is(unsigned int a, unsigned int b, const char *msg) {
    Serial.println(msg);
    if (a == b) {
        Serial.println("PASSED!");
    } else {
        Serial.print("Expected ");
        Serial.print(a);
        Serial.print(" to be ");
        Serial.print(b);
        Serial.println(" failed.");
    }
    Serial.println();
}

void setup() {

    Serial.begin(9600);
    Serial.println("running tests...");

    // getSize
    is(tag.getSize(), 4096, "getSize#0");

    // getSectorCount

    is(tag.getSectorCount(), 40, "getSectorCount#0");

    // getBlockCount

    is(tag.getBlockCount(), 256, "getBlockCount#0");

    // getBlockCountInSector

    is(tag.getBlockCountInSector(0), 4, "getBlockCountInSector#0");

    is(tag.getBlockCountInSector(32), 16, "getBlockCountInSector#1");

    // getBlockCountInSector

    is(tag.getBlockCountInSector(0), 4, "getBlockCountInSector#0");

    is(tag.getBlockCountInSector(31), 4, "getBlockCountInSector#1");

    is(tag.getBlockCountInSector(32), 16, "getBlockCountInSector#2");

    is(tag.getBlockCountInSector(40), 16, "getBlockCountInSector#3");

    // getSectorSize

    is(tag.getSectorSize(0), 4 * 16, "getSectorSize#0");

    is(tag.getSectorSize(31), 4 * 16, "getSectorSize#1");

    is(tag.getSectorSize(32), 16 * 16, "getSectorSize#2");

    is(tag.getSectorSize(40), 16 * 16, "getSectorSize#3");

    // addressToSector

    is(tag.addressToSector(10), 2, "addressToSector#0");

    is(tag.addressToSector(4), 1, "addressToSector#1");

    is(tag.addressToSector(0), 0, "addressToSector#2");

    is(tag.addressToSector(12), 3, "addressToSector#3");

    is(tag.addressToSector(127), 31, "addressToSector#4");

    is(tag.addressToSector(128), 32, "addressToSector#5");

    is(tag.addressToSector(143), 32, "addressToSector#6");

    is(tag.addressToSector(144), 33, "addressToSector#7");

    is(tag.addressToSector(255), 39, "addressToSector#8");

    // isAddressSectorTrailer

    is(tag.isAddressSectorTrailer(12), 0, "isAddressSectorTrailer#0");

    is(tag.isAddressSectorTrailer(3), 1, "isAddressSectorTrailer#1");

    is(tag.isAddressSectorTrailer(19), 1, "isAddressSectorTrailer#2");

    is(tag.isAddressSectorTrailer(22), 0, "isAddressSectorTrailer#3");

    is(tag.isAddressSectorTrailer(127), 1, "isAddressSectorTrailer#4");

    is(tag.isAddressSectorTrailer(255), 1, "isAddressSectorTrailer#5");

    is(tag.isAddressSectorTrailer(254 - 16), 0, "isAddressSectorTrailer#6");

    // getSectorTrailerAddress

    is(tag.getSectorTrailerAddress(31), 127, "getSectorTrailerAddress#4");

    is(tag.getSectorTrailerAddress(32), 143, "getSectorTrailerAddress#5");

    is(tag.getSectorTrailerAddress(39), 255, "getSectorTrailerAddress#6");

    is(tag.getSectorTrailerAddress(1), 7, "getSectorTrailerAddress#1");

    is(tag.getSectorTrailerAddress(2), 11, "getSectorTrailerAddress#2");

    is(tag.getSectorTrailerAddress(3), 15, "getSectorTrailerAddress#3");

    is(tag.getSectorTrailerAddress(31), 127, "getSectorTrailerAddress#5");

    is(tag.getSectorTrailerAddress(32), 143, "getSectorTrailerAddress#6");

    is(tag.getSectorTrailerAddress(39), 255, "getSectorTrailerAddress#7");

    Serial.println("done.");
}

void loop() {

}

int main(void) {

    init();

#if defined(USBCON)
    USBDevice.attach();
#endif

    setup();

    for (;;) {
        loop();
        if (serialEventRun)
            serialEventRun();
    }

    return 0;
}
