Or, you'll never guess what happened on the way to the presentation

CSV Parsing

Many scripting languages such as PHP and Python have built in or widely adopted libraries for reading and parsing CSV files. C++ has appears to have no such native libraries.

After some searching, our best options appeared to be to use Boost libraries or to take a 'Roll your Own' approach. Parsing CSV files can be surprisingly complicate due to an abundance of optional alternate encodings, multiple methods for handling line terminations and multiple standards for handling escape characters and literal representation of escape characters. However, since our input data, is very regular, we decided to go with the simplest thing that could possibly work and Roll our Own. This helps reduce compillation dependencies and contributes to program portability.

Early parsing code resembled this ``` std::vector parse_csv_gen_index(std::string datafile) { std::ifstream data(datafile.c_str()); std::string line; struct iminmax_data_element element; std::vector iminmax_data(0); int i;

while(std::getline(data,line)) { std::stringstream lineStream(line); std::string cell; // clean out the element structure element.label = ""; element.point.clear(); element.iminmax_index = 0.0;

  // get the first column, which contains the ID
  std::getline(lineStream,cell,',');
  element.label = cell.c_str();
  std::cout << "id = " << element.label << "\n";

  while(std::getline(lineStream,cell,','))
    {
      element.point.push_back(atof(cell.c_str()));
    } // end inner while
  // run the iMinMax indexing algorithm
  element.iminmax_index = gen_iminmax_index(element.point);
  iminmax_data.push_back(element);

} // end outer while
return( iminmax_data ); }

```

Irregularity of input data

Some of the input data files had the terminating newline character cut off. Others contained an extra empty line at the end of the file. Our home baked CSV parser was a bit brittle and seg faulted on some and threw dimensional mismatch errors on others.

It was the dark side of the 'roll your own' approach and it cost us some development time making the parser more robust.

Serialization of iminmax_data

We encountered difficulties in correctly serializing and saving our secondary data structure iminmax_data. Attempts to use the most direct and obvious object serialization methods resulted on segmentation faults when we attempted to re-load the data. The problem was related to the way strings representing point labels were stored and the null characters terminating these strings.

The solution involve more complex save and load methods that preserved additional meta-data about size and dimensionality of the data

Serialization and re-compillation

Object serialization introduced extra complexity and an extra point of failure. One pitfall that we encountered was if the code was changed and recompiled, it implied that the tree needed to be rebuilt. Code changes could potentially mean structural changes to the tree or iminmax_data. If the compiled binary and the serialized data were out of sync, undefined behavior followed along with difficult to track down bugs.

Unreliable success modes

Our choices of data structures made it very important to keep iminmax_data and the tree tightly coupled. A mistake we made was storing the point label in the tree rather than the index into iminmax_data where that point was stored. Usually, the label and the index were one and the same, but on some of the Real World Datasets used an off-by-one labeling the cascaded into a segmentation error.

This won't always work: for(i = 0; i < (long)iminmax_data.size() ; ++i){ btree.insert(iminmax_data[i].iminmax_index, atoi(iminmax_data[i].label.c_str())); }

This is what we need: for(i = 0; i < (long)iminmax_data.size() ; ++i){ btree.insert(iminmax_data[i].iminmax_index, i); }

Rare Corner Cases

The iMinMax KNN algorithm attempts to establish a candidate set in the partition of the central search point, than improve on it by using a decreasing radius search strategy. In most cases, we had no shortage of first partition candidates. But in a couple of the clustered data sets, the original search point was in an empty partition & no original candidates were. The code implicitly assumed that we had at least 1 candidate, and on this assumption, indexed into the candidate set to obtain the radius of the decreasing search space.

But with no candidates, there was no radius, just a seg fault

