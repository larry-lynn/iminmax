echo "Changing working directory to that of main program"
cd ..

distributions="uniform cluster Real"
dimensions="16 64 256"
path_to_data="../../csci540_data"
path_to_queries="queries"

for distribution in $distributions
do
  #echo $distribution
  for dimension in $dimensions
  do
    #echo $distribution $dimension
    if [ $distribution == "Real" ]
      then
      sizes="160 1600 16000"
    else
      sizes="1000 10000 100000"   
    fi

    for size in $sizes
    do
      #data_filename_format=""
      #echo $distribution $dimension $size
      if [ $distribution == "uniform" ]
         then
         data_filename_format="uniform_$dimension*$size.txt"
         query_file_format="${distribution}_${dimension}_${size}_"
      elif [ $distribution == "cluster" ]
         then
         data_filename_format="${dimension}Dimension10cluster${size}Points.csv"
         query_file_format="${distribution}_${dimension}_${size}_"
         #echo $data_filename_format
      else
         # distribution = Real
         data_filename_format="Real-${dimension}-${size}-random.txt"
         query_file_format="${distribution}-${dimension}-${size}-"
         #echo $data_filename_format
      fi 
      data_file=`find $path_to_data -name "$data_filename_format"`
      #echo $data_file
      qp_file=`find $path_to_queries -name "${query_file_format}qp.txt"`
      qrn_file=`find $path_to_queries -name "${query_file_format}qrn.txt"`
      qrw_file=`find $path_to_queries -name "${query_file_format}qrw.txt"`
      qn_file=`find $path_to_queries -name "${query_file_format}qn.txt"`
      #echo $qp_file
      #echo $qrn_file
      #echo $qrw_file
      #echo $qn_file
      #create index, build tree
      echo "PROCESSING INPUT FILE $data_file"
      echo ">>>>>>>>>>>>>>>>>"
      ./main_program -b --datafile $data_file --savetf cache

      #point query
      echo "PERFORMING POINT QUERY $qp_file"
      echo ">>>>>>>>>>>>>>>>>"
      ./main_program --loadtf cache --qp $qp_file

      #range queries
      echo "PERFORMING NARROW RANGE QUERY $qrn_file"
      echo ">>>>>>>>>>>>>>>>>"
      ./main_program --loadtf cache --qr $qrn_file 
      echo "PERFORMING WIDE RANGE QUERY $qrw_file"
      echo ">>>>>>>>>>>>>>>>>"
      ./main_program --loadtf cache --qr $qrw_file 

      #KNN query
      echo "PERFORMING KNN QUERY $qn_file"
      echo ">>>>>>>>>>>>>>>>>"
      ./main_program --loadtf cache --qn $qn_file --knn 3

      rm cache.tree
      rm cache.data

      echo "##################"
      echo " "

    done
  done
done