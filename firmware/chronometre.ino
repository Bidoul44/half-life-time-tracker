#define DATA_PIN 23
#define CLOCK_PIN 18
#define LATCH_PIN 5

// Mapping réel de ton afficheur :
// A = haut droite
// B = haut
// C = bas droite
// D = bas
// E = bas gauche
// F = haut gauche
// G = milieu
//
// 0 = segment allumé
// 1 = segment éteint

const byte chiffres[10] = {
  0b11000000, // 0
  0b11111010, // 1
  0b10100100, // 2
  0b10110000, // 3
  0b10011010, // 4
  0b10010001, // 5
  0b10000001, // 6
  0b11111000, // 7
  0b10000000, // 8
  0b10010000  // 9
};

// Variables pour le chronomètre
unsigned long tempsDepart = 0;
unsigned long tempsEcoule = 0;

void afficher(byte digit, byte segments) {
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, digit);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, segments);
  digitalWrite(LATCH_PIN, HIGH);
}

void setup() {
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  
  tempsDepart = millis(); // Initialisation du chrono
}

void loop() {
  // 1. Calcul du temps écoulé en secondes
  tempsEcoule = (millis() - tempsDepart) / 1000;

  // 2. Extraction des minutes et secondes (Format MM:SS)
  int minutes = (tempsEcoule / 60) % 100; // Limité à 99 min
  int secondes = tempsEcoule % 60;

  // Découpage pour chaque afficheur
  byte digit1 = minutes / 10;   // Dizaines de minutes
  byte digit2 = minutes % 10;   // Unités de minutes
  byte digit3 = secondes / 10;  // Dizaines de secondes
  byte digit4 = secondes % 10;  // Unités de secondes

  // 3. Rafraîchissement des 4 afficheurs (Multiplexage rapide)
  
  // Afficheur 1 (Dizaines de minutes)
  afficher(0b00000001, chiffres[digit1]);
  delay(2);

  // Afficheur 2 (Unités de minutes)
  afficher(0b00000010, chiffres[digit2]);
  delay(2);

  // Afficheur 3 (Dizaines de secondes)
  afficher(0b00000100, chiffres[digit3]);
  delay(2);

  // Afficheur 4 (Unités de secondes)
  afficher(0b00001000, chiffres[digit4]);
  delay(2);
}