#ifndef INTERFACEIO_H
#define INTERFACEIO_H

#include "Arduino.h"
#include <LiquidCrystal.h> // inclure �galement dans le .ino
//#include "Bouton.h"    // g�re un bouton poussoir



// Affectation des entr�es / sorties
enum ArduinoPinout
{
	PIN_RS = 2, // broche de l'autorisation de lecture du LCD
	PIN_E = 3, // broche E (activation du registre) du LCD
	PIN_D4 = 4, // broche D4 du LCD
	PIN_D5 = 5, // broche D5 du LCD
	PIN_D6 = 6, // broche D6 du LCD
	PIN_D7 = 7, // broche D7 du LCD

	PIN_BP_H = 9,  // pour le r�glage de l'heure
	PIN_BP_MIN = 10,  // pour le r�glage des minutes
	PIN_BP_MENU = 8,  // pour commander le r�glage
	PIN_SORTIE_REVEIL = 11  // commande du dispositif de r�veil
};

class InterfaceIO
{
public:
	InterfaceIO();

	void afficherMessage(const String & msg, int ligne)
	{
		m_lcd.setCursor(0, ligne);
		m_lcd.print(msg);
	}

	void afficherHeure(int heures, int minutes, int secondes, int ligne);
	void effacerEcran(void);

	//const Bouton & getBPHeure(void) const { return BPHeure; }

	//void listen(void)
	//{
	//	BPHeure.listen();
	//}

private:
	LiquidCrystal	m_lcd;	// LCD
	//Bouton BPHeure;
};

#endif //INTERFACEIO_H