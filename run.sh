#script di avvio

#!/bin/sh
module load agile-B24-r5

#export PFILES=/home/student/PARFILES

#Blob
cp ../AG_utils/include/Blob.h include/Blob.h
cp ../AG_utils/src/Blob.cpp src/Blob.cpp

# Blobs Finder
cp ../AG_utils/include/BlobsFinder.h include/BlobsFinder.h
cp ../AG_utils/src/BlobsFinder.cpp src/BlobsFinder.cpp

# BayesianClassifierForBlobs
cp ../AG_utils/include/BayesianClassifierForBlobs.h include/BayesianClassifierForBlobs.h
cp ../AG_utils/src/BayesianClassifierForBlobs.cpp src/BayesianClassifierForBlobs.cpp


# Map Converter
cp ../AG_utils/include/FolderManager.h include/FolderManager.h
cp ../AG_utils/src/FolderManager.cpp src/FolderManager.cpp

# Folder Manager
cp ../AG_utils/include/MapConverter.h include/MapConverter.h
cp ../AG_utils/src/MapConverter.cpp src/MapConverter.cpp

# File Writer
cp ../AG_utils/include/FileWriter.h include/FileWriter.h
cp ../AG_utils/src/FileWriter.cpp src/FileWriter.cpp


make

./bin/AG_extspot-v2-performance-evaluator ./testSet 95 0.1 100 100 4

