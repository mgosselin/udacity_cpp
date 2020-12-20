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

enum class State {kStart, kFinish, kEmpty, kClosed, kObstacle, kPath};

// a small array to loop over when expanding to immediate nearby nodes
const int delta[4][2]{{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

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

// check if a new candidate node is a valid one
bool CheckValidCell(int x, int y, vector<vector<State>> &grid) {
	bool response = 0;
  	int x_dim = grid.size();
  	int y_dim = grid[0].size();
  	// check that we're on the grid
  	if((x>=0&&x<x_dim)&&(y>=0&&y<y_dim)) {
      	// check if it's an empty space
    	if(grid[x][y]==State::kEmpty){
        	response = 1;
        }
    }
    return response;
}

// Add to the list of open nodes to explore
void AddToOpen(int x, int y, int g, int h, vector<vector<int>> &open, vector<vector<State>> &grid) {
  vector<int> node{x, y, g, h};
  open.push_back(node);
  grid[x][y] = State::kClosed;
}

// explore for new neighbor nodes, check their validity, add the valid ones
void ExpandNeighbors(vector<int> &current, int goal[2], vector<vector<int>> &open, vector<vector<State>> &grid) {
  int x = current[0];
  int y = current[1];
  int g = current[2];
  
  const vector<vector<int>> offsets = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
  
  // loop over the four adjacent cells
  for(int i=0; i<4; i++){
    int neighbor_x = x + offsets[i][0];
    int neighbor_y = y + offsets[i][1];
    
    // check if valid cell
    if(CheckValidCell(neighbor_x, neighbor_y, grid)){
      // add a new cell
      int g2 = g+1;
      int h2 = Heuristic(neighbor_x, neighbor_y, goal[0], goal[1]);
      AddToOpen(neighbor_x, neighbor_y, g2, h2, open, grid);
    }
  }
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
  // add the starting node to the open vector
  AddToOpen(x,y,g,h,open,grid);

  // so long as there are open nodes left, sort them, then get the "current" one
  while(open.size() > 0) {
    CellSort(&open);
    vector<int> current = open.back();
    open.pop_back();
    grid[current[0]][current[1]] = State::kPath;

    if(current[0] == goal[0] && current[1] == goal [1]) {
      grid[init[0]][init[1]] = State::kStart;
      grid[goal[0]][goal[1]] = State::kFinish;
      return grid;
    }
    // if we didn't arrive at the goal, expand the candidate nodes
    ExpandNeighbors(current, goal, open, grid);
  }

  cout << "No path found!" << "\n";
  return std::vector<vector<State>>{};
}

string CellString(State cell) {
  switch(cell) {
    case State::kObstacle: return "⛰️   ";
    case State::kPath: return "🚗  ";
    case State::kStart: return "🚦  ";
    case State::kFinish: return "🏁   ";
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

#include "test.cpp"

int main() {
  int init[2]{0, 0};
  int goal[2]{4, 5};
  vector<vector<State>> board = ReadBoardFile("1.board");
  vector<vector<State>> solution = Search(board, init, goal);
  PrintBoard(solution);
  
  // Tests, provided by Udacity
  TestHeuristic();
  TestAddToOpen();
  TestCompare();
  TestSearch();
  TestCheckValidCell();
  TestExpandNeighbors();
}
