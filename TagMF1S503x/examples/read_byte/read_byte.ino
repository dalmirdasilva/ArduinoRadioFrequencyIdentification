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
    if (tag.activate()) {
        Serial.println("Card detected");
        Serial.print("Read byte is: ");
        Serial.println(tag.readByte(1, 0), HEX);
        if (reader.getLastError() != Reader::NO_ERROR) {
            Serial.print("Last error was: ");
            Serial.println(reader.getLastError());
        }
        tag.halt();
        delay(2000);
    }
}