#pragma once
#include <gl/glew.h>
#include <gl/freeglut.h>

#include <vector>
#include <map>
#include "model.hpp"
#include "heightmap.hpp"
#include "texture.hpp"

class Forest {
public:
	/// sets the population file name
	Forest(std::string forestPopulationFileName);
	/// empty
	~Forest();
	/// reads the population file and stores the positions
	void initialize(HeightMapLoader* heightmap);
	/// returns the map of positions for "tree1", "tree2" and "tree3"
	std::map<std::string, std::vector<vec3f>>* getPositionsMap();
private:
	std::string m_sFilename;
	HeightMapLoader* m_pHeightmap;

	/// treeId ("tree1", "tree2" and "tree3") and the positions on which that tree has to be drawn
	std::map<std::string, std::vector<vec3f>> m_mPositions;
	void readPositions();
};