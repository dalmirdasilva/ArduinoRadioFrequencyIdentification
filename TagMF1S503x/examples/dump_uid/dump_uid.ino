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
    Serial.println("Initializing...");
    reader.initialize();
    tag.setupAuthenticationKey(Tag::KEY_A, keyA);
    Serial.println("Waiting card proximity...");
}

void loop() {
    if (tag.activate()) {
        Serial.print("Card detected.\nuid: ");
        Tag::Uid uid = tag.getUid();
        for (int i = 0; i < uid.size; i++) {
            Serial.print(uid.uid[i], HEX);
            Serial.print(" ");
        }
        Serial.println("\nDone.");
        tag.halt();
        delay(1000);
    }
}
