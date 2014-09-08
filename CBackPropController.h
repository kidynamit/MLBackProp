#pragma once
#include "ccontcontroller.h"
#include "CNeuralNet.h"
#include <assert.h>

#define VIEWING_RADIUS					50.0
#define DIST_MINE_SUPERMINE_ROCK_CUTOFF	10.0
#define RESOURCE_MULTIPLIER				6.0

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

