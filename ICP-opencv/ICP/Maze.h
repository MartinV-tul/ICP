#pragma once
#include <string>
class Maze
{
	public:
		Maze(int xSize, int ySize);
		int XSize;
		int YSize;
		std::string* maze;
		int StartX;
		int StartY;
		int EndX;
		int EndY;
};

