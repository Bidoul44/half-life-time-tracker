#include <Preferences.h>

#define DATA_PIN 23
#define CLOCK_PIN 18
#define LATCH_PIN 5

#define BUTTON_PIN 4


// ======================================================
// MEMOIRE
// ======================================================

Preferences preferences;


// ======================================================
// AFFICHAGE
// ======================================================

// 0 = segment allumé
// 1 = segment éteint

const byte chiffres[10] = {
  0b11000000, // 0
  0b11111100, // 1
  0b10100010, // 2
  0b10101000, // 3
  0b10011100, // 4
  0b10001001, // 5
  0b10000001, // 6
  0b11101100, // 7
  0b10000000, // 8
  0b10001000  // 9
};


// ======================================================
// CHRONOMETRE
// ======================================================

// Secondes depuis le début de l'heure actuelle
unsigned long secondesChrono = 0;

// Nombre total d'heures terminées
unsigned long heuresTotales = 0;

// Moment où le chrono a commencé/repris
unsigned long tempsDepart = 0;

bool chronoEnMarche = false;


// ======================================================
// SAUVEGARDE
// ======================================================

// Dernière seconde sauvegardée
unsigned long derniereSecondeSauvegardee = 0;


// ======================================================
// BOUTON
// ======================================================

bool boutonEtat = HIGH;
bool ancienEtatBouton = HIGH;

unsigned long debutAppui = 0;

const unsigned long DUREE_APPUI_LONG = 2000;

// Gestion du double clic
bool premierClicEnAttente = false;

unsigned long tempsPremierClic = 0;

const unsigned long DELAI_DOUBLE_CLIC = 400;

bool appuiLongTraite = false;


// ======================================================
// AFFICHAGE DES HEURES
// ======================================================

bool affichageHeures = false;


// ======================================================
// ALERTE HORAIRE
// ======================================================

bool clignotementHoraire = false;

bool affichageVisible = true;

unsigned long dernierClignotement = 0;

const unsigned long INTERVALLE_CLIGNOTEMENT = 500;

// Les alertes ne sont activées qu'après avoir
// démarré le chrono après un redémarrage.
bool alerteHoraireAutorisee = false;


// ======================================================
// DOUBLE CLIC
// ======================================================

bool clignotementDoubleClic = false;

int nombreClignotements = 0;

unsigned long dernierClignotementDoubleClic = 0;

const unsigned long INTERVALLE_DOUBLE_CLIC = 120;


// ======================================================
// AFFICHAGE
// ======================================================

void afficher(byte digit, byte segments) {

  digitalWrite(LATCH_PIN, LOW);

  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, digit);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, segments);

  digitalWrite(LATCH_PIN, HIGH);
}


void eteindreAffichage() {

  digitalWrite(LATCH_PIN, LOW);

  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0xFF);

  digitalWrite(LATCH_PIN, HIGH);
}


// ======================================================
// MISE A JOUR DU TEMPS REEL
// ======================================================

void mettreAJourTemps() {

  if (!chronoEnMarche) {
    return;
  }

  unsigned long nouveauTemps =
    (millis() - tempsDepart) / 1000UL;


  // ----------------------------------------------------
  // Une ou plusieurs heures peuvent s'être écoulées
  // ----------------------------------------------------

  if (nouveauTemps >= 3600) {

    unsigned long nouvellesHeures =
      nouveauTemps / 3600;

    heuresTotales += nouvellesHeures;

    secondesChrono =
      nouveauTemps % 3600;


    // Nouveau point de départ
    tempsDepart =
      millis() - (secondesChrono * 1000UL);


    // Alerte horaire
    if (alerteHoraireAutorisee) {

      clignotementHoraire = true;

      affichageVisible = true;

      dernierClignotement = millis();
    }

    // Sauvegarde immédiate après le changement d'heure
    sauvegarderTemps();


    Serial.print("HEURE TERMINEE - Total : ");
    Serial.print(heuresTotales);
    Serial.println(" h");

    return;
  }


  // Temps normal
  secondesChrono = nouveauTemps;
}


// ======================================================
// SAUVEGARDE
// ======================================================

void sauvegarderTemps() {

  // IMPORTANT :
  // on met d'abord à jour le temps réel avant d'écrire
  // dans la mémoire.

  if (chronoEnMarche) {

    mettreAJourTemps();
  }


  preferences.putULong("secondes", secondesChrono);
  preferences.putULong("heures", heuresTotales);

  derniereSecondeSauvegardee = secondesChrono;


  Serial.print("SAUVEGARDE -> ");
  Serial.print(heuresTotales);
  Serial.print(" h ");
  Serial.print(secondesChrono);
  Serial.println(" s");
}


// ======================================================
// CHARGEMENT
// ======================================================

void chargerTemps() {

  secondesChrono =
    preferences.getULong("secondes", 0);

  heuresTotales =
    preferences.getULong("heures", 0);


  // Sécurité : si la valeur est invalide
  if (secondesChrono >= 3600) {

    heuresTotales += secondesChrono / 3600;

    secondesChrono %= 3600;
  }


  Serial.print("RECUPERATION -> ");
  Serial.print(heuresTotales);
  Serial.print(" h ");
  Serial.print(secondesChrono);
  Serial.println(" s");
}


// ======================================================
// RESET COMPLET
// ======================================================

void resetChrono() {

  secondesChrono = 0;

  heuresTotales = 0;

  chronoEnMarche = false;

  affichageHeures = false;

  clignotementHoraire = false;

  clignotementDoubleClic = false;

  affichageVisible = true;

  alerteHoraireAutorisee = false;

  premierClicEnAttente = false;


  // Sauvegarde immédiate du zéro
  preferences.putULong("secondes", 0);
  preferences.putULong("heures", 0);

  derniereSecondeSauvegardee = 0;


  Serial.println("====================");
  Serial.println("RESET COMPLET");
  Serial.println("====================");
}


// ======================================================
// DEMARRER / REPRENDRE
// ======================================================

void demarrerChrono() {

  // On repart exactement à partir de secondesChrono
  tempsDepart =
    millis() - (secondesChrono * 1000UL);

  chronoEnMarche = true;

  alerteHoraireAutorisee = true;

  // Le prochain changement de seconde pourra être sauvegardé
  derniereSecondeSauvegardee = secondesChrono;


  Serial.println("CHRONO DEMARRE");
}


// ======================================================
// PAUSE
// ======================================================

void mettreEnPause() {

  // Récupération exacte du temps avant l'arrêt
  mettreAJourTemps();

  chronoEnMarche = false;

  // Sauvegarde immédiate
  preferences.putULong("secondes", secondesChrono);
  preferences.putULong("heures", heuresTotales);

  derniereSecondeSauvegardee = secondesChrono;


  Serial.println("CHRONO EN PAUSE");
}


// ======================================================
// GESTION DU BOUTON
// ======================================================

void gererBouton() {

  boutonEtat = digitalRead(BUTTON_PIN);


  // ----------------------------------------------------
  // DEBUT DE L'APPUI
  // ----------------------------------------------------

  if (boutonEtat == LOW &&
      ancienEtatBouton == HIGH) {

    debutAppui = millis();

    appuiLongTraite = false;
  }


  // ----------------------------------------------------
  // BOUTON MAINTENU
  // ----------------------------------------------------

  if (boutonEtat == LOW) {

    if (!appuiLongTraite &&
        millis() - debutAppui >= DUREE_APPUI_LONG) {

      resetChrono();

      appuiLongTraite = true;
    }
  }


  // ----------------------------------------------------
  // RELACHEMENT
  // ----------------------------------------------------

  if (boutonEtat == HIGH &&
      ancienEtatBouton == LOW) {

    unsigned long dureeAppui =
      millis() - debutAppui;


    // Appui court
    if (dureeAppui < DUREE_APPUI_LONG &&
        !appuiLongTraite) {


      // -----------------------------------------------
      // Une alerte horaire est en cours
      // -----------------------------------------------

      if (clignotementHoraire) {

        clignotementHoraire = false;

        affichageVisible = true;

        premierClicEnAttente = false;

        Serial.println("ALERTE HORAIRE ARRETEE");
      }


      // -----------------------------------------------
      // DOUBLE CLIC
      // -----------------------------------------------

      if (premierClicEnAttente) {

        if (millis() - tempsPremierClic <=
            DELAI_DOUBLE_CLIC) {

          // Deuxième clic
          premierClicEnAttente = false;


          // IMPORTANT :
          // on met à jour le temps AVANT le double clic.
          //
          // Ainsi le double clic ne fait jamais perdre
          // la progression du chrono.

          mettreAJourTemps();


          // Sauvegarde immédiate
          preferences.putULong(
            "secondes",
            secondesChrono
          );

          preferences.putULong(
            "heures",
            heuresTotales
          );


          lancerAffichageHeures();

          ancienEtatBouton = boutonEtat;

          return;
        }
      }


      // Premier clic
      premierClicEnAttente = true;

      tempsPremierClic = millis();
    }
  }


  ancienEtatBouton = boutonEtat;
}


// ======================================================
// TRAITEMENT DU PREMIER CLIC
// ======================================================

void gererPremierClic() {

  if (!premierClicEnAttente) {
    return;
  }


  // Le délai du double clic est dépassé
  if (millis() - tempsPremierClic >
      DELAI_DOUBLE_CLIC) {

    premierClicEnAttente = false;


    // Si on consulte les heures :
    // retour au chrono
    if (affichageHeures) {

      affichageHeures = false;

      Serial.println("RETOUR AU CHRONO");

      return;
    }


    // Sinon :
    // chrono en marche -> pause
    // chrono arrêté -> démarrage

    if (chronoEnMarche) {

      mettreEnPause();

    } else {

      demarrerChrono();
    }
  }
}


// ======================================================
// LANCER AFFICHAGE DES HEURES
// ======================================================

void lancerAffichageHeures() {

  Serial.print("HEURES TOTALES : ");
  Serial.println(heuresTotales);


  clignotementDoubleClic = true;

  nombreClignotements = 0;

  dernierClignotementDoubleClic =
    millis();

  affichageVisible = true;

  affichageHeures = false;
}


// ======================================================
// CLIGNOTEMENT DOUBLE CLIC
// ======================================================

void gererClignotementDoubleClic() {

  if (!clignotementDoubleClic) {
    return;
  }


  if (millis() -
      dernierClignotementDoubleClic >=
      INTERVALLE_DOUBLE_CLIC) {

    dernierClignotementDoubleClic =
      millis();


    affichageVisible =
      !affichageVisible;


    if (!affichageVisible) {

      nombreClignotements++;
    }


    // Deux clignotements terminés
    if (nombreClignotements >= 2 &&
        affichageVisible) {

      clignotementDoubleClic = false;

      affichageHeures = true;

      affichageVisible = true;
    }
  }
}


// ======================================================
// SAUVEGARDE A CHAQUE NOUVELLE SECONDE
// ======================================================

void gererSauvegardeChaqueSeconde() {

  if (!chronoEnMarche) {
    return;
  }


  // Met à jour le temps sans modifier
  // inutilement la mémoire flash
  mettreAJourTemps();


  // Une nouvelle seconde est atteinte
  if (secondesChrono !=
      derniereSecondeSauvegardee) {

    // Sauvegarde de la nouvelle valeur
    preferences.putULong(
      "secondes",
      secondesChrono
    );

    preferences.putULong(
      "heures",
      heuresTotales
    );


    derniereSecondeSauvegardee =
      secondesChrono;


    Serial.print("Sauvegarde seconde : ");
    Serial.print(heuresTotales);
    Serial.print(" h ");
    Serial.print(secondesChrono);
    Serial.println(" s");
  }
}


// ======================================================
// CLIGNOTEMENT HORAIRE
// ======================================================

void gererClignotementHoraire() {

  if (!clignotementHoraire) {
    return;
  }


  if (millis() -
      dernierClignotement >=
      INTERVALLE_CLIGNOTEMENT) {

    dernierClignotement =
      millis();

    affichageVisible =
      !affichageVisible;
  }
}


// ======================================================
// AFFICHAGE DES HEURES
// ======================================================

void afficherHeuresTotales() {

  // 4 chiffres maximum
  unsigned long heures =
    heuresTotales % 10000;


  byte digit1 =
    heures / 1000;

  byte digit2 =
    (heures / 100) % 10;

  byte digit3 =
    (heures / 10) % 10;

  byte digit4 =
    heures % 10;


  afficher(
    0b00000001,
    chiffres[digit1]
  );

  delay(2);


  afficher(
    0b00000010,
    chiffres[digit2]
  );

  delay(2);


  afficher(
    0b00000100,
    chiffres[digit3]
  );

  delay(2);


  afficher(
    0b00001000,
    chiffres[digit4]
  );

  delay(2);
}


// ======================================================
// AFFICHAGE DU CHRONO
// ======================================================

void afficherChrono() {

  if (!affichageVisible) {

    eteindreAffichage();

    delay(2);

    return;
  }


  // ----------------------------------------------------
  // MM:SS
  // ----------------------------------------------------

  byte minutes =
    secondesChrono / 60;

  byte secondes =
    secondesChrono % 60;


  byte digit1 =
    minutes / 10;

  byte digit2 =
    minutes % 10;

  byte digit3 =
    secondes / 10;

  byte digit4 =
    secondes % 10;


  // Dizaines de minutes
  afficher(
    0b00000001,
    chiffres[digit1]
  );

  delay(2);


  // Unités de minutes
  afficher(
    0b00000010,
    chiffres[digit2] & 0b01111111
  );

  delay(2);


  // Dizaines de secondes
  afficher(
    0b00000100,
    chiffres[digit3]
  );

  delay(2);


  // Unités de secondes
  afficher(
    0b00001000,
    chiffres[digit4]
  );

  delay(2);
}


// ======================================================
// AFFICHAGE PRINCIPAL
// ======================================================

void gererAffichage() {

  // Double clic en cours
  if (clignotementDoubleClic) {

    if (!affichageVisible) {

      eteindreAffichage();

      delay(2);

      return;
    }

    afficherChrono();

    return;
  }


  // Affichage des heures
  if (affichageHeures) {

    afficherHeuresTotales();

    return;
  }


  // Affichage normal
  afficherChrono();
}


// ======================================================
// SETUP
// ======================================================

void setup() {

  Serial.begin(115200);


  // ----------------------------------------------------
  // BROCHES
  // ----------------------------------------------------

  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);

  // Bouton entre GPIO 4 et GND
  pinMode(BUTTON_PIN, INPUT_PULLUP);


  // ----------------------------------------------------
  // MEMOIRE
  // ----------------------------------------------------

  preferences.begin("chrono", false);


  // Récupération de la dernière sauvegarde
  chargerTemps();


  // ----------------------------------------------------
  // ETAT INITIAL
  // ----------------------------------------------------

  // Ne jamais démarrer automatiquement
  chronoEnMarche = false;

  affichageHeures = false;

  clignotementHoraire = false;

  clignotementDoubleClic = false;

  affichageVisible = true;

  alerteHoraireAutorisee = false;

  premierClicEnAttente = false;


  derniereSecondeSauvegardee =
    secondesChrono;


  Serial.println();
  Serial.println("==============================");
  Serial.println("       TIMER TRACKER");
  Serial.println("==============================");

  Serial.print("Heures totales : ");
  Serial.println(heuresTotales);

  Serial.print("Temps actuel : ");
  Serial.print(secondesChrono);
  Serial.println(" secondes");

  Serial.println("Etat : ARRETE");
  Serial.println("==============================");
}


// ======================================================
// LOOP
// ======================================================

void loop() {

  // Bouton
  gererBouton();

  // Détermine si le premier clic était simple
  gererPremierClic();

  // Double clic
  gererClignotementDoubleClic();

  // Mise à jour du chrono
  mettreAJourTemps();

  // Sauvegarde à chaque nouvelle seconde
  gererSauvegardeChaqueSeconde();

  // Alerte horaire
  gererClignotementHoraire();

  // Affichage
  gererAffichage();
}