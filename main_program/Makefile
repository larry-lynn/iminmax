all:
	g++  -c -o includes/stx/btree.o includes/stx/btree.h
#	g++  -c -o includes/stx/btree_map.o includes/stx/btree_map.h
#	g++  -c -o includes/stx/btree_set.o includes/stx/btree_set.h
#	g++  -c -o includes/stx/btree_multiset.o includes/stx/btree_multiset.h
	g++  -c -o includes/stx/btree_multimap.o includes/stx/btree_multimap.h
	g++  -c -o includes/iminmax/iminmax.o includes/iminmax/iminmax.h
	g++  -Wall  -c -o includes/getopt_pp/getopt_pp.o includes/getopt_pp/getopt_pp.cpp
	ar -cvq libgetopt_pp.a includes/getopt_pp/getopt_pp.o
#	g++  -Wall  -c -o iminmax_utils.o iminmax_utils.cpp
#	ar -cvq libiminmax_utils.a iminmax_utils.o
	g++  -Wall  -c -o main_program.o main_program.cpp
#	g++ -o main_program main_program.o -lgetopt_pp -liminmax_utils -L.
	g++ -o main_program main_program.o -lgetopt_pp -L.
clean:
	# STX B+Tree
	rm includes/stx/btree.o
#	rm includes/stx/btree_map.o
#	rm includes/stx/btree_set.o
#	rm includes/stx/btree_multiset.o
	rm includes/stx/btree_multimap.o
	# Command line parser:
	rm includes/getopt_pp/getopt_pp.o
	rm libgetopt_pp.a
	# iMinMax
	rm includes/iminmax/iminmax.o

	# Main program
	rm main_program
	rm main_program.o
#	rm iminmax_utils.o
#	rm libiminmax_utils.a

	# Saved Data Files
#	rm treeinfo.txt
#	rm *.tree
#	rm *.data