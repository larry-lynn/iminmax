## Disk Access

The entire program is heavily I/O bound. Take the 
example of building the tree with one of the largest data sets

```bash 
./main_program -b --datafile ../../csci540_data/256Dimension10cluster100000Points.csv --savetf cache 
Tree Build Time: 183.865 ms 
Index Build Time [includes tree build time]: 2121.62 ms 
Tree statistics: 
Items:100000 
Nodes:7261 
Leaves:6776 
InnerNodes:485 
Levels:5 
Average Fill:0.70276 
Tree segments found in treeinfo.txt file


real 0m45.526s user 0m43.083s sys 0m2.268s
```

Program execution takes just over 45 seconds of real time on our 
build server. But just over 2 seconds of that is algorithm 
execution time; the rest is I/O

## Memory

The memory usage for the program is highest while reading the data, 
building the index and saving the index data structures to disk. 
Memory usage peaks at 602MB for the largest data sets. The program is 
not currently memory bound but could become so if the size of the data 
sets increased.

## CPU

The program is serial rather than parallel and can easily max out the 
capabilities of a single CPU (or a single core of a multicore machine). 
This is not currently problematic as the bottlenecks are elsewhere.

## Observations

### All datasets, All queries

On the build server, to process all data files and execute all queries 
in all query files, the current cost in real-time is

4 minutes 38 seconds

We serialize the 2 main data structures and save them to disk 
within the index build mode. This is intended to save resources and 
real execution time preserving the work of building the tree and 
the index. This turns out to be unhelpful for our algorithm.

### Size of Serialized Data

The size of the serialized binary structures is larger on disk than the 
original CSV files. The increase in required disk space and 
resulting I/0 penalty is around 18%.

```bash 
du -hc ../../csci540_data/256Dimension10cluster100000Points.csv 
169M ../../csci540_data/256Dimension10cluster100000Points.csv 
169M total

./main_program -b --datafile ../../csci540_data/256Dimension10cluster100000Points.csv --savetf cache 
Tree Build Time: 184.341 ms 
Index Build Time [includes tree build time]: 2119.85 ms 
Tree statistics: 
Items:100000 
Nodes:7261 
Leaves:6776 
InnerNodes:485 
Levels:5 
Average Fill:0.70276 
Tree segments found in treeinfo.txt file

du -hc cache.* 
197M cache.data 
1.9M cache.tree 
199M total 
```

On the build server we appear to be getting an I/O speed of 8.6 MB/Sec.

For this dataset, the build once, reuse many times approach costs us an 
extra 26 seconds (for 2 rounds of I/O on cache.tree and cache.data files) 
on the first build/query cycle. For every subsequent query cycle, 
we save 2.1 seconds in index build time; however we buy this savings 
at the cost of 3.4 seconds to read the serialized data structures 
rather than the original CSV because the serialized data is bigger 
than the original CSV.

Let **I** be the cost for reading or writing the index 
(both serialized data structures)  
Let **C** be the cost of reading the original data files in CSV format.  
Let **delta** be **I - C**  
Let **q** be the number of queries   

The the build once, reuse many times approach costs us **2 I + q delta**

In the end, this approach costs us time and, increases program complexity 
and reduces program maintainability. If we were to re-do this project, 
we would argue against the requirement to implement this approach.

### Memory Constraints on Performance

As mentioned earlier, the program peaks at a memory usage of 602MB. 
This is not currently problematic as the build server has 2GB 
available. However, if the size of the data were to grow by an order 
of magnitude, we would have problems on our system. If it were to grow 
by 2 orders of magnitude, we would have problems on most other 
systems as well.

If the size of the data were unconstrained, we would need to reconsider 
the architecture of our program. We would probably need to do away with 
the giant iminmax_data vector. Also, the B+ tree that we used presumes 
that all elements can reside in main memory. We would need another B+ tree 
implementation that functioned well when only a portion of the tree could 
be stored in main memory. We probably need to push all of the data that 
we currently store in iminmax_data into the tree. This would make the 
size of the tree increase dramatically (2 orders of magnitude in 
our current worse case) and further complicate our I/O problems.

The performance impacts of making such a change would be considerable.

