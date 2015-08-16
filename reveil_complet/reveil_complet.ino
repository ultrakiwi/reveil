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

#include <FiniteStateMachine.h>
#include <LiquidCrystal.h>
#include <Time.h>

#include "Bouton.h"    // gère un bouton poussoir
#include "InterfaceIO.h"
#include "AlarmeMgr.h" // gère l'alarme

const bool etatAppui = HIGH;  // état d'un bouton quand on appuie dessus

Bouton BPReglage(PIN_BP_MENU, etatAppui);
Bouton BPHeure(PIN_BP_H, etatAppui);
Bouton BPMinute(PIN_BP_MIN, etatAppui);

// énumération permettant d'identifier facilement les états du système
// avantage : on peut faire un switch pour gérer les transitions dans la focntion loop
enum IdEtats
{
	AFFICHAGE_HEURE_ACTUELLE,
	REGLAGE_HEURE_ACTUELLE,
	REGLAGE_HEURE_REVEIL,
	REVEIL
};

InterfaceIO m_IO;

void incrementerHeure(uint8_t & heure);
void incrementerMinutes(uint8_t & minutes);
void reglageHeureReveil(void);
void reglageHeureActuelle(void);
void reveiller(void);

// initialisation des états de l'automate
State etatAffichageHeureActuelle(entreeAffichageHeureActuelle, afficherHeureActuelle, NO_EXIT);
State etatReglageHeureActuelle(entreeReglageHeureActuelle, reglageHeureActuelle, NO_EXIT);
State etatReglageHeureReveil(entreeReglageHeureReveil, reglageHeureReveil, NO_EXIT);
State etatReveil(entreeReveil, reveiller, couperReveil);

// initialisation de l'automate lui-même
FiniteStateMachine automate(etatAffichageHeureActuelle);
IdEtats etatActuel = AFFICHAGE_HEURE_ACTUELLE;

// initialisation du gestionnaire d'alarme
AlarmeMgr alarme(PIN_SORTIE_REVEIL, now(), false, false);



/// Initialisation du module.
void setup()
{
	// initialisation de la liaison série (pour le debug)
	Serial.begin(9600);
	//delay(500);
	Serial.println("InterfaceIO : init OK");
}

/// Incrémente une heure en controlant la validité.
void incrementerHeure(uint8_t & heure)
{
	if (++heure >= 24)
		heure = 0;

	//Serial.println(heure);
}

/// Incrémente les minutes en controlant la validité.
void incrementerMinutes(uint8_t & minutes)
{
	if (++minutes >= 60)
		minutes = 0;

	//Serial.println(minutes);
}

void reglageHeureReveil(void)
{
	// on scinde le temps en champs exploitables
	tmElements_t heureDecomposee;
	breakTime(alarme.getHeureAlarme(), heureDecomposee);

	// réglage de l'heure de réveil
	if(BPHeure.onPress())
	{
		incrementerHeure(heureDecomposee.Hour);
	}

	// réglage des minutes
	if (BPMinute.onPress())
	{
		incrementerMinutes(heureDecomposee.Minute);
	}

	alarme.setHeureAlarme(makeTime(heureDecomposee));

	m_IO.afficherMessage("reglage reveil", 0);
	m_IO.afficherHeure(heureDecomposee.Hour, heureDecomposee.Minute, heureDecomposee.Second, 1);
}

void reglageHeureActuelle(void)
{
	// on scinde le temps en champs exploitables
	tmElements_t heureDecomposee;
	breakTime(now(), heureDecomposee);

	// réglage de l'heure actuelle
	if(BPHeure.onPress())
	{
		incrementerHeure(heureDecomposee.Hour);
	}

	// réglage des minutes
	if(BPMinute.onPress())
	{
		incrementerMinutes(heureDecomposee.Minute);
	}

	setTime(makeTime(heureDecomposee));
	m_IO.afficherMessage("reglage horloge", 0);
	m_IO.afficherHeure(hour(), minute(), second(), 1);
}

void reveiller(void)
{
	m_IO.afficherHeure(hour(), minute(), second(), 0);
	m_IO.afficherMessage("Debout!", 1);
}

void couperReveil(void)
{
	alarme.setAlarmeActive(false);
}

/// Méthode appelée lorsqu'on entre dans l'état "afficher heure actuelle".
void entreeAffichageHeureActuelle(void)
{
	Serial.println("Passage a l'etat AFFICHAGE_HEURE_ACTUELLE");
	m_IO.effacerEcran();
	etatActuel = AFFICHAGE_HEURE_ACTUELLE;
}

/// Méthode appelée à chaque cycle quand on est dans l'état "afficher heure actuelle".
void afficherHeureActuelle(void)
{
	if(BPHeure.onPress()) // armement / désarmement de l'alarme
	{
		alarme.activable() ? alarme.activable(false) : alarme.activable(true);
		alarme.activable() ? Serial.println("alarme ON") : Serial.println("alarme OFF");
		m_IO.effacerEcran(); // nécessaire, sinon le message reste quand on désactive l'alarme		
	}

	m_IO.afficherHeure(hour(), minute(), second(), 0);

	if (alarme.activable())
	{
		tmElements_t heureReveilDecomposee;
		breakTime(alarme.getHeureAlarme(), heureReveilDecomposee);

		m_IO.afficherMessage(String("Reveil a ")
			+ (heureReveilDecomposee.Hour < 10 ? "0" : "")
			+ String(heureReveilDecomposee.Hour) + "h"
			+ (heureReveilDecomposee.Minute < 10 ? "0" : "")
			+ String(heureReveilDecomposee.Minute), 1);
	}
}

/// Méthode appelée lorsqu'on entre dans l'état "réglage heure actuelle".
void entreeReglageHeureActuelle(void)
{
	Serial.println("Passage a l'etat REGLAGE_HEURE_ACTUELLE");
	m_IO.effacerEcran();
	etatActuel = REGLAGE_HEURE_ACTUELLE;
}

/// Méthode appelée lorsqu'on entre dans l'état "réglage heure réveil".
void entreeReglageHeureReveil(void)
{
	Serial.println("Passage a l'etat REGLAGE_HEURE_REVEIL");
	m_IO.effacerEcran();
	etatActuel = REGLAGE_HEURE_REVEIL;
}

/// Méthode appelée lorsqu'on entre dans l'état "réveil".
void entreeReveil(void)
{
	Serial.println("Passage a l'etat REVEIL");
	m_IO.effacerEcran();
	etatActuel = REVEIL;
	alarme.setAlarmeActive(true);
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
		if (alarme.activerAlarme())// changement d'état
		{
			automate.transitionTo(etatReveil);
			break;
		}
		if (BPReglage.onPress())// changement d'état
			automate.transitionTo(etatReglageHeureActuelle);

		break;

	case REGLAGE_HEURE_ACTUELLE:
		if (BPReglage.onPress())// changement d'état
			automate.transitionTo(etatReglageHeureReveil);

		break;

	case REGLAGE_HEURE_REVEIL:
		if (BPReglage.onPress())// changement d'état
		{
			automate.transitionTo(etatAffichageHeureActuelle);
			break;
		}
		break;

	case REVEIL:
		if (BPReglage.onPress()
			|| BPHeure.onPress()
			|| BPMinute.onPress())	//  n'importe quel bouton coupe l'alarme
		{
			automate.transitionTo(etatAffichageHeureActuelle);
		}
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

