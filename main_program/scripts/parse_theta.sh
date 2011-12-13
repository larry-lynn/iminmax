if [ $# -ne 1 ]
then
  echo "Usage: $0 <raw-range-query-only-output-file>"
  exit -1
fi
echo "theta,average_time"
cat $1 | grep Running | cut -d ' ' -f 7 > thetas.txt
cat $1 | grep Average | cut -d ':' -f 2 | sed -e 's/^ //' | grep ms > miliseconds.txt  
paste -d '\n' thetas.txt miliseconds.txt | tr "\\n" "," | sed -e 's/ ms,/\n/g' 

rm thetas.txt
rm miliseconds.txt

