	/**
    )                                                         
 ( /(                                (         )              
 )\())  (   (  (         (    )      )\  (  ( /((             
((_)\  ))\ ))\ )(   (    )\  /((  ( ((_)))\ )\())\  (   (     
 _((_)/((_)((_|()\  )\  ((_)(_))\ )\ _ /((_|_))((_) )\  )\ )  
| \| (_))(_))( ((_)((_) | __|)((_|(_) (_))(| |_ (_)((_)_(_/(  
| .` / -_) || | '_/ _ \ | _|\ V / _ \ | || |  _|| / _ \ ' \)) 
|_|\_\___|\_,_|_| \___/ |___|\_/\___/_|\_,_|\__||_\___/_||_|  

Further details can be found in:
X. Yao, Evolving artificial neural networks, Proceedings of the IEEE,
87(9):1423-1447, September 1999                                                              

This class only serves to give you some direction as where to go. You
can modify the headers, etc. if you want
*/

#include "CBasicEA.h"
/**
This function should compute the fitness of a particular sweeper and bundle its genome and fitness together
*/
void CBasicEA::computeFitness(const std::vector<CContMinesweeper *> & parent_sweepers,
							  const std::vector<CNeuralNet *> & parent_genomes,
							  std::vector<ParentAndFitness> & fitness_out,
							  double * stats)
{
	assert(parent_sweepers.size() == parent_genomes.size());
	double minFitness = 999999;
	double maxFitness = -999999;
	double averageFitness = 0.0;
	double variance = 0.0;

	// Calculate fitness of sweeper based on life and numMines collected
	std::transform(parent_sweepers.begin(), parent_sweepers.end(), parent_genomes.begin(), fitness_out.begin(), 
		[&minFitness, &maxFitness, &averageFitness, &variance](CContMinesweeper * sweeper, CNeuralNet * genome)->ParentAndFitness
	{
		ParentAndFitness result;
		result.parent_genome = genome;
		// TODO calculate fitness 
		double liveliness = (sweeper->getTimeOfDeath() > 0) ? (sweeper->getTimeOfDeath() / CParams::iNumTicks) : 1.5;
		if (CParams::iNumMines > 0)
			result.fitness = ((sweeper->MinesGathered())) * (((sweeper->MinesGathered()) / (CParams::iNumMines)) * MAX_FITNESS + liveliness * MIN_FITNESS);
		//if (CParams::iNumTicks > 0)
		//	result.fitness += (sweeper->getTimeOfDeath() > 0 ) ? (sweeper->getTimeOfDeath() / CParams::iNumTicks )* MIN_FITNESS : MIN_FITNESS;
	
		minFitness = (result.fitness < minFitness) ? result.fitness : minFitness;
		maxFitness = (result.fitness > maxFitness) ? result.fitness : maxFitness;
		averageFitness += result.fitness;
		variance += result.fitness * result.fitness;
		return result;
	});
	if (parent_genomes.size() > 0)
		averageFitness = averageFitness / parent_genomes.size();
	stats[0] = averageFitness; 
	stats[2] = minFitness;
	stats[3] = maxFitness;
	stats[1]= parent_sweepers.size() != 0 ? sqrt((variance / parent_sweepers.size()) - (stats[0] * stats[0]) ): 0.0; // standard deviation

}
/**
Selects the fittest of the population and use selection operators on them to spawn replacements for the rest
*/
void CBasicEA::selection_crossover_mutate(std::vector<ParentAndFitness> & parents, double * stats) {
	// The best are those that above average;
	// sort the list 
	const int ELITIST_LIMIT = 8;
	std::sort(parents.begin(), parents.end(),
		[](ParentAndFitness left, ParentAndFitness right)->bool
	{ return left.fitness > right.fitness; }); // max to min
	
	// get the networks size 
	// ASSUMPTION: no variable length genotypes
	int networkSize = 0;
	for (SNeuronLayer snl : parents[0].parent_genome->vecLayers)
		networkSize += snl.numInputsPerNeuron * snl.numNeurons;

	//for (int i = 0; i < ELITIST_LIMIT; i++) {
	//	assert(i + 1 < parents.size());
	//	if (parents[i].fitness == 0 || parents[i + 1].fitness == 0)
	//		break;
	//	// std::cout <<"couple: " << parents[i].fitness << ", " << parents[i].parent_genome  << "|+|" << parents[i + 1].fitness  << ", " << parents[i + 1].parent_genome << std::endl;
	//	crossover(*parents[i].parent_genome, *parents[i + 1].parent_genome, 
	//		*parents[parents.size() - 1 - i].parent_genome, networkSize);
	//	mutate(*parents[parents.size() - 1 - i].parent_genome);
	//}
	
	// double cutoff = stats[0] - 0.5*stats[1];
	std::cout << "\nSelection crossover and mutation ... " << std::endl;
	for (int i = 0; i < parents.size(); i++) {
		assert(i + 1 < parents.size());
		if (parents[parents.size() - 1 - i].fitness == stats[2]) {
			if (parents[i].fitness == stats[2] || parents[i + 1].fitness == stats[2])
				break;
			std::cout << "couple: (" << parents[i].fitness << ", " << parents[i].parent_genome << 
				")\t+\t(" << parents[i + 1].fitness << ", " << parents[i + 1].parent_genome <<")"<< std::endl;

			crossover(*parents[i].parent_genome, *parents[i + 1].parent_genome,
				*parents[parents.size() - 1 - i].parent_genome, networkSize);
			mutate(*parents[parents.size() - 1 - i].parent_genome);
		} else
			break;
	} 
	std::cout << "Done!" << std::endl;
} 

/**
Crossover (exploration) of some sort
*/
void CBasicEA::crossover(const CNeuralNet & genotypeA, const CNeuralNet & genotypeB,
	CNeuralNet & offspring1, const int networkSize)
{
	assert(genotypeA.vecLayers.size() == genotypeB.vecLayers.size());
	//TODO:: roll your own 
	// Performing 1-Point crossover
	int selectedPoint = RandInt(0, networkSize);
	selectedPoint = (selectedPoint == networkSize) ? selectedPoint - 1 : selectedPoint;

	// perform crossover
	for (int layerL = 0; layerL < offspring1.vecLayers.size(); layerL++) {
		for (int neuronN = 0; neuronN < offspring1.vecLayers[layerL].vecNeurons.size(); neuronN++) {
			for (int weightW = 0; weightW < offspring1.vecLayers[layerL].vecNeurons[neuronN].vecWeights.size(); 
				weightW++, selectedPoint--) {
				if (selectedPoint  > 0) {
					offspring1.vecLayers[layerL].vecNeurons[neuronN].vecWeights[weightW] =
						genotypeB.vecLayers[layerL].vecNeurons[neuronN].vecWeights[weightW];
				} else {
					offspring1.vecLayers[layerL].vecNeurons[neuronN].vecWeights[weightW] =
						genotypeA.vecLayers[layerL].vecNeurons[neuronN].vecWeights[weightW];
				}
			}
		}
	}
}
/**
Mutation (exploitation) of some sort
*/ 
void CBasicEA::mutate(CNeuralNet & genotypeA)
{
	//TODO:: roll your own
	for (SNeuronLayer sl : genotypeA.vecLayers) {
		for (int i = 0; i < sl.numNeurons; i++) {
			std::transform ( sl.vecNeurons[i].vecWeights.begin () , sl.vecNeurons[i].vecWeights.end(), sl.vecNeurons[i].vecWeights.begin(), 
				[](double weight)->double { return (RandBool()) ? -1 + 2 * (RandInt(0, 100000) / 100000) : weight;  });
		}
	}
	
}

