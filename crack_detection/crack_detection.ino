#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>


#define SCREEN_WIDTH 128  
#define SCREEN_HEIGHT 64  
#define OLED_RESET    -1  
#define SCREEN_ADDRESS 0x3C  

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define PIEZO_PIN A0   // Piezo sensor connected to A0
#define RED_LED D5     // Red LED for anomaly
#define WHITE_LED D6   // White LED for normal operation

const int sampleWindow = 1000;  
const float voltageReference = 3.3;  
const float freqThresholdLow = 5.0;  
const float freqThresholdHigh = 50.0; 

#define RXD2 D7  // GPS TX to ESP8266 D7 (GPIO13)
#define TXD2 D8  // GPS RX to ESP8266 D8 (GPIO15) (Use voltage divider)
SoftwareSerial gpsSerial(RXD2, TXD2);
TinyGPSPlus gps;
float latitude, longitude;

void setup() {
    Serial.begin(115200);
    gpsSerial.begin(9600);

    pinMode(RED_LED, OUTPUT);
    pinMode(WHITE_LED, OUTPUT);

    digitalWrite(WHITE_LED, HIGH);
    digitalWrite(RED_LED, HIGH);
    delay(1000);
    digitalWrite(RED_LED, LOW);
    
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;);
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 10);
    display.println("Piezo & GPS Init...");
    display.display();
    delay(1000);

    Serial.println("System Initialized...");
}

void loop() {
    updateGPS();

    unsigned long startMillis = millis();
    int cycles = 0;
    int sensorValue;
    float maxAmplitude = 0.0;
    float minAmplitude = voltageReference;

    while (millis() - startMillis < sampleWindow) {
        sensorValue = analogRead(PIEZO_PIN);
        float voltage = sensorValue * (voltageReference / 1023.0);
        maxAmplitude = max(maxAmplitude, voltage);
        minAmplitude = min(minAmplitude, voltage);

        if (sensorValue > 512) {  
            while (analogRead(PIEZO_PIN) > 512);
            while (analogRead(PIEZO_PIN) < 512);
            cycles++;
        }
    }

    float frequency = (float)cycles / (sampleWindow / 1000.0);
    float amplitude = maxAmplitude - minAmplitude;

    Serial.println("==========================================");
    Serial.print("ADC: "); Serial.println(sensorValue);
    Serial.print("Freq: "); Serial.print(frequency, 2); Serial.println(" Hz");
    Serial.print("GPS: ");
    if (gps.location.isValid()) {
        Serial.print(latitude, 6); Serial.print(", "); Serial.println(longitude, 6);
    } else {
        Serial.println("No Fix");
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 10);
    display.print("Freq: ");
    display.setTextSize(2);
    display.setCursor(50, 10);
    display.print(frequency, 1);
    display.print(" Hz");

    display.setTextSize(1);
    display.setCursor(0, 30);
    display.print("GPS: ");
    display.setTextSize(1);
    display.setCursor(40, 30);
    if (gps.location.isValid()) {
        display.print(latitude, 2);
        display.print(",");
        display.print(longitude, 2);
    } else {
        display.print("No Fix");
    }

    if (frequency < freqThresholdLow || frequency > freqThresholdHigh) {
        Serial.println("⚠  Anomaly Detected! ⚠");
        digitalWrite(WHITE_LED, LOW);
        blinkRedLED();
        display.setTextSize(2);
        display.setCursor(20, 50);
        display.print("Anomaly!");
    } else {
        Serial.println("✅ Normal Operation");
        digitalWrite(RED_LED, LOW);
        digitalWrite(WHITE_LED, HIGH);
        display.setTextSize(2);
        display.setCursor(20, 50);
        display.print("Normal");
    }

    display.display();
    delay(500);
}

void updateGPS() {
    while (gpsSerial.available() > 0) {
        if (gps.encode(gpsSerial.read())) {
            if (gps.location.isValid()) {
                latitude = gps.location.lat();
                longitude = gps.location.lng();
            }
        }
    }
}

void blinkRedLED() {
    for (int i = 0; i < 5; i++) {
        digitalWrite(RED_LED, HIGH);
        delay(100);
        digitalWrite(RED_LED, LOW);
        delay(100);
    }
}
