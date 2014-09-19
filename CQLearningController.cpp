/**
         (                                      
   (     )\ )                                   
 ( )\   (()/(   (    ) (        (        (  (   
 )((_)   /(_)) ))\( /( )(   (   )\  (    )\))(  
((_)_   (_))  /((_)(_)|()\  )\ |(_) )\ )((_))\  
 / _ \  | |  (_))((_)_ ((_)_(_/((_)_(_/( (()(_) 
| (_) | | |__/ -_) _` | '_| ' \)) | ' \)) _` |  
 \__\_\ |____\___\__,_|_| |_||_||_|_||_|\__, |  
                                        |___/   

Refer to Watkins, Christopher JCH, and Peter Dayan. "Q-learning." Machine learning 8. 3-4 (1992): 279-292
for a detailed discussion on Q Learning
*/
#include "CQLearningController.h"


CQLearningController::CQLearningController(HWND hwndMain):
	CDiscController(hwndMain),
	_grid_size_x(CParams::WindowWidth / CParams::iGridCellDim + 1),
	_grid_size_y(CParams::WindowHeight / CParams::iGridCellDim + 1)
{
	m_qTable = new double **[CParams::iNumSweepers];
	
	for (int i = 0; i < CParams::iNumSweepers; i++) {
		m_qTable[i] = new double*[_grid_size_x * _grid_size_y];
		for (int j = 0; j < _grid_size_x * _grid_size_y; j++) 
			m_qTable[i][j] = new double[4];
	}

}
/**
 The update method should allocate a Q table for each sweeper (this can
 be allocated in one shot - use an offset to store the tables one after the other)

 You can also use a boost multiarray if you wish
*/
void CQLearningController::InitializeLearningAlgorithm(void)
{
	for (int i_sweepers = 0; i_sweepers < CParams::iNumSweepers; i_sweepers++)
		for (int i_states = 0; i_states < _grid_size_x * _grid_size_y; i_states++)
			for (int i_actions = 0; i_actions < 4; i_actions++)
				m_qTable[i_sweepers][i_states][i_actions] = 0.0;
}
/**
 The immediate reward function. This computes a reward upon achieving the goal state of
 collecting all the mines on the field. It may also penalize movement to encourage exploring all directions and 
 of course for hitting supermines/rocks!
*/
double CQLearningController::R(int x,int y, int sweeper_no){
	//TODO: roll your own here!
	int state = x * CParams::iGridCellDim + y;

	double i_immediateReward = 0.0;

	SVector2D<int> pos_rock = m_vecObjects[m_vecSweepers[sweeper_no]->getClosestRock ()]->getPosition();
	SVector2D<int> pos_mine = m_vecObjects[m_vecSweepers[sweeper_no]->getClosestMine ()]->getPosition();
	SVector2D<int> pos_supermine = m_vecObjects[m_vecSweepers[sweeper_no]->getClosestSupermine ()]->getPosition();
	return i_immediateReward;	
}
/**
The update method. Main loop body of our Q Learning implementation
See: Watkins, Christopher JCH, and Peter Dayan. "Q-learning." Machine learning 8. 3-4 (1992): 279-292
*/
bool CQLearningController::Update(void)
{
	//m_vecSweepers is the array of minesweepers
	//everything you need will be m_[something] ;)
	uint cDead = std::count_if(m_vecSweepers.begin(),
							   m_vecSweepers.end(),
						       [](CDiscMinesweeper * s)->bool{
								return s->isDead();
							   });
	if (cDead == CParams::iNumSweepers){
		printf("All dead ... skipping to next iteration\n");
		m_iTicks = CParams::iNumTicks;
	}

	for (int sw = 0; sw < CParams::iNumSweepers; ++sw){
		if (m_vecSweepers[sw]->isDead()) 
			continue;
		/**
		Q-learning algorithm according to:
		Watkins, Christopher JCH, and Peter Dayan. "Q-learning." Machine learning 8. 3-4 (1992): 279-292
		*/
		//1:::Observe the current state:
		// get possible actions at current state
		//TODO
		SVector2D<int> disc_posSweeper = m_vecSweepers[sw]->Position();
		disc_posSweeper.x = (int)(disc_posSweeper.x / CParams::iGridCellDim);
		disc_posSweeper.y = (int)(disc_posSweeper.y / CParams::iGridCellDim);
		int iCurrentState = disc_posSweeper.x * CParams::iGridCellDim + disc_posSweeper.y;

		//2:::Select action with highest historic return:
		//TODO
		double maxObservableAction = MIN_REWARD - 1;
		ROTATION_DIRECTION nextAction = ROTATION_DIRECTION::EAST;
		int maxAction = -1;
		for (int i = 0 ; i < 4 ; i++) {
			if (maxObservableAction < m_qTable [sw][iCurrentState][i]) {
				maxObservableAction =  m_qTable [sw][iCurrentState][i];
				maxAction = i;
			}
		}
		if (maxAction != -1)
			nextAction = (ROTATION_DIRECTION) maxAction;
		m_vecSweepers[sw]->setRotation (nextAction); 
		//now call the parents update, so all the sweepers fulfill their chosen action
	}
	
	CDiscController::Update(); //call the parent's class update. Do not delete this.
	
	for (int sw = 0; sw < CParams::iNumSweepers; ++sw){
		if (m_vecSweepers[sw]->isDead()) 
			continue;
		//TODO:compute your indexes.. it may also be necessary to keep track of the previous state
		//3:::Observe new state:
		//TODO
		SVector2D<int> disc_posSweeper = m_vecSweepers[sw]->PrevPosition();
		disc_posSweeper.x = (int)(disc_posSweeper.x / CParams::iGridCellDim);
		disc_posSweeper.y = (int)(disc_posSweeper.y / CParams::iGridCellDim);
		int iPreviousState = disc_posSweeper.x * CParams::iGridCellDim + disc_posSweeper.y;

		disc_posSweeper = m_vecSweepers[sw]->Position();
		disc_posSweeper.x = (int)(disc_posSweeper.x / CParams::iGridCellDim);
		disc_posSweeper.y = (int)(disc_posSweeper.y / CParams::iGridCellDim);
		int iCurrentState = disc_posSweeper.x * CParams::iGridCellDim + disc_posSweeper.y;
		
		ROTATION_DIRECTION currentDirection = m_vecSweepers[sw]->getRotation();
		double valueIteration = MIN_REWARD - 1; 
		for (int i = 0; i < 4 ; i ++ ) {
			valueIteration = (valueIteration < m_qTable[sw][iCurrentState][i]) ?  m_qTable[sw][iCurrentState][i] : valueIteration;
		}
		double reward = R (disc_posSweeper.x, disc_posSweeper.y, sw);
		m_qTable [sw][iPreviousState][currentDirection] = (1 - LEARNING_FACTOR) * m_qTable [sw][iPreviousState][currentDirection] + LEARNING_FACTOR * (reward - DISCOUNT_FACTOR * valueIteration);

	}
	return true;
}

CQLearningController::~CQLearningController(void)
{
	for (int i = 0; i < CParams::iNumSweepers; i++) {
		for (int j = 0; j < _grid_size_x * _grid_size_y; j++){ 
			delete [] m_qTable[i][j];
		}
		delete [] m_qTable[i];
	}
	delete[] m_qTable;
}
