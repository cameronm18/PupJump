
#include "ResourceManager.h"
#include "GameObject.h"
#include <vector>


class SaveGame
{
public:



	//constructor
	SaveGame() {}

	//read high score from file
	static int Load(const char* file);

	//write new high score to file
	static void Save(const char* file, int newScore);

	
};
