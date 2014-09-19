#pragma once
#include "cdisccontroller.h"
#include "CParams.h"
#include "CDiscCollisionObject.h"
#include <cmath>
#include <cassert>

#define MAX_REWARD 			50.0
#define MIN_REWARD 			-50.0
#define LEARNING_FACTOR  	0.7
#define DISCOUNT_FACTOR		0.2

typedef unsigned int uint;
class CQLearningController :
	public CDiscController
{
private:
	int _grid_size_x;
	int _grid_size_y;
	double *** m_qTable;
public:
	CQLearningController(HWND hwndMain);
	virtual void InitializeLearningAlgorithm(void);
	double R(int x, int y, int sweeper_no);
	virtual bool Update(void);
	virtual ~CQLearningController(void);
};

