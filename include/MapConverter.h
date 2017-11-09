/*
 * Copyright (c) 2017
 *     Leonardo Baroncelli, Giancarlo Zollino,
 *
 * Any information contained in this software
 * is property of the AGILE TEAM and is strictly
 * private and confidential.
 * 
 * https://github.com/Leofaber/MapConverter
*/


#include "FitsUtils.h"
#include <iostream>
#include <string>

using namespace std;

//template <class T>
class Map {

	public:
  		//T image;
		int ** image;
		int rows;
		int cols;
		string imagePath;

		Map(){ 
			rows = 0;
			cols = 0;
			imagePath = "";
		}

		~Map(){
			for (  int i = 0; i < rows; i++){
				delete [] image[i];
			}
			delete [] image;
			image = 0;
		}
	
		 
		void initializeImage(string imgPath, int r, int c){
			imagePath = imgPath;			
			rows = r;
			cols = c;
			image = new int*[rows];
			for (int y = 0; y < rows; ++y){
				image[y] = new int[cols];
			}
		}
		
};

class MapConverter
{
	public:

		
		// Convert an image in a **double matrix
		static double ** mapPathToDoublePtr(const char * imagePath);
		
		// Convert an image in a **double matrix
		static /*Map<int **>*/ Map mapPathToIntPtr(const char * imagePath);
	


	private:
		// Constructor
		MapConverter();
};
