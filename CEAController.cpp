/**
    )                                                         
 ( /(                                (         )              
 )\())  (   (  (         (    )      )\  (  ( /((             
((_)\  ))\ ))\ )(   (    )\  /((  ( ((_)))\ )\())\  (   (     
 _((_)/((_)((_|()\  )\  ((_)(_))\ )\ _ /((_|_))((_) )\  )\ )  
| \| (_))(_))( ((_)((_) | __|)((_|(_) (_))(| |_ (_)((_)_(_/(  
| .` / -_) || | '_/ _ \ | _|\ V / _ \ | || |  _|| / _ \ ' \)) 
|_|\_\___|\_,_|_| \___/ |___|\_/\___/_|\_,_|\__||_\___/_||_|  
                                              
	   (                )        (  (           
	   )\            ( /((       )\ )\  (  (    
	 (((_)  (   (    )\())(   ( ((_|(_)))\ )(   
	 )\___  )\  )\ )(_))(()\  )\ _  _ /((_|()\  
	((/ __|((_)_(_/(| |_ ((_)((_) || (_))  ((_) 
	 | (__/ _ \ ' \))  _| '_/ _ \ || / -_)| '_| 
	  \___\___/_||_| \__|_| \___/_||_\___||_|   
                                            
*/
#include "CEAController.h"


CEAController::CEAController(HWND hwndMain):
	CContController(hwndMain)
{
	//InitializeLearningAlgorithm();
}

void CEAController::InitializeLearningAlgorithm(void)
{
	std::cout << "Generating genomes ... ";
	//TODO: set up any data structures you need to store the genomes here
	// Selecting direct encoding of the genomes i.e each genome attribute will be a weight	
	genomes = std::vector<CNeuralNet *>(CParams::iNumSweepers);
	
	uint no_training_samples;
	uint dist_effect_cutoff;
	uint no_inputs;
	uint no_hidden;
	uint no_layers;
	uint no_out;
	double learning_rate;
	double mse_cutoff;
	ifstream f(CParams::sTrainingFilename.c_str());
	assert(f.is_open());

	f >> no_training_samples;
	f >> no_inputs;
	f >> no_hidden;
	f >> no_layers;
	f >> no_out;
	f >> learning_rate;
	f >> mse_cutoff;
	f.close();
	for (int i = 0; i < CParams::iNumSweepers; i++)
		genomes[i] = new CNeuralNet(no_inputs, no_hidden, no_layers, no_out, learning_rate, mse_cutoff);
	std::cout << "Done!" << std::endl;

}

/**
Returns the dot product between the sweeper's look vector and the vector from the sweeper to the object
*/
inline double dot_between_vlook_and_vObject(const CContMinesweeper &s,const CContCollisionObject &o){
	SVector2D<double> vLook = s.getLookAt();
	SVector2D<double> pt = o.getPosition();
	//get the vector to the point from the sweepers current position:
	SVector2D<double> vObj(SVector2D<double>(pt.x,pt.y) - s.Position());
	Vec2DNormalize<double>(vObj);
	//remember (MAM1000 / CSC3020) the dot product between two normalized vectors returns
	//1 if the two vectors point in the same direction
	//0 if the two vectors are perpendicular
	//-1 if the two vectors are pointing in opposite directions
	return Vec2DDot<double>(vLook,vObj);
}

bool CEAController::Update(void)
{
	uint cDead = std::count_if(m_vecSweepers.begin(),
							   m_vecSweepers.end(),
						       [](CContMinesweeper * s)->bool{
								return s->isDead();
							   });
	if (cDead == CParams::iNumSweepers){
		printf("All dead ... skipping to next iteration\n");
		m_iTicks = CParams::iNumTicks;
	}

	if (m_iTicks+1 > CParams::iNumTicks ){
		//TODO: when the round is over select, cross over and mutate here
		std::vector<ParentAndFitness> parentFitness (CParams::iNumSweepers);
		double * stats = new double[4];
		CBasicEA::computeFitness(m_vecSweepers, genomes, parentFitness, stats);

		std::cout << "Displaying Fitness values with Parameters ... ";
		for (int i = 0; i < CParams::iNumSweepers; i++) { 
			//assert(parentFitness[i].parent_genome == genomes[i]); 

			std::cout << parentFitness[i].fitness << " : (" << (m_vecSweepers[i]->getTimeOfDeath()) 
				<< "," << m_vecSweepers[i]->MinesGathered() << ") ";
		}
		std::cout << std::endl;
		CBasicEA::selection_crossover_mutate(parentFitness, stats);
		std::cout << "Done!\n----------" << std::endl;

	}
	CContController::Update(); //call the parent's class update. Do not delete this.
	double resource_ratio = (CParams::iNumMines / (CParams::iNumSuperMines + CParams::iNumMines));
	double resource_speed_multiplier = (resource_ratio)* RESOURCE_MULTIPLIER + 1;
	for (int i = 0; i < CParams::iNumSweepers; i++) {
		CContMinesweeper * s = m_vecSweepers[i]; 
		// Dot products between the target mine, supermine and the look of the sweeper
		double dot_target_mine = dot_between_vlook_and_vObject(*s, *m_vecObjects[(s)->getTargetMine()]);
		double dot_supermine = dot_between_vlook_and_vObject(*s, *m_vecObjects[(s)->getClosestSupermine()]);
		// Distances of the target mine and supermine to the sweeper
		double dist_target_mine = Vec2DLength(m_vecObjects[(s)->getTargetMine()]->getPosition() - (s)->Position());
		double dist_supermine = Vec2DLength(m_vecObjects[(s)->getClosestSupermine()]->getPosition() - (s)->Position());

		// double dist_mine_supermine = Vec2DLength(m_vecObjects[(s)->getTargetMine()]->getPosition() - m_vecObjects[(s)->getClosestSupermine()]->getPosition());
		// When approaching a supermine or mine slow down
		(s)->setSpeed(min(dist_target_mine, dist_supermine), int(resource_speed_multiplier));
		// Clamp to reduce the noise i.e focus on the range of the dot products that should be relevant for the networks
		// The narrower the range the better and finer the output.
		Clamp(dot_supermine, 0.0, 0.5) ;
		Clamp(dot_target_mine, 0.5, 1);
		//Classify the output a
		double dots[2] = { dot_target_mine, dot_supermine };
		uint response = genomes[i]->classify((const double*)&dots);
		//respond accordingly
		if (response == 0){ // turn towards the mine
			SPoint pt(m_vecObjects[(s)->getTargetMine()]->getPosition().x,
				m_vecObjects[(s)->getTargetMine()]->getPosition().y);
			(s)->turn(pt, 1);
		}
		else if (response == 1) {//turn away from a rock or supermine
			SPoint pt(m_vecObjects[(s)->getClosestSupermine()]->getPosition().x,
				m_vecObjects[(s)->getClosestSupermine()]->getPosition().y);
			(s)->turn(pt, 1, false);
		} 
		else if (response == -1) {
			// Do Nothing (Maintain the course of actions)
		}
	}

	return true; //method returns true if successful. Do not delete this.
}

CEAController::~CEAController(void)
{
	//TODO:: release any allocations here
	for (int i = 0; i < CParams::iNumSweepers; i++) {
		delete genomes[i];
		genomes[i] = nullptr;
	}
}
