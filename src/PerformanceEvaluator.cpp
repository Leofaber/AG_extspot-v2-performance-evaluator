#include "PerformanceEvaluator.h"



PerformanceEvaluator::PerformanceEvaluator(string _testSetPath, double _threshold, double _CDELT1, double _CDELT2, double _PSF){

	testSetPath = _testSetPath;
	threshold = _threshold;
	CDELT1 = _CDELT1;
	CDELT2 = _CDELT2;
	PSF = _PSF;
	
}

void PerformanceEvaluator::evaluate(){
	cout << "\n\nEvaluation...\n\n" << endl;
	
	// Riempio il vector<string> filenames con i nomi delle mappe contenute nel path
	filenames = FolderManager::getFileNamesFromFolder(testSetPath);
	
	// Estrae i blob dalle immagini
	/*vector< pair < string , Blob * > > allBlobs = */extractBlobsFromImages();
	
	// Crea il test set
	createTestSetMap(/*allBlobs*/);
	
	// PER OGNI SOGLIA

		// Crea le previsioni
		map< string, pair < CustomPoint , char > > predictions = createPredictionsMap();
		
		// Calcola le performance
		computePerformance(predictions);
	
	   
	cout << "\n\n" << endl;
   
	
}

/*vector< pair < string , Blob * > >*/void PerformanceEvaluator::extractBlobsFromImages() {
	

	//vector< pair < string , Blob * > > allBlobs;
	
	for(vector<string>::const_iterator it = filenames.begin(); it < filenames.end(); ++it ) {
		
		string imageName = *it;

		string imagePathName = testSetPath;
		int countBlob = 0;
		
		imagePathName.append("/" + imageName);
		
		
		AgileMap agileMapTool(imagePathName.c_str());
		
		CustomMap map = MapConverter::mapPathToIntPtr(imagePathName.c_str());
		
				
		///Trovo i blobs all'interno dell'imagine tramite Blobs finder
		vector<Blob*> blobs = BlobsFinder::findBlobs(imagePathName, map.image,map.rows,map.cols,agileMapTool.GetXbin() ,agileMapTool.GetYbin() );
		 
		///Inserisco i blob all'interno di allBlobs
		for(vector<Blob *>::const_iterator it = blobs.begin(); it < blobs.end(); ++it ) {
			
			
					

			
			// insert ele
			string newName ="";
			countBlob += 1;
			
			size_t foundPatternDotCts = imageName.find(".cts");
			imageName = imageName.substr(0,foundPatternDotCts);
			newName.append(imageName + "_BLOB" + to_string(countBlob));
			
			//cout << newName << endl;
			allBlobs.push_back(make_pair(newName,*it));
			//cout << "allBlobs element: " << it << endl;
			newName.clear();
		
		}
		
		
			
	}
	
	cout << "Numero elementi inseriti in allBlobs: "<<allBlobs.size() << endl;	
	//return allBlobs;
}


void PerformanceEvaluator::createTestSetMap(/*vector< pair < string , Blob * > >& allBlobs*/){
	

	for(vector< pair < string , Blob * > >::iterator i = allBlobs.begin(); i != allBlobs.end(); i++){
		
		pair < string , Blob * > blobInstance = *i;
		
		// unique blob identifier
		string blobIdentifier = blobInstance.first;
		
		// blob pointer
		Blob * blobPtr = blobInstance.second;
		
				
		if( blobIdentifier.compare(0,1,"B") == 0 ){
			CustomPoint centroidPX = blobPtr->getCentroid();
			pair<CustomPoint, char> tmp = make_pair(centroidPX,'B');
			testSet.insert(make_pair(blobIdentifier, make_pair(centroidPX, 'B')));
		}
		else if(blobPtr->getNumberOfPhotonsInBlob() > 1 && blobPtr->isCentered()){
			CustomPoint centroidPX = blobPtr->getCentroid();
			pair<CustomPoint, char> tmp = make_pair(centroidPX,'F');
			testSet.insert(make_pair(blobIdentifier, make_pair(centroidPX, 'F')));
		}

	}

	
	cout << "Elementi inseriti in testSet\n" << endl;
	
	
	/*for(map<string, pair < CustomPoint, char  > >::const_iterator it = testSet.begin(); it != testSet.end(); ++it)
	{
		std::cout << "testSet element: " << it->first << " (" << it->second.first.y << "," << it->second.first.x << "), " << it->second.second << "\n";
	}*/
	
	
}

map< string, pair < CustomPoint , char > > PerformanceEvaluator::createPredictionsMap() {
	
	map< string, pair < CustomPoint , char > > predictions;

	
	for(vector< pair < string , Blob * > >::iterator i = allBlobs.begin(); i != allBlobs.end(); i++){
		
		pair < string , Blob * > blobInstance = *i;
		
		// unique blob identifier
		string blobIdentifier = blobInstance.first;
		
		// blob pointer
		Blob * blobPtr = blobInstance.second;
		
		CustomPoint centroid = blobPtr->getGalacticCentroid();
		
		vector<pair<string,double> > predicted = reverendBayes.classify(blobPtr);
		
		double fluxProbability = predicted[1].second;
		
		if(fluxProbability*100 >= threshold) 
			
			predictions.insert(make_pair(blobIdentifier, make_pair(centroid, 'F')));
				
		else 
			predictions.insert(make_pair(blobIdentifier, make_pair(centroid, 'B')));
			
				
	}
	
	cout << "Elementi inseriti in classificatioSet" << endl;

	return predictions;
	
	/*for(map<string, pair < CustomPoint, char  > >::const_iterator it = predictions.begin(); it != predictions.end(); ++it)
	{
		std::cout << "testSet element: " << it->first << " (" << it->second.first.y << "," << it->second.first.x << "), " << it->second.second << "\n";
	}*/

}

void PerformanceEvaluator::computePerformance(map< string, pair < CustomPoint , char > >& predictions) {


	int TP = 0;
	int TN = 0;
	int FP = 0;
	int FN = 0;
	int totalInstances = 0;
	int totalFluxInstances = 0;
	int totalBackgroundInstances = 0;

	double falsePositiveRate = 0;
	double falseNegativeRate = 0;

	double sensitivity = 0;
	double specificity = 0;
	double accuracy = 0;
	double f_measure = 0;
	double precision = 0;
	double k_choen = 0; 
	

	double errorDistancesTotal = 0;
	double errorDistancesMean = 0;
	double errorDistancesDeviation = 0;

	//cout << "testSet size: " << testSet.size() << ", classficationSet size: " << predictions.size() << ", allBlobs size: "<< allBlobs.size() << endl;

	if(testSet.size() != predictions.size()){
		cout << "Different map size "<< endl;  // differing sizes, they are not the same
		exit(EXIT_FAILURE);
	}else{
		//cout << "Equal map size!" << endl;
	}

	map<string, pair < CustomPoint, char  > >::const_iterator i, j;
			
	for(i = testSet.begin(), j = predictions.begin(); i != testSet.end(); ++i, ++j)
	{
		pair <string, pair < CustomPoint, char  > > testSetInstance = *i;
		pair <string, pair < CustomPoint, char  > > predictionInstance = *j;
		
		
		// unique blob identifiers
		string testSetInstanceIdentifier = testSetInstance.first; //string blobIdentifierTestSet = testSetInstance.first;  
		string predictionInstanceIdentifier = predictionInstance.first;
		
		// real centroid
		CustomPoint realCentroid;
		realCentroid.y = testSetInstance.second.first.y;
		realCentroid.x = testSetInstance.second.first.x;
		
		//cout << realCentroid.y << " , " << realCentroid.x << endl;
		
		// predicted centroid
		CustomPoint predictedCentroid;
		predictedCentroid.y = predictionInstance.second.first.y;
		predictedCentroid.x = predictionInstance.second.first.x;
		
		//cout << predictedCentroid.y << " , " << predictedCentroid.x << endl;

		string path = testSetInstanceIdentifier;
		string newPath = "";
		size_t foundPatternBLOB = path.find("_BLOB");
		path = path.substr(0,foundPatternBLOB);
		newPath.append(testSetPath + "/" + path + ".cts");
		
		
		AgileMap agileMapTool(newPath.c_str());
		double errorDistancesIstance = agileMapTool.SrcDist(realCentroid.y,realCentroid.x, predictedCentroid.y, predictedCentroid.x);
		//cout << "Error distance: " << errorDistancesIstance << endl;
		errorDistancesTotal += errorDistancesIstance;

		
				
		// blob labels
		char labelTestSet = testSetInstance.second.second;
		char labelClassificationSet = predictionInstance.second.second;
		
		if(testSetInstanceIdentifier == predictionInstanceIdentifier && labelTestSet == 'F' && labelClassificationSet == 'F') {
			
			TP += 1;
			//cout << testSetInstanceIdentifier << " = " << predictionInstanceIdentifier << " and " <<labelTestSet << " = " << labelClassificationSet << endl;
			
		} else if (testSetInstanceIdentifier == predictionInstanceIdentifier && labelTestSet == 'B' && labelClassificationSet == 'F') {
			
			FP += 1;
			//cout << testSetInstanceIdentifier << " = " << predictionInstanceIdentifier << " and " <<labelTestSet << " = " << labelClassificationSet << endl;
			
		} else if (testSetInstanceIdentifier == predictionInstanceIdentifier && labelTestSet == 'B' && labelClassificationSet == 'B') {
			
			TN += 1;
			//cout << testSetInstanceIdentifier << " = " << predictionInstanceIdentifier << " and " <<labelTestSet << " = " << labelClassificationSet << endl;
			
		}else if (testSetInstanceIdentifier == predictionInstanceIdentifier && labelTestSet == 'F' && labelClassificationSet == 'B') {
			
			FN += 1;
			//cout << testSetInstanceIdentifier << " = " << predictionInstanceIdentifier << " and " <<labelTestSet << " = " << labelClassificationSet << endl;
			
		}
		
	}
	
	cout << "Performance computed!\n" << endl;
	
	totalInstances = TP +TN + FN +FP;
	
	totalFluxInstances = TP + FN;

	totalBackgroundInstances = TN + FP;

	errorDistancesMean = errorDistancesTotal / totalFluxInstances;
	
	sensitivity = TP/(double)(TP+FN);
	
	specificity = TN/(double)(TN+FP);
	
	accuracy = sensitivity * (TP/(double)totalInstances) + specificity * (TN/(double)totalInstances);
	
	precision = TP/(double)(TP+FP);
	
	f_measure = 2 * ((precision * sensitivity) / (precision + sensitivity));
	
	k_choen = ( ( (TP + TN) / (double)totalInstances ) - 0.5) / 0.5 ;
	
	falsePositiveRate = FP/(double)totalInstances;
	
	falseNegativeRate = FN/(double)totalInstances;
	
	/// Output printing
	cout << "\n\nOutuput printing \n\n" << endl;
	cout << "Total istances: " << totalInstances << endl;
	cout << "TP: " << TP << endl;
	cout << "TN: " << TN << endl;
	cout << "FP: " << FP << endl;
	cout << "FN: " << FN << endl;
	cout << "EDM: " << errorDistancesMean << endl;
	cout << "FalsePostiveRate: " << setprecision(2) << falsePositiveRate << endl;
	cout << "FalseNegativeRate: " << falseNegativeRate << endl;
	cout << "Kappa statistic: " << k_choen << endl;
	cout << "Accuracy: " << accuracy << endl;
	cout << "FMeasure: " << f_measure << endl;

	/// Outuput file writing
	
	string output2write = "";
	output2write.append("Total Number of Instances: " + to_string(totalInstances) + ", Correctly Classified Instances: " + to_string( TP + TN )  + ", Incorrectly Classified Instances: " + to_string( FP + FN) + ", Kappa statistic: " + to_string(k_choen) +", False Negatives Rate:" + to_string(falseNegativeRate) + ", False Positives Rate: " + to_string(falsePositiveRate) + ", Accuracy: "+ to_string(accuracy) + ", FMeasure: " + to_string(f_measure) + ", Error Distances Mean: " + to_string(errorDistancesMean) );
	FileWriter::write2File("log.txt", output2write);
}


