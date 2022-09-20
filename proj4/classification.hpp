#include <unistd.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <algorithm> 
#include <cstring> 
#include <cmath>
#include <ctime>
#include <cstdlib>
#define FILEDIR "dataset/glass.data"
#define TEST_DATA_RATE 33
#define TEST_DATA_RATE_TOTAL_PART 100
#define TRAIN_DATA_RATE 5
#define TREE_NUM 1
#define CLASS_NUM 7

using namespace std;

class Example{
	public:
		int classType;
		vector<double> tag;
};

class Node{
	public:
		Node *leftNode, *rightNode;
		int leftTerminalTag, rightTerminalTag;	

		double value;
		int tagNum;
};

class SimpleNode{
	public:
		double value;
		int tagNum;
};


vector<Example> readData();
Node* CART_build(vector<Example>);
vector<Example> random_part(vector<Example> &);
vector<Example> apart_data(vector<Example> &);
double Gini_impurity(vector<Example>);
Node* vote(vector<Node*>);
void dump_screen(Node*);
bool compareFunc (SimpleNode, SimpleNode); 
void test_vaildity(Node*, vector<Example>);