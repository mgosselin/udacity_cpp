#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::cout;
using std::ifstream;
using std::istringstream;
using std::string;
using std::vector;
using std::abs;

enum class State {kEmpty, kClosed, kObstacle, kPath};


vector<State> ParseLine(string line) {
    istringstream sline(line);
    int n;
    char c;
    vector<State> row;
    while (sline >> n >> c && c == ',') {
      if (n == 0) {
        row.push_back(State::kEmpty);
      } else {
        row.push_back(State::kObstacle);
      }
    }
    return row;
}


vector<vector<State>> ReadBoardFile(string path) {
  ifstream myfile (path);
  vector<vector<State>> board{};
  if (myfile) {
    string line;
    while (getline(myfile, line)) {
      vector<State> row = ParseLine(line);
      board.push_back(row);
    }
  }
  return board;
}

// compare two nodes' distances
bool Compare(vector<int> node1, vector<int> node2) {
  int f1 = node1[2] + node1[3];
  int f2 = node2[2] + node2[3];
  return f1>f2;
}

// search heuristic
int Heuristic(int x1, int y1, int x2, int y2) {
	return abs(x2-x1)+abs(y2-y1);
}

// sort the vector of nodes
void CellSort(vector<vector<int>> *v) {
  sort(v->begin(), v->end(), Compare);
}

// Add to the list of open nodes to explore
void AddToOpen(int x, int y, int g, int h, vector<vector<int>> &open, vector<vector<State>> &grid) {
  vector<int> node{x, y, g, h};
  open.push_back(node);
  grid[x][y] = State::kClosed;
}

// search function: implements A*
vector<vector<State>> Search(vector<vector<State>> grid, int init[2], int goal[2]) {
// Create the vector of open nodes.
  vector<vector<int>> open {};
  
  // TODO: Initialize the starting node. 
  int x = init[0];
  int y = init[1];
  int g = 0;
  int h = Heuristic(init[0],init[1],goal[0],goal[1]);
  vector<int> node{x,y,g,h};

  // so long as there are open nodes left, sort them, then get the "current" one
  while(open.size() > 0) {
    CellSort(&open);
    vector<int> current = open.back();
    open.pop_back();
    grid[current[0]][current[1]] = State::kPath;
    cout << current[0] << ", " << current[1] << "\n";
    if(current[0] == goal[0] && current[1] == goal [1]) {
      return grid;
    }
  }
  
  // add the starting node to the open vector
  AddToOpen(x,y,g,h,open,grid);

  cout << "No path found!" << "\n";
  return std::vector<vector<State>>{};
}

string CellString(State cell) {
  switch(cell) {
    case State::kObstacle: return "⛰️   ";
    default: return "0   "; 
  }
}


void PrintBoard(const vector<vector<State>> board) {
  for (int i = 0; i < board.size(); i++) {
    for (int j = 0; j < board[i].size(); j++) {
      cout << CellString(board[i][j]);
    }
    cout << "\n";
  }
}


int main() {
  int init[2]{0, 0};
  int goal[2]{4, 5};
  vector<vector<State>> board = ReadBoardFile("1.board");
  vector<vector<State>> solution = Search(board, init, goal);
  PrintBoard(solution);
}
