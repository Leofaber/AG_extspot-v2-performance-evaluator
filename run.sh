#script di avvio

#!/bin/sh
module load agile-B24-r5

#export PFILES=/home/student/PARFILES

#Blob
cp ../AG_extspot-v2/include/Blob.h include/Blob.h
cp ../AG_extspot-v2/src/Blob.cpp src/Blob.cpp

# Blobs Finder
cp ../AG_extspot-v2/include/BlobsFinder.h include/BlobsFinder.h
cp ../AG_extspot-v2/src/BlobsFinder.cpp src/BlobsFinder.cpp

# Gamma Ray Detector
cp ../AG_extspot-v2/include/GammaRayDetector.h include/GammaRayDetector.h
cp ../AG_extspot-v2/src/GammaRayDetector.cpp src/GammaRayDetector.cpp


# Map Converter
cp ../AG_utils/include/FolderManager.h include/FolderManager.h
cp ../AG_utils/src/FolderManager.cpp src/FolderManager.cpp

# Folder Manager
cp ../AG_utils/include/MapConverter.h include/MapConverter.h
cp ../AG_utils/src/MapConverter.cpp src/MapConverter.cpp


make

./bin/AG_extspot-v2-bayesian-model-evaluator trainingset 100 100 4

