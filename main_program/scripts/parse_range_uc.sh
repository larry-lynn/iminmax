if [ $# -ne 1 ]
then
  echo "Usage: $0 <raw-range-query-only-output-file>"
  exit -1
fi
cat $1 | grep PERFORM | cut -d '/' -f 3 > query_names.txt
cat $1 | grep Average | cut -d ':' -f 2 | sed -e 's/^ //' > miliseconds.txt 
paste -d '\n' query_names.txt miliseconds.txt | tr "\\n" "," | sed -e 's/ ms,/\n/g' > key_val.csv
echo "size,uniform_16,uniform_64,uniform_256,cluster_16,cluster_64,cluster_256"
# row 1
cat key_val.csv | head -18 | sed -n 'p;N;N' | cut -d ',' -f 2 | tr "\\n" "," | sed -e 's/,$/\n/' | sed -e 's/^/1000,/'  
# row 2
cat key_val.csv | head -18 | tail -17 | sed -n 'p;N;N' | cut -d ',' -f 2 | tr "\\n" "," | sed -e 's/,$/\n/' | sed -e 's/^/10000,/'  
# row 3
cat key_val.csv | head -18 | tail -16 | sed -n 'p;N;N' | cut -d ',' -f 2 | tr "\\n" "," | sed -e 's/,$/\n/' | sed -e 's/^/100000,/'  
rm query_names.txt
rm miliseconds.txt
rm key_val.csv
