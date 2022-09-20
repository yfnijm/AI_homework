#include "classification.hpp"
int rate;

int main(int argc, char** argv){
//	rate = atoi(argv[1]);

	srand((unsigned int)time(NULL) + getpid());
	vector<Example> dataSet = readData();
	vector<Example>& trainDataSet = dataSet;
//	int treeNum = TREE_NUM;
	int treeNum = 5;

	vector<Node*> forest;
	vector<Example> testDataSet = apart_data(trainDataSet);
	while(treeNum--){
		vector<Example> out_of_bag = dataSet;
		vector<Example>& out_of_bag_ref = out_of_bag;
		vector<Example> randomDataSet  = random_part(out_of_bag_ref);

		Node* tmp = CART_build(randomDataSet);
		if(randomDataSet.size() > 1) forest.push_back(tmp);
		test_vaildity(tmp, out_of_bag);
	}
	Node* finialTree = vote(forest);
	dump_screen(finialTree);
	test_vaildity(finialTree, testDataSet);
	return 0;
}
