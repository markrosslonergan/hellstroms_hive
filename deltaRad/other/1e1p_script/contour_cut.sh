#This is the script for contour selection
#! /bin/bash

echo "Going to delete old files, if you dont stop."
sleep 10
rm  *.root
rm -r BDT*
xml_file=variables.xml
log=record.txt

now=$(date +"%T")
echo "Delete old root files and weight folders; Current time : $now"
echo "-----Beee------! Start training!" > ./$log 2>&1
./eLEE -x $xml_file -o train  >> ./$log 2>&1
echo "Finish!!!!!!!!!!!" >> ./$log 2>&1

now=$(date +"%T")
echo "Current time : $now"
echo "-------------------------------------------------------------------------------------------" >> ./$log 2>&1
echo "Now runs var:" | tee -a ./$log 
./eLEE -x $xml_file -o var >> ./$log 2>&1
echo "Finish!!!!!!!!!!!" >> ./$log 2>&1

now=$(date +"%T")
echo "Current time : $now"
echo "-------------------------------------------------------------------------------------------" >> ./$log 2>&1
echo "Now runs app:" | tee -a ./$log 
./eLEE -x $xml_file -o app >> ./$log 2>&1
echo "Finish!!!!!!!!!!!" >> ./$log 2>&1

now=$(date +"%T")
echo "-------------------------------------------------------------------------------------------" >> ./$log 2>&1
echo "Now runs -o datamc -r:" | tee -a ./$log 
./eLEE -x $xml_file -o datamc -r >> ./$log 2>&1
echo "Finish!!!!!!!!!!!" >> ./$log 2>&1

now=$(date +"%T")
echo "Current time : $now"
echo "-------------------------------------------------------------------------------------------" >> ./$log 2>&1
echo "Now runs -o sig:" | tee -a ./$log 
./eLEE -x $xml_file -o sig  >> ./$log 2>&1
echo "Finish!!!!!!!!!!!" >> ./$log 2>&1

now=$(date +"%T")
echo "Current time : $now"
echo "-------------------------------------------------------------------------------------------" >> ./$log 2>&1
echo "Now runs -o datamc --number :" >> ./$log 2>&1
./eLEE -x $xml_file -o datamc -n '0' >> ./$log 2>&1
echo "Finish!!!!!!!!!!!" >> ./$log 2>&1

#now=$(date +"%T")
#echo "Current time : $now"
#echo "-------------------------------------------------------------------------------------------" >> ./$log 2>&1
#echo "Now runs -o response :" >> ./$log 2>&1
#./eLEE -x $xml_file -o response >> ./$log 2>&1
#
#echo "Finish!!!!!!!!!!!" >> ./$log 2>&1
#now=$(date +"%T")
#echo "Current time : $now"
#echo "-------------------------------------------------------------------------------------------" >> ./$log 2>&1
#echo "Now runs -o recomc :" >> ./$log 2>&1
#./eLEE -x $xml_file -o recomc -n '2'>> ./$log 2>&1
#echo "Finish!!!!!!!!!!!" >> ./$log 2>&1
#now=$(date +"%T")
#echo "Current time : $now"
#echo "-------------------------------------------------------------------------------------------" >> ./$log 2>&1
#echo "Now runs -o eff:" >> ./$log 2>&1
#./eLEE -x $xml_file -o eff>> ./$log 2>&1


