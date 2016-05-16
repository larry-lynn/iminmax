# iminmax

## An implementation of the iMinMax multidimensional indexing method with extensions.

This project is based primarily on the work published in the following 2 papers:

* Indexing the edges—a simple and yet efficient approach to high-dimensional indexing by Beng Chin Ooi, Kian-Lee Tan, Cui Yu and Stephane Bressan

* Decreasing Radius K-Nearest Neighbor Search using Mapping-based Indexing Schemes by Qingxiu Shi and Bradford G. Nickerson

---

### Our implementation supports: 
1. the iMinMax indexing algorithm 
2. iMinMax point queries
3. iMinMax range queries 
4. iMinMax K nearest neighbor queries  
5. variable values of a master control knob, theta 
6. search for optimal theta through the use of a distribution median

![Range Query theta 0](/images/range-query-theta-0-v3.png)
