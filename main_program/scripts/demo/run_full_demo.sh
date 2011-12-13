bash short_build.sh
bash point_query.sh 
bash range_query.sh  
bash knn_query.sh 
bash knn_query_sequential.sh 

# Extensions
bash build_skewed_distribution_0_theta.sh
bash range_query_skewed_distribution_0_theta.sh | grep -v ID

bash precalculate_theta_skewed.sh 
bash build_skewed_distribution_0.6003_theta.sh
bash range_query_skewed_distribution_0.6003_theta.sh | grep -v ID
#bash precalculate_theta_clustered.sh  