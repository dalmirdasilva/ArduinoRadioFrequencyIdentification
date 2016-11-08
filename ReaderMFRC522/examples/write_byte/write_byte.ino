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

    unsigned char keyA[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

    if (tag.activate()) {
        Serial.println(" Card detected: ");
        Tag::Uid uid = tag.getUid();
        for (int i = 0; i < uid.size; i++) {
            Serial.println(uid.uid[i], HEX);
        }
        if (tag.authenticate(TagMF1S503x::KEY_A, 1, keyA)) {
            Serial.println("Successfully authenticated. \n Byte read is: ");
            Serial.println(tag.readByte(1, 10));
            tag.halt();
        }
    } else {
        Serial.println(reader.getLastError());
    }
    delay(100);
}