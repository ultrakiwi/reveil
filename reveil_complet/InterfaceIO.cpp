#include "InterfaceIO.h"

InterfaceIO::InterfaceIO()
	:
	m_lcd(PIN_RS, PIN_E, PIN_D4, PIN_D5, PIN_D6, PIN_D7)
	//BPHeure(PIN_BP_H, etatAppui)
{
	// initialisation de l'�cran
	m_lcd.begin(16, 2); // 16 caract�res par ligne, 2 lignes
	m_lcd.noAutoscroll();
}


/// Affiche une heure sur l'�cran LCD.
void InterfaceIO::afficherHeure(int heures, int minutes, int secondes, int ligne)
{
	m_lcd.setCursor(0, ligne);

	afficherMessage((heures < 10 ? "0" : "") + String(heures) + String(":") +
		(minutes < 10 ? "0" : "") + String(minutes) + ":" +
		(secondes < 10 ? "0" : "") + String(secondes), ligne);
}

/// Efface l'�cran LCD.
void InterfaceIO::effacerEcran(void)
{
	m_lcd.clear();
}
