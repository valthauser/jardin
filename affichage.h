
//==============================================================//
//  Systeme d'arrosage autonome                                 //
//  Developpé par Vincent Althauser pour le SERFA de Mulhouse  //
//==============================================================//


#ifndef AFFICHAGE_H
#define AFFICHAGE_H

#include <Arduino.h>
#include "arrosage.h"

// Déclarations externes des variables et objets
extern float temp;
extern float tauxHumSol;
extern uint16_t photoResistance;
extern DFRobot_RGBLCD1602 lcd;
extern DHT dht;
extern float distanceMoyenne;
extern float hum;
extern int besoinLux;
extern String heureMqtt;

//topic LIERRE
extern int topicLierreLum;
extern int topicLierreSol;
extern int topicLierreTemp;

// topic Orchiqee
extern String topicDamien;
extern int topicOrchideeTemp;
extern int topicOrchideeHum;
extern int topicOrchideeSol;
extern int topicOrchideeLum;

// Déclaration de la fonction afficherNiveauEau
void afficherNiveauEau(float distance);



void attendreBufferSerieDisponible() {
  while (Serial.availableForWrite() <= 0) {
    delay(10);  // Attendre que le buffer série soit disponible
  }
}

void viderBufferSerie() {
  while (Serial.available() > 0) {
    Serial.read();  // Lire et jeter les données
  }
}

//======================================================
//     Affichage valeurs dans moniteur serie
//======================================================
void afficherMoniteurSerie() {
  hum = dht.readHumidity() / 1.22;     // Lire l'humidité de l'air depuis le capteur DHT, ajustée par un facteur
  temp = dht.readTemperature() - 0.9;  // Lire la température de l'air depuis le capteur DHT, ajustée par un facteur
  distanceMoyenne = calculNiveau();    // Calculer la distance moyenne de l'eau en appelant la fonction calculNiveau()

  attendreBufferSerieDisponible();
  Serial.print("Niveau Eau: ");
  Serial.print(distanceMoyenne, 1);  // Afficher la distance moyenne de l'eau

  attendreBufferSerieDisponible();
  Serial.print("  Hum Air: ");
  Serial.print(hum, 1);  // Afficher l'humidité de l'air

  attendreBufferSerieDisponible();
  Serial.print("  Temp Air: ");
  Serial.print(temp, 1);  // Afficher la température de l'air
  Serial.print("°C");

  attendreBufferSerieDisponible();
  Serial.print("  Const temper: ");
  Serial.print(temperatureIdeal, 0);  // Afficher la valeur convertie en lux

  attendreBufferSerieDisponible();
  Serial.print("  Hum Sol: ");
  Serial.print(tauxHumSol, 0);  // Afficher l'humidité du sol
  Serial.print("%");
  Serial.print("  const humSol: ");
  Serial.print(humSol, 0);  // Afficher l'humidité du sol
  Serial.print("%");

  attendreBufferSerieDisponible();
  Serial.print("  Val Capteur: ");
  Serial.print(analogRead(PHOTO_PIN));  // Afficher la valeur brute de la photorésistance

  attendreBufferSerieDisponible();
  Serial.print("  Lux: ");
  Serial.print(photoResistance, 0);  // Afficher la valeur convertie en lux

  attendreBufferSerieDisponible();
  Serial.print("  Lux Ideal: ");
  Serial.println(besoinLux, 0);  // Afficher la valeur idéale de lux
                                 //Serial.println(WiFi.localIP());
}




void affichageMQTT() {
  unsigned long startTime = millis();
  // Afficher les données du topic LIERRE

  Serial.println("Temp Lierre: " + String(topicLierreTemp));
  Serial.println("Lum Lierre: " + String(topicLierreLum));
  Serial.println("Sol Lierre: " + String(topicLierreSol));

  /// Afficher les données du topic ORCHIDEE
  Serial.println("Temp Orchidée: " + String(topicOrchideeTemp));
  Serial.println("Sol Orchidée: " + String(topicOrchideeSol));
  Serial.println("Lum Orchidée: " + String(topicOrchideeLum));

  /*if (allMqttDataReceived) {*/  // Afficher les données de 'palmier' seulement si toutes les données MQTT sont reçues
    /*attendreBufferSerieDisponible();
    Serial.println("NomMQTT: " + palmierNom);
    Serial.println("TempMQTT: " + String(temperatureIdeal));     // Convertir en String
    //Serial.println("Hum airMQTT: " + String(palmierHumideAir));  // Convertir en String
    Serial.println("Hum solMQTT: " + String(humSol));            // Convertir en String
    Serial.println("LumMQTT: " + String(besoinLux));             // Convertir en String
    //Serial.println("ExpoMQTT: " + String(palmierExposition));    // Convertir en String
    //Serial.println("DescriMQTT: " + palmierDescription);
  //}*/
}




//======================================================
//     Gestion affichage ecran LCD
//======================================================
void afficherEcranLcd() {
  lcd.clear();          // Effacer l'écran LCD
  lcd.setCursor(0, 0);  // Déplacer le curseur à la position (0,0)
  lcd.write(byte(0));   // Écrire l'icône de goutte d'eau
  lcd.print(hum, 0);    // Écrire la valeur de l'humidité de l'air
  lcd.print("%");
  lcd.setCursor(5, 0);       // Déplacer le curseur à la position (5,0)
  lcd.write(byte(1));        // Écrire l'icône de température
  lcd.print(temp, 1);        // Écrire la valeur de la température de l'air
  lcd.write(byte(4));        // Écrire l'icône de degré
  lcd.setCursor(0, 1);       // Déplacer le curseur à la position (0,1)
  lcd.write(byte(2));        // Écrire l'icône de plante
  lcd.print(tauxHumSol, 0);  // Écrire la valeur de l'humidité du sol
  lcd.print("%");

  lcd.setCursor(5, 1);       // Déplacer le curseur à la position (0,1)
  lcd.print("L");
  lcd.print(photoResistance);

  afficherNiveauEau(calculNiveau());  // Afficher le niveau d'eau en permanence
}


//======================================================
//     Affichage niveau eau
//======================================================

void afficherNiveauEau(float distance) {
  // Calculer les seuils en fonction des distances
  float seuilMin = hauteurEauMax - hauteurEauMin;  // distance pour niveau d'eau minimal
  float seuilMoyen = seuilMin/2;          // distance pour niveau d'eau moyen

  lcd.setCursor(15, 1);  // Positionner le curseur pour afficher l'icône du niveau d'eau
  if (distance >= seuilMin) {
    lcd.write(byte(5));          // Affiche "niveau bas"
    niveauEauSuffisant = false;  // Met à jour la variable, indiquant que le niveau d'eau est trop bas pour arroser
  } else {
    if (distance >= seuilMoyen) {
      lcd.write(byte(7));  // Affiche "niveau moyen"
    } else {
      lcd.write(byte(6));  // Affiche "niveau élevé"
    }
    niveauEauSuffisant = true;  // L'eau est à un niveau suffisant pour permettre l'arrosage
  }
}



//======================================================
//     Affichage mise à jour ligne arrosage
//======================================================
/*void rafraichirEcranLcdArrosage() {
  lcd.setCursor(0, 1);           // Déplacer le curseur à la position (0,1)
  lcd.write(byte(2));            // Écrire l'icône de plante
  lcd.print(tauxHumSol, 0);      // Écrire la valeur de l'humidité du sol
  lcd.print("%");
  lcd.setCursor(5, 1);       // Déplacer le curseur à la position (5,0)
  lcd.write(byte(1));        // Écrire l'icône de température
  lcd.print(temp, 1);        // Écrire la valeur de la température de l'air
  lcd.write(byte(4));        // Écrire l'icône de degré
}*/

//======================================================
//     Affichage Ecran LCD Arrosage Possible
//======================================================

void afficherEcranLcdArrosage() {
  lcd.clear();  // Effacer l'écran LCD
  lcd.setRGB(255, 255, 0);
  lcd.setCursor(0, 0);           // Déplacer le curseur à la position (0,0)
  lcd.print("Arrosage plante");  // Écrire le texte de l'arrosage
  lcd.setCursor(0, 1);           // Déplacer le curseur à la position (0,1)
  lcd.write(byte(2));            // Écrire l'icône de plante
  lcd.print(tauxHumSol, 0);      // Écrire la valeur de l'humidité du sol
  lcd.print("%");
  lcd.setCursor(5, 1);       // Déplacer le curseur à la position (5,0)
  lcd.write(byte(1));        // Écrire l'icône de température
  lcd.print(temp, 1);        // Écrire la valeur de la température de l'air
  lcd.write(byte(4));        // Écrire l'icône de degré

  // Déclaration de la variable de temps pour rafraîchir l'affichage
  /*static unsigned long lastUpdate = 0;
  const unsigned long updateInterval = 2000;  // Intervalle de 2 secondes

  // Vérifiez s'il est temps de rafraîchir l'affichage
  if (millis() - lastUpdate >= updateInterval) {
    lastUpdate = millis();
    rafraichirEcranLcdArrosage();  // Rafraîchir l'affichage
  }*/
}



//======================================================
//     Affichage Ecran LCD Arrosage Impossible
//======================================================

void afficherEcranLcdArrosageImpossible() {
  lcd.clear();  // Effacer l'écran LCD
  lcd.setRGB(255, 0, 0);
  lcd.setCursor(0, 0);          // Déplacer le curseur à la position (0,0)
  lcd.print("Aucun Arrosage");  // Écrire la valeur de l'humidité de l'air
  lcd.setCursor(0, 1);
  lcd.print("Manque eau");
}

#endif
