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
	
	insertAllBlobs();
	
	insertTestSet();
	
	insertClassificationSet();
	
	computePerformance();
	
	   
	cout << "\n\n" << endl;
   
	
}

void PerformanceEvaluator::insertAllBlobs() {
	
	vector<Blob*> blobs;
	
	for(vector<string>::const_iterator it = filenames.begin(); it < filenames.end(); ++it ) {
		
		string imageName = *it;
		string imagePathName = "testSet/";
		int countBlob = 0;
		
		imagePathName.append(imageName);
		
		AgileMap agileMapTool(imagePathName.c_str());
		
		CustomMap map = MapConverter::mapPathToIntPtr(imagePathName.c_str());
		
				
		///Trovo i blobs all'interno dell'imagine tramite Blobs finder
		blobs = BlobsFinder::findBlobs(imagePathName, map.image,map.rows,map.cols,agileMapTool.GetXbin() ,agileMapTool.GetYbin() );
		 
		///Inserisco i blob all'interno di allBlobs
		for(vector<Blob *>::const_iterator it = blobs.begin(); it < blobs.end(); ++it ) {
			
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
	
	cout << "Elementi inseriti in allBlobs\n" << endl;	
	
}


void PerformanceEvaluator::insertTestSet(){
	

	//BYLEO
	for(vector< pair < string , Blob * > >::iterator i = allBlobs.begin(); i != allBlobs.end(); i++){
		
		pair < string , Blob * > blobInstance = *i;
		
		// unique blob identifier
		string blobIdentifier = blobInstance.first;
		
		// blob pointer
		Blob * blobPtr = blobInstance.second;
		
				
		if( blobIdentifier.compare(0,1,"B") == 0 ){
			//CustomPoint centroid = blobPtr->getGalacticCentroid();
			CustomPoint centroidPX = blobPtr->getCentroid();
			pair<CustomPoint, char> tmp = make_pair(centroidPX,'B');
			testSet.insert(make_pair(blobIdentifier, make_pair(centroidPX, 'B')));
		}
		else if(blobPtr->getNumberOfPhotonsInBlob() > 1 && blobPtr->isCentered()){
			CustomPoint centroidPX = blobPtr->getCentroid();
			pair<CustomPoint, char> tmp = make_pair(centroidPX,'F');
			testSet.insert(make_pair(blobIdentifier, make_pair(centroidPX, 'F')));
		} else {
			CustomPoint centroidPX = blobPtr->getCentroid();
			pair<CustomPoint, char> tmp = make_pair(centroidPX,'B');
			testSet.insert(make_pair(blobIdentifier, make_pair(centroidPX, 'B')));
		}

	}

	
	cout << "Elementi inseriti in testSet\n" << endl;
	
	
	for(map<string, pair < CustomPoint, char  > >::const_iterator it = testSet.begin(); it != testSet.end(); ++it)
	{
		std::cout << "testSet element: " << it->first << " (" << it->second.first.y << "," << it->second.first.x << "), " << it->second.second << "\n";
	}
	
	
}

void PerformanceEvaluator::insertClassificationSet() {
	
	vector<pair<string,double> > prediction;
	
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
			
			classificationSet.insert(make_pair(blobIdentifier, make_pair(centroid, 'F')));
				
		else 
			classificationSet.insert(make_pair(blobIdentifier, make_pair(centroid, 'B')));
			
				
	}
	
	cout << "Elementi inseriti in classificatioSet" << endl;
	
	for(map<string, pair < CustomPoint, char  > >::const_iterator it = classificationSet.begin(); it != classificationSet.end(); ++it)
	{
		std::cout << "testSet element: " << it->first << " (" << it->second.first.y << "," << it->second.first.x << "), " << it->second.second << "\n";
	}

}

void PerformanceEvaluator::computePerformance() {

	TP = TN =  FP = FN = 0;
	
	cout << "computePerformance!\n" << endl;
	
	cout << "testSet size: " << testSet.size() << ", classficationSet size: " << classificationSet.size() << ", allBlobs size: "<< allBlobs.size() << endl;

	if(testSet.size() != classificationSet.size())
		cout << "Different map size "<< endl;  // differing sizes, they are not the same
	else
		cout << "Equal map size!" << endl;

	map<string, pair < CustomPoint, char  > >::const_iterator i, j;
			
	for(i = testSet.begin(), j = classificationSet.begin(); i != testSet.end(); ++i, ++j)
	{
		pair <string, pair < CustomPoint, char  > > testSetInstance = *i;
		pair <string, pair < CustomPoint, char  > > classificationSetInstance = *j;
		
		
		// unique blob identifiers
		string blobIdentifierTestSet = testSetInstance.first;
		string blobIdentifierClassificationSet = classificationSetInstance.first;
		
		// real centroid
		CustomPoint realCentroid;
		realCentroid.y = testSetInstance.second.first.y;
		realCentroid.x = testSetInstance.second.first.x;
		
		cout << realCentroid.y << " , " << realCentroid.x << endl;
		
		// predicted centroid
		CustomPoint predictedCentroid;
		predictedCentroid.y = classificationSetInstance.second.first.y;
		predictedCentroid.x = classificationSetInstance.second.first.x;
		
		cout << predictedCentroid.y << " , " << predictedCentroid.x << endl;

		const char * path = "testSet/B000000026_001_BG_01000s_0_1.cts";
		agileMapTool(path);
		double errorDistancesIstance = agileMapTool.SrcDist(realCentroid.y,realCentroid.x, predictedCentroid.y, predictedCentroid.x);
		cout << "Error distance: " << errorDistancesIstance << endl;
		//errorDistances.push_back(errorDistancesIstance);
		
		
		
		
		// compute error distance of predicted centroid from real centroid
		/*for(vector < double > :: const_iterator k = errorDistances.begin(); k != errorDistances. end() ; ++k) {
			cout << "Ciao" << endl;
			double errorDistancesIstance = *k;
			errorDistancesIstance = agileMapTool.SrcDist(realCentroid.y,realCentroid.x, predictedCentroid.y, predictedCentroid.x);
			cout << "Error distance: " << *k << endl;
		}*/
		
		
		// blob labels
		char labelTestSet = testSetInstance.second.second;
		char labelClassificationSet = classificationSetInstance.second.second;
		
		if(blobIdentifierTestSet == blobIdentifierClassificationSet && labelTestSet == 'F' && labelClassificationSet == 'F') {
			
			TP += 1;
			cout << blobIdentifierTestSet << " = " << blobIdentifierClassificationSet << " and " <<labelTestSet << " = " << labelClassificationSet << endl;
			
		} else if (blobIdentifierTestSet == blobIdentifierClassificationSet && labelTestSet == 'B' && labelClassificationSet == 'F') {
			
			FP += 1;
			cout << blobIdentifierTestSet << " = " << blobIdentifierClassificationSet << " and " <<labelTestSet << " = " << labelClassificationSet << endl;
			
		} else if (blobIdentifierTestSet == blobIdentifierClassificationSet && labelTestSet == 'B' && labelClassificationSet == 'B') {
			
			TN += 1;
			cout << blobIdentifierTestSet << " = " << blobIdentifierClassificationSet << " and " <<labelTestSet << " = " << labelClassificationSet << endl;
			
		}else if (blobIdentifierTestSet == blobIdentifierClassificationSet && labelTestSet == 'F' && labelClassificationSet == 'B') {
			
			FN += 1;
			cout << blobIdentifierTestSet << " = " << blobIdentifierClassificationSet << " and " <<labelTestSet << " = " << labelClassificationSet << endl;
			
		}
		
	}
	
	cout << "TP: " << TP << endl;
	cout << "TN: " << TN << endl;
	cout << "FP: " << FP << endl;
	cout << "FN: " << FN << endl;

}


