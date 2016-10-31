#include <RegisterBasedSPIDevice.h>
#include <RadioFrequencyIdentificationMFRC522.h>

#define SS_PIN      10
#define RST_PIN     3

RegisterBasedSPIDevice device(SS_PIN);
RadioFrequencyIdentificationMFRC522 rfid(&device, RST_PIN);

void setup() {
    Serial.begin(9600);
    Serial.println("initialing");
    rfid.initialize();
    Serial.println("done");
}

void loop() {
    if (rfid.sendRequestTypeA()) {
        Serial.println("Card detected.");
    }
}