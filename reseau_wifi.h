
//==============================================================//
//  Systeme d'arrosage autonome                                 //
//  Developpé par Vincent Althauser pour le SERFA de Mulhouse  //
//==============================================================//


#ifndef RESEAU_WIFI_H
#define RESEAU_WIFI_H

#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>

//======================================================
//     Configuration réseau WIFI, serveur, MQTT
//======================================================

const char* ssid = "ragnarok";       // Nom du réseau WiFi
const char* password = "123456789";  // Mot de passe du réseau WiFi
IPAddress local_IP(192, 168, 4, 2);  // Adresse IP locale
IPAddress gateway(192, 168, 4, 2);   // Passerelle
IPAddress subnet(255, 255, 255, 0);  // Masque de sous-réseau
WebServer server(80);                // Serveur web sur le port 80

//===================================//
//  MQTT  //
const char* ssidMqtt = "mqttwifi";  // Modifie cette valeur
const char* passwordMqtt = "Da#Vi@Gi#";  // Modifie cette valeur
const char* mqtt_server = "192.168.4.1";  // Adresse IP WIFI du Raspberry Pi
extern WiFiClient espClient;
extern PubSubClient client;

#endif
