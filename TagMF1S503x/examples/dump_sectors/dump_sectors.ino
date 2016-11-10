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
    unsigned char buffer[18];

    if (tag.activate()) {
        Serial.println("card detected: ");
        Tag::Uid uid = tag.getUid();
        for (int i = 0; i < uid.size; i++) {
            Serial.println(uid.uid[i], HEX);
        }
        bool error = false;
        for (int address = 63; !error && address >= 0; address--) {
            if ((address + 1) % 4 == 0) {
                Serial.print("\n sector: ");
                Serial.println(address / 4);
            }
            if (tag.authenticate(Tag::KEY_A, address, keyA)) {
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
                    Serial.println("read error.");
                    error = true;
                }
            } else {
                Serial.println("authentication error.");
                error = true;
            }
        }
        tag.halt();
    } else {
        Serial.println(reader.getLastError());
    }
    delay(100);
}