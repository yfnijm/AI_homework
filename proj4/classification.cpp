#include "classification.hpp"
extern int rate;

vector<Example> readData(){
	vector<Example> dataSet;
	ifstream iFile(FILEDIR);
	while(!iFile.eof()){
		string tmp;
		getline(iFile, tmp);

		if(!tmp.size()) continue;
		char cstr[tmp.length() + 1];
		strcpy(cstr, tmp.c_str());
		
		Example correntExp;
		char* tok = strtok(cstr, ",");
		tok = strtok(NULL, ",");
		while(1){
			double readValue = atof(tok);
			correntExp.tag.push_back(readValue);
			char* tmp_ptr = strtok(NULL, ",");
			if(!tmp_ptr){
				/*
				if(!strcmp(tok, "Iris-setosa")) correntExp.classType = 1;
				else if(!strcmp(tok, "Iris-versicolor")) correntExp.classType = 2;
				else if(!strcmp(tok, "Iris-virginica")) correntExp.classType = 3;
				break;*/
				correntExp.classType = atoi(tok);
				break;
			}
			tok = tmp_ptr;
		}
		dataSet.push_back(correntExp);
	}
	return dataSet;
}
//usiing for tree bagging
vector<Example> random_part(vector<Example> &dataSet){
//return dataSet;

	bool in_bag[dataSet.size()] = {0}; 
	vector<Example> partedData;
	for(int i=0; i<(dataSet.size()/TRAIN_DATA_RATE); i++){
		partedData.push_back(dataSet[rand() % dataSet.size()]);
		in_bag[i] = 1;
	}
	
	vector<Example> out_of_bag;
	for(int i=0; i<dataSet.size(); i++){
		if(!in_bag[i]) out_of_bag.push_back(dataSet[i]);
	}

	dataSet = out_of_bag;

	
	bool arr[partedData[0].tag.size()] = {0};
	int notUsedNum = 3;
	while(notUsedNum){
		int randAttribute = rand() % partedData[0].tag.size();
		if(arr[randAttribute] == 0) {
			arr[randAttribute] = 1;
			notUsedNum --;
		}
	}

	for(int n=0; n<partedData[0].tag.size(); n++){
		if(arr[n]){
			for(int i=0; i<partedData.size(); i++){
				partedData[i].tag[n] = 0.0;
			}
		}
	}
	return partedData;
}

//using for part testing data and training data
vector<Example> apart_data(vector<Example>& dataSet){
	vector<Example> testDataSet;

	for(vector<Example>::iterator it=dataSet.begin(); it < dataSet.end(); ){
		if((rand() % TEST_DATA_RATE_TOTAL_PART) < TEST_DATA_RATE){
			testDataSet.push_back(*it);
			it = dataSet.erase(it);
		}else it++;
	}
	return testDataSet;
}

Node* CART_build(vector<Example> dataSet){
	if(dataSet.size() < 2) return NULL; 
//	if(Gini_impurity(dataSet) < 0.00001) return NULL;
	int tagNum = dataSet[0].tag.size();
	
	vector<vector<double>> allThreshold;

	int selectAttribute = rand() % tagNum;
	for (int i=0; i<tagNum; i++){
		vector<double> appearNum;
		for(int j=0; j<dataSet.size(); j++){
			appearNum.push_back(dataSet[j].tag[i]);
		}
		sort(appearNum.begin(), appearNum.end());

		vector<double> useableThreshold;
		double firstNum, secondNum = appearNum[0];
		for(int j=1; j<appearNum.size(); j++){
			if(appearNum[j] == secondNum) continue;
			firstNum = secondNum;
			secondNum = appearNum[j];
			useableThreshold.push_back((firstNum + secondNum)/2);
		}
		allThreshold.push_back(useableThreshold);
	}
	
	double minImpurity = 1.5;
	Node* bestNode = new Node;
	vector<Example> refFirstSet;
	vector<Example> refSecondSet;
	double refFirstGini = 1.0, refSecondGini = 1.0;

	for(int j=0; j<allThreshold.size(); j++){

		for(int k=0; k<allThreshold[j].size(); k++){
			vector<Example> firstSet, secondSet;
			for (int l=0; l<dataSet.size(); l++){
				if(dataSet[l].tag[j] > allThreshold[j][k]) firstSet.push_back(dataSet[l]);
				else secondSet.push_back(dataSet[l]);
			}
			double firstGini = Gini_impurity(firstSet) * firstSet.size() / dataSet.size(); 
			double secondGini = Gini_impurity(secondSet) * secondSet.size() / dataSet.size();
			double correntImpurity = firstGini + secondGini;
			if(correntImpurity < minImpurity){
				minImpurity = correntImpurity;
				bestNode->value = allThreshold[j][k];
				bestNode->tagNum = j;
				refFirstSet = firstSet;
				refSecondSet = secondSet;

				refFirstGini = firstGini;
				refSecondGini = secondGini;
			}
		}
	}
	
//	if(refFirstSet.size() == 0 || refSecondSet.size() == 0) return NULL;

	if(refFirstGini > 0.00001) bestNode->leftNode = CART_build(refFirstSet);
	else {
	bestNode->leftNode = NULL;
		bestNode->leftTerminalTag = refFirstSet[0].classType;
	}
	if(refSecondGini > 0.00001) bestNode->rightNode = CART_build(refSecondSet);
	else {
	bestNode->rightNode = NULL;
		bestNode->rightTerminalTag = refSecondSet[0].classType;
	}
	
	return bestNode;
}

double Gini_impurity(vector<Example> dataSet){
	const int classNum = CLASS_NUM;
	int arr[classNum + 1] = {0};
	for (int i=0; i<dataSet.size(); i++){
		arr[dataSet[i].classType]++;
	}
	double sum = 0.0;
	for (int i=1; i<classNum+1; i++) sum += arr[i]* arr[i] * 1.0;
	sum /= (dataSet.size() * dataSet.size());
	return 1.0 - sum;
}

bool compareFunc (SimpleNode node1, SimpleNode node2) { 
	return (node1.value + node1.tagNum*1000 < node2.value + node2.tagNum*1000); 
}

Node* vote(vector<Node*> forest){
	if(!forest.size()) return NULL;

	vector<SimpleNode> allNode;
//	int nullcounter = 0;
	for(int i=0; i<forest.size(); i++){
//		if(!forest[i]) {
	//		nullcounter++;
//			continue;
//		}
		SimpleNode correntNode;
		correntNode.value = forest[i]->value;
		correntNode.tagNum = forest[i]->tagNum;
		allNode.push_back(correntNode);
	}
	sort(allNode.begin(), allNode.end(), compareFunc);

	int arr[allNode.size()] = {1};
	for(int i=1; i<allNode.size(); i++){
		if(allNode[i].tagNum == allNode[i-1].tagNum &&
			allNode[i].value == allNode[i-1].value){
			arr[i] = arr[i-1] + 1;
		}else arr[i] = 1;
	}

	int maxCount = 0;
	int seq = -1;
	for(int i=0; i<allNode.size(); i++){
		if(arr[i] > maxCount){
			maxCount = arr[i];
			seq = i;
		}
	}
//	if(nullcounter > maxCount) return NULL;

	SimpleNode selectNodeInfo = allNode[seq];

	Node* selectNode = new Node;
	selectNode->value = selectNodeInfo.value;
	selectNode->tagNum = selectNodeInfo.tagNum;

	vector<Node*> subLeftForest;
	vector<Node*> subRightForest;

	for(int i=0; i<forest.size(); i++){
		if(forest[i]->value == selectNodeInfo.value &&
			forest[i]->tagNum == selectNodeInfo.tagNum ){
			if(forest[i]->leftNode){
				subLeftForest.push_back(forest[i]->leftNode);
			}else selectNode->leftTerminalTag = forest[i]->leftTerminalTag;
			if(forest[i]->rightNode){
				subRightForest.push_back(forest[i]->rightNode);
			}else selectNode->rightTerminalTag = forest[i]->rightTerminalTag;
		}else {
		//	deleteTree(forest[i]);
		}
	}

	selectNode->leftNode = vote(subLeftForest);
	selectNode->rightNode = vote(subRightForest);

	return selectNode;
}


void dump_screen(Node* corrent){
	cout << "value: " << corrent->value << " " << corrent->tagNum << endl;

	cout << "left ";
	if(corrent->leftNode) dump_screen(corrent->leftNode);
	else cout << "terminal: " << corrent->leftTerminalTag << endl;

	cout << "right ";
	if(corrent->rightNode) dump_screen(corrent->rightNode);
	else cout << "terminal: " << corrent->rightTerminalTag << endl;
}

void test_vaildity(Node* tree, vector<Example> dataSet){
	int correctNum = 0;
	for(int i=0; i<dataSet.size(); i++){
		Node* traveling = tree;
		int correntClassType;
		while(traveling){
			if(dataSet[i].tag[traveling->tagNum] > traveling->value){
				correntClassType = traveling -> leftTerminalTag;
				traveling = traveling -> leftNode;
			}else{
				correntClassType = traveling -> rightTerminalTag;
				traveling = traveling->rightNode;
			}
		}
		if(correntClassType == dataSet[i].classType) correctNum++;
	}
	ofstream outFile;
	outFile.open("dataOutput.txt", fstream::out | fstream::app);
	outFile << (correctNum * 100) / dataSet.size() << endl;
	outFile.close();

//	cout << "Validity: " << correctNum * 100 / dataSet.size() << "%" << endl;
	cout  << correctNum * 100 / dataSet.size() << endl;
}
