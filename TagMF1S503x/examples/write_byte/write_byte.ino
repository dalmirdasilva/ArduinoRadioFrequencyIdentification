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

    unsigned char readBuf[18] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

    if (tag.activate()) {
        Serial.println("Card detected");
        if (tag.writeByte(1, 0, 0xf1)) {
            Serial.println("Write a single byte with value 0xf1 successfully to block 1 at position 0.");
        } else {
            Serial.println(reader.getLastError());
        }
        if (tag.readBlock(1, readBuf)) {
            Serial.println("Read operation successfully performed from block 1.");
            for (int i = 0; i < 16; i++) {
                Serial.print(readBuf[i], HEX);
                Serial.print(" ");
            }
        } else {
            Serial.println(reader.getLastError());
        }
        tag.halt();
        delay(2000);
    }
}