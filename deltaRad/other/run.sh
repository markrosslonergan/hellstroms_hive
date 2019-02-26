#! /bin/bash

now=$(date +"%T")
echo "Current time : $now"
echo "-----Beee------! Start training!" > ./record.txt 2>&1
./deltaRad_electron -x default_bnb.xml -o train -b >> ./record.txt 2>&1
./deltaRad_electron -x default.xml -o train -c >> ./record.txt 2>&1

echo "Finish!!!!!!!!!!!" >> ./record.txt 2>&1
now=$(date +"%T")
echo "Current time : $now"
echo "-------------------------------------------------------------------------------------------" >> ./record.txt 2>&1
echo "Now runs app -var:" | tee -a ./record.txt 
./deltaRad_electron -o var #>> ./record.txt 2>&1

echo "Finish!!!!!!!!!!!" >> ./record.txt 2>&1
now=$(date +"%T")
echo "Current time : $now"
echo "-------------------------------------------------------------------------------------------" >> ./record.txt 2>&1
echo "Now runs app --bnb:" | tee -a ./record.txt 
./deltaRad_electron -o app --bnb > /dev/null #>> ./record.txt 2>&1

echo "Finish!!!!!!!!!!!" >> ./record.txt 2>&1
now=$(date +"%T")
echo "Current time : $now"
echo "-------------------------------------------------------------------------------------------" >> ./record.txt 2>&1
echo "Now runs app --cosmic:"| tee -a ./record.txt 
./deltaRad_electron -o app --cosmic > /dev/null # >> ./record.txt 2>&1

echo "Now runs app:" | tee -a ./record.txt 
./deltaRad_electron -o app >> ./record.txt 2>&1

echo "Finish!!!!!!!!!!!" >> ./record.txt 2>&1
now=$(date +"%T")
echo "Current time : $now"
echo "-------------------------------------------------------------------------------------------" >> ./record.txt 2>&1
echo "Now runs -o sig:" | tee -a ./record.txt 
./deltaRad_electron -o sig  >> ./record.txt 2>&1

echo "-------------------------------------------------------------------------------------------" >> ./record.txt 2>&1
echo "Now runs -r -o datamc --number 2 :" | tee -a ./record.txt 
./deltaRad_electron -r -o datamc >> ./record.txt 2>&1
echo "Finish!!!!!!!!!!!" >> ./record.txt 2>&1
now=$(date +"%T")
echo "Current time : $now"


echo "-------------------------------------------------------------------------------------------" >> ./record.txt 2>&1
echo "Now runs -o stack :" | tee -a ./record.txt 
./deltaRad_electron -o stack -n '2'>> ./record.txt 2>&1

echo "Finish!!!!!!!!!" >> ./record.txt 2>&1
now=$(date +"%T")
echo "Current time : $now"
echo "-------------------------------------------------------------------------------------------" >> ./record.txt 2>&1
echo "Now runs -o datamc --number :" >> ./record.txt 2>&1
./deltaRad_electron -o datamc -n '2' >> ./record.txt 2>&1

echo "Finish!!!!!!!!!!!" >> ./record.txt 2>&1
now=$(date +"%T")
echo "Current time : $now"
echo "-------------------------------------------------------------------------------------------" >> ./record.txt 2>&1
echo "Now runs -o response :" >> ./record.txt 2>&1
./deltaRad_electron -o response >> ./record.txt 2>&1

echo "Finish!!!!!!!!!!!" >> ./record.txt 2>&1
now=$(date +"%T")
echo "Current time : $now"
echo "-------------------------------------------------------------------------------------------" >> ./record.txt 2>&1
echo "Now runs -o recomc :" >> ./record.txt 2>&1
./deltaRad_electron -o recomc -n 2>> ./record.txt 2>&1
echo "Finish!!!!!!!!!!!" >> ./record.txt 2>&1
now=$(date +"%T")
echo "Current time : $now"
echo "-------------------------------------------------------------------------------------------" >> ./record.txt 2>&1
echo "Now runs -o eff:" >> ./record.txt 2>&1
./deltaRad_electron -o eff>> ./record.txt 2>&1


