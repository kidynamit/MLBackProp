#include "CMinesweeper.h"

CMinesweeper::~CMinesweeper(void)
{
}

void CMinesweeper::Reset(){
	this->m_dMinesGathered = 0;
	this->m_bDead = false;
}
void CMinesweeper::die(int timeofdeath){
	this->m_bDead = true;
	this->m_iTimeOfDeath = timeofdeath;
}
bool CMinesweeper::isDead() const{
	return m_bDead;
}
int	CMinesweeper::getClosestMine() const
{
	return m_iClosestMine;
}
int	CMinesweeper::getClosestRock() const
{
	return m_iClosestRock;
}
int	CMinesweeper::getClosestSupermine() const
{
	return m_iClosestSupermine;
}
int CMinesweeper::getTargetMine() const
{
	return m_iTargetMine;
}
int CMinesweeper::getTimeOfDeath() const
{	
	return ((this->isDead ()) ?  m_iTimeOfDeath : -1);
}
