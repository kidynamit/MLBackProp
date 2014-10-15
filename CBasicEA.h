/**
	This class is only there as a place holder you can modify this to work with you EA controller any way you want
*/
#pragma once
#include "cneuralnet.h"
#include "CContMinesweeper.h"
#include <vector>
#include <assert.h>
#include <algorithm>

#define MAX_FITNESS		2000.0
#define MIN_FITNESS		100.0
typedef struct {
	CNeuralNet * parent_genome;
	double fitness;
} ParentAndFitness;

class CBasicEA
{
private:
	static void crossover(const CNeuralNet & genotypeA, const CNeuralNet & genotypeB, CNeuralNet & offspring1, CNeuralNet & offspring2, const int networkSize);
	static void mutate(CNeuralNet & genotypeA);

public:
	static void computeFitness(const std::vector<CContMinesweeper *> & parent_sweepers,
							   const std::vector<CNeuralNet *> & parent_genomes,
							   std::vector<ParentAndFitness> & fitness_out, 
							   double * stats);
	static void selection_crossover_mutate(std::vector<ParentAndFitness> & parents, double * stats);
};

