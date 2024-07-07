
//==============================================================//
//  Systeme d'arrosage autonome                                 //
//  Developpé par Vincent Althauser pour le SERFA de Mulhouse  //
//==============================================================//


#ifndef CONDITIONS_CRITIQUES_H
#define CONDITIONS_CRITIQUES_H

#include <Arduino.h>
//#include "variables_composants.h"

// Déclarations externes des variables et objets
extern float temp; // température actuelle
extern float tauxHumSol; // taux d'humidité du sol actuel
extern uint16_t photoResistance; // valeur de la résistance de la photorésistance (représente la luminosité)
extern DFRobot_RGBLCD1602 lcd; // objet LCD pour afficher les informations
extern DHT dht; // objet DHT pour mesurer la température et l'humidité
extern const uint16_t besoinLuxTolerance; // constante pour la tolérance de la luminosité idéale
extern const uint8_t temperatureTolerance; // constante pour la tolérance de la température idéale
extern int temperatureIdeal; // température idéale
extern int humSol; // taux d'humidité du sol idéal
extern int humTolerance; // constante pour la tolérance de l'humidité du sol idéale
extern int besoinLux; // luminosité idéale

//======================================================
//     Gestion Conditions critiques
//======================================================
void verifierConditionsCritiques() {
  // Détection des conditions
  
  bool tempExcessive = temp > (temperatureIdeal + temperatureTolerance); // Vrai si la température actuelle dépasse la température idéale plus une certaine tolérance  
  bool tempInsuffisante = temp < (temperatureIdeal - temperatureTolerance); // vrai si la température actuelle est inférieure à la température idéale moins une certaine tolérance  
  bool soilExcessif = tauxHumSol > (humSol + humTolerance); // Vrai si l'humidité du sol actuelle dépasse l'humidité idéale plus une certaine tolérance  
  bool soilInsuffisant = tauxHumSol < (humSol - humTolerance); // Vrai si l'humidité du sol actuelle est inférieure à l'humidité idéale moins une certaine tolérance
  bool lightExcessive = photoResistance > (besoinLux + besoinLuxTolerance); // Vrai si la luminosité actuelle dépasse la luminosité idéale plus une certaine tolérance 
  bool lightInsuffisante = photoResistance < (besoinLux - besoinLuxTolerance);  // Vrai si la luminosité actuelle est inférieure à la luminosité idéale moins une certaine tolérance


   // Contrôle de l'éclairage en fonction des conditions et de l'heure
  if (lightInsuffisante) { // Vérification de la luminosité insuffisante
    if (photoResistance >= 100) { // Vérification de photoResistance >= 300 pour simuler le crepuscule, la nuit et l'aube
      digitalWrite(LED_PIN, LOW); // Allumer l'éclairage
    } else {
      digitalWrite(LED_PIN, HIGH); // Éteindre l'éclairage si photoResistance < 200
    }
  } else {
    digitalWrite(LED_PIN, HIGH); // Éteindre l'éclairage
  }
    

  if (tempInsuffisante) {
    digitalWrite(LED_chauffage, LOW);  // couper chauffage
  } else {
    digitalWrite(LED_chauffage, HIGH);  // allumer chauffage
  }

  // Définition des conditions pour la couleur du LCD
  if ((tempExcessive || soilExcessif || lightExcessive) && (tempInsuffisante || soilInsuffisant || lightInsuffisante)) {
    lcd.setRGB(128, 0, 128);  // Violet pour indiquer des conditions mixtes
  } else if (tempExcessive || soilExcessif || lightExcessive) {
    lcd.setRGB(255, 0, 0);  // Rouge pour conditions excessives
  } else if (tempInsuffisante || soilInsuffisant || lightInsuffisante) {
    lcd.setRGB(0, 0, 255);  // Bleu pour conditions insuffisantes
  } else {
    lcd.setRGB(0, 255, 0);  // Vert pour conditions optimales
  }

// Affichage des conditions spécifiques
lcd.setCursor(12, 0); // Positionne le curseur à la position (12,0) sur la première ligne
if (tempExcessive) lcd.print("TH"); // Si la température est excessive, affiche "TH" (Temperature Haute)
if (tempInsuffisante) lcd.print("Tb"); // Si la température est insuffisante, affiche "Tb" (Temperature basse)
lcd.setCursor(4, 1); // Positionne le curseur à la position (4,1) sur la deuxième ligne
if (soilExcessif) lcd.print("SH"); // Si l'humidité du sol est excessive, affiche "SH" (Sol Haut)
if (soilInsuffisant) lcd.print("Sb"); // Si l'humidité du sol est insuffisante, affiche "Sb" (Sol bas)
lcd.setCursor(10, 1); // Positionne le curseur à la position (10,1) sur la deuxième ligne
if (lightExcessive) lcd.print("LH"); // Si la luminosité est excessive, affiche "LH" (Lumiere Haute)
if (lightInsuffisante) lcd.print("Lb"); // Si la luminosité est insuffisante, affiche "Lb" (Lumiere basse)
}

//======================================================
//     Verification etat des capteurs
//======================================================
void verifierEtatCapteurs() {
  float readHumidity = dht.readHumidity();
  float readTemperature = dht.readTemperature();

  if (isnan(readHumidity) || isnan(readTemperature)) {
    lcd.setCursor(0, 0);
    lcd.print("DHT11 Err");
  }
}

#endif
