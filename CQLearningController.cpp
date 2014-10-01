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
			m_qTable[i][j] = new double[12];
	}
	m_iCollectedMines = new int[CParams::iNumSweepers];
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
			for (int i_actions = 0; i_actions < 12; i_actions++)
				m_qTable[i_sweepers][i_states][i_actions] = /*(i_actions <= 4)*/ 0.0;
}
/**
 The immediate reward function. This computes a reward upon achieving the goal state of
 collecting all the mines on the field. It may also penalize movement to encourage exploring all directions and 
 of course for hitting supermines/rocks!
*/
double CQLearningController::R(int x, int y, int sweeper_no){
	//TODO: roll your own here!
	if ((m_vecSweepers[sweeper_no]->isDead()))
		return MIN_REWARD * 100;
	std::vector<CDiscCollisionObject *>::iterator collision = std::find_if(m_vecObjects.begin(), m_vecObjects.end(),
		[&sweeper_no, this](CDiscCollisionObject * obj)->bool
	{
		return obj->getPosition().x == this->m_vecSweepers[sweeper_no]->Position().x &&
			obj->getPosition().y == this->m_vecSweepers[sweeper_no]->Position().y;
	});
	if (collision == m_vecObjects.end())
		return 0.0;
	else if ((*collision)->getType() == CCollisionObject::Mine)
		//m_iCollectedMines[sweeper_no] < m_vecSweepers[sweeper_no]->MinesGathered())
		return MAX_REWARD * 40;
	else if ((*collision)->getType() == CCollisionObject::SuperMine)
		return  MIN_REWARD * 40;
	else
		return -1;

	//if (m_vecSweepers[sweeper_no]->isDead())
	//	return -100.0;
	//else if (collision != m_vecObjects.end() && (*collision)->getType() == CDiscCollisionObject::SuperMine)
	//	return -400.0;
	//else
	//	return -1.0;
}
/**
The update method. Main loop body of our Q Learning implementation
See: Watkins, Christopher JCH, and Peter Dayan. "Q-learning." Machine learning 8. 3-4 (1992): 279-292
*/
bool CQLearningController::Update(void)
{
	//m_vecSweepers is the array of minesweepers
	//everything you need will be m_[something] ;)
	uint cDead = std::count_if(m_vecSweepers.begin(), m_vecSweepers.end(),
		[](CDiscMinesweeper * s)->bool{	return s->isDead(); });
	int collectedMines = std::count_if(m_vecObjects.begin(), m_vecObjects.end(),
		[](CDiscCollisionObject * s)->bool { return s->getType() == CDiscCollisionObject::Mine && s->isDead(); });
	
	
	if (cDead == CParams::iNumSweepers || collectedMines == CParams::iNumMines )
		m_iTicks = CParams::iNumTicks;
	if (m_iTicks == CParams::iNumTicks) {
		std::cout << "[ITER " << m_iIterations << "]\t" ;
		std::cout << ((CParams::iNumMines != 0) ? (int(collectedMines * 100 / CParams::iNumMines)) : 0) << "% Success RATE ; ";
		std::cout << ((CParams::iNumSweepers != 0) ? (int(cDead * 100 / CParams::iNumSweepers)) : 0) << "% Death RATE" << std::endl;
	}
	for (int sw = 0; sw < CParams::iNumSweepers; ++sw){
		SVector2D<int> disc_posSweeper = m_vecSweepers[sw]->Position();
		int iCurrentState = int(disc_posSweeper.x / CParams::iGridCellDim) * _grid_size_y  + int (disc_posSweeper.y / CParams::iGridCellDim);
		if (m_vecSweepers[sw]->isDead())
			continue;
		/**
		Q-learning algorithm according to:
		Watkins, Christopher JCH, and Peter Dayan. "Q-learning." Machine learning 8. 3-4 (1992): 279-292
		*/
		//1:::Observe the current state:
		// get possible actions at current state
		// m_qTable[sw][iCurrentState][4] = double(m_iIterations + 1);
		m_iCollectedMines[sw] = m_vecSweepers[sw]->MinesGathered();
		//2:::Select action with highest historic return:
		ROTATION_DIRECTION nextAction = ROTATION_DIRECTION::EAST;
		// implementing soft max
		std::transform(m_qTable[sw][iCurrentState], m_qTable[sw][iCurrentState] + 4, m_qTable[sw][iCurrentState] + 4, 
			[](double act)->double { return exp(act / TEMPERATURE); });
		double actions_sum = 0.0;
		std::accumulate(m_qTable[sw][iCurrentState] + 4, m_qTable[sw][iCurrentState] + 8, actions_sum);
		if (actions_sum)
			std::transform(m_qTable[sw][iCurrentState] + 4, m_qTable[sw][iCurrentState] + 8, m_qTable[sw][iCurrentState] + 4,
				[&actions_sum](double prob)->double { return prob/actions_sum; });
		double max = -999999;
		for (int i = 0; i < 4; i++ ) {
			if (m_qTable[sw][iCurrentState][i + 4]  > max && m_qTable[sw][iCurrentState][i + 8] != m_iIterations + 1) {
				nextAction = (ROTATION_DIRECTION)(i);
				max = m_qTable[sw][iCurrentState][i];
			} /*else if (m_qTable[sw][iCurrentState][i + 4] == max)
				nextAction = (RandBool()) ? (ROTATION_DIRECTION)(i) : nextAction;*/
		}
		m_qTable[sw][iCurrentState][nextAction + 8] = m_iIterations + 1;
		m_vecSweepers[sw]->setRotation (nextAction); 
		//now call the parents update, so all the sweepers fulfill their chosen action
	}
	
	CDiscController::Update(); //call the parent's class update. Do not delete this.
	
	for (int sw = 0; sw < CParams::iNumSweepers; ++sw){
		//TODO:compute your indexes.. it may also be necessary to keep track of the previous state
		//3:::Observe new state:
		//TODO
		SVector2D<int> disc_prevSweeper (m_vecSweepers[sw]->PrevPosition());
		int iPreviousState = int(disc_prevSweeper.x / CParams::iGridCellDim) * _grid_size_y + int(disc_prevSweeper.y / CParams::iGridCellDim);

		SVector2D<int> disc_posSweeper (m_vecSweepers[sw]->Position());
		int iCurrentState = int(disc_posSweeper.x / CParams::iGridCellDim) * _grid_size_y + int(disc_posSweeper.y / CParams::iGridCellDim);

		ROTATION_DIRECTION currentDirection = m_vecSweepers[sw]->getRotation();

		auto max_possible_state = std::max_element(m_qTable[sw][iCurrentState], m_qTable[sw][iCurrentState] + 4);
		double reward = R(m_vecSweepers[sw]->Position().x, m_vecSweepers[sw]->Position().y, sw);
		m_qTable [sw][iPreviousState][currentDirection] = (1 - LEARNING_FACTOR) * m_qTable [sw][iPreviousState][currentDirection] 
			+ LEARNING_FACTOR * (reward + DISCOUNT_FACTOR * (*max_possible_state));
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
	delete[] m_iCollectedMines;
}

