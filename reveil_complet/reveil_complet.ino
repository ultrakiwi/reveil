/// Réveil programmable.
/// Gère l'heure actuelle et l'heure programmable ; tout s'affiche sur un
/// écran LCD. A l'heure programmée, la sortie de l'alarme passe à l'état haut.
/// Navigation affichage de l'heure -> réglage heure actuelle -> réglage heure réveil
/// via un BP ; 2 autres BP pour ajuster les heures / les minutes.
/// Les boutons sont gérés avec une classe spécifique Bouton.
/// L'alarme dure une minute. Le déclenchement de l'alarme est activable / désactivable
/// via un appui sur le bouton Heures (quand on est en mode affichage de l'heure actuelle).
/// Lorsque l'alarme est activable, l'heure programmée est affichée en plus de l'heure actuelle.
/// L'ensemble est géré par une machine d'états.
///
/// Points d'amélioration :
/// meilleure gestion du temps (via une horloge dédiée)

#include <FiniteStateMachine.h>
#include <LiquidCrystal.h>
#include <Time.h>

#include "Bouton.h"    // gère un bouton poussoir
#include "AlarmeMgr.h" // gère l'alarme

// Définition de constantes
const int pinRS = 2; // broche de l'autorisation de lecture du LCD
const int pinE = 3; // broche E (activation du registre) du LCD
const int pinD4 = 4; // broche D4 du LCD
const int pinD5 = 5; // broche D5 du LCD
const int pinD6 = 6; // broche D6 du LCD
const int pinD7 = 7; // broche D7 du LCD

const int pinBPHeure = 9;  // pour le réglage de l'heure
const int pinBPMinute = 10;  // pour le réglage des minutes
const int pinBPReglage = 8;  // pour commander le réglage
const int pinSortieReveil = 11;  // commande du dispositif de réveil

const bool etatAppui = HIGH;  // état d'un bouton quand on appuie dessus

// objets permanents
LiquidCrystal lcd(pinRS, pinE, pinD4, pinD5, pinD6, pinD7);

Bouton BPReglage(pinBPReglage, etatAppui);
Bouton BPHeure(pinBPHeure, etatAppui);
Bouton BPMinute(pinBPMinute, etatAppui);

// énumération permettant d'identifier facilement les états du système
// avantage : on peut faire un switch pour gérer les transitions dans la focntion loop
enum IdEtats
{
	AFFICHAGE_HEURE_ACTUELLE,
	REGLAGE_HEURE_ACTUELLE,
	REGLAGE_HEURE_REVEIL
};

// initialisation des états de l'automate
State etatAffichageHeureActuelle(entreeAffichageHeureActuelle, afficherHeureActuelle, NO_EXIT);
State etatReglageHeureActuelle(entreeReglageHeureActuelle, reglageHeureActuelle, NO_EXIT);
State etatReglageHeureReveil(entreeReglageHeureReveil, reglageHeureReveil, NO_EXIT);
// initialisation de l'automate lui-même
FiniteStateMachine automate(etatAffichageHeureActuelle);
IdEtats etatActuel = AFFICHAGE_HEURE_ACTUELLE;

// initialisation du gestionnaire d'alarme
AlarmeMgr alarme(pinSortieReveil, now(), false);

/// Initialisation du module.
void setup()
{
	// initialisation de l'écran
	lcd.begin(16, 2); // 16 caractères par ligne, 2 lignes
	lcd.noAutoscroll();

	// initialisation de la liaison série (pour le debug)
	Serial.begin(9600);
}

/// Gère les interactions utilisateurs pouvant modifier une certaine heure (actuelle ou réveil).
/// \return heure modifiée
time_t reglerHeure(time_t heureOrigine)
{
	// on scinde le temps en champs exploitables
	tmElements_t heureDecomposee;
	breakTime(heureOrigine, heureDecomposee);

	// réglage des heures
	if (BPHeure.onPress())
	{
		heureDecomposee.Hour = heureDecomposee.Hour + 1;

		if (heureDecomposee.Hour >= 24)
			heureDecomposee.Hour = 0;

		Serial.println(heureDecomposee.Hour);
	}

	// réglage des minutes
	if (BPMinute.onPress())
	{
		heureDecomposee.Minute = heureDecomposee.Minute + 1;

		if (heureDecomposee.Minute >= 60)
			heureDecomposee.Minute = 0;

		Serial.println(heureDecomposee.Minute);
	}

	return makeTime(heureDecomposee);
}

/// Affiche une heure sur l'écran LCD.
void afficherHeure(int heures, int minutes, int secondes, int ligne)
{
	lcd.setCursor(0, ligne);
	String heureAffichee;

	if (heures < 10)
		heureAffichee += String("0");

	heureAffichee += String(heures);
	heureAffichee += String(":");

	if (minutes < 10)
		heureAffichee += String("0");

	heureAffichee += String(minutes);
	heureAffichee += String(":");

	if (secondes < 10)
		heureAffichee += String("0");

	heureAffichee += String(secondes);
	lcd.print(heureAffichee);
}
void reglageHeureReveil(void)
{
	alarme.setHeureAlarme(reglerHeure(alarme.getHeureAlarme()));
	lcd.setCursor(0, 0);
	lcd.print("reglage reveil");
	tmElements_t heureDecomposee;
	breakTime(alarme.getHeureAlarme(), heureDecomposee);
	afficherHeure(heureDecomposee.Hour, heureDecomposee.Minute, heureDecomposee.Second, 1);
}

void reglageHeureActuelle(void)
{
	time_t heureActuelle = now();
	heureActuelle = reglerHeure(heureActuelle);
	setTime(heureActuelle);
	lcd.setCursor(0, 0);
	lcd.print("reglage horloge");
	afficherHeure(hour(), minute(), second(), 1);
}

/// Efface l'écran LCD.
void effacerEcran(void)
{
	lcd.clear();
}

/// Méthode appelée lorsqu'on entre dans l'état "afficher heure actuelle".
void entreeAffichageHeureActuelle(void)
{
	Serial.println("Passage a l'etat AFFICHAGE_HEURE_ACTUELLE");
	effacerEcran();
	etatActuel = AFFICHAGE_HEURE_ACTUELLE;
}

/// Méthode appelée à chaque cycle quand on est dans l'état "afficher heure actuelle".
void afficherHeureActuelle(void)
{
	if (BPHeure.onPress()) // armement / désarmement de l'alarme
	{
		alarme.activable() ? alarme.activable(false) : alarme.activable(true);
		alarme.activable() ? Serial.println("alarme ON") : Serial.println("alarme OFF");
		effacerEcran(); // nécessaire, sinon le message reste quand on désactive l'alarme
	}

	afficherHeure(hour(), minute(), second(), 0);

	if (alarme.activable())
	{
		tmElements_t heureReveilDecomposee;
		breakTime(alarme.getHeureAlarme(), heureReveilDecomposee);
		lcd.setCursor(0, 1);
		String MsgReveil("reveil a ");
		if (heureReveilDecomposee.Hour < 10) MsgReveil += String("0");
		MsgReveil += String(heureReveilDecomposee.Hour) + String("h");
		if (heureReveilDecomposee.Minute < 10) MsgReveil += String("0");
		MsgReveil += String(heureReveilDecomposee.Minute);
		lcd.print(MsgReveil);
	}
}

/// Méthode appelée lorsqu'on entre dans l'état "réglage heure actuelle".
void entreeReglageHeureActuelle(void)
{
	Serial.println("Passage a l'etat REGLAGE_HEURE_ACTUELLE");
	effacerEcran();
	etatActuel = REGLAGE_HEURE_ACTUELLE;
}

/// Méthode appelée lorsqu'on entre dans l'état "réglage heure réveil".
void entreeReglageHeureReveil(void)
{
	Serial.println("Passage a l'etat REGLAGE_HEURE_REVEIL");
	effacerEcran();
	etatActuel = REGLAGE_HEURE_REVEIL;
}

void loop()
{
	// Rafraîchissement de l'état des boutons
	BPReglage.listen();
	BPHeure.listen();
	BPMinute.listen();

	switch (etatActuel)
	{
	case AFFICHAGE_HEURE_ACTUELLE:
		if (BPReglage.onPress())// changement d'état
			automate.transitionTo(etatReglageHeureActuelle);

		break;

	case REGLAGE_HEURE_ACTUELLE:
		if (BPReglage.onPress())// changement d'état
			automate.transitionTo(etatReglageHeureReveil);

		break;

	case REGLAGE_HEURE_REVEIL:
		if (BPReglage.onPress())// changement d'état
			automate.transitionTo(etatAffichageHeureActuelle);

		break;

	default:
		Serial.println("Erreur dans la machine d'etats : etat inconnu!");
		break;
	}

	// traitement de l'état actuel
	automate.update();

	// mise à jour de l'état de l'alarme
	alarme.loop();

	delay(100);
}
