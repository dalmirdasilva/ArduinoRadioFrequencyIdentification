#include <Arduino.h>
#include <RegisterBasedSPIDevice.h>
#include <RadioFrequencyIdentificationMFRC522.h>

#define SS_PIN      10
#define RST_PIN     3
#define LED_PIN     A1

RegisterBasedSPIDevice device(SS_PIN);
RadioFrequencyIdentificationMFRC522 reader(&device, RST_PIN);

void setup() {
    Serial.begin(9600);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.println("initializing");

    reader.initialize();

    Serial.print("version: ");
    switch (reader.getVersion()) {
    case RadioFrequencyIdentificationMFRC522::CLONE:
        Serial.println("clone");
        break;
    case RadioFrequencyIdentificationMFRC522::V0_0:
        Serial.println("v0_0");
        break;
    case RadioFrequencyIdentificationMFRC522::V1_0:
        Serial.println("v1_0");
        break;
    case RadioFrequencyIdentificationMFRC522::V2_0:
        Serial.println("v2_0");
        break;
    default:
        return false;
    }

    Serial.print("self test: ");
    if (reader.performSelfTest()) {
        digitalWrite(LED_PIN, HIGH);
        Serial.println("passed");
    } else {
        Serial.println("<failed>");
    }
    
    Serial.println("done");
}

void loop() {
}