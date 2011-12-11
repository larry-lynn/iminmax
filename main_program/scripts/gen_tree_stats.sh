echo "Changing working directory to that of main program"
cd ..
datafiles=`find ../../csci540_data -name '*.csv' | grep -v centers`
for file in $datafiles
do
  echo "Processing File: " $file
  echo "##########"
  ./main_program -b --datafile $file --savetf cache
  echo "Done Processing File: " $file
  echo ">>>>>>>>>>"
  echo " "
done
rm cache.*

datafiles=`find ../../csci540_data -name '*.txt'`
for file in $datafiles
do
  echo "Processing File: " $file
  echo "##########"
  ./main_program -b --datafile $file --savetf cache
  echo "Done Processing File: " $file
  echo ">>>>>>>>>>"
  echo " "
done
rm cache.*