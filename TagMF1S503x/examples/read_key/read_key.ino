#include <ReaderMFRC522.h>
#include <RegisterBasedSPIDevice.h>
#include <TagMF1S503x.h>

#define SS_PIN          10
#define RST_PIN         3

#define SECTOR          4
#define KEY_SIZE        6

RegisterBasedSPIDevice device(SS_PIN);
ReaderMFRC522 reader(&device, RST_PIN);
TagMF1S503x tag(&reader);

unsigned char keyA[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
unsigned char key[KEY_SIZE];

void setup() {
    Serial.begin(9600);
    Serial.println("Initialing...");
    reader.initialize();
    tag.setupAuthenticationKey(Tag::KEY_A, keyA);
    Serial.println("Waiting card proximity...");
}

void loop() {
    if (tag.activate()) {
        Serial.println("Card detected.");
        Serial.println("Reading key...");
        if (tag.readKey(SECTOR, Tag::KEY_B, key)) {
            for (int i = 0; i < KEY_SIZE; i++) {
                Serial.print(key[i], HEX);
                Serial.print(" ");
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
