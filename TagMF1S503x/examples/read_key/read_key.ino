#include <ReaderMFRC522.h>
#include <RegisterBasedSPIDevice.h>
#include <MifareClassic.h>
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
    Serial.println("Initializing...");
    reader.initialize();
    tag.setupAuthenticationKey(MifareClassic::KEY_A, keyA);
    Serial.println("Waiting card proximity...");
}

void loop() {
    if (tag.activate()) {
        Serial.println("Card detected.");
        Serial.println("Reading key...");
        if (tag.readKey(SECTOR, MifareClassic::KEY_B, key)) {
            for (int i = 0; i < KEY_SIZE; i++) {
                Serial.print(key[i], HEX);
                Serial.print(" ");
            }
        }
        Serial.println();
        tag.halt();
        delay(1000);
    }
}
