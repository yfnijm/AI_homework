#include <iostream>
#include <cmath>
#include <vector>
#include <list>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <unistd.h>
using namespace std;


// 1 2 3
// 4 x 5
// 6 7 8
const int surround_x[8] = {-1, -1, -1,  0,  0,  1,  1,  1};
const int surround_y[8] = {-1,  0,  1, -1,  1, -1,  0,  1};

class Variable;
class MineMap{
	public:
		int x_size;
		int y_size;
		int mine_num;
	
		vector<vector<int>> corrent;
		vector<vector<int>> hints;
		
		bool end_flag = false;

		list<Variable> KB0;
		list<list<Variable>> KB;

		MineMap(int x, int y, int num){
			x_size = x;
			y_size = y;
			mine_num = num;
		}
};

class Variable{
	public:
		int x, y;
		bool state;

		Variable(int input_x, int input_y, bool isNotNeg){
			x = input_x;
			y = input_y;
			state = isNotNeg;
		}
		Variable(){}

};

MineMap* mapInit(int);

void matchAndChoose(MineMap&);
bool testSingle(MineMap&);
void matching(MineMap&, int);
int compareVar(Variable, Variable);


void update(MineMap&);
void updating_sent(MineMap&, Variable &);
void all_surround_KB(MineMap &, Variable &, bool);
void assign_sent(MineMap &, int , Variable &, bool);
void checkPushingSent(MineMap&, list<Variable>);
bool checkPushingSent_s(MineMap&, list<Variable>, list<list<Variable>>::iterator&);	

void screenDump(MineMap&);

