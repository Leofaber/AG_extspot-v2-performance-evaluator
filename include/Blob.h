/*
 * Copyright (c) 2017
 *     Leonardo Baroncelli, Giancarlo Zollino,
 *
 * Any information contained in this software
 * is property of the AGILE TEAM and is strictly
 * private and confidential.
*/


#ifndef BLOB_H
#define BLOB_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>       
#include <vector>
#include <string>
#include <iostream>

using namespace std;
 
struct CustomPoint{
	int x; // colonne
	int y; // righe
};


class Blob
{
    public:

        /**
            Create a new Blob starting from the contour pixels.
            Computes the centroid of the blob.
            Finds the pixels that are inside the blob.
            Finds the number of photons inside the blob.
            Compute the gray level pixel mean of the blob.
			Compute the photon's closeness
        */
        Blob(vector<CustomPoint>& _contourPixels, vector<pair<CustomPoint,int>>& _blobPixels, double ** image, int ** photonImage, double CDELT1, double CDELT2);


        /**
            Return the centroid of the blob
        */
        CustomPoint getCentroid();


        /**
            Return the sum of all the grey levels of the pixels of the blob divided by the number of those pixels.
        */
        double getPixelsMean();

        /**
            Return all the contour points
        */
        vector<CustomPoint> getContour();

        /**
            Return the number of pixels inside the blob
        */
        int getNumberOfPixels();

		/**
			Return the blob's area (degree)
		*/
		double getArea();

        /**
            Return the number of photons inside the blob (before any stretching or smoothing)
        */
        int getNumberOfPhotonsInBlob();

        /**
            Return all the Pixels (a CustomPoint with a grey level) that are inside the blob
        */
        vector<pair<CustomPoint,int>> getBlobPixels();


        /**
            Return the sum of the distances of each photons from the centroid divided by the number of photons
        */
        double getPhotonsCloseness();

        /**
            Check if the blob's centroid is included in a range
        */
        bool isCentered();


     private:



        vector<CustomPoint> contour;

        vector<pair<CustomPoint,int>> blobPixels;

        int numberOfPixels;

		double pixelArea;

		double blobArea;
	
        vector<pair<CustomPoint,int>> photonsInBlob;

        CustomPoint centroid;
 	
		double pixelMean;

        double photonsCloseness;


       
 		vector<pair<CustomPoint,int>> computePhotonsBlob(int ** photonImage);

        CustomPoint computeCentroid();
       
        double computePixelMean();
        
        double computePhotonsCloseness();

        double getDistanceFromCentroid(CustomPoint p);


};

#endif // BLOB_H
