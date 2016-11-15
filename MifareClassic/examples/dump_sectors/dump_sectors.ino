#include <ReaderMFRC522.h>
#include <RegisterBasedSPIDevice.h>
#include <MifareClassic.h>

#define SS_PIN      10
#define RST_PIN     3

RegisterBasedSPIDevice device(SS_PIN);
ReaderMFRC522 reader(&device, RST_PIN);
MifareClassic tag(&reader);

unsigned char keyA[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
unsigned char buffer[18];

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
        for (int address = 63; address >= 0; address--) {
            if ((address + 1) % 4 == 0) {
                Serial.print("\nSector: ");
                Serial.println(address / 4);
            }
            if (tag.readBlock(address, buffer)) {
                for (int j = 0; j < 16; j++) {
                    if (buffer[j] < 0x10) {
                        Serial.print("0");
                    }
                    Serial.print(buffer[j], HEX);
                    Serial.print(" ");
                }
                Serial.println();
            } else {
                Serial.println("Read error.");
            }
        }
        tag.halt();
        delay(1000);
    }
}
