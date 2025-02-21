#include <SoftwareSerial.h>
#include <TinyGPS++.h>

const int piezoPinA0 = A0;  // First piezoelectric sensor
const int piezoPinA1 = A1;  // Second piezoelectric sensor
const int ledPin = 13;      // Flickering LED (alerts)
const int ledPin2 = 11;     // Constant LED (normal operation)

const float freqThresholdLow = 20.0;  
const float freqThresholdHigh = 60.0;  

const int sampleWindow = 1000; // 1 second sample window

// GPS setup
SoftwareSerial gpsSerial(8, 9);
TinyGPSPlus gps;
float latitude, longitude;

void setup() {
  Serial.begin(9600);  
  gpsSerial.begin(9600);  
  pinMode(piezoPinA0, INPUT);
  pinMode(piezoPinA1, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);
}

void loop() {
  updateGPS();
  
  unsigned long startMillis = millis();
  int cyclesA0 = 0, cyclesA1 = 0;
  int adcValueA0, adcValueA1;
  float maxAmplitudeA0 = 0.0, maxAmplitudeA1 = 0.0;

  // Measure vibrations over 1 second
  while (millis() - startMillis < sampleWindow) {
    adcValueA0 = analogRead(piezoPinA0);
    adcValueA1 = analogRead(piezoPinA1);

    maxAmplitudeA0 = max(maxAmplitudeA0, adcValueA0 * (5.0 / 1023.0));
    maxAmplitudeA1 = max(maxAmplitudeA1, adcValueA1 * (5.0 / 1023.0));

    if (adcValueA0 > 512) {  
      while (analogRead(piezoPinA0) > 512);  
      while (analogRead(piezoPinA0) < 512);  
      cyclesA0++;
    }
    if (adcValueA1 > 512) {  
      while (analogRead(piezoPinA1) > 512);  
      while (analogRead(piezoPinA1) < 512);  
      cyclesA1++;
    }
  }

  float frequencyA0 = (float)cyclesA0 / (sampleWindow / 1000.0);
  float frequencyA1 = (float)cyclesA1 / (sampleWindow / 1000.0);

  float avgSensorValue = (adcValueA0 + adcValueA1) / 2.0;
  float avgAmplitude = (maxAmplitudeA0 + maxAmplitudeA1) / 2.0;
  float avgFrequency = (frequencyA0 + frequencyA1) / 2.0;

  // *** FORMATTED OUTPUT ***
  Serial.println("==========================================");
  Serial.println("          PIEZOELECTRIC SENSOR DATA       ");
  Serial.println("==========================================");
  
  Serial.print("Sensor A0 Value: "); Serial.print(adcValueA0);
  Serial.print("  |  Sensor A1 Value: "); Serial.print(adcValueA1);
  Serial.print("  |  Avg Value: "); Serial.println(avgSensorValue);

  Serial.print("Max Amplitude A0: "); Serial.print(maxAmplitudeA0, 2);
  Serial.print(" V  |  Max Amplitude A1: "); Serial.print(maxAmplitudeA1, 2);
  Serial.print(" V  |  Avg Amplitude: "); Serial.print(avgAmplitude, 2);
  Serial.println(" V");

  Serial.print("Frequency A0: "); Serial.print(frequencyA0, 2);
  Serial.print(" Hz  |  Frequency A1: "); Serial.print(frequencyA1, 2);
  Serial.print(" Hz  |  Avg Frequency: "); Serial.print(avgFrequency, 2);
  Serial.println(" Hz");

  Serial.println("==========================================");

  // *** GPS DATA SECTION ***
  // Serial.println("          GPS LOCATION DATA              ");
  // Serial.println("==========================================");
  // if (gps.location.isValid()) {
  //   Serial.print("Latitude: "); Serial.print(latitude, 6);
  //   Serial.print("  |  Longitude: "); Serial.println(longitude, 6);
  // } else {
  //   Serial.println("GPS: No Fix");
  // }
  // Serial.println("==========================================");

  // *** ANOMALY DETECTION ***
  Serial.println("          ANOMALY DETECTION              ");
  Serial.println("==========================================");

  if (cyclesA0 == 0 && cyclesA1 == 0) {
    Serial.println("No frequency detected. System in normal state.");
    digitalWrite(ledPin, LOW);
    digitalWrite(ledPin2, HIGH);
  } else {
    if (avgFrequency < freqThresholdLow || avgFrequency > freqThresholdHigh) {
      Serial.println("⚠️  Anomaly Detected! ⚠️");
      digitalWrite(ledPin2, LOW);
      flickerLED();
    } else {
      Serial.println("✅ Normal Operation - No Anomaly Detected.");
      digitalWrite(ledPin, LOW);
      digitalWrite(ledPin2, HIGH);
    }
  }
  Serial.println("==========================================\n");

  delay(1000);
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

void flickerLED() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
  }
}
