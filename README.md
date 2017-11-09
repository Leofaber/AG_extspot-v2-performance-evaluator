# AG_extspot-v2-performance-evaluator

Lo scopo di questo tool è valutare le performances del classificatore Bayesiano di AG_extspot-v2. 

## Input:
* const char * path al test set
* double classificationThreshold
* double thresholdStep
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

## Implementazione: class PerformanceEvaluator -> metodo evaluate()

* Si definisce `map< string , pair < CustomPoint , char * > > testSet`
* Si definisce `map<    double, map<string,pair<CustomPoint,char*>>      > classificationSetByThresholds`
* Si definisce un `vector<double> thresholds` che conterrà tutte le soglie di classificazione.
* Calcolo valori di soglia:
    * fino a che non arrivo a 100
        * aggiungo a `thresholds` la classificationThreshold
        * classificationThreshold += thresholdStep
* Per ogni valore in `thresholds` si aggiunge una entry a `classificationSetByThresholds`.
* `FolderManager` -> popola una lista `vector<string> filenames`
* Si definisce una lista `vector< pair<string, Blob *> > allBlobs`  dove `string` è l'identificatore univo del blob. Esempio `MAP1000_313.123_65.223_BLOB1 : b`
* Per ogni filename in filenames
    * `MapConverter` -> converte il fits in Map
    * `BlobsFinder` -> aggiunge tutti i blobs in `allBlobs`
* Per ogni blob in `allBlobs`:
    * aggiungiamo una entry alla lista testSet. Ad esempio `MAP1000_313.123_65.223_BLOB1 : [  (45 , 30) , F ]`
        * un blob è etichettato come flusso (F) se e solo se `b->getNumberOfPhotonsInBlob() > 1` && `b->isCentered()`
    * Per ogni classificationThreshold in `classificationSetByThresholds`:
        * Prendiamo la chiave: `double classificationThreshold`
        * Prendiamo il valore: `map<string,pair<CustomPoint,char*> classificationSet` e popoliamolo con:
            * Il nome (string) lo prendiamo dalla chiave di `testSet`
            * `BayesianClassifierForBlobs` -> calcola la percentuale. Se è >= classificationThreshold si etichetta come F (char *).
            * Il `CustomPoint` si prende da `blob->getGalacticCentroid()` /*TODO->change the reference system, convert to galactic*/
            * Esempio: `MAP1000_313.123_65.223_BLOB1 : [  (48.3030 , 51.3011) , F ]`
 
* Per ogni soglia di classificazione (ovvero per ogni chiave in `classificationSetByThresholds`):
    * Prendiamo il valore: `map<string,pair<CustomPoint,char*>> classificationSet`
    * Creiamo le variabili per il calcolo della performance:
        * `TP,TN,FP,FN`
        * `vector<double> errorDistances`
    * Confrontiamo la lista `testSet` con la lista `classificationSet`, iterandole:
        * Confrontiamo l'etichetta vera con l'etichetta predetta -> aggiorniamo `TP,TN,FP,FN`
        * Calcoliamo la distanza sferica tra CustomPoint vero e CustomPoint predetto -> aggiorniamo `errorDistances`
    * Scrive su file:
        * `Total Number of Instances, Correctly Classified Instances, Incorrectly Classified Instances, Kappa statistic, False Negatives Rate, False Positives Rate, Accuracy, FMeasure, Error Distances Mean, Error Distances Deviation`

## Appunti 
* Inserimento di una entry in una mappa `map.insert(std::pair<key_type, value_type>(key, value));`
* Iterare una mappa 
    * ` map<string, int> symbolTable;`
    * ` for (map<string, int>::iterator it = symbolTable.begin(); it != symbolTable.end(); it++ )`
    * ` {    std::cout << it->first  // string (key)`
    * `      << ':'`
    * `      << it->second   // string's value `
    * `      << std::endl ;`
    * ` }`
