#pragma once

#include "utils.h"
#include "SVector2D.h"	
#include "CParams.h"

class CMinesweeper
{
protected:
	//the number of Mines gathered by the sweeper 
	double			m_dMinesGathered;
	//the scale of the sweeper when drawn
	double			m_dScale;
	int				m_iTimeOfDeath  = -1;


	//index position of closest mine
	int			m_iClosestMine;
	int			m_iClosestRock;
	int			m_iClosestSupermine;
	int			m_iTargetMine;

	bool			m_bDead;
public:
	CMinesweeper(void):m_dMinesGathered(0),
					   m_dScale(CParams::iSweeperScale),
					   m_bDead(false), m_iTargetMine(-1){}
	virtual ~CMinesweeper(void);

	void		IncrementMinesGathered(){++m_dMinesGathered;}
	double		MinesGathered()const{return m_dMinesGathered;}
	void		Reset();
	void		die(int timeofdeath = 0);
	bool		isDead() const;
	int			getClosestMine() const;
	int			getClosestRock() const;
	int			getClosestSupermine() const;
	int			getTargetMine() const;
	int			getTimeOfDeath() const;
	void		setTargetMine(int iMine);
};

