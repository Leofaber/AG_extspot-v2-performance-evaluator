# AG_extspot-v2-performance-evaluator

Lo scopo di questo tool è valutare le performances del classificatore Bayesiano di AG_extspot-v2. 

## Input:
* const char * path al test set
* double minThreshold
* double maxThreshold
* double step
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
* BayesianClassifierForBlobs.h/.cpp
* FolderManager.h/.cpp
* FileWriter.h/.cpp
* MapConverer.h/.cpp
* Blob.h/.cpp
* BlobsFinder.h/.cpp

## Avvio:
Eseguire lo script run.sh che:
* carica agile
* copia le dipendeze da AG_extspot-v2 (BayesianClassifierForBlobs, Blob e BlobsFinder ) e da AG_utils (FolderManager, MapConverter,FileWriter)
* compila
* chiama l’eseguibile passandogli parametri di default (testset, 95, 0.1, 100, 100, 4)

## Workflow del tool
* Creare la lista del test set 
* Creare la lista delle classificazioni (ha lo stesso formato della lista del test set)
* Calcolare output confrontando la lista del test set con la lista delle classificazioni

## Creare la lista del test set (ANALISI DEL PROBLEMA):
La cartella ‘test-set’ contiene N immagini. Ogni immagine ha un nome fatto così: 
* B000000001_001_BG_01000s_0_1.cts 
oppure 
* F000000001_001_BG_01000s_0_1.cts

Se il primo carattere del nome è una “b” allora l’immagine è stata simulata con background > 0 e flusso = 0.
Se il primo carattere del nome è una “f” allora l’immagine è stata simulata con background = 0 e flusso > 0.

Per creare il vero test-set (formato da istanze di blob etichettati e non immagini etichettate), si devono estrarre i blobs dalle immagini e metterli etichettati (F o B) in una lista.
    
Inoltre nella stessa lista vogliamo riportare, per ogni blob, un *identificativo univoco* ( formato dal nome del file + il numero del blob in quel file, ad esempio `MAP1000_313.123_65.223_BLOB1`) e il centroide dell’evento in coordinate galattiche (che sappiamo essere lo stesso per ogni mappa ovvero il centro della mappa).

Un esempio di un elemento della lista è il seguente:
`MAP1000_313.123_65.223_BLOB1 : [  (45 , 30) , F ]`

Problema: 
* Per le mappe con solo flusso, è possibile che, all’interno della mappa, compaia qualche fotone di background: questi daranno origine a blobs di background che NON devono essere etichettati come flusso, ma bensì come background.
Soluzione:
* creare una funzione che verifica se il blob sia centrato oppure contenga solo 1 fotone: in quel caso si etichetta il blob come background.

## Creare la lista delle classificazioni (ANALISI DEL PROBLEMA):
Si utilizza il `BayesianClassifierForBlobs` per calcolare le probabilità di un blob di essere flusso, si etichetta usando la `classificationThreshold` passata in input. 
Si ricava il centroide del blob in coordinate galattiche chiamando il metodo `b->getGalacticCentroid()` che deve essere ancora implementato in Blob.h.

*Problema1:*
* Dato che dobbiamo creare il grafico di andamento della false positeve rate al variare della soglia, dobbiamo calcolare le misure di performance per molti valori di soglia. 
*Soluzione1:*
* Si dà in input non solo il `double classificationThreshold` ma anche un `double thresholdStep`: quindi si calcolano tanti valori di soglia partendo da `classificationThreshold` incrementandolo di `thresholdStep` fino ad arrivare ad un valore >= 100.
* Per ogni valore di soglia così trovato si istanzia un `map< string , pair < CustomPoint , char * > > classificationSet<THRESHOLD>`
* Queste mappe si inseriscono all'interno di un ulteriore mappa, le cui chiavi sono le soglie:  `map<    double, map<string,pair<CustomPoint,char*>>      >`

*Problema2:*
* Per creare un elemento di una mappa `map<string,pair<CustomPoint,char*>>` ci vogliono diverse righe di codice.
*Soluzione2:*
* Creare una funzione che che inserisce l'elemento `string,pair<CustomPoint,char*>` dato in input `string,CustomPoint,char*`

## Implementazione: 
* class PerformanceEvaluator
    * il costruttore accetta: string testSetPath, double threshold, double CDELT1, double CDELT2, double PSF
    
* metodo evaluate()
    * Si definisce `map< string , pair < CustomPoint , char > > testSet`
    * Si definisce `map< string , pair < CustomPoint , char > > classificationSet`
    * `FolderManager` -> popola una lista `vector<string> filenames`
    * Si definisce una lista `vector< pair<string, Blob *> > allBlobs`  dove `string` è l'identificatore univo del blob. Esempio `FMAP1000_313.123_65.223_BLOB1 : b`
    * Per ogni filename in filenames    (si popola la lista `allBlobs`)
        * `MapConverter` -> converte il fits in CustomMap
        * `BlobsFinder`::findBlobs(int ** image, int rows, int cols, double CDELT1, double CDELT2) -> ritorna un `vector<Blob* >`. Per ogni blob trovato, si aggiunge in `allBlobs` la coppia make_pair(`filename`+countBlob , puntatore al blob) 
    * Per ogni blob in `allBlobs`:      (si popolano le liste `testSet` e `classificationSet`)
        * aggiungiamo una entry alla lista `testSet`. Ad esempio `FMAP1000_313.123_65.223_BLOB1 : [  (45 , 30) , F ]`
            * se il primo carattere dell'identificatore univoco del blob è 'B' allora l'etichetta è 'B'.
            * se il primo carattere dell'identificatore univoco del blob è 'F' allora l'etichetta è 'F' se e solo se `b->getNumberOfPhotonsInBlob() > 1` && `b->isCentered()` altrimenti l'etichetta è 'B'.   
        * aggiungiamo una entry alla lista `classificationSet`:
            * Il nome (string) lo prendiamo dall'elemento corrente di `allBlobs`
            * Il `CustomPoint` si prende da `blob->getGalacticCentroid()` /*TODO->change the reference system, convert to galactic*/
            * Per calcolare l'etichetta si chiama il `BayesianClassifierForBlobs` -> calcola la percentuale:
                * Se è >= classificationThreshold si etichetta come 'F'.
            * Esempio: `FMAP1000_313.123_65.223_BLOB1 : [  (48.3030 , 51.3011) , F ]``
     * Creiamo le variabili per il calcolo della performance:
        * `TP,TN,FP,FN`
        * `vector<double> errorDistances`
     * Confrontiamo la lista `testSet` con la lista `classificationSet`, iterandole:
        * Confrontiamo l'etichetta vera con l'etichetta predetta -> aggiorniamo `TP,TN,FP,FN`
        * Calcoliamo la distanza sferica tra CustomPoint vero e CustomPoint predetto -> aggiorniamo `errorDistances`
     * Scrive su file:
        * `Total Number of Instances, Correctly Classified Instances, Incorrectly Classified Instances, Kappa statistic, False Negatives Rate, False Positives Rate, Accuracy, FMeasure, Error Distances Mean, Error Distances Deviation`

