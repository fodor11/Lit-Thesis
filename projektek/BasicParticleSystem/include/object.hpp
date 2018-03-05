#pragma once
#include "model.hpp"
#include "objectLoader.hpp"
#include "../src/tdogl/Program.h"
#include <glm/glm.hpp>

class Tree
{
public:
	/// loads the object and the billboard displaylist
	Tree(string fileName, tdogl::Program* shaderProgram);
	/// empty
	~Tree();
	void drawTree();
	void drawBillBoard();
	void translate(glm::vec3 vector);


private:
	tdogl::Program* m_pProgram;
	glm::mat4 m_mModel = glm::mat4();
	glm::vec3 m_vTranslationVector=glm::vec3();

	std::vector<GLuint> m_iTextureIDs;
	std::vector<GLuint> m_iTreeVAOs;
	std::vector<GLuint> m_vVBOs;
	std::vector<unsigned int> m_vNumberOfVertices;
	GLuint m_iTreeVBO;
	string m_fileName;
	
	float m_height;
	void getHeight(BoundingBox& boundingBox);

	float m_scale;
	/// vector of texture ID-s
	/// the last texture is the billboard image
	std::vector<GLuint> m_textures;
	GLuint loadTexture(string fileNames);

	void loadObjectVAOs();

	GLuint m_uiBillBoardVAO;
	void loadBillBoardVAO();
};