thetas="-1.0 -0.9 -0.8 -0.7 -0.6 -0.5 -0.4 -0.3 -0.2 -0.1 0.0 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0"
echo "Changing directory to that of main program"
cd ..
for theta in $thetas
do
  #echo $theta
  echo "Building tree, clustered data, theta = $theta"
  ./main_program -b --datafile ../../csci540_data/iminmax_team_data/256Dimension1cluster10000Points.csv --savetf cache --theta $theta
  echo "Running range query with theta = $theta"
  ./main_program --loadtf cache --qr queries/variable_theta_queries/variable_theta_range1.csv --theta $theta
  echo ">>>>>>>>>>>>>>>>>>>>>>>>"
  rm cache*
done