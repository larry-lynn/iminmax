#ifndef IMINMAX_H
#define IMINMAX_H

#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <algorithm>

#include "../stx/btree_multimap.h"

using namespace std;

struct iminmax_data_element
{
  string label;
  vector<double> point;
  double iminmax_index;
};

struct iminmax_knn_element
{
	string label;
	double distance;
};

struct iminmax_knn_element_less {
    bool operator ()(iminmax_knn_element const& a, iminmax_knn_element const& b) const
	{
		return (a.distance < b.distance);
    }
};

template <typename _Key, typename _Data, bool _Debug>
struct btree_map_traits
{
    static const bool   selfverify = false; 
    static const bool   debug = _Debug; 
    static const int    leafslots = BTREE_MAX( 8, 256 / (sizeof(_Key) + sizeof(_Data)) ); 
    static const int    innerslots = BTREE_MAX( 8, 256 / (sizeof(_Key) + sizeof(void*)) );
};

typedef double key_type; 
typedef int value_type;

//debugging
#ifdef BTREE_DEBUG
const bool DEBUGGING = true;
#else
const bool DEBUGGING = true;
#endif

typedef stx::btree_multimap<key_type, value_type, less<key_type>,
        btree_map_traits<key_type,value_type,DEBUGGING>, allocator<pair<key_type, value_type> > > btree_mm;

class iMinMax
{

private:
    btree_mm btree;
	double theta;
    vector<iminmax_data_element> iminmax_data;
	int dimensions;
    double tree_build_time, index_build_time;

public:

    iMinMax()
	{
		theta = 0.0;
		dimensions = 0;
		tree_build_time = 0.0;
		index_build_time = 0.0;
	}
	
	iMinMax(double theta) : theta(theta) { dimensions = 0; }

    ~iMinMax() { }

    void save_tree(const char* filepath)
	{
		ofstream fout;
		fout.open(filepath, ios::out);
		btree.dump(fout);
		fout.close();
	}
	
    void load_tree(const char* filepath)
	{
		ifstream fin;
		bool success;
		fin.open(filepath, ios::in);
		success = btree.restore(fin);
		fin.close();
		if(!success){ throw 8;}
	}

	void save_data(const char* filepath)
	{
		ofstream fout;
		fout.open(filepath, ios::binary);
		int data_size = iminmax_data.size();
		int point_degree = iminmax_data[0].point.size();
		//Write the number of records and the degree of points for read purposes
		fout.write((const char*)&data_size, sizeof(data_size));
		fout.write((const char*)&point_degree, sizeof(point_degree));
		//Write theta to file
		fout.write((const char*)&theta, sizeof(theta));
		//necessary to convert to char for ofstream
		char  label_array[8];
		//write out each item individually 
		for(int i = 0; i < (int)iminmax_data.size(); i++)
		{
			iminmax_data[i].label.copy(label_array, iminmax_data[i].label.size(), 0);
			label_array[iminmax_data[i].label.size()] = '\0';
			fout.write(label_array, 8);
			//Write point coordinates
			for(int j = 0; j < (int)iminmax_data[i].point.size(); j++)
			{
				fout.write((const char*)&iminmax_data[i].point[j], sizeof(iminmax_data[i].point[j]));
			}
			fout.write((const char*)&iminmax_data[i].iminmax_index, sizeof(iminmax_data[i].iminmax_index));
		}
		fout.close();
	}
	void load_data(const char* filepath)
	{
		//clear data structure since we will pushing new data onto it
		iminmax_data.clear();
		struct iminmax_data_element element;
		ifstream fin;
		fin.open(filepath, ios::binary);
		int data_size;
		int point_degree;
		//Read the number of records and the degree of points 
		fin.read((char*)&data_size, sizeof(data_size));
		fin.read((char*)&point_degree, sizeof(point_degree));
		//Get theta
		fin.read((char*)&theta, sizeof(theta));
		element.point.resize(point_degree);

		set_dimensions(element.point.size() );

		char  label_array[8];
		//Get each item in the struct
		for(int i = 0; i < data_size; i++)
		{
			element.label = "label";
			element.iminmax_index = 0.0;
			fin.read(label_array, 8);
			element.label = label_array;
			//read point coordinates
			for(int j = 0; j < (int)element.point.size(); j++)
			{
				fin.read((char*)&element.point[j], sizeof(element.point[j]));
				// presumed for debugging, commenting out
				//cout << element.point[j] << endl;
			}
			fin.read((char*)&element.iminmax_index, sizeof(element.iminmax_index));
			iminmax_data.push_back(element);
		}
		fin.close();
	}
	void print_data()
	{
		for(int i = 0; i < (int)iminmax_data.size(); i++)
		{
			cout << iminmax_data[i].label << " : ";
			for(int j = 0; j < (int)iminmax_data[i].point.size(); j++)
			{
				cout << iminmax_data[i].point[j];
				if( (j+1) < (int)iminmax_data[i].point.size() ){
				  cout << ",";
				}
			}
			cout << " : " << iminmax_data[i].iminmax_index << endl;
		}
	}
	
	void insert_point(double key, int index)
	{
		btree.insert(key, index);
	}

	void print_tree_stats()
	{
		btree_mm::tree_stats tree_stats;
		tree_stats = btree.get_stats();
		cout << "Items:" << tree_stats.itemcount << endl;
		cout << "Nodes:" << tree_stats.nodes() << endl;
		cout << "Leaves:" << tree_stats.leaves << endl;
		cout << "InnerNodes:" << tree_stats.innernodes << endl;
		cout << "Levels:" << tree_stats.levels << endl;
		cout << "Average Fill:" << tree_stats.avgfill_leaves() << endl;
	}

	void print_tree_segments(const char* filepath)
	{
		ofstream fout;
		fout.open(filepath, ios::out);
		btree.print_tree_segments(fout);
		fout.close();
	}

	vector<iminmax_data_element> parse_csv_gen_index(string datafile)
	{
		struct iminmax_data_element element;
                int i;
		timeval start1,stop1,result1,start2,stop2,result2;

		iminmax_data = parse_csv_gen_vector(datafile);

		// parsing successful, all points same dim; set dimensions
		dimensions = (int)iminmax_data[0].point.size();

                gettimeofday(&start2, NULL); // ignoring the 2nd parameter which is the timezone
		// run the iMinMax indexing algorithm
		for(i = 0; i < (long)iminmax_data.size() ; ++i){
		  iminmax_data[i].iminmax_index = gen_iminmax_index(iminmax_data[i].point);
		}

                // now that iminmax_data has been populated, use it to populate tree
                // and time tree construction
                gettimeofday(&start1, NULL); // ignoring the 2nd parameter which is the timezone

		for(i = 0; i < (long)iminmax_data.size() ; ++i){
		  // btree.insert(key, value)
                  btree.insert(iminmax_data[i].iminmax_index, i);
                }

		gettimeofday(&stop1, NULL); // ignoring the 2nd parameter which is the timezone
		timersub(&stop1, &start1, &result1);
		//result now contains the difference between start.stop in seconds and microseconds
		tree_build_time = result1.tv_sec * 1000.0 + result1.tv_usec/1000.0; // 1000000 microseconds per second

                // end index timer & get index creation time
		gettimeofday(&stop2, NULL); // ignoring the 2nd parameter which is the timezone
		timersub(&stop2, &start2, &result2);
		index_build_time = result2.tv_sec * 1000.0 + result2.tv_usec/1000.0; // 1000000 microseconds per second

		return( iminmax_data );
	}

	double gen_iminmax_index(vector<double> point)
	{
		double minv = 1.0;
		double maxv = 0.0;
		int mind = 0;
		int maxd = 0;
		int i;

		for(i = 0; i < (long)point.size(); ++i) {
			if( point[i] < minv ) {
				minv = point[i];
				mind = i;
			}
			if( point[i] > maxv ) {
				maxv = point[i];
				maxd = i;
			}
		}

		if( (minv + theta) < (1 - maxv) ) { 
			return ( mind + minv );
		}
		else {
			return ( maxd + maxv );
		}
	}

	vector<iminmax_data_element> parse_csv_gen_query(string datafile, bool range_mode=false)
	{
		vector<iminmax_data_element> query_data(0);
		struct iminmax_data_element element;

		query_data = parse_csv_gen_vector(datafile, range_mode);
		// check that query dims match iminmax dims
		if ( dimensions != (int)query_data[0].point.size() ){
		  throw 3;
		}

		return( query_data );		
	}

	static vector<iminmax_data_element> parse_csv_gen_vector(string datafile, bool range_mode=false)
	{
		ifstream data(datafile.c_str());
		string line;
		struct iminmax_data_element element;
		vector<iminmax_data_element> csv_vector_data(0);
		int num_columns = 0;

		while(getline(data, line))
		{
			stringstream  lineStream(line);
			string        cell;
			// clean out the element structure
			element.label = "";
			element.point.clear();      
			element.iminmax_index = 0.0;

			// get the first column, which contains the point label
			// skip this step for parsing a range query
			if(!range_mode){
			  getline(lineStream, cell, ',');
			  element.label = cell.c_str();
			}

			while(getline(lineStream, cell, ','))
			{
				element.point.push_back(atof(cell.c_str()));
			} // end inner while

			// Set number of columns by first data point:
			if (num_columns == 0)
			{
				num_columns = element.point.size();
			}
			// Check for consistent number of columns:
			else if ( ((int)element.point.size() != num_columns) && ((int)element.point.size() > 0) ) 
			{
			// A point has dimensions inconsistent with others
				throw 2;
			}			
			if ( (int)element.point.size() > 0 )
			{
			// don't save any elements for a blank line
			        csv_vector_data.push_back(element);
			}

		} // end outer while
		return( csv_vector_data );
	}
	//get the median point, probably for the purpose of calculating theta 
	vector<double> get_iminmax_median()
	{
		vector<double> median_point;
		vector<double> dimension;
		//iterate through dimension in outer loop
		for(int i = 0; i < (int)iminmax_data.front().point.size(); i++)
		{
			dimension.clear();
			//get the values for the current dimension at each point
			for(int j = 0; j < (int)iminmax_data.size(); j++ )
			{
				dimension.push_back(iminmax_data.at(j).point.at(i));
			}
			//get median for current dimension and add to point
			median_point.push_back(get_median(dimension));
		}
		return median_point;
	}
	vector<double> get_iminmax_median_approximate()
	{
		vector<double> median_point;
		vector<double> dimension;
		//iterate through dimension in outer loop
		for(int i = 0; i < (int)iminmax_data.front().point.size(); i++)
		{
			dimension.clear();
			//get the values for the current dimension at each point
			for(int j = 0; j < (int)iminmax_data.size(); j++ )
			{
				dimension.push_back(iminmax_data.at(j).point.at(i));
			}
			//get approximate median for current dimension and add to point
			median_point.push_back(approximate_median_anyN(dimension));
			}
		return median_point;
	}
	double get_median(vector<double> dimensions)
	{
		double median;
		sort(dimensions.begin(), dimensions.end());
		if(dimensions.size() % 2 == 0)
		{
			median = (dimensions.at(dimensions.size()/2) + dimensions.at(dimensions.size()/2-1))/2.0;	
		}
		else
		{
			median = dimensions.at(dimensions.size()/2);
		}
		return median;
	}
	double get_theta(vector<double> median_point)
	{
		double opt_theta;
		double max = *max_element(median_point.begin(), median_point.end());
		double min = *min_element(median_point.begin(), median_point.end());
		opt_theta = 1 - max - min;
		return opt_theta;	
	}
	//Code altered from pyramid teams wiki
	//http://code.google.com/p/karthikpillai/wiki/Algorithm_Median
	//from paper web.cs.wpi.edu/~hofri/medsel.pdf
	//This function returns the approximate median of the array A[0,...,Size-1]
	double approximate_median_anyN(vector<double> A)
	{
		int Size = (int)A.size();
		bool LeftToRight=false;
		int Threshold(50);
		int Left(0),Step(1),Rem,i(0),j,k;
		double swap1,swap2;
 		while(Size>Threshold)
		{
			LeftToRight = !(LeftToRight);
			Rem = Size % 3;
			if(LeftToRight)
			{
				i = Left;
			}
			else
			{
				i=Left + (3+Rem)*Step;
			}
                
			for(int h=0; h<(Size/3)-1; h++)
			{
				//triplet_adjust
				j = i + Step;
				k = i + 2;
				if(A.at(i) < A.at(j))
				{
					if (A.at(k) < A.at(i))
					{
						swap1 = A.at(i);
						swap2 = A.at(j);
						A.at(i) = swap2;
						A.at(j) = swap1;
					}
					else if (A.at(k) < A.at(j))
					{
						swap1 = A.at(j);
						swap2 = A.at(k);
						A.at(j) = swap2;
						A.at(k) = swap1;
					}
				}
				else
				{
					if (A.at(i) < A.at(k))
					{
						swap1 = A.at(i);
						swap2 = A.at(j);
						A.at(i) = swap2;
						A.at(j) = swap1;
					}
					else if (A.at(k) > A.at(j))
					{
						swap1 = A.at(j);
						swap2 = A.at(k);
						A.at(j) = swap2;
						A.at(k) = swap1;
					}
				}
				i=i+3*Step;
			}

			if(LeftToRight)
			{
				Left = Left + Step;
			}
			else
			{
				i=Left;
				Left=Left+(1+Rem)*Step;
			}
			A = selection_sort(A,i,3+Rem,Step);
           
			if(Rem==2)
			{
				if(LeftToRight){
					swap1 = A[i+Step];
					swap2 = A[i+2*Step];
					A[i+Step]=swap2;
					A[i+2*Step]=swap1;
				}
				else
				{
					swap1 = A[i+2*Step];
					swap2 = A[i+3*Step];
					A[i+2*Step]=swap2;
					A[i+3*Step]=swap1;
				}
			}

			Step=3*Step; Size=Size/3;
		}
		A = selection_sort(A,Left,Size,Step);
		return A[Left+Step * ((Size-1)/2)];
	} 
	vector<double> selection_sort(vector<double> A, int Left, int Size, int Step)
	{
		int min;
		double swap1, swap2;
		for(int i = Left; i < Left + (Size - 1) * Step; i = i + Step)
		{
			min = i;
			for(int j = i + Step; j < Left + Size * Step; j = j + Step)
			{
				if(A.at(j) < A.at(min))
				{
					min = j;
				}
			}
			swap1 = A.at(i);
			swap2 = A.at(min);
			A.at(i)=swap2;
			A.at(min)=swap1;
		}
		return A;
	}
	/*********************************Cole's Point Query***************************************/
	vector<iminmax_data_element> point_query(vector<double> point)
	{
		struct iminmax_data_element element;
		vector<iminmax_data_element> results;
		double key = gen_iminmax_index(point);
		vector<int> candidate_indices;
		stx::CountQueryNodes("Start");
		btree_mm::iterator b_iter;
		b_iter = btree.find(key);
		bool point_match;
		//return empty results if key is not found in the btree
		if (b_iter == btree.end())
		{
			return results;
		}
		
		//look through all points with the correct key value
		while(b_iter.key() == key)
		{
			point_match = true;
			for(int i = 0; i < (int)point.size(); i++)
			{
				//retrieve the point value from data structure
				if(iminmax_data[b_iter.data()].point[i] != point[i])
				{
					point_match = false;
					break;
				}
			}
			//add the point to the results if it is a match
			if(point_match)
			{
				results.push_back(iminmax_data[b_iter.data()]);
			}
			b_iter++;
		}
		cout << "Nodes accessed for point query: " << stx::CountQueryNodes("Get") << endl;	
	
		return results;
	}
	/*********************************Cole's Point Query***************************************/
	
	// Returns a vector of point IDs found within a given range query:
	vector<string> range_query(vector<double> lowerCorner, vector<double> upperCorner)
	{
		// Points must have the same number of dimensions:
		if (lowerCorner.size() != upperCorner.size())
		{
			// Mismatched range points:
			throw 1;
		}

		// Number of dimensions:
		int d = (int)lowerCorner.size();
		
		if (d != dimensions)
		{
			// Range dimensions don't match data's dimensions:
			throw 3;
		}
		
		double minLow = 1.0;
		double maxLow = 0.0;
		double minHigh = 1.0;
		double maxHigh = 0.0;
		
		// Rearrange range values to go from "lower left" to "upper right":
		for(int i = 0; i < d; i++)
		{
			// If going from high to low instead of low to high:
			if (lowerCorner[i] > upperCorner[i])
			{
				// Swap ith cells:
				double temp = upperCorner[i];
				upperCorner[i] = lowerCorner[i];
				lowerCorner[i] = temp;
			}
			// Find the max of the lower bound:
			if (lowerCorner[i] > maxLow)
			{
				maxLow = lowerCorner[i];
			}
			// Find the min of the lower bound:
			if (lowerCorner[i] < minLow)
			{
				minLow = lowerCorner[i];
			}
			// Find the max of the upper bound:
			if (upperCorner[i] > maxHigh)
			{
				maxHigh = upperCorner[i];
			}
			// Find the min of the upper bound:
			if (upperCorner[i] < minHigh)
			{
				minHigh = upperCorner[i];
			}
		}
		
		stx::CountQueryNodes("Start");
		
		// Vector of points that are within the range:
		vector<string> resultSet;
		
		// For Theorem #2:
		bool maxEdge = ((minLow + theta) >= (1 - maxLow));
		bool minEdge = ((minHigh + theta) < (1 - maxHigh));
		
		// Perform d subqueries:
		for(int i = 0; i < d; i++)
		{
			// Theorem #2:
			if ((maxEdge && (upperCorner[i] < maxLow)) ||
			    (minEdge && (lowerCorner[i] > minHigh)))
			{
				//cout << "Evoking theorem #2 - skipping subquery " << i << endl;
				continue;
			}
			
			// Get candidate set for this ith subdimension:
			vector<iminmax_data_element> candidateSet;

			// Equation 1:
			if (maxEdge)
			{
				// i is the integer part, lower/upperCorner is the decimal part:
				candidateSet = range_subquery(i + maxLow, i + upperCorner[i]);
			}
			else if (minEdge)
			{
				// i is the integer part, lower/upperCorner is the decimal part:
				candidateSet = range_subquery(i + lowerCorner[i], i + minHigh);
			}			
			else
			{
				// i is the integer part, lower/upperCorner is the decimal part:
				candidateSet = range_subquery(i + lowerCorner[i], i + upperCorner[i]);
			}			
			
			// Test candidate points:
			for(long j = 0; j < (long)candidateSet.size(); j++)
			{
				// Iterate through the dimensions
				for(int dim = 0; dim < d; dim++)
				{
					// If point's dim is lower than the range's lower bound or higher than the range's higher bound,
					// remove from candidate set:
					if (candidateSet[j].point[dim] < lowerCorner[dim] || candidateSet[j].point[dim] > upperCorner[dim])
					{
						// Remove candidate point from candidate set:
						candidateSet.erase(candidateSet.begin() + j);
						j--;
						// This point has been removed, move on to next point:
						break;
					}
				}
			}
			
			// Move the refined candidate set to the result set:
			for (long j = 0; j < (long)candidateSet.size(); j++)
			{
				resultSet.push_back(candidateSet[j].label);
			}
		}
		
		cout << "Nodes accessed by range query: " << stx::CountQueryNodes("Get") << endl;
		
		// Compare results to sequential scan:
		/*
		vector<string> seqIds = sequential_range_query(lowerCorner, upperCorner);
		cout << "Sequential scan IDs: " << endl;
		for(long i = 0; i < (long)seqIds.size(); i++)
		{
			cout << "  " << seqIds[i] << endl;
		}
		*/
		
		// Return the result set:
		return resultSet;
	}

	// Returns a list of k points ordered by distance from the query point:
	vector<iminmax_knn_element> knn_query(vector<double> point, int k)
	{
		if ((int)point.size() != dimensions)
		{
			throw 3;
		}
	
		// The IDs and distances of the K nearest neighbors to the point:
		vector<iminmax_knn_element> resultSet;
		
		// Find the index of the point:
		double point_index = gen_iminmax_index(point);
		// Find the partition of the point:
		double partition = (int)point_index;
		
		stx::CountQueryNodes("Start");
		
		// Initiliazer iterator to walk the leaves:
		btree_mm::iterator bi_right;
		btree_mm::iterator bi_left;
		
		// Start iterators at leaf with point's index:
		bi_right = btree.lower_bound(point_index);
		bi_left = btree.lower_bound(point_index);
		// Otherwise bi_right and bi_left are referencing the same point:
		bi_right++;
		
		// Continue until the end of the point's partition or found k candidates:
		while(bi_right != btree.end() && bi_right.key() < partition + 1 &&
			  bi_left != btree.begin() && bi_left.key() > partition)
		{
			// Consider right iterator:
			if (bi_right != btree.end() && bi_right.key() < partition + 1)
			{
				// Add points to candidate set:
				//cout << "Right: " << bi_right.key() << ", " << bi_right.data() << endl;
				refine_knn_query(resultSet, iminmax_data[bi_right.data()], point, k);
			
				// Move right:
				bi_right++;
			}

			// Consider right iterator:
			if (bi_left != btree.begin() && bi_left.key() >= partition) 
			{
				// Add points to candidate set:
				//cout << "Left: " << bi_left.key() << ", " << bi_left.data() << endl;
				refine_knn_query(resultSet, iminmax_data[bi_left.data()], point, k);
			
				// Move iterator left:
				bi_left--;
			}

			// Break out if found k points:
			if ((long)resultSet.size() >= k) { break; }
		}
		
		// TODO: Order the dimensions to test more intelligently?
		for(int i = 0; i < dimensions; i++)
		{
			// Already searched the partition of the center point:
			if (i == partition) { continue; }

			// Update the range needed to search for the subqueries:
			double dMax;
			double min;
			double max;
            if ( (long)resultSet.size() == 0) {
				// degenerate case; found no neighbors in first partition
				dMax = 0.5;
				min = i;
				max = i + 1;
			}
			else {
				dMax = resultSet[(int)resultSet.size() - 1].distance;
				
				double minLow = 1.0;
				double maxLow = 0.0;
				double minHigh = 1.0;
				double maxHigh = 0.0;
				
				// Used for Theorem 2 and Cases 1 & 2 of Equation 1:
				for (int j = 0; j < dimensions; j++)
				{
					// Find min and max in each dimension:
					double lowerBound = (point[j] - dMax > 0.0) ? (point[j] - dMax) : 0.0;
					double upperBound = (point[j] + dMax < 1.0) ? (point[j] + dMax) : 1.0;

					// Find the max of the lower bound:
					if (lowerBound > maxLow)
					{
						maxLow = lowerBound;
					}
					// Find the min of the lower bound:
					if (lowerBound < minLow)
					{
						minLow = lowerBound;
					}
					// Find the max of the upper bound:
					if (upperBound > maxHigh)
					{
						maxHigh = upperBound;
					}
					// Find the min of the upper bound:
					if (upperBound < minHigh)
					{
						minHigh = upperBound;
					}
				}
				
				min = (point[i] - dMax > 0.0) ? (point[i] - dMax) : 0.0;
				max = (point[i] + dMax < 1.0) ? (point[i] + dMax) : 1.0;

				// For Theorem #2:
				bool maxEdge = ((minLow + theta) >= (1 - maxLow));
				bool minEdge = ((minHigh + theta) < (1 - maxHigh));
		
				// Theorem #2:
				if ((maxEdge && (max < maxLow)) ||
					(minEdge && (min > minHigh)))
				{
					//cout << "Evoking theorem #2 - skipping subquery " << i << endl;
					continue;
				}

				// Equation 1:
				if (maxEdge)
				{
					// i is the integer part, lower/upperCorner is the decimal part:
					//candidateSet = range_subquery(i + maxLow, i + upperCorner[i]);
					min = i + maxLow;
					max = i + max;
					
				}
				else if (minEdge)
				{
					// i is the integer part, lower/upperCorner is the decimal part:
					//candidateSet = range_subquery(i + lowerCorner[i], i + minHigh);
					min = i + min;
					max = i + minHigh;
				}			
				else
				{
					// i is the integer part, lower/upperCorner is the decimal part:
					//candidateSet = range_subquery(i + lowerCorner[i], i + upperCorner[i]);
					min = i + min;
					max = i + max;
				}			
			}
			//cout << i << ": [" << min << "," << max << "]" << endl;

			// Find points in the range subquery:
			vector<iminmax_data_element> candidateSet = range_subquery(min, max);
			// Refine the candidates from the subquery:
			for(long j = 0; j < (long)candidateSet.size(); j++)
			{
				refine_knn_query(resultSet, candidateSet[j], point, k);
			}
		}

		cout << "Nodes accessed for KNN query: " << stx::CountQueryNodes("Get") << endl;
		
		return resultSet;
	}

	//Altered Code from IDistance team's wiki
	//http://code.google.com/p/idistance/wiki/SequentialScan
	//return ids for a given point location
	vector<iminmax_data_element> sequential_point_query(vector<double>p)
	{
		bool equal;
		vector<iminmax_data_element> ret_points;
		//int index;
		for(int i = 0; i < (int)iminmax_data.size(); i++)
		{
		  equal = true;
		  for(int j = 0; j < (int)iminmax_data[i].point.size(); j++)
		  {
			//if all dimensions are equal, the points are equal
			if(iminmax_data[i].point[j] != p[j])
				equal = false;
		  }
		  if(equal == true)
		  {
			  //add point id to return list
			  ret_points.push_back(iminmax_data[i]);
		  }
		}
		return ret_points;
	}


	//return anything in the range between low_range, high_range 
	vector<string> sequential_range_query(vector<double> lowerCorner, vector<double> upperCorner)
	{
		bool inrange;
		vector<string> ret_points;
		for(int i = 0; i < (int)iminmax_data.size(); i++)
		{
		  inrange = true;
		  for(int j = 0; j < (int)iminmax_data[i].point.size(); j++)
		  {
			//if > low range and < high range, it's in range
			if(iminmax_data[i].point[j] < lowerCorner[j] || iminmax_data[i].point[j]  > upperCorner[j])
				inrange = false;
		  }
		  if(inrange == true)
		  {
			  //add point id to return list
			  ret_points.push_back(iminmax_data[i].label);
		  }
		}
		return ret_points;
	}


	//keep an ordered list from smallest to largest of neighbors
	vector<iminmax_knn_element> sequential_knn_query(vector<double> p, int num)
	{
		vector<int> knn;            //list of knn indices
		vector<double> knn_dist;    //distance of those knn indices
		vector<iminmax_knn_element> knn_return;
		iminmax_knn_element knn_element;
		double current_dist;
		double sum;
		int k;
		for(int i = 0; i < (int)iminmax_data.size(); i++)
		{
			//calculate distance
			sum = 0;
			for(int j = 0; j < (int)iminmax_data[i].point.size(); j++)
			{
				sum += pow(iminmax_data[i].point[j] - p[j], 2);
			}
			current_dist = sqrt(sum);
			if(!knn_dist.empty())
			{   //if it needs to be inserted before last element
				if(current_dist < knn_dist.back())
				{
					//finding the right insert spot
					for(k=knn_dist.size()-1; current_dist < knn_dist.at(k) && k>0; k--)
					{}
					//if k=0 we need to know if we insert at 0 or 1
					if(k==0 && current_dist < knn_dist.at(0))
					{
						knn.insert(knn.begin(), i);
						knn_dist.insert(knn_dist.begin(), current_dist);
					}
					else
					{
						//insert at one greater than k
						knn.insert(knn.begin()+k+1, i);
						knn_dist.insert(knn_dist.begin()+k+1, current_dist);
					}
					//if we now have num+1 neighbors
					if((int)knn.size() > num)
					{
						knn.pop_back();
						knn_dist.pop_back();
					}
				}
				else
				{   //if we're still less than how many neighbors we want,
					//we can just add it to the end
					if((int)knn.size() < num)
					{
						knn.push_back(i);
						knn_dist.push_back(current_dist);
					}
				}
			}
			else
			{   //add to empty list
				knn.push_back(i);
				knn_dist.push_back(current_dist);
			}
		}

		for(int i = 0; i < (int)knn.size(); i++)
		{
			knn_element.label = iminmax_data[knn[i]].label;
			knn_element.distance = knn_dist[i];
			knn_return.push_back(knn_element);
		}
		return knn_return;
	}

        // Larry's time accessors
	double get_tree_build_time()
	{
	  return tree_build_time; 
	}

	double get_index_build_time()
	{
	  return index_build_time; 
	}
	// Larry's getters and setters
	int get_dimensions()
	{
	  return dimensions;
	}
	void set_dimensions(int i)
	{
	  dimensions = i;
	}

private:

	// Finds all key values between min and max
	vector<iminmax_data_element> range_subquery(double min, double max)
	{
		// Initialize candidate set:
		vector<iminmax_data_element> candidateSet;
		
		// Initiliazer iterator to walk the leaves:
		btree_mm::iterator bi;
		
		// Start iterator at leaf with lower bound:
		bi = btree.lower_bound(min);
		
		// Continue until over upper bound:
		while(bi != btree.end() && bi.key() <= max)
		{
			// Add points to candidate set:
			candidateSet.push_back(iminmax_data[bi.data()]);
			
			// Continue to next point:
			bi++;
		}
		
		// Return candidate set (to be refined):
		return candidateSet;
	}

	// Returns the Euclidean distance between two points:
	double calculate_distance(vector<double> point, vector<double> centerPoint)
	{
		// Check to make sure dimensions of points match:
		if (point.size() != centerPoint.size())
		{
			throw 4;
		}
		
		// Calculate Euclidean distance:
		double sum = 0.0;		
		for(int i = 0; i < (int)point.size(); i++)
		{
			sum += ((point[i] - centerPoint[i]) * (point[i] - centerPoint[i]));
		}
		
		return sqrt(sum);
	}
	
	// Update the candidate set:
	void refine_knn_query(vector<iminmax_knn_element> &candidateSet, iminmax_data_element testPoint, vector<double> centerPoint, int k)
	{
		iminmax_knn_element element;
		
		// If less than k in the candidate set, automatically add point:
		if ((int)candidateSet.size() < k)
		{
			element.label = testPoint.label;
			element.distance = calculate_distance(testPoint.point, centerPoint);
			candidateSet.push_back(element);
		}
		else
		{
			// Calculate distance between center point and test point:
			double distance = calculate_distance(testPoint.point, centerPoint);
			
			//cout << "Testing " << candidateSet[k - 1].label << " against " << testPoint.label << endl;
			
			// Is the point closer than the current farthest?
			if (distance < candidateSet[k - 1].distance)
			{				
				// Remove current farthest:
				candidateSet.pop_back();
				
				// Replace with closer point:
				iminmax_knn_element element;
				element.label = testPoint.label;
				element.distance = distance;
				candidateSet.push_back(element);
			}
		}

		// Sort to keep farthest point at the back:
		sort(candidateSet.begin(), candidateSet.end(), iminmax_knn_element_less());

		return;
	}
};

#endif
