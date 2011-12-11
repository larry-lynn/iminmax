# credit to http://docs.scipy.org/doc/numpy/reference/generated/numpy.random.multivariate_normal.html

import numpy as np
import sys

def gen_covariance_matrix(d):
  # based on a modified identity matrix
  columns = []
  for i in range(d):
    row = []
    for j in range(d):
      if i == j:
        row.append(0.002)
      else:
        row.append(0.0)
    columns.append(row[:]) 
  return columns
  
def gen_static_mean(d):
  mean = []
  for i in range(d):
    mean.append(0.2)
  return mean

# main program begins here
d = 256
size = 10000
min_val = 0.0
max_val = 1.0

mean = gen_static_mean(d)
cov = gen_covariance_matrix(d)

x = np.random.multivariate_normal(mean,cov)
averages = []

i = 0
while(i < size):
  x = np.random.multivariate_normal(mean,cov)
  # check all values are in range
  in_range = True
  for j in range(d):
    if ((x[j] <= min_val) or (x[j] >= max_val) ):
      in_range = False
      break
  
  if(in_range):
    i = i + 1
    avg = float(sum(x)) / len(x)
    averages.append(avg)
    sys.stdout.write(str(i) + ",")
    for j in range(d):  
      #print '%.4f' % (x[j]),
      sys.stdout.write( '%.4f' % (x[j]), )
      if j < (d - 1):
        print ",",
    print ""

average_of_averages = float(sum(averages)) / len(averages)

#print "big mean = " + str(average_of_averages)

