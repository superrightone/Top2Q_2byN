CC=g++
CFLAGS=-lsdsl -std=c++11

all: *.cpp *.h
	$(CC) $(CFLAGS) -O3 -o act_demo act_demo.cpp MyRDAG.cpp MyRDAG_node.cpp Oned_topk.cpp

act_demo: act_demo.o MyRDAG.o MyRDAG_node.o Oned_topk.o
	$(CC) $(CFLAGS) -O3 -o act_demo act_demo.cpp MyRDAG.cpp MyRDAG_node.cpp Oned_topk.cpp

act_demo.o: act_demo.cpp
	$(CC) $(CFLAGS) -c act_demo.cpp MyRDAG.cpp MyRDAG_node.cpp Oned_topk.cpp
MyRDAG_node.o: MyRDAG_node.cpp MyRDAG_node.h
	$(CC) $(CFLAGS) -c MyRDAG_node.cpp Oned_topk.cpp
clear:
	rm act_demo *.o
