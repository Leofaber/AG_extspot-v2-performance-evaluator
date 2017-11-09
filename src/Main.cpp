/*
 * Copyright (c) 2017
 *     Leonardo Baroncelli, Giancarlo Zollino
 *
 * Any information contained in this software
 * is property of the AGILE TEAM and is strictly
 * private and confidential.
 * 
 * https://github.com/Leofaber/AG_extspot-v2-bayesian-model-evaluator
*/
#include <iostream>
#include <stdlib.h>
#include "PerformanceEvaluator.h"


using namespace std;

const char* startString = {
"################################################################\n"
"###  Task AG_extspot-v2-performance-evaluator  v.1.0.0       ###"
};

const char* endString = {
"### AG_extspot-v2-performance-evaluator .....................###\n"
"################################################################"
};


int main(int argc, char*argv[]){
	
	cout << startString << endl;

	const char * trainingSetPath = argv[1];
	double classificationThreshold = atof(argv[2]);
	double step = atof(argv[3]);
	double CDELT1 = atof(argv[4]);
	double CDELT2 = atof(argv[5]);
	double PSF = atof(argv[6]);

	cout << "\nTest set path: "<< trainingSetPath << endl;
	cout << "Classification Threshold: "<< classificationThreshold << endl;
	cout << "step: "<< step << endl;
	cout << "CDELT1: "<< CDELT1 << endl;
	cout << "CDELT2: "<< CDELT2 << endl;
	cout << "PSF: "<< PSF << endl;

	/*
		It will write the performances on a txt file
	*/
	PerformanceEvaluator pe;
	pe.evaluate();
	


	cout << endString << endl;

	return 0;
}
