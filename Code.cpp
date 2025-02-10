#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define TX_PIN 10
#define RX_PIN 11
#define GSM_TX 7
#define GSM_RX 8

Adafruit_MPU6050 mpu;
TinyGPSPlus gps;
SoftwareSerial gpsSerial(TX_PIN, RX_PIN);
SoftwareSerial gsmSerial(GSM_TX, GSM_RX);

void setup() {
    Serial.begin(115200);
    gpsSerial.begin(9600);
    gsmSerial.begin(9600);

    if (!mpu.begin()) {
        Serial.println("Failed to initialize MPU6050 sensor!");
        while (1);
    }
    Serial.println("MPU6050 initialized.");
}

void loop() {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    float accelThreshold = 2.5;
    if (abs(a.acceleration.x) > accelThreshold || abs(a.acceleration.y) > accelThreshold || abs(a.acceleration.z) > accelThreshold) {
        Serial.println("Accident detected!");
        sendAlert();
    }

    while (gpsSerial.available() > 0) {
        gps.encode(gpsSerial.read());
    }
    delay(1000);
}

void sendAlert() {
    float latitude = gps.location.lat();
    float longitude = gps.location.lng();
    
    Serial.print("Latitude: ");
    Serial.println(latitude, 6);
    Serial.print("Longitude: ");
    Serial.println(longitude, 6);
    
    gsmSerial.println("AT+CMGF=1");
    delay(1000);
    gsmSerial.println("AT+CMGS=\"+1234567890\""); // Replace with emergency number
    delay(1000);
    gsmSerial.print("Accident detected! Location: ");
    gsmSerial.print("https://maps.google.com/?q=");
    gsmSerial.print(latitude, 6);
    gsmSerial.print(",");
    gsmSerial.print(longitude, 6);
    gsmSerial.write(26);
    delay(5000);
}
