#include <ReaderMFRC522.h>
#include <RegisterBasedSPIDevice.h>
#include <TagMF1S503x.h>

#define SS_PIN      10
#define RST_PIN     3

RegisterBasedSPIDevice device(SS_PIN);
ReaderMFRC522 reader(&device, RST_PIN);
TagMF1S503x tag(&reader);

unsigned char keyA[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

void setup() {
    Serial.begin(9600);
    Serial.println("initialing");
    reader.initialize();
    tag.setupAuthenticationKey(Tag::KEY_A, keyA);
    Serial.println("done");
}

void loop() {

    unsigned char writeBuf[18] = { 2,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4 };
    unsigned char readBuf[18] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

    if (tag.activate()) {
        Serial.println("Card detected");
        if (tag.writeBlock(1, writeBuf)) {
            Serial.println("Write operation successfully performed to block 1.");
        }
        if (tag.readBlock(1, readBuf)) {
            Serial.println("Read operation successfully performed from block 1.");
            for (int i = 0; i < 16; i++) {
                Serial.print(readBuf[i], HEX);
                Serial.print(" ");
            }
        }
        tag.halt();
        delay(2000);
    }
}