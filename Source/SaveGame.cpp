#include "SaveGame.h"

#include <iostream>
#include <fstream>
#include <sstream>


int SaveGame::Load(const char* file)
{


	//load from file
	unsigned int tileCode;

	std::string line;
	std::ifstream fstream(file);
	std::vector<std::vector<unsigned int>> tileData;

	if (fstream)
	{
		while (std::getline(fstream, line))	
		{
			std::istringstream sstream(line);
			std::vector<unsigned int> row;
			while (sstream >> tileCode)		
			{
				std::cout << line << std::endl;
				return std::stoi(line);
			}

		}


	}

}

void SaveGame::Save(const char* file, int newScore)
{

	std::ofstream outFile(file);

	if (outFile.is_open())
	{
		outFile << newScore;
		outFile.close();
	}
	else
	{
		std::cerr << "Failed to open file. \n";
	}

}