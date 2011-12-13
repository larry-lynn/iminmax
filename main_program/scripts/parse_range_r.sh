if [ $# -ne 1 ]
then
  echo "Usage: $0 <raw-range-query-only-output-file>"
  exit -1
fi
cat $1 | grep PERFORM | cut -d '/' -f 3 > query_names.txt
cat $1 | grep Average | cut -d ':' -f 2 | sed -e 's/^ //' > miliseconds.txt 
paste -d '\n' query_names.txt miliseconds.txt | tr "\\n" "," | sed -e 's/ ms,/\n/g' > key_val.csv
echo "size,real_16,real_64,real_256"
# row 1
cat key_val.csv | tail -9 | sed -n 'p;N;N' | cut -d ',' -f 2 | tr "\\n" "," | sed -e 's/,$/\n/' | sed -e 's/^/160,/'  
# row 2
cat key_val.csv | tail -9 | tail -8 | sed -n 'p;N;N' | cut -d ',' -f 2 | tr "\\n" "," | sed -e 's/,$/\n/' | sed -e 's/^/1600,/'  
# row 3
cat key_val.csv | tail -9 | tail -7 | sed -n 'p;N;N' | cut -d ',' -f 2 | tr "\\n" "," | sed -e 's/,$/\n/' | sed -e 's/^/16000,/'  
rm query_names.txt
rm miliseconds.txt
rm key_val.csv
