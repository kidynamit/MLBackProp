/*
 * CNeuralNet.h
 *
 *  Created on: 26 Dec 2013
 *      Author: benjamin
 */

#ifndef CNEURALNET_H_
#define CNEURALNET_H_
#include <vector>
#include <cmath>
#include <algorithm>
#include <stdlib.h>
#include <cstring>
#include <stdio.h>
#include <stdint.h>
#include <cassert>

#include "utils.h"

#define MAX_WEIGHTS			1.0	
#define MIN_WEIGHTS			-1.0
#define ACTIVE_RESPONSE		5.0
#define MOMENTUM_RATE		0.3

typedef unsigned int uint;
class CBasicEA; //forward declare EA class, which will have the power to access weight vectors

struct SNeuron {
	int numInputs;
	std::vector<double> vecWeights, vecInputs, vecPreviousWeights;
	double partialOutput=0.0, errorTerm=0.0;
	double calculatePartialOutput(void);
	SNeuron(int numInputs_,double minWeights=MIN_WEIGHTS, double maxWeights=MAX_WEIGHTS );
};

struct SNeuronLayer {
	int numNeurons, numInputsPerNeuron;
	std::vector<SNeuron> vecNeurons;
	SNeuronLayer(int numNeurons_, int numInputsPerNeuron_);
};

class CNeuralNet {
	friend class CBasicEA;
protected:
	void feedForward(const double * const inputs); //you may modify this to do std::vector<double> if you want
	void propagateErrorBackward(const double * const desiredOutput); //you may modify this to do std::vector<double> if you want
	double meanSquaredError(const double * const desiredOutput); //you may modify this to do std::vector<double> if you want
public:
	CNeuralNet(uint inputLayerSize_, uint hiddenLayerSize_, uint nHiddenLayers_, uint outputLayerSize_, double lRate_, double mse_cutoff_);
	void initWeights();
	void train(const double ** const inputs,const double ** const outputs, uint trainingSetSize); //you may modify this to do std::vector<std::vector<double> > or do boost multiarray or something else if you want
	uint classify(const double * const input); //you may modify this to do std::vector<double> if you want
	double getOutput(uint index) const; 
	inline double sigmoid(double netInput, double response=ACTIVE_RESPONSE);
	virtual ~CNeuralNet();
private:
	uint inputLayerSize, outputLayerSize, hiddenLayerSize, nHiddenLayers;
	double learningRate, mseCutoff;
	std::vector<SNeuronLayer> vecLayers;
};

#endif /* CNEURALNET_H_ */
