# rm hltKeysForTrigger.txt
# echo "********** Run A **********" > hltKeysForTrigger.txt
# ./getHLTkey.py --firstRun 190456 --lastRun 193621 --perKey >> hltKeysForTrigger.txt
# echo "********** Run A recover **********" >> hltKeysForTrigger.txt
# ./getHLTkey.py --firstRun 190782 --lastRun 190949 --perKey >> hltKeysForTrigger.txt
# echo "********** Run B **********" >> hltKeysForTrigger.txt
# ./getHLTkey.py --firstRun 193833 --lastRun 196531 --perKey >> hltKeysForTrigger.txt
# echo "********** Run C1 **********" >> hltKeysForTrigger.txt
# ./getHLTkey.py --firstRun 198022 --lastRun 198913 --perKey >> hltKeysForTrigger.txt
# echo "********** Run C2 **********" >> hltKeysForTrigger.txt
# ./getHLTkey.py --firstRun 198934 --lastRun 203746 --perKey >> hltKeysForTrigger.txt
# echo "********** Run D **********" >> hltKeysForTrigger.txt
# ./getHLTkey.py --firstRun 203768 --lastRun 208686 --perKey >> hltKeysForTrigger.txt

# Usage instructions in (UserCode)/LIP/TopTaus/Readme.txt


#if not interested to prescales columns, search and replace 
# csv >> hltKeysForTrigger
# -->
# csv | grep "run;" >> hltKeysForTrigger
# and viceversa

rm *csv
rm hltKeysForTrigger.txt
echo "********** Run A **********" > hltKeysForTrigger.txt
./makeHLTPrescaleTable.py --firstRun 190456 --lastRun 193621 --path='HLT_IsoMu24_v*' outA.csv
cat outA.csv >> hltKeysForTrigger.txt
./makeHLTPrescaleTable.py --firstRun 190456 --lastRun 193621 --path='HLT_IsoMu24_eta2p1_v*' outAeta.csv
cat outAeta.csv >> hltKeysForTrigger.txt
echo "********** Run A recover **********" >> hltKeysForTrigger.txt					    
./makeHLTPrescaleTable.py --firstRun 190782 --lastRun 190949 --path='HLT_IsoMu24_v*' outAR.csv 
cat outAR.csv >> hltKeysForTrigger.txt
./makeHLTPrescaleTable.py --firstRun 190782 --lastRun 190949 --path='HLT_IsoMu24_eta2p1_v*' outAReta.csv 
cat outAReta.csv >> hltKeysForTrigger.txt
echo "********** Run B **********" >> hltKeysForTrigger.txt  					    
./makeHLTPrescaleTable.py --firstRun 193833 --lastRun 196531 --path='HLT_IsoMu24_v*' outB.csv
cat outB.csv >> hltKeysForTrigger.txt
./makeHLTPrescaleTable.py --firstRun 193833 --lastRun 196531 --path='HLT_IsoMu24_eta2p1_v*' outBeta.csv
cat outBeta.csv >> hltKeysForTrigger.txt
echo "********** Run C1 **********" >> hltKeysForTrigger.txt 					    
./makeHLTPrescaleTable.py --firstRun 198022 --lastRun 198913 --path='HLT_IsoMu24_v*' outC1.csv 
cat outC1.csv >> hltKeysForTrigger.txt
./makeHLTPrescaleTable.py --firstRun 198022 --lastRun 198913 --path='HLT_IsoMu24_eta2p1_v*' outC1eta.csv 
cat outC1eta.csv >> hltKeysForTrigger.txt
echo "********** Run C2 **********" >> hltKeysForTrigger.txt 					    
./makeHLTPrescaleTable.py --firstRun 198934 --lastRun 203746 --path='HLT_IsoMu24_v*' outC2.csv 
cat outC2.csv >> hltKeysForTrigger.txt
./makeHLTPrescaleTable.py --firstRun 198934 --lastRun 203746 --path='HLT_IsoMu24_eta2p1_v*' outC2eta.csv 
cat outC2eta.csv >> hltKeysForTrigger.txt
echo "********** Run D **********" >> hltKeysForTrigger.txt  					    
./makeHLTPrescaleTable.py --firstRun 203768 --lastRun 208686 --path='HLT_IsoMu24_v*' outD.csv
 cat outD.csv >> hltKeysForTrigger.txt
./makeHLTPrescaleTable.py --firstRun 203768 --lastRun 208686 --path='HLT_IsoMu24_eta2p1_v*' outDeta.csv
 cat outDeta.csv >> hltKeysForTrigger.txt


