# Systeme d'Arrosage Autonome

Ce projet est un système d'arrosage autonome développé par Vincent Althauser. Le système utilise un microcontrôleur ESP32 pour surveiller et contrôler les conditions de croissance des plantes, telles que la température, l'humidité du sol et la luminosité, et ajuste automatiquement l'arrosage en fonction des besoins des plantes.

## Table des Matières

- [Introduction](#introduction)
- [Fonctionnalités](#fonctionnalités)
- [Matériel Requis](#matériel-requis)
- [Installation](#installation)
- [Utilisation](#utilisation)
- [Dépendances](#dépendances)
- [Structure du Projet](#structure-du-projet)
- [Contribuer](#contribuer)
- [Auteurs](#auteurs)
- [Licence](#licence)

## Introduction

Ce projet vise à automatiser le processus d'arrosage des plantes en utilisant des capteurs pour surveiller les conditions environnementales et en ajustant l'arrosage en conséquence. Il utilise également MQTT pour la communication avec un serveur distant pour la surveillance à distance et la configuration des paramètres.

## Fonctionnalités

- Surveillance de la température et de l'humidité de l'air.
- Surveillance de l'humidité du sol et de la luminosité.
- Contrôle automatique de l'arrosage en fonction des conditions mesurées.
- Interface web pour la visualisation des données en temps réel.
- Notifications via MQTT pour les conditions critiques.
- Configuration WiFi et serveur web embarqué.

## Matériel Requis

- ESP32
- Capteur DHT11 (Température et Humidité)
- Capteur d'humidité du sol
- Photorésistance
- Moteur pas à pas et driver
- Module WiFi
- Écran LCD DFRobot RGB LCD1602
- Divers composants électroniques (résistances, LED, etc.)

## Installation

1. Clonez ce dépôt GitHub :

   ```bash
   git clone https://github.com/votre-utilisateur/votre-repo.git
   cd votre-repo
   ```

2. Téléchargez et installez [Arduino IDE](https://www.arduino.cc/en/software).

3. Ajoutez les bibliothèques nécessaires dans l'Arduino IDE :

   - Stepper
   - Arduino.h
   - WiFi
   - PubSubClient
   - DHT
   - Wire
   - DFRobot_RGBLCD1602

4. Ouvrez le fichier `V.ino` dans l'Arduino IDE.

5. Configurez les paramètres WiFi dans le fichier `reseau_wifi.h`.

6. Téléversez le code sur votre ESP32.

## Utilisation

1. Connectez les capteurs et les actionneurs aux broches spécifiées dans le fichier `pins.h`.

2. Allumez l'ESP32. Il se connectera automatiquement au réseau WiFi configuré et commencera à surveiller les conditions environnementales.

3. Accédez à l'interface web via l'adresse IP affichée dans le moniteur série pour visualiser les données en temps réel et configurer les paramètres.

## Dépendances

Ce projet dépend des bibliothèques suivantes :

- [Stepper](https://www.arduino.cc/en/Reference/Stepper)
- [Arduino.h](https://www.arduino.cc/reference/en/language/structure/further-syntax/arduinoh/)
- [WiFi](https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi)
- [PubSubClient](https://github.com/knolleary/pubsubclient)
- [DHT](https://github.com/adafruit/DHT-sensor-library)
- [Wire](https://www.arduino.cc/en/Reference/Wire)
- [DFRobot_RGBLCD1602](https://github.com/DFRobot/DFRobot_RGBLCD1602)

## Structure du Projet

- `V.ino` : Fichier principal contenant le code de contrôle du système.
- `affichage.h` : Gestion de l'affichage des données sur l'écran LCD.
- `arrosage.h` : Fonctions de gestion de l'arrosage.
- `conditions_critiques.h` : Vérification des conditions critiques (température, humidité, etc.).
- `icones.h` : Définition des icônes personnalisées pour l'écran LCD.
- `mqtt.h` : Gestion de la communication MQTT.
- `pins.h` : Définition des broches utilisées par les capteurs et actionneurs.
- `reseau_wifi.h` : Configuration du réseau WiFi et du serveur web.
- `web.h` : Gestion de l'interface web.

## Auteur

- Vincent Althauser
