# AG_extspot-v2-performance-evaluator

## Input:
* const char * path al test set
* double CDELT1
* double CDELT2
* double PSF

## Output:
* float tpCount;
* float fpCount;
* float tnCount;
* float fnCount;
* float precision;
* float recall;
* float specificity;
* float Cohen Kappa;
* double distance (degrees) error mean 
* double distance (degrees) error deviation

## Dipendenze:
* GammaRayDetector.h
* FolderManager.h
* MapConverer.h
* Blob.h
* BlobsFinder.h

## Avvio:
Eseguire lo script run.sh che:
* carica agile
* copia le dipendeze da AG_extspot-v2 (GammaRayDetector, Blob e BlobsFinder ) e da AG_utils (FolderManager, MapConverter)
* compila
* chiama l’eseguibile passandogli parametri di default (100,100,4)

## Creare il test set (ANALISI DEL PROBLEMA):
La cartella ‘test-set’ contiene N immagini. Ogni immagine ha un nome fatto così: 
* B000000001_001_BG_01000s_0_1.cts 
oppure 
* F000000001_001_BG_01000s_0_1.cts

Se il primo carattere del nome è una “b” allora l’immagine è stata simulata con background > 0 e flusso = 0.
Se il primo carattere del nome è una “f” allora l’immagine è stata simulata con background = 0 e flusso > 0.

Per creare il vero test-set (formato da istanze di blob etichettati e non immagini etichettate), si devono estrarre i blobs dalle immagini e metterli etichettati (F o B) in una lista.
    
Inoltre nella stessa lista vogliamo riportare, per ogni blob, un identificativo univoco ( formato dal nome del file + il numero del blob in quel file, ad esempio `MAP1000_313.123_65.223_BLOB1`) e il centroide dell’evento in coordinate galattiche (che sappiamo essere lo stesso per ogni mappa ovvero il centro della mappa).

Un esempio di un elemento della lista è il seguente:
`MAP1000_313.123_65.223_BLOB1 : [  (50 , 50) , F ]`

Problema: 
Per le mappe con solo flusso, è possibile che, all’interno della mappa, compaia qualche fotone di background: questi daranno origine a blobs di background che NON devono essere etichettati come flusso, ma bensì come background.
* creare una funzione che verifica se il blob sia centrato oppure contenga solo 1 fotone: in quel caso si etichetta il blob come background.


## Creare il test set (IMPLEMENTAZIONE):

* map< string , pair < Point , char * > > 



