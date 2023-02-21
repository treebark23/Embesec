#ifndef ARDUCHIZ_H
#define ARDUCHIZ_H

/// Errors Control

#define E_OK 0
#define E_NOT_OK 1

typedef uint8_t E_Return;

/// Symmetric Keys:

#define ESP_NOW_PMK_KEY "SomeExample@K3y"
#define ESP_NOW_LMK_KEY "OtherExample@K3y"

#define MAC_ENCR_KEY "AndAnother@n3"

/// ESP WiFi Settings

#define WIFI_CONNECT_SSID "My ssid name"
#define WIFI_CONNECT_PASSWORD "someweakpassword"

#define WIFI_HOST_SSID "Host ssid name"
#define WIFI_HOST_PASSWORD "password"

/// IoT Mac Addresses

#define IOT_HUB_MAC 0xBA, 0xBA, 0xCA, 0xCA, 0x00, 0x00
#define IOT_STATION_MAC 0x00, 0x00, 0xBA, 0xBA, 0xCA, 0xCA

#endif