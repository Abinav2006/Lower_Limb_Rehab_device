#include <WiFi.h>
#include <WebSocketsServer.h>

// WiFi credentials
const char* ssid = "DESKTOP-1E2I2O3 8975";
const char* password = "-d30578C";

// WebSocket server
WebSocketsServer webSocket = WebSocketsServer(81);

// FSR pins
#define FSR1 2
#define FSR2 1
#define FSR3 0

// Calibration
int base1, base2, base3;
int maxVal = 3000;

// Averaging
int readFSR(int pin) {
  long sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(pin);
    delay(2);
  }
  return sum / 10;
}

// Convert to %
float getPercent(int value, int base) {
  value = constrain(value, base, maxVal);
  return (value - base) * 100.0 / (maxVal - base);
}

// WebSocket event
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  // not required for now
}

void setup() {
  Serial.begin(115200);

  analogReadResolution(12);
  analogSetPinAttenuation(FSR1, ADC_11db);
  analogSetPinAttenuation(FSR2, ADC_11db);
  analogSetPinAttenuation(FSR3, ADC_11db);

  // WiFi connect
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
  Serial.print("Status: ");
  Serial.println(WiFi.status());
  delay(1000);
}

  Serial.println("\nConnected!");
  Serial.println(WiFi.localIP());

  // Start WebSocket
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  // Baseline calibration
  delay(2000);
  base1 = analogRead(FSR1);
  base2 = analogRead(FSR2);
  base3 = analogRead(FSR3);

  Serial.println("Baseline captured");
}

void loop() {
  webSocket.loop();

  int fsr1 = readFSR(FSR1);
  int fsr2 = readFSR(FSR2);
  int fsr3 = readFSR(FSR3);

  float p1 = getPercent(fsr1, base1);
  float p2 = getPercent(fsr2, base2);
  float p3 = getPercent(fsr3, base3);

  // Create JSON data
  String data = "{";
  data += "\"fsr1\":" + String(fsr1) + ",";
  data += "\"fsr2\":" + String(fsr2) + ",";
  data += "\"fsr3\":" + String(fsr3) + ",";
  data += "\"p1\":" + String(p1,1) + ",";
  data += "\"p2\":" + String(p2,1) + ",";
  data += "\"p3\":" + String(p3,1);
  data += "}";

  // Send to all clients
  webSocket.broadcastTXT(data);

  delay(200);
}