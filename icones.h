
//==============================================================//
//  Systeme d'arrosage autonome                                 //
//  Developpé par Vincent Althauser pour le SERFA de Mulhouse  //
//==============================================================//


#ifndef ICONES_H
#define ICONES_H

#include <Arduino.h>

//======================================================
//     Définition des icones personnalisees
//======================================================
byte iconGoutte[8] = { 0b00100, 0b00100, 0b01110, 0b01110, 0b11111, 0b11111, 0b01110, 0b00000 };
byte iconTemperature[8] = { 0b00100, 0b01010, 0b01010, 0b01110, 0b01110, 0b01110, 0b01110, 0b00000 };
byte iconPlante[8] = { 0b01010, 0b01110, 0b10101, 0b01110, 0b10101, 0b00100, 0b00100, 0b01110 };
//byte iconSoleil[8] = {0b00000, 0b10101, 0b01010, 0b10001, 0b10001, 0ba01010, 0b10101, 0b00000};
byte iconDegre[8] = { 0b00110, 0b01001, 0b00110, 0b00000, 0b01100, 0b10000, 0b10000, 0b01100 };
byte iconReservoir[8] = { 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b10001, 0b11111 };
byte fullIcon[8] = { 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111 };
byte iconMoitiePlein[8] = { 0b10001, 0b10001, 0b10001, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111 };

#endif
