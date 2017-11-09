/*
 * Copyright (c) 2017
 *     Leonardo Baroncelli, Giancarlo Zollino,
 *
 * Any information contained in this software
 * is property of the AGILE TEAM and is strictly
 * private and confidential.
*/


#include "BlobsFinder.h"

BlobsFinder::BlobsFinder()
{
    //ctor
}

vector<Blob*> BlobsFinder::findBlobs(int** data, int rows, int cols, double CDELT1, double CDELT2) {


 


	/*
		Conversione double ** -> Mat
	*/
	Mat originalImage(rows, cols, CV_8UC1, Scalar(0));

	for(int y = 0; y < rows; y++){
		for(int x =0; x < cols; x++){
			originalImage.at<uchar>(y,x)=(uchar)data[y][x];
			//cout << data[y][x] << " ";
		}
		//cout << "\n";
	}
	



 
	Mat tempImage = originalImage.clone();
 


	
    /// STRETCHING
	BlobsFinder::nonLinearStretch(&tempImage,0.05);
	/* PRINT DEBUG
	cout << "\n\n\n" << endl;
	for(int i = 0; i < rows; i++){
		for(int j =0; j < cols; j++){
			cout << (unsigned int)tempImage.at<uchar>(i,j) << " ";
		}
		cout << "\n" << endl;
	}*/
	//printImage(tempImage, "stretchgin");
	

  

    /// GAUSSIAN FILTERING
	BlobsFinder::gaussianBlur(&tempImage, Size(21, 21), 4); // 17x17   2.5    23x23   3
	/* PRINT DEBUG
	cout << "\n\n\n" << endl;
	for(int i = 0; i < rows; i++){
		for(int j =0; j < cols; j++){
			cout << (unsigned int)tempImage.at<uchar>(i,j) << " ";
		}
		cout << "\n" << endl;
	}*/
	//printImage(tempImage, "blur");
 	
	 


	/// FIND THE CONTOUR OF EACH BLOB
    vector<Blob*> blobs;
    vector<vector<Point> > opencvContours;
    vector<Vec4i> hierarchy;
	Mat copyOfImage = tempImage.clone();

    findContours( copyOfImage, opencvContours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);

 
	
	// Convert Map -> double ** 
	double ** editedImage;
	editedImage = new double *[rows];
  	
	for(int y = 0; y < rows; y++){
		editedImage[y] = new double[cols];

		for(int x =0; x < cols; x++){
			editedImage[y][x] = (double) tempImage.at<uchar>(y,x);
		}
	}
 

    int indexx=0;
    for(vector<vector<Point> >::iterator i = opencvContours.begin(); i < opencvContours.end(); i++){

        vector<Point> currentContuor = *i;


        /// CREATING A BLOB (se e solo se non è un blob contenuto in un altro blob..
        if(hierarchy[indexx][3]==-1){

			
			/*
				Convert vector<Point> -> vector<CustomPoint>				
			*/
			vector<CustomPoint> currentCustomContuor;
			for(vector<Point> ::iterator ii = currentContuor.begin(); ii < currentContuor.end(); ii++){
				CustomPoint cp;
				Point p = *ii;
				//cout <<"point:  "<< *ii << endl;
				cp.x = p.x;
				cp.y = p.y;
				currentCustomContuor.push_back(cp);
			}

			/*
				Compute blob's pixels
			*/
			vector<pair<CustomPoint,int>> blobPixels = computePixelsOfBlob(currentContuor,tempImage);
			/*
				Create Blob
			*/

			Blob* b = new Blob(currentCustomContuor,blobPixels,editedImage,data, CDELT1, CDELT2);
			/*
				Push Blob in list
			*/
			blobs.push_back(b);
 
		}
            
        indexx++;

    }


	/*
		!!!!FREE MEMORY!!!!
	
	*/
	

 	for (int i = 0; i < rows; i++)
	{
		delete [] editedImage[i];
	}
	delete [] editedImage;
	editedImage = 0;

	cout << "Number of blobs: "<< blobs.size() << endl;
	 
    return blobs;
}

/**********************************

	computePixelsOfBlob

**********************************/
vector<pair<CustomPoint,int>> BlobsFinder::computePixelsOfBlob(vector<Point>& c, Mat image){

	 vector<pair<CustomPoint,int>> pixels;
     for(int y = 0; y < image.rows; y++){
        for(int x=0; x < image.cols; x++){
            Point p(x,y);
            double isInside = pointPolygonTest(c,p,false);
            if( (isInside > 0) | (isInside == 0)){

				CustomPoint cp;
				cp.x = p.x;
				cp.y = p.y;
                pixels.push_back(make_pair(cp,(int)image.at<uchar>(y,x)));
            }
        }
    }
	return pixels;
}








/***********************************

	STRETCHING

***********************************/
void BlobsFinder::nonLinearStretch(Mat* image, float r)
{
   // Mat image = inputImage.clone();
	int rows = image->rows;
	int cols = image->cols;
	for (int i = 0; i < rows; ++i){
		for (int j = 0; j < cols; ++j){
			unsigned int pixelValue = (unsigned int)image->at<uchar>(i, j);
			image->at<uchar>(i, j) =  (uchar) pow(255, 1 - r)* pow(pixelValue,r);
		}
	}
   // return image;

}









/**********************************

	SMOOTHING

**********************************/
void BlobsFinder::gaussianBlur(Mat* image, Size kernelSize, double sigma) {
	Mat outputImg;
	GaussianBlur(*image, outputImg, kernelSize, sigma, 0, 0);
 
	*image = outputImg;
	//return outputImg;
}














/**********************************

	PRINT FUNCTIONS

**********************************/
void BlobsFinder::printImage(Mat image, string windowName){

	Mat stretchedImage = image.clone();
	int rows = image.rows;
	int cols = image.cols;
	
	int max = 0;
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j)
		{
			int pixelValue = (int)image.at<uchar>(i, j);
			if (pixelValue > max)
				max = pixelValue;
		}
	}
	//cout << "max: " << max;


	if (max == 0){
		cout << "[Stretching] Error: max = 0!" << endl;
		//return inputImage;
	}
	// slope for linear stretching
	int slope = int(255 / max);


	// linear stretching
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{	
			int pixelValue = (unsigned int)image.at<uchar>(i, j);
			int newPixelValue = pixelValue*slope;
			stretchedImage.at<uchar>(i, j) = (uchar)newPixelValue;

		}
	}

    namedWindow( windowName, CV_WINDOW_AUTOSIZE );
	Mat outputImage;
    resize(stretchedImage, outputImage, Size(0, 0), 5, 5, INTER_LINEAR);
    imshow( windowName, outputImage );
	waitKey(0);

}
void BlobsFinder::printImageBlobs(int rows,int cols, vector<Blob> blobs, string windowName){
    Mat temp3ChannelImage(rows,cols, CV_32FC3, Scalar(0,0,0));
    for(vector<Blob>::iterator i = blobs.begin(); i != blobs.end(); i++){
        printImageBlob(temp3ChannelImage,*i,windowName);
    }
    waitKey(0);
}
void BlobsFinder::printImageBlob(Mat inputImage, Blob b, string windowName) {
   	Mat outputImage;

    ///DRAW CONTOUR
    vector<CustomPoint> contour = b.getContour();
    Vec3b color( rand()&255, rand()&255, rand()&255 );
    for(vector<CustomPoint>::iterator i = contour.begin(); i != contour.end(); i++){
        CustomPoint p = *i;
        inputImage.at<Vec3b>(p.y,p.x) = color;
    }

    ///DRAW CENTROID
    CustomPoint centroid = b.getCentroid();
    inputImage.at<Vec3b>(centroid.y, centroid.x) = Vec3b(0,0,255);

    namedWindow( windowName, CV_WINDOW_AUTOSIZE );
    resize(inputImage, outputImage, Size(0, 0), 3, 3, INTER_LINEAR);
    imshow( windowName, outputImage );

}



//Mat kernel =  getGaussianKernel(21, 1, CV_64F );


	/*
	cout << "KERNEL:" << endl;
	Mat kernelImage(Size(1, 49), CV_64FC1, Scalar(0)); 

	for (int i = 0; i < 21; ++i) {
		for (int j = 0; j < 1; ++j)
		{
			double pixelValue = (double)kernel.at<double>(i, j);
 			cout << pixelValue << " ";
 			kernelImage.at<double>(pixelValue, j) = 100; 
 
		}
		cout << "\n";
	}
	namedWindow( "kernel", CV_WINDOW_AUTOSIZE );
    	imshow( "kernel", kernelImage );
	waitKey(0);
	*/