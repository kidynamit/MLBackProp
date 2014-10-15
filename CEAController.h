#pragma once
#include "ccontcontroller.h"
#include <vector>
#include "CNeuralNet.h"
#include "CParams.h"
#include "CBasicEA.h"

#define DIST_MINE_SUPERMINE_ROCK_CUTOFF	10.0
#define RESOURCE_MULTIPLIER				5.0

class CEAController : public CContController
{
protected:
	std::vector<CNeuralNet *> genomes;
public:
	CEAController(HWND hwndMain);
	virtual void InitializeLearningAlgorithm(void);
	virtual bool Update(void);
	virtual ~CEAController(void);
};

