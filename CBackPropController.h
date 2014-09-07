#pragma once
#include "ccontcontroller.h"
#include "CNeuralNet.h"
#include <assert.h>

#define VIEWING_RADIUS				50
#define MINE_SUPERMINE_ROCK_CUTOFF	10

class CBackPropController :
	public CContController
{
protected:
	CNeuralNet* _neuralnet;
public:
	CBackPropController(HWND hwndMain);
	virtual void InitializeLearningAlgorithm(void);
	virtual bool Update(void);
	~CBackPropController(void);
};

