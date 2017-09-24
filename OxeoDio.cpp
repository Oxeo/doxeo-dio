#include "OxeoDio.h"
#include "Arduino.h"

OxeoDio::OxeoDio() {
	setReceiverPin(2);
	setSenderPin(3);
}

void OxeoDio::setReceiverPin(int pin) {
	pinMode(pin, INPUT);
	_receiverPin = pin;
}

void OxeoDio::setSenderPin(int pin) {
	pinMode(pin, OUTPUT);
	_senderPin = pin;
}

unsigned long OxeoDio::read() {
  int i = 0;                //compteur boucle
  unsigned long t = 0;      //recepteur du delai entre 2 fronts descendant
  byte prevBit = 0;         //avant dernier "bit" 433 reçue
  byte bit = 0;             //dernier "bit" 433 reçue
  unsigned long sender = 0; //code DIO au format unsigned long

  t = pulseIn(_receiverPin, LOW, 1000000);
  if (t > 2550 && t < 2900) //déchiffrage au deuxieme cadenas du signal DIO
    while (i < 64) {
      t = pulseIn(_receiverPin, LOW, 1000000);  //reception des bits
      if (t > 260 && t < 450) { //si environ 310 + ou - 30 c'est un bit à 0
        bit = 0;
      } else if (t > 1250 && t < 1500) { //si environ 1300 + ou - 50 c'est un bit à 1
        bit = 1;
      } else {    //sinon la série est érronnée, on sort de la fonction
        //Serial.print("break: ");
        //Serial.println(t);
        break;
      }
      if (i % 2 == 1) { // on ne traite que les deuxièmes bits (codage manchester)
        if ((prevBit ^ bit) == 0) //si faux = codage manchester KO, on sort
          break;
        //on stocke chaque bit dans l'unsigned long
        sender <<= 1;
        sender |= prevBit;
      }
      //on enregistre le bit précédent
      prevBit = bit;
      ++i;
    }
  if (i == 64)  // si on a 64 bit (i=64) on a un code DIO détecté
    return sender;
  return 0; //sinon pas de code DIO
}


const unsigned int THIGH = 220, TSHORT = 350, TLONG=1400;

void OxeoDio::send(unsigned long data) {
	unsigned long shiftedCode;
	
    for (int i = 0; i < 5; i++) { // 20 cycles pour init. d'une prise
	  shiftedCode = data;
      //Sequence de verrou anoncant le départ du signal au recepeteur
      digitalWrite(_senderPin, HIGH);
      delayMicroseconds(THIGH);
      digitalWrite(_senderPin, LOW); 
      delayMicroseconds(2675);
	  
      for (int i = 0; i < 32; i++) {
        if (shiftedCode & 0x80000000L) {
          digitalWrite(_senderPin, HIGH);
          delayMicroseconds(THIGH);
          digitalWrite(_senderPin, LOW);
          delayMicroseconds(TLONG);
          digitalWrite(_senderPin, HIGH);
          delayMicroseconds(THIGH);
          digitalWrite(_senderPin, LOW);
          delayMicroseconds(TSHORT);
        } else {
          digitalWrite(_senderPin, HIGH);
          delayMicroseconds(THIGH);
          digitalWrite(_senderPin, LOW);
          delayMicroseconds(TSHORT);
          digitalWrite(_senderPin, HIGH);
          delayMicroseconds(THIGH);
          digitalWrite(_senderPin, LOW);
          delayMicroseconds(TLONG);
        }
        shiftedCode <<= 1;
      }
	  
      digitalWrite(_senderPin, HIGH);
      delayMicroseconds(THIGH);
      digitalWrite(_senderPin, LOW);
      delayMicroseconds(10600);
      digitalWrite(_senderPin, HIGH);
      delayMicroseconds(THIGH);
    }
}

//http://homeeasyhacking.wikia.com/wiki
//
//Device Addressing Edit
//
//To send a dimming level a special modified bit is placed
//at bit 27 (See Specification)
//
//Encoding Edit
//
//Manchester coding is used:
//Data 0 = Manchester 01          Data 1 = Manchester 10
//A Manchester 0 is a High for 275uS and Low for 275uS
//A Manchester 1 is a High for 275uS and Low for 1225uS
//So.......
//0 = High 275uS, Low 275uS, High 275uS, Low 1225uS
//1 = High 275uS, Low 1225uS, High 275uS, Low 275uS
//A preamble is sent before each command which is High 275uS, Low 2675uS
//
//When sending a dim level a special bit is placed in bit 27
//Dim bit 27 = High 275uS, Low 275uS, High 275uS, Low 275uS.
//This seems a bit odd, and goes against the manchester coding
//specification !
//
//XXXXXXXXXXXXXXXXXXXXXXXXXXX01100  (32 bits)
//         bit 26, on / off
//      bit 27-31, button number
