## Index algorithm and tree build stats

![Index and Tree Build Times](/images/index_tree_creation_results.png)

Processing the input data is linearly related to both dimensionality and 
data size. Note that these results are for CPU time only. 
They do not include either reading the CSV data file or writing the 
serialized binary data structures.

The IO costs are considerable, but they are beyond the scope of what 
we are trying to measure for this experiment. For reference purposes, 
the output below demonstrates how 
IO dominates CPU time by an order of magnitude

```bash 
time ./main_program -b --datafile ../../csci540_data/256Dimension10cluster100000Points.csv --savetf cache 
Tree Build Time: 183.564 ms 
Index Build Time [includes tree build time]: 2133.41 ms 
Tree statistics: 
Items:100000 
Nodes:7261 
Leaves:6776 
InnerNodes:485 
Levels:5 
Average Fill:0.70276 
Tree segments found in treeinfo.txt file

real 0m45.758s user 0m42.827s sys 0m2.328s 
```

## Post Indexing Query Results

Unless otherwise indicated, all of these results were obtained with the 
tunable parameter theta set to 0 both during tree build and during 
query execution. The theta used during build and querying must 
match or the algorithms may produce inaccurate results.

## Point Query Stats

![Point Query Clustered](/images/point_uniform_clustered.png)

(note: uniform & clustered performance is very similar and the results are averaged together. Also, times are graphed in terms of time saved over sequential rather than absolute times)

The iMinMax algorithm is highly efficient for point queries. It consistently beats a sequential scan and the savings in terms of time improve dramatically as both the data dimensionality and size increases.

### iMinMax Point Query

```bash
./main_program --loadtf cache --qp queries/cluster_queries/cluster_256_100000_qp.txt 
Perform point queries 
Perform Query: 1 
Nodes accessed for point query: 5 
Found: 1 ID: 50 
Query Time: 0.09 ms 
Perform Query: 2 
Nodes accessed for point query: 5 
Found: 1 ID: 51 
Query Time: 0.074 ms 
Average Query Time: 0.082 ms
```

The number of nodes accessed is low, usually matching the height of 
the tree. Query times are correspondingly low.

### Sequential Point Query

```bash
./main_program --loadtf cache --qp queries/cluster_queries/cluster_256_100000_qp.txt -s 
Perform point queries 
Perform Query: 1 
Found: 1 ID: 50 
Query Time: 2273.61 ms 
Perform Query: 2 
Found: 1 
ID: 51 
Query Time: 2273.5 ms 
Average Query Time: 2273.56 ms
```

Sequential search uses the iminmax_data structure rather than going 
through the tree, so no node access counts are reported. 
However, the lack of the tree as an indexing structure is 
clearly demonstrated in the average query time. 
KNN Point query performs 4 orders of magnitude better in some cases.

## Range Query Stats

![Range Query Execution Times](/images/range_uniform_clustered.png)


The KNN system is a good fit for hypercube shaped queries against 
multi-demensional data. In the general case, we can still expect a 
large amount of processing, especially if the query is such that it 
causes a dimensionality explosion. However, our results benefit from the 
character of the range queries that were provided. For the most part, 
the start and end points of the range query differ in only one dimension.

The nature of the queries allows the auxiliary theorems of the 
KNN range algorithm to have a dramatic effect and substantially 
prune down the number of sub-queries that need to be performed 
an consequently the number of tree nodes that must be accessed.

Sub-query pruning leads to excellent performance on range queries. 
However, we believe that this sort of performance is an accident 
inherent in the nature of the query input files and cannot be 
expected in the general case.

### iMinMax Range Query

```bash
./main_program --loadtf cache --qr queries/cluster_queries/cluster_256_100000_qrw.txt 
Perform Query: 1 
Nodes accessed by range query: 5 
Found: 1 
ID: 50 
Query Time: 0.363 ms 
Perform Query: 2 
Nodes accessed by range query: 5 
Found: 1 ID: 51 
Query Time: 0.11 ms 
Average Query Time: 0.2365 ms
```
Again, the number of nodes accessed is extremely low. 
We believe this is due to sub-query pruning. 
Query time is correspondingly low.

### Sequential Range Query

./main_program --loadtf cache --qr queries/cluster_queries/cluster_256_100000_qrw.txt -s Perform Query: 1 Found: 1 ID: 50 Query Time: 3053.97 ms Perform Query: 2 Found: 1 ID: 51 Query Time: 3048.18 ms Average Query Time: 3051.07 ms

Given the queries we've been instructed to run, beating the sequential search is like shooting fish in a barrel. Again, we are getting a performance increase of up to 4 orders of magnitude.

KNN Query Stats

The KNN queries are perhaps our most interesting results (short of the extensions). iMinMax is really not optimized for KNN, but extensions have been added to it based on insights into the relationship between the euclidean distance and iMinMax index distance

http://iminmax.googlecode.com/git/images/knn_uniform_clustered.png

Again, the Y axis measures time saved over the sequential search. In most cases, iMinMax KNN performs better than sequential, but in some cases it performs worse. In general the iMinMax KNN query performs better as the dimensionality of the dataset increases.

iMinMax KNN Query

./main_program --loadtf cache --qn queries/cluster_queries/cluster_256_100000_qn.txt --knn 3 Perform Query: 1 Nodes accessed for KNN query: 5540 Results: Neighbor: 50; Distance: 0 Neighbor: 8808; Distance: 0.565941 Neighbor: 7065; Distance: 0.576985 Query Time: 2703.86 ms Perform Query: 2 Nodes accessed for KNN query: 5807 Results: Neighbor: 51; Distance: 0 Neighbor: 6058; Distance: 0.561889 Neighbor: 3341; Distance: 0.577814 Query Time: 2912.45 ms Average Query Time: 2808.16 ms

Note that the cout of nodes accessed is up by 3 orders of magnitude over the earlier queries. CPU time is up even more (due to the extra computational work of calculating distances). No more free lunch.

iMinMax Sequential Query

./main_program --loadtf cache --qn queries/cluster_queries/cluster_256_100000_qn.txt --knn 3 -s Perform Query: 1 Results: Neighbor: 50; Distance: 0 Neighbor: 8808; Distance: 0.565941 Neighbor: 8541; Distance: 0.576095 Query Time: 6879.39 ms Perform Query: 2 Results: Neighbor: 51; Distance: 0 Neighbor: 6058; Distance: 0.561889 Neighbor: 3119; Distance: 0.571302 Query Time: 6879.4 ms Average Query Time: 6879.39 ms

In the best case, KNN performance about 2.5x fast as sequential search. It's a far cry from improvements of orders of magnitude we saw in earlier. But these are the most expensive queries of the main set and the improvements we are getting amount to real, human perceptible improvements. If not dramatic, they are at least respectable.

