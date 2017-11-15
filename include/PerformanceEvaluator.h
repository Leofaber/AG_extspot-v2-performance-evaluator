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
#include <iomanip>

#include"Blob.h"
#include"FolderManager.h"
#include"MapConverter.h"
#include"BlobsFinder.h"
#include"AgileMap.h"
#include"BayesianClassifierForBlobs.h"
#include"FileWriter.h"

using namespace std;

class PerformanceEvaluator{

	public:
		PerformanceEvaluator(string testSetPath, double threshold, double CDELT1, double CDELT2, double PSF);
		void evaluate();
		

	private:

		string testSetPath;
		double threshold;
		double CDELT1;
		double CDELT2;
		double PSF;
		BayesianClassifierForBlobs reverendBayes;

		// Le immagini
		vector<string> filenames;

		vector< pair < string , Blob * > > allBlobs;

		// Il test set ->  <identificativo dell'istanza, pair<Centroide,Etichetta di classificazione> >
		map< string, pair < CustomPoint , char > > testSet;





		/*
			Estrae tutti i blobs dalle immagini del test set e li mette in un vettore di coppie (identificativo, puntatore al Blob)
		*/
		/*vector< pair < string , Blob * > >*/void extractBlobsFromImages();

		/*
			Prende la lista dei blobs e gli filtra scegliendo gli elementi che andranno a formare il test set			
		*/
		void createTestSetMap(/*vector< pair < string , Blob * > >& allBlobs*/);

		/*
			Effettua la classificazione sul Blob * di ogni elemento di allBlobs. Ritorna una mappa <identificativo, coppia (centroide, etichetta di classificazione predetta)		
		*/
		map< string, pair < CustomPoint , char > > createPredictionsMap(); 

		/*
			Confronta ogni elemento di testSet con il corrispondente elemento di predictions.
		*/
		void computePerformance(map< string, pair < CustomPoint , char > >& predictions);
 

		

		
		


};
