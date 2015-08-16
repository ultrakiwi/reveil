#ifndef ALARMEMGR_H
#define ALARMEMGR_H

#include <Arduino.h>
#include <Time.h>


/// gestionnaire d'alarmes.
class AlarmeMgr
{
public:

	static const int NB_ALARMES_MAX = 4;	///< nombre maximal d'alarmes

	AlarmeMgr(int pinCommandeSonnerie)
		:
		m_pinCommandeSonnerie(pinCommandeSonnerie),
		m_nbAlarmes(0),
		m_alarmeActive(false)
	{
		pinMode(m_pinCommandeSonnerie, OUTPUT);
	}


	const time_t & getHeureAlarme(const int & ind) { return m_heureAlarme[ind]; }
	void setHeureAlarme(const int & ind, time_t heure) { m_heureAlarme[ind] = heure; }

	bool getAlarmeActive(void) const { return m_alarmeActive; }
	void setAlarmeActive(bool active) { m_alarmeActive = active; }

	int getNbAlarmes(void) const { return m_nbAlarmes; }
	void setNbAlarmes(const unsigned int & nb) { if (nb <= NB_ALARMES_MAX) m_nbAlarmes = nb; }

	/// Vrai si l'alarme doit être déclenchée.
	/// Critère actuel : l'heure actuelle est égale à l'heure d'une l'alarme
	/// (comparaison des heures, minutes et secondes), et cette alarme est activable.
	/// _return true si l'alarme doit être activée.
	bool activerAlarme(void)
	{
		for (int i(0); i < m_nbAlarmes; ++i)
		{
			tmElements_t heureReveilDecomposee;
			breakTime(m_heureAlarme[i], heureReveilDecomposee);
			if (heureReveilDecomposee.Hour == hour()
				&& heureReveilDecomposee.Minute == minute()
				&& heureReveilDecomposee.Second == second())
				return true;
		}
		return false;
	}

	void loop()
	{
		digitalWrite(m_pinCommandeSonnerie, (m_alarmeActive ? HIGH : LOW));
	}

private:
	int		m_pinCommandeSonnerie;			///< broche d'activation de la sonnerie
	int m_nbAlarmes;						///< nombre d'alarmes actives
	time_t m_heureAlarme[NB_ALARMES_MAX];	///< heure d'activation des alarmes
	bool	m_alarmeActive;					///< vrai si l'alarme est actuellement activée
};
#endif //ALARMEMGR_H
