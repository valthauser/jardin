
//==============================================================//
//  Systeme d'arrosage autonome                                 //
//  Developpé par Vincent Althauser pour le SERFA de Mulhouse  //
//==============================================================//


#ifndef ARROSAGE_H
#define ARROSAGE_H

#include <Arduino.h>

// Déclarations externes des variables et objets utilisés dans ce fichier
extern DFRobot_RGBLCD1602 lcd;       // Écran LCD utilisé pour afficher des informations
extern float tauxHumSol;             // Taux d'humidité du sol mesuré par un capteur
extern bool niveauEauSuffisant;      // Indique si le niveau d'eau dans le réservoir est suffisant
extern const uint8_t hauteurEauMax;  // Niveau d'eau maximal dans le réservoir (en cm)
extern const uint8_t hauteurEauMin;  // Niveau d'eau minimal dans le réservoir (en cm)
extern Stepper myStepper;            // Moteur pas à pas utilisé pour actionner la pompe à eau
extern const long tempsArrosage;     // Durée d'arrosage (en ms)
extern const long attenteArrosage;   // Temps d'attente entre deux arrosages (en ms)
extern bool arrosageEnCours;         // Indique si un arrosage est en cours
extern int etapeArrosage;            // Étape actuelle de l'arrosage en cours

// Fonction pour calculer le niveau d'eau dans le réservoir en utilisant un capteur à ultrasons
float calculNiveau() {
  float sommeDistances = 0;
  int nombreDeMesures = 10;

  // Effectue plusieurs mesures de distance et calcule la moyenne pour plus de précision
  for (int i = 0; i < nombreDeMesures; i++) {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH);
    float distance = duration * 0.034 / 2;
    sommeDistances += distance;

    delay(10);
  }

  float distanceMoyenne = sommeDistances / nombreDeMesures;
  return distanceMoyenne;
}

/*
// Fonction pour arroser une plante en actionnant la pompe à eau avec le moteur pas à pas
// (commentée car non utilisée dans le code actuel)
void arroserPlante() {
    myStepper.setSpeed(10);
    myStepper.step(-512);
    delay(2000);
    myStepper.step(512);
}
*/

// Fonction pour gérer l'arrosage en cours en fonction de l'étape actuelle
void gererArrosage() {
  if (etapeArrosage == 0) {
    
    myStepper.setSpeed(10);
    myStepper.step(512);  // Effectue un mouvement de -512 steps
    delay(tempsArrosage);  // Attend 3 secondes
    etapeArrosage = 1;     // Passe à l'étape suivante
  } else if (etapeArrosage == 1) {
    myStepper.setSpeed(15);
    myStepper.step(-512);  // Effectue un mouvement de 512 steps
    delay(attenteArrosage);
    /*Serial.print("Hum Sol: ");
        Serial.print(tauxHumSol);
        Serial.println("%");*/
    arrosageEnCours = false;  // Termine l'arrosage
    etapeArrosage = 0;        // Réinitialise les étapes d'arrosage
  }
  client.loop();
}


#endif
