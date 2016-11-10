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

    unsigned char buf[18] = { 0x22, 0x11, 0x11, 0x11, 0x22, 0x22, 0x1e, 0x1f, 0xa1, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18 };

    if (tag.activate()) {
        Serial.println("Card detected: ");
        Serial.println("Successfully authenticated.");
        if (tag.writeBlock(1, buf)) {
            Serial.print("data wrote successfully.");
        } else {
            Serial.print("error when writing: ");
            Serial.println(reader.getLastError());
        }
        tag.halt();
        delay(2000);
    } else {
        Serial.println(reader.getLastError());
    }
    delay(100);
}