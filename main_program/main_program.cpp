#include <iostream>
#include <vector>
//Include for timing functions
#include <sys/time.h>

#include "includes/getopt_pp/getopt_pp.h"
#include "includes/iminmax/iminmax.h"

using namespace GetOpt;

double get_avg_query_time(vector<double>);
void usage();

double get_avg_query_time(vector<double> query_times)
{
	double total_query_time = 0;
	for(int i = 0; i <  (int)query_times.size(); i++)
	{
		total_query_time += query_times[i];
	}
	return total_query_time / query_times.size();
}

int main(int argc, char* argv[])
{
    std::string qp;
    std::string qr;
    std::string qn;
    std::string datafile;
    std::string savetf;
    std::string loadtf;
    int knn;
    double theta;
    bool b = false;
    bool v = false;
    bool s = false;
    bool p = false;
    int i, j;
	//variables for tracking query time
	vector<double> query_times;
	timeval start, stop, result;
	double avg_time;

    vector<iminmax_data_element> iminmax_data;

    try {
 
        GetOpt_pp ops(argc, argv);

        ops >> Option("qp", qp, "unset"); // default "unset"
        ops >> Option("qr", qr, "unset"); // default "unset"
        ops >> Option("qn", qn, "unset"); // default "unset"
        ops >> Option("datafile", datafile, "unset"); // default "unset"
        ops >> Option("savetf", savetf, "unset"); // default "unset"
        ops >> Option("loadtf", loadtf, "unset"); // default "unset"
        ops >> Option("knn", knn, 0); // default 0
        ops >> Option("theta", theta, 0.0); // default 0.0
        ops >> OptionPresent('b', b);
        ops >> OptionPresent('v', v);
        ops >> OptionPresent('p', p);
        ops >> OptionPresent('s', s);

        iMinMax iminmax(theta);

        // MODE 1: parse input file, generate indices, construct tree & save it
	if(b){
	  // validate correct input combinations for tree building
	  if( (datafile.compare("unset") == 0) || (savetf.compare("unset")) == 0){
	      throw 6;
	  }
	    iminmax_data = iminmax.parse_csv_gen_index(datafile);
	    string tree_data_file = savetf;
    	    string iminmax_data_file = savetf;
	    tree_data_file.append(".tree");
	    iminmax_data_file.append(".data");


	    std::cout << "Tree Build Time: " << iminmax.get_tree_build_time() << " ms" << std::endl;
	    std::cout << "Index Build Time [includes tree build time]: " 
		    << iminmax.get_index_build_time() << " ms" << std::endl;
	    std::cout << "Tree statistics: " << std::endl;
	    iminmax.print_tree_stats();
	    std::cout << "Tree segments found in treeinfo.txt file" << std::endl;
	    iminmax.print_tree_segments("treeinfo.txt");
	    std::cout << std::endl;

	    iminmax.save_tree( tree_data_file.c_str() );
	    iminmax.save_data( iminmax_data_file.c_str() );
	}

	// MODE 2: point query
	else if(qp.compare("unset") != 0){
	  if( (loadtf.compare("unset")) == 0){
	      throw 7;
	  }
	  string tree_data_file = loadtf;
	  string iminmax_data_file = loadtf;
	  vector<iminmax_data_element> results;
  	  vector<iminmax_data_element> query;
	  tree_data_file.append(".tree");
	  iminmax_data_file.append(".data");
	  
          // cout << "tree data file name = " << tree_data_file << endl;
          // cout << "iminmax_data_file name = " << iminmax_data_file << endl;

	  iminmax.load_tree( tree_data_file.c_str() );
	  iminmax.load_data( iminmax_data_file.c_str() );
      
	  // cout << "iminmax dimensions: " << iminmax.get_dimensions() << endl;

	  // iminmax.print_tree_stats();
	  // iminmax.print_data();
	  // iminmax.print_tree_segments("treeinfo-loaded.txt");

	  cout << "Perform point queries" << endl;
	  query = iminmax.parse_csv_gen_query(qp);
		
	  for(i = 0; i < (int)query.size(); ++i){
	    cout << "Perform Query: " << (i + 1) << endl;
		//start timer for this query
		gettimeofday(&start, NULL);
	    results = iminmax.point_query(query[i].point);
		//stop timer
		gettimeofday(&stop, NULL); // ignoring the 2nd parameter which is the timezone
		// get time
		timersub(&stop, &start, &result);
		//push time into vector
		query_times.push_back(result.tv_sec * 1000.0 + result.tv_usec/1000.0);

	    cout << "Found: " << (long)results.size() << endl;
	    for(long j = 0; j < (long)results.size(); ++j)
	      {
		cout << "ID: " << results[j].label << endl;
	      }
		cout << "Query Time: " << query_times[i] << " ms" << endl;
	  }
	  avg_time = get_avg_query_time(query_times);
	  cout << "Average Query Time: " << avg_time << " ms" << endl;
	  cout << endl;
	}

	// MODE 3: range query
	else if(qr.compare("unset") != 0){
	  if( (loadtf.compare("unset")) == 0){
	      throw 9;
	  }
	  string tree_data_file = loadtf;
	  string iminmax_data_file = loadtf;
  	  vector<iminmax_data_element> query;
	  vector<string> results;
	  struct iminmax_data_element lowerCorner;
	  struct iminmax_data_element upperCorner;
	  bool range_mode = true;

	  // load tree and iminmax_data
	  tree_data_file.append(".tree");
	  iminmax_data_file.append(".data");
	  iminmax.load_tree( tree_data_file.c_str() );
	  iminmax.load_data( iminmax_data_file.c_str() );

	  query = iminmax.parse_csv_gen_query(qr, range_mode);
	  // a properly formatted range query must have an even num rows
          if( ( ( (int)query.size() ) % 2) == 1){
	    throw 5;
	  }
	
	  for(i = 0; i < (int)query.size(); i = i + 2){
	    cout << "Perform Query: " << ( (i/2) + 1) << endl;
	    lowerCorner = query[i];
	    upperCorner = query[i+1];
		//start timer for this query
		gettimeofday(&start, NULL);
		//get results for query
	    results = iminmax.range_query(lowerCorner.point, upperCorner.point);
		//stop timer
		gettimeofday(&stop, NULL); // ignoring the 2nd parameter which is the timezone
		// get time
		timersub(&stop, &start, &result);
		//push time into vector
		query_times.push_back(result.tv_sec * 1000.0 + result.tv_usec/1000.0);

	    cout << "Found: " << (long)results.size() << endl;
	    for(long j = 0; j < (long)results.size(); ++j)
	      {
		cout << "ID: " << results[j] << endl;
	      }
	      cout << "Query Time: " << query_times.back() << " ms" << endl;
	  }
	  avg_time = get_avg_query_time(query_times);
	  cout << "Average Query Time: " << avg_time << " ms" << endl;
	  cout << endl;      
	}

	// MODE 4: KNN query
	else if(qn.compare("unset") != 0){
	  if( ((loadtf.compare("unset")) == 0) ||  (knn <= 0) ){
	      throw 9;
	  }
	  string tree_data_file = loadtf;
	  string iminmax_data_file = loadtf;
  	  vector<iminmax_data_element> query;
	  struct iminmax_data_element center;
	  vector<iminmax_knn_element> resultSet;

	  // load tree and iminmax_data
	  tree_data_file.append(".tree");
	  iminmax_data_file.append(".data");
	  iminmax.load_tree( tree_data_file.c_str() );
	  iminmax.load_data( iminmax_data_file.c_str() );

	  query = iminmax.parse_csv_gen_query(qn);
		
	  for(i = 0; i < (int)query.size(); ++i){
	    cout << "Perform Query: " << (i + 1) << endl;
	    center = query[i];
		//start timer for this query
		gettimeofday(&start, NULL);
		//get results for query
	    resultSet = iminmax.knn_query(center.point, knn);
		//stop timer
		gettimeofday(&stop, NULL); // ignoring the 2nd parameter which is the timezone
		// get time
		timersub(&stop, &start, &result);
		//push time into vector
		query_times.push_back(result.tv_sec * 1000.0 + result.tv_usec/1000.0);

	    cout << "Results:" << endl;

	    for(long j = 0; j < (long)resultSet.size(); ++j)
	      {
		cout << "Neighbor: " << resultSet[j].label << ";\t";
		cout << "Distance: " << resultSet[j].distance << endl;
	      }
		cout << "Query Time: " << query_times[i] << " ms" << endl;
	  }
	  avg_time = get_avg_query_time(query_times);
	  cout << "Average Query Time: " << avg_time << " ms" << endl;
	  cout << endl;

	}
	//MODE 5: prepocess data and display optimal theta based on median point
	else if(p)
	{
		if((loadtf.compare("unset")) == 0)
		{
			throw 10;
		}
		string tree_data_file = loadtf;
		string iminmax_data_file = loadtf;
		tree_data_file.append(".tree");
		iminmax_data_file.append(".data");
		iminmax.load_tree( tree_data_file.c_str() );
		iminmax.load_data( iminmax_data_file.c_str() );
		cout << "Optimal Theta (exact median): " << iminmax.get_theta(iminmax.get_iminmax_median()) << endl;
		cout << "Optimal Theta (approximate median): " << iminmax.get_theta(iminmax.get_iminmax_median_approximate()) << endl;
	}

	else {
	  usage();
	}

        // Verbose mode -- print more output to stdout
        if(v){
          std::cout << "qp = " << qp << std::endl;
          std::cout << "qr = " << qr << std::endl;
          std::cout << "qn = " << qn << std::endl;
          std::cout << "knn = " << knn << std::endl;
          std::cout << "theta = " << theta << std::endl;
          std::cout << "datafile = " << datafile << std::endl;
          std::cout << "savetf = " << savetf << std::endl;
          std::cout << "loadtf = " << loadtf << std::endl;
          if(b){ std::cout << "b flag is set" << std::endl; }
          else{ std::cout << "b flag is not set" << std::endl; }
          if(s){ std::cout << "s flag is set" << std::endl; }
          else{ std::cout << "s flag is not set" << std::endl; }
          if(p){ std::cout << "p flag is set" << std::endl; }
          else{ std::cout << "p flag is not set" << std::endl; }


          // verbose indexing output
         for(i = 0; i < (long)iminmax_data.size() ; ++i){
           std::cout << "id = " << iminmax_data[i].label << "\n";      

           std::cout << "point = ";
           for(j = 0; j < (long)iminmax_data[i].point.size(); ++j){
             std::cout << iminmax_data[i].point[j] << ", ";
           } 
  
           std::cout << "\niMinMax index = " << iminmax_data[i].iminmax_index << "\n";
           std::cout << "\n\n";
         }
       } // End verbose output

	/* Example iminmax tree functions:
	iminmax.insert_point(1.1, 3);
	iminmax.save_tree("btree.bin");
	iminmax.print_tree_stat();
	iminmax.load_tree("btree.bin");
	iminmax.print_tree_stat();
	*/

	}
	catch (int e)
	{
		cout << "Exception #"  << e << ":" << endl;
		switch(e) {
			case 1:
				cout << "The corner points for the range query had different dimensions!" << endl;
				break;
			case 2:
				cout << "The dimensions of a point did not match the rest of the data!" << endl;
				break;
			case 3:
				cout << "The dimensions of the query did not match the dimensions of the data!" << endl;
				break;
			case 4:
				cout << "The dimensions of two points did not match while trying to calculate distance." << endl;
				break;
	                case 5:
		                cout << "The range query file did not contain an even number of rows" << endl;
		                break;
			case 6:
				cout << "Illegal arg combination for tree build; need -b, --datafile and --savetf" << endl;
				break;
			case 7:
				cout << "Illegal arg combination for point query; need --qp and --loadtf" << endl;
				break;
			case 8:
				cout << "Unable to load B+ tree from file" << endl;
				break;
			case 9:
				cout << "Illegal arg combination for range query; need --qr and --loadtf" << endl;
				break;
			case 10:
				cout << "Illegal arg combination for preprocessing; need -p and --loadtf" << endl;
				break;


			default:
				cout << "Unknown error occurred!" << endl;				
				break;
		}
		cout << "Exiting Abnormally" << endl;
		exit(1);
	}

        // End Liessman's additions 

    return 0;
}

void usage () {
  string line;
  ifstream usage_file ("readme/usage.txt");
  if (usage_file.is_open())
    {
      while ( usage_file.good() )
	{
	  getline (usage_file,line);
	  cout << line << endl;
	}
      usage_file.close();
    }

  else cout << "Unable to open usage file"; 

}
