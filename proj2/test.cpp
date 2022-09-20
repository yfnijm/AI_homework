#include <iostream>
#include <vector>
#include <stack>
using namespace std;

int main (){
	stack<vector<int>> s;
	vector<int> v;
	s.push(v);
	v.push_back(10);
	cout << s.top().size() << v.size();
	return 0;
}
