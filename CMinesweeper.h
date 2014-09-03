#pragma once

#include "utils.h"
#include "SVector2D.h"	
#include "CParams.h"

struct STarget{
	int		iTargetIndex;
	double	dDistanceToTarget;
	STarget(int targetindex=0, double distanceToTarget=9999 )
		: iTargetIndex(targetindex), dDistanceToTarget(distanceToTarget) {}
};

class CMinesweeper
{
protected:
	//the number of Mines gathered by the sweeper 
	double			m_dMinesGathered;
	//the scale of the sweeper when drawn
	double			m_dScale;

	//index position of closest mine
	STarget		m_iClosestMine;
	int			m_iClosestRock;
	int			m_iClosestSupermine;
	STarget		m_iTargetMine;

	bool			m_bDead;
public:
	CMinesweeper(void):m_dMinesGathered(0),
					   m_dScale(CParams::iSweeperScale),
					   m_iTargetMine(STarget(-1)), 
					   m_bDead(false){}
	virtual ~CMinesweeper(void);

	void		IncrementMinesGathered(){++m_dMinesGathered;}
	double		MinesGathered()const{return m_dMinesGathered;}
	void		Reset();
	void		die();
	bool		isDead() const;
	int			getClosestMine();
	int			getClosestRock();
	int			getClosestSupermine();
};

