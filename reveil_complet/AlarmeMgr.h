#ifndef ALARMEMGR_H
#define ALARMEMGR_H

#include <Arduino.h>
#include <Time.h>

/// gestionnaire d'alarme
class AlarmeMgr
{
public:
	AlarmeMgr(int pinCommandeSonnerie, time_t heureAlarme, bool alarmeActivable) :
		m_pinCommandeSonnerie(pinCommandeSonnerie),
		m_heureAlarme(heureAlarme),
		m_alarmeActivable(alarmeActivable)
	{
		pinMode(m_pinCommandeSonnerie, OUTPUT);
	}

	bool activable(void) const { return m_alarmeActivable; }

	/// active ou désactive l'alarme
	void activable(bool activable) { m_alarmeActivable = activable; }

	const time_t & getHeureAlarme(void) { return m_heureAlarme; }
	void setHeureAlarme(time_t heure) { m_heureAlarme = heure; }

	void loop()
	{
		if (m_alarmeActivable)
		{
			tmElements_t heureReveilDecomposee;
			breakTime(m_heureAlarme, heureReveilDecomposee);
			if (heureReveilDecomposee.Hour == hour()
				&& heureReveilDecomposee.Minute == minute())
			{
				digitalWrite(m_pinCommandeSonnerie, HIGH);
			}
			else
				digitalWrite(m_pinCommandeSonnerie, LOW);

		}
		else
			digitalWrite(m_pinCommandeSonnerie, LOW);
	}

private:
	int m_pinCommandeSonnerie;	///< broche d'activation de la sonnerie
	time_t m_heureAlarme;		///< heure à laquelle déclencher l'alarme
	bool m_alarmeActivable;		///< vrai si l'alarme peut sonner
};
#endif //ALARMEMGR_H
