
//==============================================================//
//  Systeme d'arrosage autonome                                 //
//  Developpé par Vincent Althauser pour le SERFA de Mulhouse  //
//==============================================================//


#include <Stepper.h>
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Wire.h>
#include "DFRobot_RGBLCD1602.h"
#include "pins.h"
#include "icones.h"
#include "reseau_wifi.h"
#include "mqtt.h"
#include "web.h"
#include "conditions_critiques.h"
#include "affichage.h"
#include "arrosage.h"

// Constantes
const uint16_t besoinLuxTolerance = 500;          // Tolérance de luminosité en lux
const uint8_t temperatureTolerance = 4;           // Tolérance de température en degrés Celsius
const uint8_t hauteurEauMax = 14;                 // Hauteur maximale de l'eau en cm
const uint8_t hauteurEauMin = 2;                  // Hauteur minimale de l'eau en cm, se qui doit rester dans le reservoir
const long tempsArrosage = 4000;                  // Durée de l'arrosage en millisecondes
const long attenteArrosage = 10000;               // Temps d'attente entre deux arrosages en millisecondes
const unsigned long affichageIntervalle = 2000;  // Intervalle d'affichage MQTT en millisecondes

// Variables globales et valeurs par defaut si pas connexion au MQTT
int temperatureIdeal = 21;  // Température par defaut idéale en degrés Celsius
int humSol = 45;            // Humidité du sol par defaut idéale en pourcentage
int besoinLux = 2400;       // Besoin de luminosité par defaut en lux
int humTolerance = 15;      // Tolérance d'humidité en pourcentage

uint16_t valeurHumSol;  // Valeur de l'humidité du sol lue par le capteur
float tauxHumSol;       // Taux d'humidité du sol calculé

// Variables pour récupérer les données MQTT de l'orchidée
String topicDamien = "";
int topicOrchideeTemp = 0;
int topicOrchideeHum = 0;
int topicOrchideeSol = 0;
int topicOrchideeLum = 0;

// Variables pour récupérer les données MQTT du lierre
int topicLierreLum;
int topicLierreSol;
int topicLierreTemp;

// Variables pour récupérer les données MQTT du palmier
String palmierNom = "";
String palmierDescription = "";
int palmierTemperature = 0;
int palmierHumideAir = 0;
int palmierHumideTerre = 0;
int palmierLumiere = 0;
int palmierExposition = 0;

float hum;                       // Humidité ambiante lue par le capteur DHT
float temp;                      // Température ambiante lue par le capteur DHT
uint16_t photoResistance;        // Valeur de la photo-résistance
float distanceMoyenne;           // Distance moyenne mesurée par le capteur de distance
bool niveauEauSuffisant = true;  // Indicateur de niveau d'eau suffisant

DHT dht(DHTPIN, DHTTYPE);                // Initialisation du capteur DHT
DFRobot_RGBLCD1602 lcd(0x2D, 16, 2);     // Initialisation de l'écran LCD
Stepper myStepper(2048, 17, 5, 19, 13);  // Initialisation du moteur pas à pas

WiFiClient espClient;            // Client WiFi
PubSubClient client(espClient);  // Client MQTT

bool arrosageEnCours = false;  // Indicateur d'arrosage en cours
int etapeArrosage = 0;         // Étape actuelle de l'arrosage

// Variables pour la reconnexion MQTT
unsigned long lastReconnectAttempt = 0;
const unsigned long reconnectInterval = 5000;
int reconnectAttempts = 0;
unsigned long dernierAffichage = 0;

// Interval de publication publish MQTT
unsigned long dernierTempsPublication = 0;
const unsigned long intervallePublication = 10000;  // 10 secondes


//===========================================
// Initialisations
//===========================================

void setup() {
  Serial.begin(115200);  // Initialisation de la communication série à 115200 bauds

  setupWifiMqtt();  // Configuration du WiFi et du MQTT

  if (Ping.ping(mqtt_server)) {
    Serial.println("Broker MQTT atteignable, tentative de connexion...");
    client.setServer(mqtt_server, 1883);  // Configuration du serveur MQTT et du port d'écoute (1883)
    client.setCallback(callback);         // Définition de la fonction de callback pour les messages MQTT reçus
    reconnect();                          // Tentative de reconnexion MQTT
  } else {
    Serial.println("Broker MQTT non atteignable. Vérifiez l'état du Raspberry Pi.");
  }

  if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
    Serial.println("Echec Config Point Acces");
  }

  if (!WiFi.softAP(ssid, password)) {  // Configuration du point d'accès WiFi avec le SSID et le mot de passe
    Serial.println("Echec Demarrage Point Acces");
  } else {
    Serial.print("Point Acces \"");
    Serial.print(ssid);
    Serial.println("\" demarre");
    Serial.print("Adresse IP: ");
    Serial.println(WiFi.softAPIP());  // Affichage de l'adresse IP du point d'accès
  }

  server.on("/", HTTP_GET, []() {  // Configuration de la route racine du serveur web pour répondre avec la page HTML
    server.send(200, "text/html", pageHtml());
  });

  server.on("/data", HTTP_GET, recupererDonneesCapteur);  // Configuration de la route "/data" du serveur web pour répondre avec les données des capteurs

  server.begin();  // Démarrage du serveur web

  Serial.println("serveur HTTP OK");

  // Configuration des broches et initialisation des périphériques
  //myStepper.setSpeed(5);
  pinMode(LED_chauffage, OUTPUT);        // Configuration de la broche LED_chauffage en sortie
  pinMode(LED_PIN, OUTPUT);              // Configuration de la broche LED_PIN en sortie
  pinMode(TRIG_PIN, OUTPUT);             // Configuration de la broche TRIG_PIN en sortie
  pinMode(ECHO_PIN, INPUT);              // Configuration de la broche ECHO_PIN en entrée
  dht.begin();                           // Initialisation du capteur DHT
  lcd.init();                            // Initialisation de l'écran LCD
  lcd.customSymbol(0, iconGoutte);       // Définition d'un symbole personnalisé pour l'icône de goutte d'eau
  lcd.customSymbol(1, iconTemperature);  // Définition d'un symbole personnalisé pour l'icône de température
  lcd.customSymbol(2, iconPlante);       // Définition d'un symbole personnalisé pour l'icône de plante
  lcd.customSymbol(4, iconDegre);        // Définition d'un symbole personnalisé pour l'icône de degré
  lcd.customSymbol(5, iconReservoir);    // Définition d'un symbole personnalisé pour l'icône de réservoir d'eau
  lcd.customSymbol(6, fullIcon);         // Définition d'un symbole personnalisé pour l'icône de réservoir d'eau plein
  lcd.customSymbol(7, iconMoitiePlein);  // Définition d'un symbole personnalisé pour l'icône de réservoir d'eau à moitié plein
  lcd.clear();                           // Effacement de l'écran LCD

  subscribeTopics();  // Souscription aux topics MQTT
}


//===========================================
// Boucle principale
//===========================================

void loop() {
  if (!client.connected()) {
    routine();
    reconnectNonBloquant();  // Reconnexion non bloquante au serveur MQTT
  }

  client.loop();

  routine();  // Appel du programme principal

  // Affichage toutes les 10 secondes pour MQTT
  if (millis() - dernierAffichage >= affichageIntervalle) {
    affichageMQTT();              // Appelle la fonction d'affichage des données MQTT
    dernierAffichage = millis();  // Met à jour le dernier temps d'affichage
  }
}


//===========================================
// Fonction de reconnexion non bloquante
//===========================================

void reconnectNonBloquant() {                                    // Récupère le temps courant en millisecondes
  unsigned long currentTime = millis();                          // Vérifie si le délai de reconnexion est dépassé
  if (currentTime - lastReconnectAttempt > reconnectInterval) {  // Met à jour le dernier temps de tentative de reconnexion
    lastReconnectAttempt = currentTime;                          // Tente de se connecter au serveur MQTT avec le nom de client "ESP32ClientVincent"
    if (client.connect("ESP32ClientVincent")) {                  // Si la connexion réussit, affiche un message de confirmation et réinitialise le compteur de tentatives de reconnexion
      Serial.println("Connecté au MQTT!");
      reconnectAttempts = 0;
    } else {
      // Si la connexion échoue, affiche un message d'erreur avec le code d'état de la connexion
      Serial.print("Échec de connexion, code erreur: ");
      Serial.println(client.state());  // Incrémente le compteur de tentatives de reconnexion
      reconnectAttempts++;             // Si le nombre de tentatives de reconnexion dépasse 2, affiche un message d'erreur et redémarre l'ESP32
      if (reconnectAttempts >= 2) {
        Serial.println("Échec de la connexion après plusieurs tentatives, redémarrage du ESP32...");
        ESP.restart();
      }
    }
  }
}


//===========================================
// Programme principal
//===========================================

void routine() {
  if (arrosageEnCours) {
    gererArrosage();  // Gestion de l'arrosage
  } else {
    // Utiliser millis() pour éviter les délais bloquants
    static unsigned long lastSensorReadTime = 0;
    if (millis() - lastSensorReadTime >= 2000) {
      lastSensorReadTime = millis();
      valeurHumSol = analogRead(34);                    // Lecture de l'humidité du sol
      tauxHumSol = map(valeurHumSol, 0, 4095, 0, 100);  // Conversion de la valeur lue en pourcentage

      //client.loop();  // Boucle MQTT


      // Publication des données des capteurs toutes les 10 secondes
      if (millis() - dernierTempsPublication >= intervallePublication) {
        publish_sensor_data();               // Appelle la fonction de publication des données des capteurs
        dernierTempsPublication = millis();  // Met à jour le dernier temps de publication
      }


      server.handleClient();  // Gestion des requêtes HTTP

      if (Serial.availableForWrite() > 0) {
        afficherMoniteurSerie();  // Affichage des données sur le moniteur série
      }

      afficherEcranLcd();                                             // Affichage des données sur l'écran LCD
      photoResistance = (analogRead(PHOTO_PIN) * 6000 / 4096) * 0.4;  // Lecture de la luminosité
      verifierConditionsCritiques();                                  // Vérification des conditions critiques
      verifierEtatCapteurs();                                         // Vérification de l'état des capteurs

      if (tauxHumSol < (humSol - humTolerance)) {
        if (niveauEauSuffisant) {
          Serial.println("Condition pour gererArrosage remplie");
          afficherEcranLcdArrosage();  // Affichage de l'écran d'arrosage
          arrosageEnCours = true;
          etapeArrosage = 0;
        } else {
          afficherEcranLcdArrosageImpossible();  // Affichage de l'écran d'arrosage impossible
          arrosageEnCours = false;
        }
      }
    }
  }
}
