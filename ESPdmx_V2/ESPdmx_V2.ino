#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h> // Pour ESP8266
#include <ESPDMX.h>
#include <ESP8266WiFi.h> // Pour ESP8266
#include <ArduinoJson.h>

const char* ssid = "YourRouter";
const char* password = "PASSWORD";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws"); // WebSocket server

DMXESPSerial dmx;

int channel = 1; // Initialise avec un canal par défaut

void setup() {
  Serial.begin(115200);
  dmx.init();

  // Définir le nom d'hôte
  WiFi.hostname("ESPDMX");

  // Connexion Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Affiche l'adresse IP
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialisation du serveur WebSocket
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  // Démarrer le serveur HTTP
  server.begin();
  Serial.println("WebSocket server started");
}

// Gestion des événements WebSocket
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, 
               AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len) {
      if (info->opcode == WS_TEXT) {
        data[len] = 0; // Terminer la chaîne reçue

        // Exemple de JSON : {"channel": 123, "value": 45}
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, (char*)data);
        int receivedChannel = doc["channel"];
        int receivedValue = doc["value"];

        if (receivedChannel >= 1 && receivedChannel <= 512) {
          channel = receivedChannel;
          Serial.print("Channel set to: ");
          Serial.println(channel);
        }

        if (receivedValue >= 0 && receivedValue <= 255) {
          dmx.write(channel, receivedValue); // Met à jour la valeur du canal
          dmx.update(); // Assure-toi que les données sont envoyées
          Serial.print("Ch: ");
          Serial.print(channel);
          Serial.print(" Value: ");
          Serial.println(receivedValue);
        }
      }
    }
  }
}

void loop() {
  // Pas besoin de faire quoi que ce soit ici avec WebSocket
  static unsigned long lastUpdateTime = 0;
  if (millis() - lastUpdateTime > 100) { // Update toutes les 100ms
    dmx.update();
    lastUpdateTime = millis();
  }
}
