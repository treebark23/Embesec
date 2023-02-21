#include <esp_now.h>
#include <WiFi.h>
#include <Arduchiz.h>
#include <SpritzCipher.h>

#define SERIAL_BRATE 115200

#define LED_TEST 19

// WIFI SETTINGS
#define CONNECTION_TIMEOUT 20 // 500 per try => 10 sec = 20
const char* ssid = WIFI_CONNECT_SSID;
const char* password = WIFI_CONNECT_PASSWORD;
int timeoutCounter = 0;

// Peers settings:
uint8_t peerAddress[] = {IOT_HUB_MAC};
esp_now_peer_info_t peerInfo;

static const char* PMK_KEY = ESP_NOW_PMK_KEY;
static const char* LMK_KEY = ESP_NOW_LMK_KEY;

typedef struct struct_message {
  int tickcount;
  int var1;
  int var2;
  byte signature[4];
} struct_message;

struct_message messageBuff;

// Serial Functions
int incomingByte = 0;
void CheckSerial()
{
  if(Serial.available() > 0){
    incomingByte = Serial.read();

    Serial.print("I received from Serial: ");
    Serial.println(incomingByte, DEC);

    if(incomingByte == 49){
      messageBuff.var1 = 1;
      messageBuff.var2 = 1;
      if(sendMessage(peerAddress, messageBuff, sizeof(messageBuff)) == E_OK)
        Serial.println("LEDON Sent");
    }
    else if (incomingByte == 50){
      messageBuff.var1 = 1;
      messageBuff.var2 = 2;
      if(sendMessage(peerAddress, messageBuff, sizeof(messageBuff)) == E_OK)
        Serial.println("LEDOFF Sent");
    }
  }
}

// Peer Functions

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLastPacket Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void OnDataReceive(const uint8_t * mac_addr, const uint8_t *incomingData, int len)
{
  Serial.print("\nPacket received");
  memcpy(&messageBuff, incomingData, sizeof(messageBuff));
  if(messageBuff.var1 == messageBuff.var2){
    digitalWrite(LED_TEST, HIGH);
  }
  else{
    digitalWrite(LED_TEST, LOW);
  }
}

E_Return sendMessage(uint8_t* destAddress, struct_message msg, uint8_t size)
{
  int retVal = E_NOT_OK;

  msg.tickcount = millis()%1000;
  // MAC Generate over data

  if(esp_now_send(destAddress, (uint8_t *) &msg, size)){
    return retVal = E_OK;
  }

  return retVal; 
}

E_Return setupEspNow()
{
  // ESP Connection
  if(esp_now_init() != ESP_OK){
    Serial.println("There was an error initializing ESP-NOW!");
    return E_NOT_OK;
  }
  memcpy(peerInfo.peer_addr, peerAddress, 6);
  peerInfo.channel = 0;

  esp_now_set_pmk((uint8_t *)PMK_KEY);
  for(uint8_t i = 0; i < 16; i++){
    peerInfo.lmk[i] = LMK_KEY[i];
  }
  peerInfo.encrypt = true;

  if(esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return E_NOT_OK;
  }

  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataReceive);

  return E_OK;
}

// WiFi functions

void get_network_info(WiFiEvent_t wifi_event, WiFiEventInfo_t wifi_info)
{
    if(WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected to the WiFi network");
        Serial.print("[*] Network information for ");
        Serial.println(ssid);

        Serial.println("[+] BSSID : " + WiFi.BSSIDstr());
        Serial.print("[+] Gateway IP : ");
        Serial.println(WiFi.gatewayIP());
        Serial.print("[+] Subnet Mask : ");
        Serial.println(WiFi.subnetMask());
        Serial.println((String)"[+] RSSI : " + WiFi.RSSI() + " dB");
        Serial.print("[+] ESP32 IP : ");
        Serial.println(WiFi.localIP());
        Serial.print("[+] ESP32 MAC: ");
        Serial.println(WiFi.macAddress());
    }
}

void disconnect_event(WiFiEvent_t wifi_event, WiFiEventInfo_t wifi_info)
{
  Serial.println("\nDisconnected from the WiFi Network");
  WiFi.begin(ssid, password);
}

void setupWiFi()
{
  WiFi.onEvent(get_network_info, ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(disconnect_event, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting to Wifi");
  while(WiFi.status() != WL_CONNECTED){
      Serial.print(".");
      delay(500);
      timeoutCounter++;
      if(timeoutCounter >= CONNECTION_TIMEOUT){
        Serial.println("Connection time out");
        ESP.restart();
      }
  }
}

// Crypto Functions

E_Return setupCrypto()
{
  int retVal = E_OK;

  return retVal;
}

// setup() function -- runs once at startup --------------------------------

void setup() 
{
  Serial.begin(SERIAL_BRATE);
  pinMode(LED_TEST, OUTPUT);
  digitalWrite(LED_TEST, LOW);

  // Crypto
  setupCrypto();

  // WIFI Connection
  WiFi.mode(WIFI_STA); //Optional
  //setupWiFi();

  // ESP Now
  if(setupEspNow() == E_NOT_OK){
    return;
  }
}

// loop() function -- runs repeatedly as long as board is on ---------------

void loop() 
{
  int retVal = 0;
  CheckSerial();
}
