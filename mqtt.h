
//==============================================================//
//  Systeme d'arrosage autonome                                 //
//  Developpé par Vincent Althauser pour le SERFA de Mulhouse  //
//==============================================================//


#ifndef MQTT_H
#define MQTT_H

#include <Arduino.h>       // Inclusion de la bibliothèque principale d'Arduino
#include <WiFi.h>          // Inclusion de la bibliothèque pour la gestion du WiFi
#include <PubSubClient.h>  // Inclusion de la bibliothèque pour la gestion du protocole MQTT
#include <ESPping.h>       // Inclusion de la bibliothèque pour la gestion des pings réseau
//#include "variables_composants.h"  // Inclusion des variables globales et des objets des composants

// Déclarations externes des variables et objets
extern int humSol;                 // Variable pour stocker l'humidité du sol
extern float tauxHumSol;           // Variable pour stocker le taux d'humidité du sol
extern float temp;                 // Variable pour stocker la température
extern uint16_t photoResistance;   // Variable pour stocker la valeur de la résistance de la photorésistance
extern String palmierNom;          // Variable pour stocker le nom du palmier
extern String palmierDescription;  // Variable pour stocker la description du palmier
extern int palmierTemperature;     // Variable pour stocker la température idéale du palmier
extern int palmierHumideAir;       // Variable pour stocker l'humidité idéale de l'air pour le palmier
extern int palmierHumideTerre;     // Variable pour stocker l'humidité idéale du sol pour le palmier
extern int palmierLumiere;         // Variable pour stocker la luminosité idéale pour le palmier
extern int palmierExposition;      // Variable pour stocker l'exposition idéale pour le palmier
extern int temperatureIdeal;       // Variable pour stocker la température idéale
extern int besoinLux;              // Variable pour stocker la luminosité idéale
extern String heureMqtt;           // Variable pour stocker l'heure MQTT

// Variables pour lierre
extern int topicLierreLum;   // Variable pour stocker la valeur de la luminosité du topic MQTT "lierre/photoresistance"
extern int topicLierreSol;   // Variable pour stocker la valeur de l'humidité du sol du topic MQTT "lierre/humSol"
extern int topicLierreTemp;  // Variable pour stocker la valeur de la température du topic MQTT "lierre/temp"

// Variables pour orchidée
extern String topicDamien;     // Variable pour stocker la valeur du topic MQTT "orchidee"
extern int topicOrchideeTemp;  // Variable pour stocker la valeur de la température du topic MQTT "orchidee/temp"
extern int topicOrchideeHum;   // Variable pour stocker la valeur de l'humidité de l'air du topic MQTT "orchidee/humAir"
extern int topicOrchideeSol;   // Variable pour stocker la valeur de l'humidité du sol du topic MQTT "orchidee/humSol"
extern int topicOrchideeLum;   // Variable pour stocker la valeur de la luminosité du topic MQTT "orchidee/luminosite"

extern WiFiClient espClient;                   // Objet pour la connexion WiFi
extern PubSubClient client;                    // Objet pour la connexion MQTT
extern unsigned long lastReconnectAttempt;     // Variable pour stocker la dernière tentative de reconnexion MQTT
extern const unsigned long reconnectInterval;  // Variable pour stocker l'intervalle entre les tentatives de reconnexion MQTT
extern int reconnectAttempts;                  // Variable pour stocker le nombre de tentatives de reconnexion MQTT
bool allMqttDataReceived = false;              // Variable pour stocker si toutes les données MQTT ont été reçues

//===========================//
//  Protocol MQTT            //
//===========================//
char receivedNames[256];  // Tableau pour stocker les noms reçus via MQTT
// Fonction de rappel appelée lors de la réception de messages MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';  // Assure que le payload est une chaîne terminée
  String message = String((char*)payload);

  static bool nomReceived = false;
  static bool temperatureReceived = false;
  static bool humideAirReceived = false;
  static bool humideTerreReceived = false;
  static bool lumiereReceived = false;
  static bool expositionReceived = false;
  static bool descriptionReceived = false;

  // reception topic LIERRE
  if (strcmp(topic, "lierre/photoresistance") == 0) {  // Gère tous les sous-topics de 'orchidee'
    topicLierreLum = message.toInt();
  } else if (strcmp(topic, "lierre/humSol") == 0) {
    topicLierreSol = message.toInt();
  } else if (strcmp(topic, "lierre/temp") == 0) {
    topicLierreTemp = message.toInt();
  }

  // reception topic ORCHIDEE
  /*if (strcmp(topic, "orchidee/temp") == 0) {  // Gère tous les sous-topics de 'orchidee'
    topicOrchideeTemp = message.toInt();
  }  else if (strcmp(topic, "orchidee/humSol") == 0) {
    topicOrchideeSol = message.toInt();
  } else if (strcmp(topic, "orchidee/luminosite") == 0) {
    topicOrchideeLum = message.toInt();
  } else*/ if (strcmp(topic, "orchidee/all") == 0) {
    char *token = strtok((char*)payload, ",");
    int i = 0;
    while (token != NULL) {
      switch (i) {
        case 2:
          topicOrchideeTemp = atoi(token);
          break;
        case 1:
          topicOrchideeSol = atoi(token);
          break;
        case 0:
          topicOrchideeLum = atoi(token);
          break;
      }
      token = strtok(NULL, ",");
      i++;
    }
  }


  // reception topic PALMIER
  if (strcmp(topic, "plants/palmier/nom_plante") == 0) {
    palmierNom = message;
    nomReceived = true;
  } else if (strcmp(topic, "plants/palmier/temperature") == 0) {
    temperatureIdeal = message.toInt();
    temperatureReceived = true;
  } else if (strcmp(topic, "plants/palmier/humide_air") == 0) {
    palmierHumideAir = message.toInt();
    humideAirReceived = true;
  } else if (strcmp(topic, "plants/palmier/humide_terre") == 0) {
    humSol = message.toInt();
    humideTerreReceived = true;
  } else if (strcmp(topic, "plants/palmier/lumiere") == 0) {
    besoinLux = message.toInt();
    lumiereReceived = true;
  } /*else if (strcmp(topic, "plants/palmier/exposition") == 0) {
    palmierExposition = message.toInt();
    expositionReceived = true;
  }*/ else if (strcmp(topic, "plants/palmier/description") == 0) {
    palmierDescription = message;
    descriptionReceived = true;
  }

  if (nomReceived && temperatureReceived && humideAirReceived && humideTerreReceived && lumiereReceived && expositionReceived && descriptionReceived) {
    allMqttDataReceived = true;  // Toutes les données MQTT ont été reçues
  }
}

// Fonction pour configurer la connexion WiFi et MQTT
void setupWifiMqtt() {
  delay(10);                           // Délai pour permettre aux composants de s'initialiser
  WiFi.begin(ssidMqtt, passwordMqtt);  // Connexion au réseau WiFi
  if (WiFi.status() != WL_CONNECTED) {
    delay(500);  // Délai entre les tentatives de connexion
    Serial.print(".");
  }
  Serial.println("Connecté au WiFi");  // Confirmation de la connexion au WiFi
}

void subscribeTopics() {

  client.subscribe("plante/heure");
  client.subscribe("plants/palmier/nom_plante");
  client.subscribe("plants/palmier/temperature");
  client.subscribe("plants/palmier/humide_air");
  client.subscribe("plants/palmier/humide_terre");
  client.subscribe("plants/palmier/lumiere");
  //client.subscribe("plants/palmier/exposition");
  client.subscribe("plants/palmier/description");

  //Lierre
  client.subscribe("lierre/humSol");
  client.subscribe("lierre/photoresistance");
  client.subscribe("lierre/temp");
  client.subscribe("lierre/data"); //a faire plus tardclient.subscribe("lierre/temp");


  //Orchidee
  /*client.subscribe("orchidee/temp");
  client.subscribe("orchidee/luminosite");
  client.subscribe("orchidee/humSol");*/
  client.subscribe("orchidee/all");
}

void reconnect() {
  Serial.print("Tentative de connexion MQTT...");
  if (client.connect("chamaedorae")) {
    Serial.println("Connecté au MQTT!");
    reconnectAttempts = 0;  // Réinitialise le nombre de tentatives après une connexion réussie
  } else {
    Serial.print("Échec de connexion, code erreur: ");
    Serial.println(client.state());
    reconnectAttempts++;
    if (reconnectAttempts >= 2) {
      Serial.println("Échec de la connexion après plusieurs tentatives, redémarrage du ESP32...");
      ESP.restart();
    }
  }
}


// Fonction pour publier les valeurs des capteurs via MQTT
void publish_sensor_data() {
  String allmessage = String(photoResistance) + "," +
                    String(tauxHumSol) + "," +
                    String(temp) + "," +
                    palmierDescription;
  client.publish("palmier/all", allmessage.c_str());
  client.publish("palmier/photoresistance", String(photoResistance).c_str());  // Publie la valeur de la photoresistance
  client.publish("palmier/humSol", String(tauxHumSol).c_str());                // Publie la valeur de l'humidité du sol
  client.publish("palmier/temp", String(temp).c_str());                        // Publie la valeur de la température
  client.publish("palmier/nom_plante", palmierNom.c_str());
  client.publish("palmier/description", palmierDescription.c_str());
}

#endif
