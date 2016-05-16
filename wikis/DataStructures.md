## Introduction

Two primary data structures employed for the implementation

## Details

The STX B+ Tree is used to accelerate search operations. Elements are stored in the B+ tree in the form of key value pairs. Keys are real numbers greater than 0.0. Values are positive integers that correspond to indices in iminmax_data

## STX B+ Tree

![STX B+ Tree](/images/b-tree.png)

We used the STX B+ Tree templates from idlebox https://idlebox.net/2007/stx-btree/

This template implements B+ tree key/data container in main memory

## iMinMax Data

The second primary data structure

### iminmax_data_element

* 'label' is used to store the identifier for the point. 
For our data, this is an integer, usually corresponding to the 
sequence of the point in the input file. 
But it could be any arbitrary string: '1.23', 'foo' or 'Holy Hand Grenade'
* 'point' is a vector of doubles. This vector stores the coordinates 
of the point in an N dimensional space
* 'iminmax_index' is a positive real number. It stored the value 
obtained by running the iMinMax index algorithm on the N dimensional point. 
It is used as a key for later insertion into the B+ tree

```C++
struct iminmax_data_element { string label; vector<double> point; double iminmax_index; };
```

![iminmax_element](/images/iminmax-element.png)

### iminmax_data

iminmax_data is simply a vector comprised of S iminmax_data_elements 
where S is the size or number of points in the dtaset. 
Since iminmax_data_element also contains a vector, iminmax_data can be 
roughly thought of 
as a vector of vectors or a matrix

```C++
vector<iminmax_data_element> iminmax_data;
```

![iminmax_data](/images/iminmax-data.png)
http://iminmax.googlecode.com/git/images/iminmax-data.png

Integration of iMinMax Data with the B+ tree

Each record in the source data is represented in both iminmax_data and the B+ tree. iminmax_data contains a record of the label, a vector with the point coordinates and the iminmax_index. The B+ tree contains a record of the point in key/value form where key is the iminmax_index and value is the vector index corresponding to that point in the iminmax_data vector.

Since the B+ tree needs only a string and a double to represent a data element and iminmax_data needs a string, a double and a vector of doubles to represent the same element, the B+ tree tends to have a very small memory footprint relative to iminamx_data. The B+ tree then is simply a search mechanism where iminmax_data is the primary in-memory storage structure. for(i = 0; i < (long)iminmax_data.size() ; ++i){ // btree.insert(key, value) btree.insert(iminmax_data[i].iminmax_index, i); }

http://iminmax.googlecode.com/git/images/iminmax-btree.png

Note: for these data structures to work together correctly, it is essential that the vector index values in iminmax_data match up with the key/value payload in the B+ tree. Any mismatch causes serious bugs that are difficult to track down.

