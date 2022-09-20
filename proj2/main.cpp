#include "mineSweeper.hpp"
int board_size_x, board_size_y;
int variable_num;
int mines_num;

int main (int argc, char** argv){
	//store all kinds of input value
	cin >> board_size_x >> board_size_y >> mines_num;
	vector<vector<int>> map(board_size_x, vector<int>(board_size_y));
	vector<vector<int>> &map_ref = map;

	variable_num = 0;
	for (int i=0; i<board_size_x; i++){
		int input;
		for(int j=0; j<board_size_y; j++){
			cin >> map[i][j];
			if(map[i][j] == -1) variable_num++;
		}
	}

	//using different heuristics
	func(map_ref, atoi(argv[1]));
	return 0;
}
