/*
 * Copyright (c) 2017
 *     Leonardo Baroncelli, Giancarlo Zollino,
 *
 * Any information contained in this software
 * is property of the AGILE TEAM and is strictly
 * private and confidential.
 * 
 * https://github.com/Leofaber/AG_extspot-v2-performance-evaluator
*/
#include <map>
#include <vector>
#include <iostream>

#include"Blob.h"
#include"FolderManager.h"
#include"MapConverter.h"
#include"BlobsFinder.h"
#include"AgileMap.h"
#include"BayesianClassifierForBlobs.h"

using namespace std;

class PerformanceEvaluator{

	public:
		PerformanceEvaluator(string testSetPath, double threshold, double CDELT1, double CDELT2, double PSF );
		void evaluate();
		void insertTestSet();	//string id, pair < CustomPoint, char > cntr_lb
		void insertClassificationSet(); 
		void insertAllBlobs();
		void computePerformance();

	private:

		string testSetPath;
		double threshold;
		double CDELT1;
		double CDELT2;
		double PSF;
		int TP;
		int FP;
		int TN;
		int FN;
		vector<double> errorDistances;

		map< string, pair < CustomPoint , char > > testSet;

		map< string, pair < CustomPoint , char > > classificationSet;

		vector<string> filenames;

		vector< pair < string , Blob * > > allBlobs;

		AgileMap agileMapTool;

		BayesianClassifierForBlobs reverendBayes;
		


};
