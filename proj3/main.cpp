#include "mineSweeper.hpp"
using namespace std;

int main(int argc, char** argv){
	if(argc != 2) {
		cout << "Usage: ./mineSweeper <Difficulty>" << endl;
		exit(1);
	}
	MineMap* map = mapInit(atoi(argv[1]));
	MineMap& map_ref = *map;

	//timer
	clock_t timer = clock();

	int counter = 1;
	while(map_ref.end_flag != true){
		matchAndChoose(map_ref);
		update(map_ref);

		counter++;
	// Comment the if condition can make the program dump the gameply on screen.
		if(map_ref.end_flag){
			cout << "The "<< counter << " step:" << endl;
			screenDump(map_ref);
		}
	}

	// Store data to a file
	ofstream data_file ("data.txt", ios::out | ios::app);
	data_file << clock() - timer << " " << counter << endl;
	data_file.close();

	delete map;
	return 0;
}
