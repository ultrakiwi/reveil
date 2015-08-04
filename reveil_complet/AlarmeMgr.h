#ifndef ALARMEMGR_H
#define ALARMEMGR_H

#include <Arduino.h>
#include <Time.h>

/// gestionnaire d'alarme
class AlarmeMgr
{
public:
	AlarmeMgr(int pinCommandeSonnerie, 
		time_t heureAlarme, 
		bool alarmeActivable,
		bool alarmeActive) :
		m_pinCommandeSonnerie(pinCommandeSonnerie),
		m_heureAlarme(heureAlarme),
		m_alarmeActivable(alarmeActivable),
		m_alarmeActive(alarmeActive)
	{
		pinMode(m_pinCommandeSonnerie, OUTPUT);
	}


	const time_t & getHeureAlarme(void) { return m_heureAlarme; }
	void setHeureAlarme(time_t heure) { m_heureAlarme = heure; }

	bool activable(void) const { return m_alarmeActivable; }
	void activable(bool activable) { m_alarmeActivable = activable; }

	bool getAlarmeActive(void) const { return m_alarmeActive; }
	void setAlarmeActive(bool active) { m_alarmeActive = active; }

	/// Vrai si l'alarme doit être déclenchée.
	/// Critère actuel : l'heure actuelle est égale à l'heure de l'alarme
	/// (comparaison des heures, minutes et secondes), et l'alarme est activable.
	/// _return true si l'alarme doit être activée.
	bool activerAlarme(void)
	{
		if(!m_alarmeActivable)	// pas d'activation si elle n'est pas activable
			return false;
		else
		{
			tmElements_t heureReveilDecomposee;
			breakTime(m_heureAlarme, heureReveilDecomposee);
			return (heureReveilDecomposee.Hour == hour()
				&& heureReveilDecomposee.Minute == minute()
				&& heureReveilDecomposee.Second == second());
		}
	}

	void loop()
	{
		digitalWrite(m_pinCommandeSonnerie, (m_alarmeActive ? HIGH : LOW) );
	}

private:
	int		m_pinCommandeSonnerie;	///< broche d'activation de la sonnerie
	time_t	m_heureAlarme;			///< heure à laquelle déclencher l'alarme
	bool	m_alarmeActivable;		///< vrai si l'alarme peut sonner
	bool	m_alarmeActive;			///< vrai si l'alarme est actuellement activée
};
#endif //ALARMEMGR_H
