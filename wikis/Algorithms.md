# iMinMax Algorithms

Below are descriptions and some implementation details for the point, range, and KNN queries in iMinMax.

## Index Creation

To insert a data point into the B+-tree, each datapoint is first given a 
one-dimensional index. Let **x<sub>min</sub>** be the point's minimum 
value over 
all the dimensions and let **x<sub>max</sub>** be the point's 
maximum value over 
all the dimensions. Similarly, let **d<sub>min</sub>** and 
**d<sub>max</sub>** be the 
dimensions (integer-valued) in which **x<sub>min</sub>** and 
**x<sub>max</sub>** occur, 
respectively. The index is computed as follows:

![Index Creation Equation](/images/algorithms_index.png)

The integer part refers to the partition (dimension) of the point's 
closest edge, and the decimal part refers to the position 
along that edge. The parameter θ controls the relative influence 
of the **Min** and **Max** edges and is used to account for 
skewed data distributions.

The figure below shows where the **Max** and **Min** boundaries 
occur for **x** and **y** when θ = 0.

![Max and Min Boundaries](/images/algorithms_index_boundaries.png)

## Point Query

The point query simply calculates the iMinMax index of the query point, 
follows this index in the B+-tree down to the leaves, and checks the 
points of all matching index values for an exact match on all 
dimensions of the point. The pseudocode is shown below:


![Point Query Pseudocode](/images/algorithms_point_query_pseudocode_resized.png)

## Range Query

Similarly, range queries are also transformed before the B+-tree is 
searched. In this case, for **d**-dimensional data, 
**d** subqueries will be generated. The equation for the 
**j**<sub>th</sub> subquery is shown below:

![Range Query Equations](/images/algorithms_range_transformation.png)

Basically, the three cases correspond to the three cases shown below (θ = 0 and d = 2):

![Range Query Cases](/images/algorithms_range_cases.png)

The pseudocode below shows how each subquery is evaluated:

![Range Query Pseudocode](/images/algorithms_range_query_pseudocode.png)
http://iminmax.googlecode.com/git/images/algorithms_range_query_pseudocode.png

The range query follows 3 useful theorems:

Theorem 1

http://iminmax.googlecode.com/git/images/algorithms_theorem_1.png

Theorem 2

http://iminmax.googlecode.com/git/images/algorithms_theorem_2.png

Theorem 3

http://iminmax.googlecode.com/git/images/algorithms_theorem_3.png

Below is an example of how the range query works in 2 dimensions:

http://iminmax.googlecode.com/git/images/range-query-theta-0-v3.png

KNN Query

Below is the pseudocode for the decreasing radius KNN query with iMinMax:

http://iminmax.googlecode.com/git/images/algorithms_knn_query_pseudocode.png

Here is an illustration of the KNN algorithm working in 2 dimensions when k = 4 and θ = -1:

http://iminmax.googlecode.com/git/images/algorithms_knn_example.png


