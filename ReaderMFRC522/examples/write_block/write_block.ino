#include <ReaderMFRC522.h>
#include <RegisterBasedSPIDevice.h>
#include <TagMF1S503x.h>

#define SS_PIN      10
#define RST_PIN     3

RegisterBasedSPIDevice device(SS_PIN);
ReaderMFRC522 reader(&device, RST_PIN);
TagMF1S503x tag(&reader);

void setup() {
    Serial.begin(9600);
    Serial.println("initialing");
    reader.initialize();
    Serial.println("done");
}

void loop() {

    unsigned char buf[18] = { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };
    unsigned char keyA[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

    if (tag.activate()) {
        Serial.println(" Card detected: ");
        Tag::Uid uid = tag.getUid();
        for (int i = 0; i < uid.size; i++) {
            Serial.println(uid.uid[i], HEX);
        }
        if (tag.authenticate(TagMF1S503x::KEY_A, 1, keyA)) {
            Serial.println("Successfully authenticated.");
            if (tag.writeBlock(1, buf)) {
                Serial.print("data wrote successfully.");
            } else {
                Serial.print("error when reading: ");
                Serial.println(reader.getLastError());
            }
            Serial.println();
            Serial.println(tag.halt());
        }
    } else {
        Serial.println(reader.getLastError());
    }
    delay(100);
}
