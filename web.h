//==============================================================//
//  Systeme d'arrosage autonome                                 //
//  Developpé par Vincent Althauser pour le SERFA de Mulhouse   //
//==============================================================//

#ifndef WEB_H
#define WEB_H

#include <Arduino.h>
#include <WebServer.h>
//#include "variables_composants.h"
#include "arrosage.h" // Pour la fonction calculNiveau

// Déclarations externes des variables et objets
extern float temp;
extern float tauxHumSol;
extern uint16_t photoResistance;
extern const uint16_t besoinLuxTolerance;
extern const uint8_t temperatureTolerance;
extern int temperatureIdeal;
extern const uint8_t hauteurEauMax;
extern const uint8_t hauteurEauMin;
extern float hum;
extern int humSol;
extern int humTolerance;
extern int besoinLux;

extern String palmierDescription;

// Déclarations externes pour les données MQTT des plantes
extern int topicOrchideeTemp;
extern int topicOrchideeHum;
extern int topicOrchideeSol;
extern int topicOrchideeLum;

extern int topicLierreTemp;
extern int topicLierreSol;
extern int topicLierreLum;

//======================================================
//     PAGE WEB
//======================================================
String pageHtml() {
  String page = "<!DOCTYPE html><html><head><title>Chamaedorae Eleg</title>"
                "<meta charset='UTF-8'>"
                "<style>body { background-color: #c1f0d6; font-family: Arial; display: flex; justify-content: start; align-items: center; min-height: 100vh; flex-direction: column; }"
                "h1 { font-size: 2.2rem; font-weight: bold; width: 630px}"
                "h2 { font-size: 2.1rem; font-weight: 300;}"
                ".data-container { display: flex; flex-direction: column; align-items: start; width: 660px; }"
                ".data-item { font-size: 2rem; display: flex; justify-content:left; flex-wrap: nowrap; align-items: center; border-radius: 10px; width: 420px; height: 70px; margin: 10px; padding-left: 10px; background-color: #6f706f; }"
                ".alert { color: rgb(44, 247, 247); }"
                ".data-alert {font-size: 2rem; display: flex; justify-content:left; flex-wrap: nowrap; align-items: center; border-radius: 10px; width: 180px; height: 70px; padding-left: 10px; }"
                ".data-alert p {color: #b22222; margin: 0; padding: 0;}"
                ".row {display: flex; justify-content: left; align-items: center;}"
                ".spacer { height:50px;}"
                "span { color: rgb(208,255,0);}"
                "p { color: rgb(255,255,255); font-weight: 500; }"
                "#waterLevelIndicator { width: 80px; height: 30px; display: inline-block; }"
                ".buttons-container { display: flex; gap: 10px; margin-bottom: 20px; }"
                "button { background-color: #5fee9f; font-size: 1.8rem; width: 180px; height: 50px; border-radius: 20px; margin: 0 15px 0 15px; }"
                "#removeButton { background-color: #b22222; color:rgb(211, 211, 211); }"
                ".hidden { display: none; }"
                "</style></head><body>"
                "<div class='spacer'></div>"

                "<div class='data-container'>"
                "<h1>Chamaedorae Elegans</h1>"
                "<h2>" + String(palmierDescription) + "</h2>"
                "<div class='row'>"
                "<div class='data-item'><p>Temperature : <span id='temperature'></span></p></div>"
                "<div class='data-alert'><p id='alertTemp'></p></div>"
                "</div> "
                "<div class='row'>"
                "<div class='data-item'><p>Humidite air : <span id='humidity'></span></p></div>"
                "<div class='data-alert'></div>"
                "</div> "
                "<div class='row'>"
                "<div class='data-item'><p>Luminosite : <span id='ldr'></span></p></div>"
                "<div class='data-alert'><p id='alertLight'></p></div>"
                "</div> "
                "<div class='row'>"
                "<div class='data-item'><p>Humidite sol : <span id='humsol'></span></p></div>"
                "<div class='data-alert'><p id='alertSoil'></p></div>"
                "</div> "
                "<div class='row'>"
                "<div class='data-item'><p>Réserve d'eau : </p></div>"
                "<div class='data-alert'><span id='waterLevelIndicator'></span></div>"
                "</div> "
                "</div>"
                
                "<div class='spacer'></div>"

                "<div class='data-container'>"
                "<div class='buttons-container'>"
                "<button onclick=\"showSection('orchidee')\">Orchidee</button>"
                "<button onclick=\"showSection('lierre')\">Lierre</button>"
                "<button id=\"removeButton\" onclick=\"hideSections()\">Masquer</button>"
                "</div>"
                "</div>"

                "<div id='orchidee-section' class='data-container hidden'>"
                "<h2>Orchidee</h2>"
                "<div class='data-item'><p>Temperature : <span id='orchideeTemp'></span></p></div>"
                "<div class='data-item'><p>Humidite sol : <span id='orchideeHumSol'></span></p></div>"
                "<div class='data-item'><p>Luminosite : <span id='orchideeLum'></span></p></div>"
                "</div>"

                "<div id='lierre-section' class='data-container hidden'>"
                "<h2>Lierre</h2>"
                "<div class='data-item'><p>Temperature : <span id='lierreTemp'></span></p></div>"
                "<div class='data-item'><p>Humidite sol : <span id='lierreHumSol'></span></p></div>"
                "<div class='data-item'><p>Luminosite : <span id='lierreLum'></span></p></div>"
                "</div>"

                "<script>"
                "function showSection(sectionId) {"
                "document.getElementById('orchidee-section').classList.add('hidden');"
                "document.getElementById('lierre-section').classList.add('hidden');"
                "document.getElementById(sectionId + '-section').classList.remove('hidden');"
                "}"
                "function hideSections() {"
                "document.getElementById('orchidee-section').classList.add('hidden');"
                "document.getElementById('lierre-section').classList.add('hidden');"
                "}"
                "setInterval(function() { fetch('/data').then(response => response.json()).then(data => {"
                "document.getElementById('temperature').innerText = '  ' + data.temp + ' °C';"
                "document.getElementById('humidity').innerText = '  ' +  data.hum + ' %';"
                "document.getElementById('ldr').innerText = '  ' + data.ldr + ' Lux';"
                "document.getElementById('humsol').innerText = '  ' +  data.humsol + ' %';"
                "document.getElementById('alertTemp').innerText = data.tempExcessive ? '  Trop chaud' : (data.tempInsuffisante ? '  Trop froid' : '');"
                "document.getElementById('alertSoil').innerText = data.soilExcessif ? '  détrempé' : (data.soilInsuffisant ? '  trop sec' : '');"
                "document.getElementById('alertLight').innerText = data.lightExcessive ? '  Lum eleve' : (data.lightInsuffisante ? '  Lum faible' : '');"
                "var waterLevelIndicator = document.getElementById('waterLevelIndicator');"
                "if (data.waterLevel >= data.seuilMin) { waterLevelIndicator.style.backgroundColor = 'red'; }"            // rouge
                "else if (data.waterLevel >= data.seuilMoyen) { waterLevelIndicator.style.backgroundColor = 'yellow'; }"  // Jaune
                "else { waterLevelIndicator.style.backgroundColor = 'green'; }"                                           // vert
                "document.getElementById('orchideeTemp').innerText = data.orchideeTemp + ' °C';"
                "document.getElementById('orchideeHumSol').innerText = data.orchideeHumSol + ' %';"
                "document.getElementById('orchideeLum').innerText = data.orchideeLum + ' Lux';"
                "document.getElementById('lierreTemp').innerText = data.lierreTemp + ' °C';"
                "document.getElementById('lierreHumSol').innerText = data.lierreHumSol + ' %';"
                "document.getElementById('lierreLum').innerText = data.lierreLum + ' Lux';"
                "}).catch(error => console.error('Error fetching data:', error));"
                "}, 1000);"
                "</script>"
                "</body></html>";
  return page;
}



//======================================================
//     Recuperation valeurs capteurs pour page web
//======================================================

void recupererDonneesCapteur() {
  // Supposons que les variables temp, hum, photoResistance, et tauxHumSol sont mises à jour ailleurs dans le code
  bool tempExcessive = temp > (temperatureIdeal + temperatureTolerance);
  bool tempInsuffisante = temp < (temperatureIdeal - temperatureTolerance);
  bool soilExcessif = tauxHumSol > (humSol + humTolerance);
  bool soilInsuffisant = tauxHumSol < (humSol - humTolerance);
  bool lightExcessive = photoResistance > (besoinLux + besoinLuxTolerance);
  bool lightInsuffisante = photoResistance < (besoinLux - besoinLuxTolerance);
  float seuilMin = 4 + hauteurEauMax - hauteurEauMin;  // distance pour niveau d'eau minimal
  float seuilMoyen = 4 + (hauteurEauMax / 2);          // distance pour niveau d'eau moyen

  // mesure du niveau d'eau
  float waterLevel = calculNiveau(); 
  // Création de la réponse JSON avec les valeurs mesurées et les conditions évaluées
  String json = "{\"temp\":" + String(temp) + ", \"hum\":" + String(hum, 0) + ", \"ldr\":" + String(photoResistance) + ", \"humsol\":" + String(tauxHumSol) + ", \"waterLevel\":" + String(waterLevel) +
                ", \"tempExcessive\":" + tempExcessive + ", \"tempInsuffisante\":" + tempInsuffisante + ", \"soilExcessif\":" + soilExcessif + ", \"soilInsuffisant\":" + soilInsuffisant + ", \"lightExcessive\":" + lightExcessive + ", \"lightInsuffisante\":" + (lightInsuffisante ? "true" : "false") + ", \"seuilMoyen\":" + String(seuilMoyen) +
                ", \"seuilMin\":" + String(seuilMin) +
                ", \"orchideeTemp\":" + String(topicOrchideeTemp) + ", \"orchideeHumSol\":" + String(topicOrchideeSol) + ", \"orchideeHumAir\":" + String(topicOrchideeHum) + ", \"orchideeLum\":" + String(topicOrchideeLum) +
                ", \"lierreTemp\":" + String(topicLierreTemp) + ", \"lierreHumSol\":" + String(topicLierreSol) + ", \"lierreLum\":" + String(topicLierreLum) + "}";

  server.send(200, "application/json", json);
}

#endif
