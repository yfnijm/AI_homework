#include "mineSweeper.hpp"
using namespace std;

//Initial the program, include:
//The difficulty,
//The real answer of the game,
//The initial safe cells to KB
//And also choose every mine and safe cells randomly.
MineMap* mapInit(int mode){
	int x, y, num;
	srand((unsigned) time(0));
	switch (mode){
		case 1:
			x = 9;
			y = 9;
			num = 10;
			break;
		case 2:
			x = 16;
			y = 16;
			num = 25;
			break;
		case 3:
			x = 16;
			y = 30;
			num = 99;
			break;
		case 4:
			cin >> x >> y >> num;
			break;
		default:
			exit(1);
	}
	MineMap* mineMap = new MineMap(x, y, num);

	mineMap->corrent.resize(x);
	mineMap->hints.resize(x);	
	for(int i=0; i<x; i++){
		mineMap->corrent[i].resize(y, -2);
		mineMap->hints[i].resize(y, 0);		
	}

	for (int mine=0; mine<num; mine++){
		int seq = rand() % (x * y - mine);
		int set_flag = 0;
		for(int i=0; i<x; i++){
			for(int j=0; j<y; j++){
				if(mineMap->hints[i][j] == -1) continue; 
				else if (!seq) {
					mineMap->hints[i][j] = -1;
					set_flag = 1;
					break;
				}
				else seq--;
			}
			if (set_flag) break;
		}
	}

	for(int i=0; i<x; i++){
		for(int j=0; j<y; j++){
			if(mineMap->hints[i][j] == -1) continue; 
			for(int surround=0; surround<8; surround++){
				int testing_x = i + surround_x[surround];
				int testing_y = j + surround_y[surround];

				if(testing_x >= 0 && testing_x < x &&
						testing_y >= 0 && testing_y < y &&
						mineMap->hints[testing_x][testing_y] == -1
				  ) mineMap->hints[i][j]++;
			}
		}
	}

	cout << "Real answer:" << endl;
	for(int i=0; i<x; i++){
		for(int j=0; j<y; j++){
			if (mineMap->hints[i][j] == -1) cout << "* ";
			else cout << mineMap->hints[i][j] << " ";
		}
		cout << endl;
	}

	//here we try to modified the number
	int safe_init = round(sqrt(x*y));
	//safe_init/=2;	
	//sleep(1);

	vector<vector<bool>> KB_init(x, vector<bool>(y, false));
	for (int safe=0; safe<safe_init; safe++){
		int seq = rand() % (x * y - num - safe);
		int set_flag = 0;
		for(int i=0; i<x; i++){
			for(int j=0; j<y; j++){
				if(mineMap->hints[i][j] == -1 || KB_init[i][j] == true) continue; 
				else if (!seq) {
					KB_init[i][j] = true;
					set_flag = 1;
					break;
				}
				else seq--;
			}
			if (set_flag) break;
		}
	}
	
	for(int i=0; i<x; i++){
		for(int j=0; j<y; j++){
			if(KB_init[i][j] == true){
				Variable newVar(i, j, false);
				list<Variable> newSent;
			newSent.push_back(newVar);
				mineMap->KB.push_back(newSent);
			}
		}
	}
	
	return mineMap;
}

void matchAndChoose(MineMap &map){
	if(map.KB0.size() == map.x_size * map.y_size){
		map.end_flag = true;
		return;
	}

	if (testSingle(map)) {
		matching(map, 2);
		return ;
	}
	matching(map, 2);
	if (testSingle(map)) return ;
	
	map.end_flag = true;
}

// The function is used for check single-iteral clause.
bool testSingle(MineMap &map){
	int counter = 0;
	for(list<list<Variable>>::iterator iter = map.KB.begin(); iter != map.KB.end(); ++iter){
		counter++;
		if(iter->size() == 1){
			map.KB0.push_back(iter->front());
			map.KB.erase(iter);
			return true;
		}	
	}
	return false;
}

//The function is used for resolvation 
//The retunr and the mode is used for test differnt matching way
void matching(MineMap &map, int mode = 1){
//	return ;
	// Search two clauses which we need then do the resolvation
	for (list<list<Variable>>::iterator iter_i = map.KB.begin(); iter_i != map.KB.end(); ++iter_i ){
		for (list<list<Variable>>::iterator iter_j = next(iter_i); iter_j != map.KB.end(); ++iter_j ){
			if (mode == 1) {if(iter_i->size() != 2 || iter_j->size() != 2) continue;}
			else if(mode == 2) {if(iter_i->size() != 2 && iter_j->size() != 2) continue;}

			list<Variable>::iterator iter_var1 = iter_i->begin(); 			
			list<Variable>::iterator iter_var2 = iter_j->begin();
			int pair = 0;
			Variable same_var;
			while(iter_var1 != iter_i->end() && iter_var2 != iter_j->end()){
				int tmp = compareVar(*iter_var1, *iter_var2);
				if(tmp == 0){
					if(iter_var1->state != iter_var2->state){
						pair ++;
						same_var = *iter_var1;
					}
					iter_var1++;
					iter_var2++;
				}else if(tmp == 1) iter_var2++;
				else iter_var1++;
			}
			
			// Test only with one pair between two clause
			if(pair != 1) continue;

			iter_var1 = iter_i->begin(); 			
			iter_var2 = iter_j->begin();
			
			list<Variable> new_sent;
			while(iter_var1 != iter_i->end() && iter_var2 != iter_j->end()){
				int tmp = compareVar(*iter_var1, *iter_var2);
				if(tmp == 0){
					if(iter_var1->x != same_var.x || iter_var1->y != same_var.y || iter_var1->state != same_var.state){
						new_sent.push_back(*iter_var1);
					}
					iter_var1++;
					iter_var2++;
				}else if(tmp == 1){
					new_sent.push_back(*iter_var2);
					iter_var2++;
				}else {
					new_sent.push_back(*iter_var1);
					iter_var1++;
				}
			}
			while(iter_var1 != iter_i->end()){
				new_sent.push_back(*iter_var1);
				iter_var1++;
			}
			while(iter_var2 != iter_j->end()){
				new_sent.push_back(*iter_var2);
				iter_var2++;
			}

			list<list<Variable>>::iterator& safe = iter_i;
			if(checkPushingSent_s(map, new_sent, safe)) iter_j = iter_i;
			if(iter_i == map.KB.end()) return;
			if(iter_j == map.KB.end()) break;
		}
	}
}

// Easy function to compare two vairable with coordination.
//  1: v1 > v2; 
//  0: v1 == v2; 
// -1: v1 < v2;
inline int compareVar(Variable v1, Variable v2){
	if(v1.x > v2.x) return 1;
	else if(v1.x < v2.x) return -1;
	else {
		if(v1.y > v2.y) return 1;
		else if(v1.y < v2.y) return -1;
		else if (v1.y == v2.y) return 0;
	}
}

// Check there are any duplicate clause
// And also check about are there any stricter clause any delete the unless information
// And the ""safe"" parameter is used for the other function need.
bool checkPushingSent_s(MineMap& map,list<Variable> new_sent, list<list<Variable>>::iterator& safe){	
	bool modified_flag = false;
	for (list<list<Variable>>::iterator iter_i = map.KB.begin(); iter_i != map.KB.end(); ){
		int pair = 0;
		list<Variable>::iterator iter_var1 = iter_i->begin(); 			
		list<Variable>::iterator iter_var2 = new_sent.begin();
		
		while(iter_var1 != iter_i->end() && iter_var2 != new_sent.end()){
			int tmp = compareVar(*iter_var1, *iter_var2);
			if(tmp == 0){
				if(iter_var1->state == iter_var2->state){
					pair ++;
				}
				iter_var1++;
				iter_var2++;
			}else if(tmp == 1) iter_var2++;
			else iter_var1++;
		}
		if(pair == iter_i->size()){
			return modified_flag;
		}else if(pair == new_sent.size()){
			if (safe == iter_i){
				safe++;
			}
			modified_flag = true;
			iter_i = map.KB.erase(iter_i);
		}else {
			iter_i++;
		}
	}
	map.KB.push_back(new_sent);
	return modified_flag;
}

// Similar to upper function but without ""safe"" paremeter
void checkPushingSent(MineMap& map,list<Variable> new_sent){	
	for (list<list<Variable>>::iterator iter_i = map.KB.begin(); iter_i != map.KB.end(); ){
		int pair = 0;
		list<Variable>::iterator iter_var1 = iter_i->begin(); 			
		list<Variable>::iterator iter_var2 = new_sent.begin();
		
		while(iter_var1 != iter_i->end() && iter_var2 != new_sent.end()){
			int tmp = compareVar(*iter_var1, *iter_var2);
			if(tmp == 0){
				if(iter_var1->state == iter_var2->state){
					pair ++;
				}
				iter_var1++;
				iter_var2++;
			}else if(tmp == 1) iter_var2++;
			else iter_var1++;
		}
		if(pair == iter_i->size()){
			return ;
		}else if(pair == new_sent.size()){
			iter_i = map.KB.erase(iter_i);
		}else {
			iter_i++;
		}
	}
	map.KB.push_back(new_sent);
}

//adding clause 
//Process the "matching" of that clause to all the remaining clauses in the KB.
//If new clauses are generated due to resolution, insert them into the KB.
//If this cell is safe:
//Query the game control module for the hint at that cell.
//Insert the clauses regarding its unmarked neighbors into the KB""
void update(MineMap &map){
	Variable handle = map.KB0.back();
	Variable& handle_ref = handle;

	//// To make sure the query is causely 
	int new_hint = map.hints[handle.x][handle.y];
	if((handle.state == true && new_hint != -1) ||(handle.state == false && new_hint == -1)){
		//if program is correct, then it would not happen.
		cout << "*****BOOM*****" << endl;
		delete (int*) 10;
		exit(1);
	}
	
	map.corrent[handle.x][handle.y] = new_hint;
	
	updating_sent(map, handle);

	if(new_hint != -1){
		int notSureNum = 0;
		int isMineNum = 0;
		for(int surround=0; surround<8; surround++){
			int test_x = handle.x + surround_x[surround];
			int test_y = handle.y + surround_y[surround];
			if(test_x >= 0 && test_x < map.x_size &&
					test_y >= 0 && test_y < map.y_size){
				if(map.corrent[test_x][test_y] == -2) notSureNum++;
				else if(map.corrent[test_x][test_y] == -1) isMineNum++;
			}
		}

		if(new_hint - isMineNum == notSureNum){
			all_surround_KB(map, handle_ref, true);
		}else if (new_hint - isMineNum == 0){
			all_surround_KB(map, handle_ref, false);
		}else {
			for(int i=0; i< 1<<notSureNum; i++){
				int counter = 0;
				for(int j=0; j<notSureNum; j++) 
					if((i >> j)%2 == 1) counter++;

				if (counter == notSureNum - (new_hint - isMineNum) + 1)
					assign_sent(map, i, handle_ref, true);

				if (counter == (new_hint - isMineNum) + 1)
					assign_sent(map, i, handle_ref, false);
			}
		}
	}

}

//After pushing a clase into KB0, delete every useless clause and resloved the clause have negtive variable 
void updating_sent(MineMap& map, Variable &handle){
	for(list<list<Variable>>::iterator iter_i = map.KB.begin(); iter_i != map.KB.end(); ){ //++ set in block
		bool sentInvaildFlag = false;
		for (list<Variable>::iterator iter_j = iter_i->begin(); iter_j != iter_i->end(); ){
			if(iter_j->x == handle.x && iter_j->y == handle.y){
				if(iter_j->state == handle.state){
					sentInvaildFlag = true;
					break;
				}else {
					iter_j = iter_i->erase(iter_j);
					if(iter_i->size() == 0){
						sentInvaildFlag = true;
					}
				}
			}else iter_j++;
		}
		if(sentInvaildFlag) iter_i = map.KB.erase(iter_i);
		else iter_i++;
	}
}

//push all of cell surround the targe cell to KB with the state
void all_surround_KB(MineMap &map, Variable &handle, bool state){
	for(int surround=0; surround<8; surround++){
		int test_x = handle.x + surround_x[surround];
		int test_y = handle.y + surround_y[surround];
		if(test_x >= 0 && test_x < map.x_size &&
				test_y >= 0 && test_y < map.y_size){
			if(map.corrent[test_x][test_y] == -2){
				Variable new_Var(test_x, test_y, state);
				list<Variable> new_sent;
				new_sent.push_back(new_Var);
				map.KB.push_back(new_sent);
			}
		}
	}
}

//To solve the C(m, n) case the function used the input integer to certain 
//clause just like what spec written. 
//ex. The int ""i"" is 57 (B111001) and the bool ""state"" is true meaning X1 or X2 or X3 or X6
void assign_sent(MineMap &map, int i, Variable &handle, bool state){
	list<Variable> new_sent;
	int order = 0;
	for(int surround=0; surround<8; surround++){
		int test_x = handle.x + surround_x[surround];
		int test_y = handle.y + surround_y[surround];
		if(test_x >= 0 && test_x < map.x_size &&
			test_y >= 0 && test_y < map.y_size &&
			map.corrent[test_x][test_y] == -2){
			if((i >> order)%2 == 1){
				Variable new_Var(test_x, test_y, state);
				new_sent.push_back(new_Var);
			}
			order++;
		}
	}
	checkPushingSent(map, new_sent);
}

//Print what the corrnet game look like to screen.
void screenDump(MineMap &map){
	for (int i=0; i<map.x_size; i++){
		for (int j=0; j<map.y_size; j++){
			if(map.corrent[i][j] == -1) cout << "\x1B[31m" <<"* " << "\x1B[0m";
			else if(map.corrent[i][j] == -2) cout  << "\x1B[92m" << ". " << "\x1B[0m";
			else if (map.corrent[i][j] == 0) cout << "\x1B[90m" << map.corrent[i][j] << "\x1B[0m" << " ";
			else cout <<  map.corrent[i][j] << " ";
		}
		cout << endl;
	}
}
