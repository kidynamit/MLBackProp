/*
                                                                           
   (               )                                        )              
 ( )\     )     ( /(       (                  (  (     ) ( /((             
 )((_) ( /(  (  )\())`  )  )(   (  `  )   (   )\))( ( /( )\())\  (   (     
((_)_  )(_)) )\((_)\ /(/( (()\  )\ /(/(   )\ ((_))\ )(_)|_))((_) )\  )\ )  
 | _ )((_)_ ((_) |(_|(_)_\ ((_)((_|(_)_\ ((_) (()(_|(_)_| |_ (_)((_)_(_/(  
 | _ \/ _` / _|| / /| '_ \) '_/ _ \ '_ \/ _` |/ _` |/ _` |  _|| / _ \ ' \)) 
 |___/\__,_\__||_\_\| .__/|_| \___/ .__/\__,_|\__, |\__,_|\__||_\___/_||_|  
                    |_|           |_|         |___/                         

 For more information on back-propagation refer to:
 Chapter 18 of Russel and Norvig (2010).
 Artificial Intelligence - A Modern Approach.
 */
#include "CNeuralNet.h"

/**
 A struct to create the artificial neuron with the number of inputs and teh weighting for each input
*/
SNeuron::SNeuron(int numInputs_) : numInputs(numInputs_+1) // + 1 is For the bias
{
	for (int i = 0; i < numInputs; ++i) {
		double w = RandomClamped();
		Clamp(w, -MAX_WEIGHTS, MAX_WEIGHTS);
		vecWeights.push_back(w);
		vecPreviousWeights.push_back(0);
	}
	partialOutput = 0;
}
double SNeuron::calculatePartialOutput(void) 
{
	partialOutput = 0.0;
	for (auto cInput = vecInputs.begin(), cWeight = vecWeights.begin();
		cInput != vecInputs.end();
		++cInput, ++cWeight)
		partialOutput += (*(cInput))* (*cWeight);
	return partialOutput;
}

/** 
 A Struct to hold a layer of Neurons.
*/
SNeuronLayer::SNeuronLayer(int numNeurons_, int numInputsPerNeuron_) : numNeurons(numNeurons_), numInputsPerNeuron(numInputsPerNeuron_)
{
	for (int i = 0; i < numNeurons; ++i)
		vecNeurons.push_back(SNeuron(numInputsPerNeuron));
}

/**
 The constructor of the neural network. This constructor will allocate memory
 for the weights of both input->hidden and hidden->output layers, as well as the input, hidden
 and output layers.
*/
CNeuralNet::CNeuralNet(uint inputLayerSize_, uint hiddenLayerSize_, 
						uint nHiddenLayers_,  uint outputLayerSize_, 
						double lRate_, double mse_cutoff_)
					: inputLayerSize(inputLayerSize_), outputLayerSize(outputLayerSize_), 
					hiddenLayerSize(hiddenLayerSize_), nHiddenLayers(nHiddenLayers_ < 0 ? 1 : nHiddenLayers_), 
					learningRate(lRate_), mseCutoff(mse_cutoff_)
{
	std::cout << "Creating Neural Network ... " ;
	std::cout << "INPUT " << inputLayerSize << ", ";
	std::cout << "HIDDEN " << hiddenLayerSize << ", ";
	std::cout << "NUM LAYERS " << nHiddenLayers + 1 << ", ";
	std::cout << "OUTPUT "<< outputLayerSize << std::endl;
	std::cout.flush();
	initWeights();
}
/**
 The destructor of the class. All allocated memory will be released here
*/
CNeuralNet::~CNeuralNet() {
	//TODO
}
/**
 Method to initialize the both layers of weights to random numbers
*/
void CNeuralNet::initWeights(){
	vecLayers.clear();
	if (hiddenLayerSize > 0) {
		vecLayers.push_back(SNeuronLayer(hiddenLayerSize, inputLayerSize));
		for ( int i = 0; i < nHiddenLayers - 1; i++ ) 
			vecLayers.push_back(SNeuronLayer(hiddenLayerSize, hiddenLayerSize));
		vecLayers.push_back(SNeuronLayer(outputLayerSize, hiddenLayerSize));
	}
	else 
	{
		vecLayers.push_back(SNeuronLayer(outputLayerSize, inputLayerSize));
	}
}
/**
 This is the forward feeding part of back propagation.
 1. This should take the input and copy the memory (use memcpy / std::copy)
 to the allocated _input array.
 2. Compute the output of at the hidden layer nodes 
 (each _hidden layer node = sigmoid (sum( _weights_h_i * _inputs)) //assume the network is completely connected
 3. Repeat step 2, but this time compute the output at the output layer
*/
void CNeuralNet::feedForward(const double * const inputs) {
	// Assume the num of inputs are equal to the inputLayerSize
	//memcpy(_inputs, inputs, sizeof(*_inputs)*sizeof(_inputs));
	std::vector<double> layerOutput, vecInputs (inputLayerSize);
	//std::copy(inputs, inputs + inputLayerSize, vecInputs.begin());
	for (int i = 0; i < inputLayerSize; ++i) 
		vecInputs.at(i) = const_cast<double&>(const_cast<double *>(inputs)[i]);
	vecInputs.push_back(1.0);// Add in the BIAS of 1

	for (auto iLayer = vecLayers.begin(); iLayer != vecLayers.end(); ++iLayer) {
		// for each neuron in the layer calculate the output from the input given
		if (!layerOutput.empty()) {
			vecInputs.clear(); vecInputs.resize(layerOutput.size());
			std::copy(layerOutput.begin(), layerOutput.end(), vecInputs.begin());
			vecInputs.push_back(1.0); // Add in the BIAS
			layerOutput.clear(); 
			
		}
		for (auto iNeuron = iLayer->vecNeurons.begin(); iNeuron != iLayer->vecNeurons.end(); ++iNeuron) {
			iNeuron->vecInputs.clear(); iNeuron->vecInputs.resize(vecInputs.size());
			std::copy (vecInputs.begin(), vecInputs.end(), iNeuron->vecInputs.begin());
			// include the bias
			//std::cout << "104: weights-" << iNeuron->vecWeights.size() << " inputs-" << iNeuron->vecInputs.size() <<"{ ";
			//for (double cInputs : iNeuron->vecInputs) { std::cout << cInputs << ", "; }
			//std::cout << " }" << std::endl;
			assert(iNeuron->vecInputs.size() == iNeuron->vecWeights.size ());
			iNeuron->calculatePartialOutput();
			(iNeuron->partialOutput) = (sigmoid(iNeuron->partialOutput));
			layerOutput.push_back(iNeuron->partialOutput);
		}
	}
	vecInputs.clear();
	layerOutput.clear();
}
/**
 This is the actual back propagation part of the back propagation algorithm
 It should be executed after feeding forward. Given a vector of desired outputs
 we compute the error at the hidden and output layers (allocate some memory for this) and
 assign 'blame' for any error to all the nodes that fed into the current node, based on the
 weight of the connection.
 Steps:
 1. Compute the error at the output layer: sigmoid_d(output) * (difference between expected and computed outputs)
    for each output
 2. Compute the error at the hidden layer: sigmoid_d(hidden) * 
	sum(weights_o_h * difference between expected output and computed output at output layer)
	for each hidden layer node
 3. Adjust the weights from the hidden to the output layer: learning rate * error at the output layer * error at the hidden layer
    for each connection between the hidden and output layers
 4. Adjust the weights from the input to the hidden layer: learning rate * error at the hidden layer * input layer node value
    for each connection between the input and hidden layers
 5. REMEMBER TO FREE ANY ALLOCATED MEMORY WHEN YOU'RE DONE (or use std::vector ;)
*/
void CNeuralNet::propagateErrorBackward(const double * const desiredOutput){
	// Compute the error at the outputLayer
	int i = 0;
	for (auto cNeuron = vecLayers.at(vecLayers.size() - 1).vecNeurons.begin();
		cNeuron != vecLayers.at(vecLayers.size() - 1).vecNeurons.end();
		++cNeuron, ++i)
		//cNeuron->errorTerm = sigmoid(cNeuron->partialOutput)*(desiredOutput[i] - cNeuron->partialOutput);
		cNeuron->errorTerm = cNeuron->partialOutput* (1 - cNeuron->partialOutput)*(desiredOutput[i] - cNeuron->partialOutput) /*/ ACTIVE_RESPONSE*/;

	// Compute the error at the hiddenLayer

	for (int layerIndex = vecLayers.size() - 2; layerIndex >= 0; layerIndex--) {
		int hiddenIndex = 0;
		for (auto cNeuron = vecLayers.at(layerIndex).vecNeurons.begin();
			cNeuron != vecLayers.at(layerIndex).vecNeurons.end();
			++cNeuron, ++hiddenIndex) {
			double cumSum = 0.0;
			for (auto oNeuron = vecLayers.at(layerIndex + 1).vecNeurons.begin();
				oNeuron != vecLayers.at(layerIndex + 1).vecNeurons.end(); ++oNeuron)
				cumSum += oNeuron->errorTerm * oNeuron->vecWeights[hiddenIndex];
			cNeuron->errorTerm = cNeuron->partialOutput * (1 - cNeuron->partialOutput) * cumSum /*/ ACTIVE_RESPONSE*/;
			//cNeuron->errorTerm = sigmoid(cNeuron->partialOutput) * cumSum;
		}
	}

	for (auto cNeuronLayer = vecLayers.begin(); cNeuronLayer != vecLayers.end(); ++cNeuronLayer) {
		for (auto cNeuron = cNeuronLayer->vecNeurons.begin(); cNeuron != cNeuronLayer->vecNeurons.end(); ++cNeuron) {
			std::vector<double> currentWeights(cNeuron->vecWeights);
			assert(cNeuron->vecWeights.size() == cNeuron->vecInputs.size());
			for (int i = 0; i < cNeuron->vecWeights.size(); ++i)
				cNeuron->vecWeights.at(i) += (1 - MOMENTUM_RATE) * learningRate * cNeuron->errorTerm * cNeuron->vecInputs.at(i) + 
											(cNeuron->vecWeights.at(i) - cNeuron->vecPreviousWeights.at(i)) * MOMENTUM_RATE;
			cNeuron->vecPreviousWeights = std::vector<double> (currentWeights);
		}
	}
}
/**
This computes the mean squared error
A very handy formula to test numeric output with. You may want to commit this one to memory
*/
double CNeuralNet::meanSquaredError(const double * const desiredOutput){
	double sum = 0.0;
	for (uint i = 0; i < outputLayerSize; ++i) {
		double err = (desiredOutput[i] - getOutput(i));
		sum += err * err;
	} 
	return sum / outputLayerSize;
}
/**
This trains the neural network according to the back propagation algorithm.
The primary steps are:
for each training pattern:
  feed forward
  propagate backward
until the MSE becomes suitably small
*/
void CNeuralNet::train(const double** const inputs_, const double ** const outputs_, uint trainingSetSize) {
	// Suppose the size of the inputs and outputs are defined by the inputLayerSize and outputLayerSize and trainingSetSize
	// Allocate memory
	double ** inputs = new double*[trainingSetSize];
	double ** outputs = new double*[trainingSetSize];
	for (uint i = 0; i < trainingSetSize; ++i) {
		inputs[i] = new double[inputLayerSize];
		memcpy(inputs[i], inputs_[i], inputLayerSize*sizeof(*inputs));
		outputs[i] = new double[outputLayerSize];
		memcpy(outputs[i], outputs_[i], outputLayerSize*sizeof(*outputs));
	}
	uint trainingIterations = 0;
	double maxMSE, accMSE;
	int count = 0, displayIter = 100;
	std::cout << "Displaying Network MSE after every " << displayIter << " iterations" << endl;
	std::cout << "(Restart training if it is taking long to train <CTRL><SHIFT><F5>)" << endl;
	do { 
		maxMSE = 0.0; accMSE = 0.0;
		// Training the Network
		for (uint32_t data = 0; data < trainingSetSize; ++data) {
			// feed forward
			feedForward(inputs_[data]);
			// propagate the error backward
			propagateErrorBackward(outputs_[data]);
			
			double tempmse = meanSquaredError(outputs_[data]);
			maxMSE = (tempmse > maxMSE) ? tempmse : maxMSE;

			accMSE += tempmse;
		}
		accMSE = accMSE / (trainingSetSize * outputLayerSize);
		if (count % displayIter == 0) 
			std::cout << "netMSE:= " << accMSE <<", ";
		count++;
		//std::cout << "maxMSE:= " << maxMSE << ", ";
	} while ( accMSE > mseCutoff);
	cout << endl;
	cout << "Training Complete ... " << endl;
	cout << "Generating Stats: " << count << " iterations @ netMSE " << accMSE << " ... ";

	// Deallocate Memory
	for (uint32_t i = 0; i < trainingSetSize; ++i) {
		delete[] inputs[i];
		delete[] outputs[i];
	} 
	delete[] inputs;
	delete[] outputs;
}
/**
Once our network is trained we can simply feed it some input though the feed forward
method and take the maximum value as the classification
*/
uint CNeuralNet::classify(const double * const input){
	feedForward(input);
	double maxOutput = 0.0;
 	uint index = -1;
	uint i = 0;
	for (auto oNeuron = vecLayers.at(vecLayers.size() - 1).vecNeurons.begin(); 
			oNeuron != vecLayers.at(vecLayers.size() - 1).vecNeurons.end(); ++oNeuron, ++i)
		if (maxOutput < oNeuron->partialOutput) {
			maxOutput = oNeuron->partialOutput;
			index = i;
		}

	return index;
}
/**
Gets the output at the specified index
*/
double CNeuralNet::getOutput(uint index) const{
	return vecLayers.at(vecLayers.size() - 1).vecNeurons.at(index).partialOutput; 
}

double CNeuralNet::sigmoid(double netInput, double response){
	return (1 / (1 + exp(-netInput/response)));
}