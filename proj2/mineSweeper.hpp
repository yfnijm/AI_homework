#include <iostream>
#include <vector>
#include <stack>
#include <array>
#include <algorithm>
#include <list>
#include <bits/stdc++.h> 
#define LCV_WITH_MRV 1
using namespace std;

const int surround_x[8] = {1, 1, 0, -1, -1, -1, 0, 1};
const int surround_y[8] = {0, 1, 1, 1, 0, -1, -1, -1};

enum mine_state{
	notSure, isMine, isntMine, errorCondition
};

class position_state{
	public:
		int x, y;
		int value;
		mine_state state;
		position_state(int init_x, int init_y){
			state = notSure;
			x = init_x;
			y = init_y;
			value = 0;
		}
};

// compare
//bool compare(position_state, position_state); 

struct stateHeapMax{
	bool operator()(const position_state& x, const position_state& y){
		return x.value < y.value;
	}
};

struct stateHeapMin{
	bool operator()(const position_state& x, const position_state& y){
		return x.value >= y.value;
	}
};

void func(vector<vector<int>>&, int);
int MRV(vector<vector<int>>&,
		stack<vector<position_state>>&,
		vector<vector<int>>&
		);
int DH(vector<vector<int>>&,
		stack<vector<position_state>>&,
		vector<vector<int>>&
		);
int LCV(vector<vector<int>>&,
		stack<vector<position_state>>&,
		vector<vector<int>>&
		);
int testEndCondition(vector<vector<int>>&,
		vector<position_state>&,
		vector<vector<int>>&	
		);
int testMidCondition(vector<vector<int>>&,
		vector<position_state>&,
		vector<vector<int>>&	
		);
