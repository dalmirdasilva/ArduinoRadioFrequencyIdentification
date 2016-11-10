#include <ReaderMFRC522.h>
#include <RegisterBasedSPIDevice.h>
#include <TagMF1S503x.h>

#define SS_PIN          10
#define RST_PIN         3

#define KEY_SIZE        6
#define SECTOR          1

RegisterBasedSPIDevice device(SS_PIN);
ReaderMFRC522 reader(&device, RST_PIN);
TagMF1S503x tag(&reader);

unsigned char keyA[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
unsigned char writeKey[KEY_SIZE] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };
unsigned char readKey[KEY_SIZE] = { 0x00 };

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
        Serial.println("Writing key...");
        if (tag.writeKey(SECTOR, Tag::KEY_B, writeKey)) {
            Serial.println("Reading key...");
            if (tag.readKey(SECTOR, Tag::KEY_B, readKey)) {
                for (int i = 0; i < KEY_SIZE; i++) {
                    Serial.print(readKey[i], HEX);
                    Serial.print(" ");
                }
            }
        }
        if (reader.getLastError() != Reader::NO_ERROR) {
            Serial.print("Last error was: ");
            Serial.println(reader.getLastError());
        }
        Serial.println();
        tag.halt();
        delay(2000);
    }
}
