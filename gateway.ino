#include <SPI.h>
#include <RF24.h>
#include <FirebaseESP8266.h>
#include <DHT.h>

#define CE_PIN D4  // Pin connected to the CE pin on the NRF24L01 module
#define CSN_PIN D8 // Pin connected to the CSN pin on the NRF24L01 module

RF24 radio(CE_PIN, CSN_PIN);

#define DHT_PIN D2     // Pin connected to the DHT sensor
#define DHT_TYPE DHT22 // Type of the DHT sensor

DHT dht(DHT_PIN, DHT_TYPE);

// Replace these with your Firebase project credentials
#define FIREBASE_HOST "your-firebase-project-id.firebaseio.com"
#define FIREBASE_AUTH "your-database-secret"
#define WIFI_SSID "your-wifi-ssid"
#define WIFI_PASSWORD "your-wifi-password"

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize NRF24L01 module
  radio.begin();
  radio.openWritingPipe(0xF0F0F0F0E1LL);

  // Initialize Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  // Initialize DHT sensor
  dht.begin();
}

void loop() {
  // Read DHT sensor data
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Send DHT sensor data to Firebase
  sendDataToFirebase(temperature, humidity);

  delay(1000); // Send data every 5 seconds
}

void sendDataToFirebase(float temperature, float humidity) {
  // Send temperature and humidity data to Firebase Realtime Database
  String path = "/sensor/data"; // Change this path as needed

  String temperaturePath = path + "/temperature";
  String humidityPath = path + "/humidity";

  Firebase.pushFloat(temperaturePath, temperature);
  Firebase.pushFloat(humidityPath, humidity);

  if (Firebase.failed()) {
    Serial.print("Sending data to Firebase failed: ");
    Serial.println(Firebase.error());
  } else {
    Serial.println("Data sent to Firebase");
  }
}
