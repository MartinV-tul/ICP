#include "Maze.h"
#include "time.h"

Maze::Maze(int xSize, int ySize) {
	XSize = xSize;
	YSize = ySize;
	maze = new std::string[xSize];
	bool startChosen = false;
	bool start = true;
	int value = RAND_MAX / 100;
	srand(time(NULL));
	for (size_t i = 0; i < xSize; i++)
	{
		maze[i] = new char[ySize];
		for (size_t j = 0; j < ySize; j++)
		{
			if ((i % 5 == 1 || j % 5 == 1) && (i != 0 && i != xSize - 1) && (j != 0 && j != ySize - 1)) {
				maze[i][j] = ' ';
				if (start) {
					start = false;
					StartX = j;
					StartY = i;
				}
				int v = rand();
				if (v < value && !startChosen) {
					startChosen = true;
					StartX = j;
					StartY = i;
				}
			}
			else {
				maze[i][j] = '#';
			}
		}
	}
}
