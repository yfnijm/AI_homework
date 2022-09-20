#include "mineSweeper.hpp"
extern int board_size_x, board_size_y;
extern int variable_num;
extern int mines_num;
int runTime;

void func(vector<vector<int>>& map, int mode){
	//using stack to store different state with vector STL
	stack<vector<position_state>> state;
	stack<vector<position_state>>& state_ref = state;

	//first state to put into stack
	vector<position_state> first;
	vector<position_state>& first_ref = first;

	//making a lookup table to let me know how the 2D map's position state 
	//store in the linear vector, if there are no correspond part, store
	//with -1.
	vector<vector<int>> lookup = map;
	vector<vector<int>> &lookup_ref = lookup;
	int index = 0;
	for(int i=0; i < board_size_x; i++){
		for(int j=0; j< board_size_y; j++){
			if(lookup[i][j] == -1) lookup[i][j] = index++;
			else lookup[i][j] = -1;
		}
	}

	//complete the first state
	for(int i=0; i < board_size_x; i++){
		for(int j=0; j< board_size_y; j++){
			if(map[i][j] == -1) {
				position_state tmp(i, j);
				first.push_back(tmp);
			}
		}
	}
	state.push(first);

	//using different heuristic function with parameter
	switch(mode){
		case 1:
			MRV(map, state_ref, lookup_ref);
			break;
		case 2:
			DH(map, state_ref, lookup_ref);
			break;
		case 3:
			LCV(map, state_ref, lookup_ref);
			break;
		default:
			break;
	}

	//if the state return successfully, print all of position of mine.
	//In the loop, if there are no answer, it would print nothing. 
	for(int i = 0; i<variable_num; i++){
		if(state.top()[i].state == isMine){
			cout << "(" << state.top()[i].x << "," << state.top()[i].y << ")";
		}
	}
	cout << endl;
}

int MRV(vector<vector<int>>& map,
		stack<vector<position_state>>& state,
		vector<vector<int>>& lookup){
	
	//copy the newest state
	vector<position_state> priority;
	priority.assign(state.top().begin(), state.top().end());

	//To check what position only have one domain, that is,
	//it must be mine or nothing in the position.
	//The reflect flag to make sure the state would different
	//between the last state.
	int reflesh_flag = 0;
	for(int i=0; i<board_size_x; i++){
		for(int j=0; j<board_size_y; j++){
			if(map[i][j] == -1) continue;
			int notSure_num = 0;
			int isMine_num = 0;
			for(int test=0; test<8; test++){
				int test_x = i + surround_x[test];
				int test_y = j + surround_y[test];
				if(test_x >= 0 && test_x < board_size_x &&
						test_y >= 0 && test_y < board_size_y &&
						map[test_x][test_y] == -1
				  ){
					if(priority[lookup[test_x][test_y]].state == notSure) notSure_num++;
					else if(priority[lookup[test_x][test_y]].state == isMine) isMine_num++;	
				}
			}
			if(map[i][j] - isMine_num == 0){
				for(int test=0; test<8; test++){
					int test_x = i + surround_x[test];
					int test_y = j + surround_y[test];
					if(test_x >= 0 && test_x < board_size_x &&
							test_y >= 0 && test_y < board_size_y &&
							map[test_x][test_y] == -1
					  ){
						if(priority[lookup[test_x][test_y]].state == notSure){ 
							priority[lookup[test_x][test_y]].state = isntMine;
							reflesh_flag = 1;

						}
					}
				}
			}else if(map[i][j] - isMine_num == notSure_num){
				for(int test=0; test<8; test++){
					int test_x = i + surround_x[test];
					int test_y = j + surround_y[test];
					if(test_x >= 0 && test_x < board_size_x &&
							test_y >= 0 && test_y < board_size_y &&
							map[test_x][test_y] == -1
					  ){
						if(priority[lookup[test_x][test_y]].state == notSure) {
							priority[lookup[test_x][test_y]].state = isMine;
							reflesh_flag = 1;
						}
					}
				}
			}else if(map[i][j] - isMine_num < 0){
				state.pop();
				return 0;
			}
		}
	}

	//Forward checking
	int isMine_num = 0;
	for(int i=0; i<variable_num; i++){
		if(priority[i].state == isMine) isMine_num++;
	}
	if(isMine_num == mines_num){
		if(!testEndCondition(map, priority, lookup)){
			state.pop();
			return 0;
		}
		state.push(priority);
		return 1;
	} else if(isMine_num > mines_num){
		state.pop();
		return 0;
	}

	//if the new state is same as the last one,
	//choose ramdon position as mine due to the 
	//remind positiin's domain is all 1 and 0,
	//I using the "notSure" state to represent.
	if(reflesh_flag){
		state.push(priority);
		if(MRV(map, state, lookup)) return 1;
	}else{
		for(int i=0; i<variable_num; i++){
			if(priority[i].state == notSure){
				priority[i].state = isMine;
				state.push(priority);
				if(MRV(map, state, lookup)){
					return 1;
				}else {
					priority[i].state = notSure;
				}
			}
		}	
	}
	return 0;
}

int DH(vector<vector<int>>& map,
		stack<vector<position_state>>& state,
		vector<vector<int>>& lookup){
	//Full name is degree heuristic.
	//in the function, choosing the position has most constrains.

	//degree isnt represent the new state but different degreee, 
	//that is, how much constrain effect the point.
	vector<position_state> degree;
	degree.assign(state.top().begin(), state.top().end());
	vector<position_state>& test_state = degree;

	//forward checking first
	int isMine_num = 0;
	for(int i=0; i<variable_num; i++){
		if(degree[i].state == isMine) isMine_num++; 
	}
	if(isMine_num > mines_num){
		return 0;
	}else if(isMine_num == mines_num){
		if(testEndCondition(map, test_state, lookup)) return 1;
		else return 0;
	}else {
		if(testMidCondition(map, test_state, lookup) == 0) return 0;
	}

	//compute the constrain in different position
	//And if there are 0 constrain, regrad as a point 
	//almost impossible to be a mine.
	for(int i=0; i<board_size_x; i++){
		for(int j=0; j<board_size_y; j++){
			if(map[i][j] != -1){
				for(int test = 0; test < 8; test++){
					int test_x = i + surround_x[test];
					int test_y = j + surround_y[test];
					if(test_x >= 0 && test_x < board_size_x &&
							test_y >= 0 && test_y < board_size_y &&
							map[test_x][test_y] == -1){
						degree[lookup[test_x][test_y]].value += map[i][j];
						if(map[i][j] == 0) degree[lookup[test_x][test_y]].value = INT_MIN;
					}
				}
			}
		}
	}
	//using heap to choose position effected by most constrain.
	make_heap(degree.begin(), degree.end(), stateHeapMax());

	//Try the position with the heap one by one.
	vector<position_state> new_state = state.top();
	new_state.assign(state.top().begin(), state.top().end());
	while(!degree.empty()){
		position_state tmp = degree.front();
		if(tmp.state != isMine){
			mine_state original_state = new_state[lookup[tmp.x][tmp.y]].state;
			new_state[lookup[tmp.x][tmp.y]].state = isMine;
			state.push(new_state); 
			if(DH(map, state, lookup)){
				return 1;
			}else{
				new_state[lookup[tmp.x][tmp.y]].state = original_state;
				state.pop();
			}
		}
		pop_heap(degree.begin(),degree.end(), stateHeapMax()); degree.pop_back();
	}
	return 0;
}


int LCV(vector<vector<int>>& map,
		stack<vector<position_state>>& state,
		vector<vector<int>>& lookup){

	vector<position_state> corrent_state;
	vector<position_state>& test_state = corrent_state;

	corrent_state.assign(state.top().begin(), state.top().end());

	int isMine_num = 0;
	for(int i=0; i<variable_num; i++){
		if(corrent_state[i].state == isMine) isMine_num++; 
	}
//cout << isMine_num << " ";


	if(isMine_num > mines_num){
		return 0;
	}else if(isMine_num == mines_num){
		if(testEndCondition(map, test_state, lookup)) return 1;
		else return 0;
	}else {
		if(testMidCondition(map, test_state, lookup) == 0) return 0;
	}

	vector<vector<int>> upperbound(board_size_x, vector<int>(board_size_y));
	vector<vector<int>> lowerbound(board_size_x, vector<int>(board_size_y));
	for(int i=0; i<board_size_x; i++){
		for(int j=0; j<board_size_y; j++){
			if(map[i][j] == -1) continue;
			int notSure_num = 0;
			int isMine_num = 0;
			for(int test=0; test<8; test++){
				int test_x = i + surround_x[test];
				int test_y = j + surround_y[test];
				if(test_x >= 0 && test_x < board_size_x &&
						test_y >= 0 && test_y < board_size_y &&
						map[test_x][test_y] == -1
				  ){
					if(corrent_state[lookup[test_x][test_y]].state == notSure) notSure_num++;
					else if(corrent_state[lookup[test_x][test_y]].state == isMine) isMine_num++;	
				}
			}
			upperbound[i][j] = isMine_num + notSure_num;
			lowerbound[i][j] = isMine_num;
		}
	}

	vector<position_state> priority;

	for(int i=0; i<variable_num; i++){
		if (corrent_state[i].state != notSure) continue;
		int handle_x = corrent_state[i].x;
		int handle_y = corrent_state[i].y;

		int effect_surround_num_upper = 0;
		int effect_surround_num_lower = 0;
		mine_state corrent_point_state = notSure;
		vector<vector<int>> effect_to_isMine (5, vector<int>(5, 0));
		vector<vector<int>> effect_to_isntMine (5, vector<int>(5, 0));

#ifndef LCV_WITH_MRV
		for(int test = 0; test < 8; test++){
			int test_x = handle_x + surround_x[test];
			int test_y = handle_y + surround_y[test];

			if(test_x < 0 || test_x >= board_size_x ||
					test_y < 0 || test_y >= board_size_y ||
					map[test_x][test_y] == -1) continue;

			if(map[test_x][test_y] > upperbound[test_x][test_y]){
				corrent_point_state = errorCondition;
			}else if(map[test_x][test_y] == upperbound[test_x][test_y]){
				if(corrent_point_state == notSure) corrent_point_state = isMine;
				else if(corrent_point_state == isntMine) corrent_point_state = errorCondition;	
			}else if(map[test_x][test_y] - upperbound[test_x][test_y] == -1){
				for(int test_block = 0; test_block<8; test_block++){
					int test_block_x = test_x + surround_x[test_block];
					int test_block_y = test_y + surround_y[test_block];

					if(test_block_x >= 0 && test_block_x < board_size_x && 
							test_block_x >= 0 && test_block_y < board_size_y &&
							map[test_block_x][test_block_y] == -1 &&
							corrent_state[lookup[test_block_x][test_block_y]].state == notSure
					  ){
						effect_to_isMine[2 + surround_x[test] + surround_x[test_block]][2 + surround_y[test] + surround_y[test_block]] = 1;
					}
				}
			}

			if(map[test_x][test_y] < lowerbound[test_x][test_y]){
				corrent_point_state = errorCondition;
			}else if(map[test_x][test_y] == lowerbound[test_x][test_y]){
				if(corrent_point_state == notSure) corrent_point_state = isntMine;
				else if(corrent_point_state == isMine) corrent_point_state = errorCondition;		
			}else if(map[test_x][test_y] - lowerbound[test_x][test_y] == 1){
				for(int test_block = 0; test_block<8; test_block++){
					int test_block_x = test_x + surround_x[test_block];
					int test_block_y = test_y + surround_y[test_block];

					if(test_block_x >= 0 && test_block_x < board_size_x && 
							test_block_x >= 0 && test_block_y < board_size_y &&
							map[test_block_x][test_block_y] == -1 &&
							corrent_state[lookup[test_block_x][test_block_y]].state == notSure
					  ){
						effect_to_isntMine[2 + surround_x[test] + surround_x[test_block]][2 + surround_y[test] + surround_y[test_block]] = 1;
					}
				}
			}

			if(corrent_point_state == errorCondition) break;
		}

#endif
#ifdef LCV_WITH_MRV
		for(int test = 0; test < 8; test++){
			int test_x = handle_x + surround_x[test];
			int test_y = handle_y + surround_y[test];

			if(test_x < 0 || test_x >= board_size_x ||
				test_y < 0 || test_y >= board_size_y ||
				map[test_x][test_y] == -1) continue;

			if(map[test_x][test_y] >= upperbound[test_x][test_y]){
				if(corrent_point_state == notSure) corrent_point_state = isntMine;
				else if(corrent_point_state == isMine) corrent_point_state = errorCondition;

				if(map[test_x][test_y] == upperbound[test_x][test_y]){
					for(int test_block = 0; test_block<8; test_block++){
						int test_block_x = test_x + surround_x[test_block];
						int test_block_y = test_y + surround_y[test_block];

						if(test_block_x >= 0 && test_block_x < board_size_x && 
								test_block_x >= 0 && test_block_y < board_size_y &&
								map[test_block_x][test_block_y] == -1 &&
								corrent_state[lookup[test_block_x][test_block_y]].state == notSure
						  ){
							corrent_state[lookup[test_block_x][test_block_y]].state = isMine;
						}
					}
				}
			}else if(map[test_x][test_y] - upperbound[test_x][test_y] == -1){
				for(int test_block = 0; test_block<8; test_block++){
					int test_block_x = test_x + surround_x[test_block];
					int test_block_y = test_y + surround_y[test_block];

					if(test_block_x >= 0 && test_block_x < board_size_x && 
							test_block_x >= 0 && test_block_y < board_size_y &&
							map[test_block_x][test_block_y] == -1 &&
							corrent_state[lookup[test_block_x][test_block_y]].state == notSure
					  ){
						effect_to_isMine[2 + surround_x[test] + surround_x[test_block]][2 + surround_y[test] + surround_y[test_block]];
					}
				}
			}

			if(map[test_x][test_y] <= lowerbound[test_x][test_y]){
				if(corrent_point_state == notSure) corrent_point_state = isMine;
				else if(corrent_point_state == isntMine) corrent_point_state = errorCondition;

				if(map[test_x][test_y] == lowerbound[test_x][test_y]){
					for(int test_block = 0; test_block<8; test_block++){
						int test_block_x = test_x + surround_x[test_block];
						int test_block_y = test_y + surround_y[test_block];

						if(test_block_x >= 0 && test_block_x < board_size_x && 
								test_block_x >= 0 && test_block_y < board_size_y &&
								map[test_block_x][test_block_y] == -1 &&
								corrent_state[lookup[test_block_x][test_block_y]].state == notSure
						  ){
							corrent_state[lookup[test_block_x][test_block_y]].state = isntMine;
						}
					}
				}
			}else if(map[test_x][test_y] - lowerbound[test_x][test_y] == 1){
				for(int test_block = 0; test_block<8; test_block++){
					int test_block_x = test_x + surround_x[test_block];
					int test_block_y = test_y + surround_y[test_block];

					if(test_block_x >= 0 && test_block_x < board_size_x && 
							test_block_x >= 0 && test_block_y < board_size_y &&
							map[test_block_x][test_block_y] == -1 &&
							corrent_state[lookup[test_block_x][test_block_y]].state == notSure
					  ){
						effect_to_isntMine[2 + surround_x[test] + surround_x[test_block]][2 + surround_y[test] + surround_y[test_block]];
					}
				}
			}
			if(corrent_point_state == errorCondition) break;
		}
#endif
		for(int m=0; m<5; m++) for(int n=0; n<5; n++) if(effect_to_isntMine[m][n]) effect_surround_num_lower++;
		for(int m=0; m<5; m++) for(int n=0; n<5; n++) if(effect_to_isMine[m][n]) effect_surround_num_upper++;

		if(corrent_point_state == errorCondition) continue;
		else{
			if(corrent_point_state == notSure || corrent_point_state == isMine){
				position_state tmp(handle_x, handle_y);
				tmp.value = effect_surround_num_lower;
				tmp.state = isMine;
				priority.push_back(tmp);
			}

			if(corrent_point_state == notSure || corrent_point_state == isntMine){
				position_state tmp(handle_x, handle_y);
				tmp.value = effect_surround_num_upper;
				tmp.state = isntMine;
				priority.push_back(tmp);
			}		
		}
	}
	random_shuffle ( priority.begin(), priority.end());
	make_heap(priority.begin(), priority.end(), stateHeapMin());
	while(!priority.empty()){	
		runTime--;
		vector<position_state> new_state = corrent_state;
		if (new_state[lookup[priority.front().x][priority.front().y]].state == notSure)		
			new_state[lookup[priority.front().x][priority.front().y]] = priority.front();
		pop_heap(priority.begin(),priority.end(), stateHeapMin()); priority.pop_back();
		state.push(new_state);

		if(LCV(map, state, lookup)) return 1;
		else state.pop();

	}
	return 0;
}






int testEndCondition(vector<vector<int>>& map,
		vector<position_state>& test_array,
		vector<vector<int>>& lookup
		){
	for(int i=0; i<board_size_x; i++){
		for(int j=0; j<board_size_y; j++){
			if(map[i][j] == -1) continue;
			int isMine_num = 0;
			for(int test=0; test<8; test++){
				int test_x = i + surround_x[test];
				int test_y = j + surround_y[test];
				if(test_x >= 0 && test_x < board_size_x &&
						test_y >= 0 && test_y < board_size_y &&
						map[test_x][test_y] == -1
				  ){
					if(test_array[lookup[test_x][test_y]].state == isMine) isMine_num++;	
				}
			}
			if(map[i][j] !=  isMine_num) return 0;
		}
	}
	return 1;
}


int testMidCondition(vector<vector<int>>& map,
		vector<position_state>& test_state,
		vector<vector<int>>& lookup
		){
	for(int i=0; i<board_size_x; i++){
		for(int j=0; j<board_size_y; j++){
			if(map[i][j] == -1) continue;
			int isMine_num = 0;
			for(int test=0; test<8; test++){
				int test_x = i + surround_x[test];
				int test_y = j + surround_y[test];
				if(test_x >= 0 && test_x < board_size_x &&
						test_y >= 0 && test_y < board_size_y &&
						map[test_x][test_y] == -1
				  ){
					if(test_state[lookup[test_x][test_y]].state == isMine) isMine_num++;	
				}
			}
			if(map[i][j] - isMine_num < 0){
				return 0;	
			}
		}
	}
	return 1;
}
