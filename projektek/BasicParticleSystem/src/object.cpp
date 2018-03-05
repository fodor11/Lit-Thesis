#include "../include/object.hpp"
#include <glm/gtc/matrix_transform.hpp>

GLuint Tree::loadTexture(string fileName)
{
	TextureLoader *txtrLoader = new TextureLoader();
	txtrLoader->loadMipMappedTexture(fileName);
	m_textures.push_back(txtrLoader->textureID());
	txtrLoader->~TextureLoader();
	return m_textures.back();
}

void Tree::getHeight(BoundingBox& boundingBox)
{
	m_height = boundingBox.getXdistance();
	if (m_height < boundingBox.getYdistance())
	{
		m_height = boundingBox.getYdistance();
	}
	if (m_height < boundingBox.getZdistance())
	{
		m_height = boundingBox.getZdistance();
	}
}
void Tree::loadObjectVAOs()
{
	ObjectLoader* objLoader = new ObjectLoader();
	objLoader->loadObjFile(m_fileName + ".obj");

	const std::vector<glm::vec3>& vertices = objLoader->getVertices();
	const std::vector<glm::vec3>& vertNormals = objLoader->getVertexNormals();
	const std::vector<glm::vec2>& textureCoords = objLoader->getTextureCoords();
	const std::map<std::string, std::vector<Face>>& faceLists = objLoader->getFaceLists();

	getHeight(objLoader->getBoundingBox());

	//temporary
	Face face;
	int v_index, vt_index, vn_index;

	m_mModel = glm::rotate(m_mModel, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	m_scale = 0.05; 
	m_mModel = glm::scale(m_mModel, glm::vec3(m_scale, m_scale, m_scale));
	
	for (auto map = faceLists.begin(); map != faceLists.end(); map++)
	{
		GLuint textureId = loadTexture(map->first);
		m_iTextureIDs.push_back(textureId);
		std::vector<glm::vec3> verticesVBO;
		std::vector<glm::vec3> vertexNormalsVBO;
		std::vector<glm::vec2> textureCoordsVBO;
		const std::vector<Face>& curr_faceList = map->second;
		for (int i = 0; i < curr_faceList.size(); i++)
		{
			face = curr_faceList[i];
			//1 vertex
			for (int j = 0; j < 3; j++)
			{
				//get indices
				v_index = face[j][0] - 1;
				vt_index = face[j][1] - 1;
				vn_index = face[j][2] - 1;

				//get values
				verticesVBO.push_back(vertices[v_index]);
				textureCoordsVBO.push_back(glm::vec2(textureCoords[vt_index].x, 1 - textureCoords[vt_index].y));
				vertexNormalsVBO.push_back(vertNormals[vn_index]);
			}
		}
		GLuint treeVAO;
		glGenVertexArrays(1, &treeVAO);
		glBindVertexArray(treeVAO);
		m_iTreeVAOs.push_back(treeVAO);

		// create and bind the VBO for vertices
		glGenBuffers(1, &m_iTreeVBO);
		m_vVBOs.push_back(m_iTreeVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_iTreeVBO);
		glBufferData(GL_ARRAY_BUFFER, verticesVBO.size() * sizeof(glm::vec3), &verticesVBO.front(), GL_STATIC_DRAW);
		// connect the xyz to the "vert" attribute of the vertex shader
		glEnableVertexAttribArray(m_pProgram->attrib("vert"));
		glVertexAttribPointer(m_pProgram->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, NULL);

		// create and bind the VBO for texture coordinates
		glGenBuffers(1, &m_iTreeVBO);
		m_vVBOs.push_back(m_iTreeVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_iTreeVBO);
		glBufferData(GL_ARRAY_BUFFER, textureCoordsVBO.size() * sizeof(glm::vec2), &textureCoordsVBO.front(), GL_STATIC_DRAW);
		// connect the uv coords to the "vertTexCoord" attribute of the vertex shader
		glEnableVertexAttribArray(m_pProgram->attrib("vertTexCoord"));
		glVertexAttribPointer(m_pProgram->attrib("vertTexCoord"), 2, GL_FLOAT, GL_FALSE, 0, NULL);

		// create and bind the VBO for normals
		glGenBuffers(1, &m_iTreeVBO);
		m_vVBOs.push_back(m_iTreeVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_iTreeVBO);
		glBufferData(GL_ARRAY_BUFFER, vertexNormalsVBO.size() * sizeof(glm::vec3), &vertexNormalsVBO.front(), GL_STATIC_DRAW);
		// connect the normal to the "vertNorm" attribute of the vertex shader
		glEnableVertexAttribArray(m_pProgram->attrib("vertNorm"));
		glVertexAttribPointer(m_pProgram->attrib("vertNorm"), 3, GL_FLOAT, GL_TRUE, 0, NULL);

		glBindVertexArray(0);
		m_vNumberOfVertices.push_back(verticesVBO.size());
	}	
	objLoader->~ObjectLoader();
}

void Tree::loadBillBoardVAO()
{
	loadTexture(m_fileName + ".png");
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> textureCoords;

	float height = m_height * m_scale;
	float halfWidth = height * 0.5f;

	//first square
	vertices.push_back(glm::vec3(-halfWidth, 0, 0));		//1
	textureCoords.push_back(glm::vec2(0, 1));
	vertices.push_back(glm::vec3(halfWidth, 0, 0));			//2
	textureCoords.push_back(glm::vec2(1, 1));
	vertices.push_back(glm::vec3(-halfWidth, height, 0));	//3
	textureCoords.push_back(glm::vec2(0, 0));
	vertices.push_back(glm::vec3(-halfWidth, height, 0));	//3
	textureCoords.push_back(glm::vec2(0, 0));
	vertices.push_back(glm::vec3(halfWidth, 0, 0));			//2
	textureCoords.push_back(glm::vec2(1, 1));
	vertices.push_back(glm::vec3(halfWidth, height, 0));	//4
	textureCoords.push_back(glm::vec2(1, 0));
	for (int i = 0; i < 6; i++)
	{
		normals.push_back(glm::vec3(0, 0, 1));
	}

	//second square
	vertices.push_back(glm::vec3(0, 0, halfWidth));			//1
	textureCoords.push_back(glm::vec2(0, 1));
	vertices.push_back(glm::vec3(0, 0, -halfWidth));		//2
	textureCoords.push_back(glm::vec2(1, 1));
	vertices.push_back(glm::vec3(0, height, halfWidth));	//3
	textureCoords.push_back(glm::vec2(0, 0));
	vertices.push_back(glm::vec3(0, height, halfWidth));	//3
	textureCoords.push_back(glm::vec2(0, 0));
	vertices.push_back(glm::vec3(0, 0, -halfWidth));		//2
	textureCoords.push_back(glm::vec2(1, 1));
	vertices.push_back(glm::vec3(0, height, -halfWidth));	//4
	textureCoords.push_back(glm::vec2(1, 0));
	for (int i = 0; i < 6; i++)
	{
		normals.push_back(glm::vec3(1, 0, 0));
	}
	glGenVertexArrays(1, &m_uiBillBoardVAO);
	glBindVertexArray(m_uiBillBoardVAO);

	// create and bind the VBO for vertices
	glGenBuffers(1, &m_iTreeVBO);
	m_vVBOs.push_back(m_iTreeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_iTreeVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
	// connect the xyz to the "vert" attribute of the vertex shader
	glEnableVertexAttribArray(m_pProgram->attrib("vert"));
	glVertexAttribPointer(m_pProgram->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, NULL);

	// create and bind the VBO for texture coordinates
	glGenBuffers(1, &m_iTreeVBO);
	m_vVBOs.push_back(m_iTreeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_iTreeVBO);
	glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(glm::vec2), &textureCoords.front(), GL_STATIC_DRAW);
	// connect the uv coords to the "vertTexCoord" attribute of the vertex shader
	glEnableVertexAttribArray(m_pProgram->attrib("vertTexCoord"));
	glVertexAttribPointer(m_pProgram->attrib("vertTexCoord"), 2, GL_FLOAT, GL_FALSE, 0, NULL);

	// create and bind the VBO for normals
	glGenBuffers(1, &m_iTreeVBO);
	m_vVBOs.push_back(m_iTreeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_iTreeVBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
	// connect the normal to the "vertNorm" attribute of the vertex shader
	glEnableVertexAttribArray(m_pProgram->attrib("vertNorm"));
	glVertexAttribPointer(m_pProgram->attrib("vertNorm"), 3, GL_FLOAT, GL_TRUE, 0, NULL);

	glBindVertexArray(0);
}


Tree::Tree(string fileName, tdogl::Program* shaderProgram)
{
	m_fileName = fileName;
	m_pProgram = shaderProgram;
	loadObjectVAOs();
	loadBillBoardVAO();
}

Tree::~Tree()
{
	glDeleteBuffers(m_vVBOs.size(), &m_vVBOs.front());
}

void Tree::drawTree()
{
	glm::vec3 specular( 0.f, 0.f, 0.f);
	glm::vec3 ambient( 0.1f, 0.1f, 0.1f);
	glm::vec3 diffuse( 0.3f, 0.3f, 0.3f);
	for (int i = 0; i < m_iTreeVAOs.size(); i++)
	{
		// bind the program (the shaders)
		m_pProgram->use();

		// set the "model" uniform in the vertex shader
		glm::mat4 tmpModel = glm::translate(glm::mat4(), m_vTranslationVector);
		tmpModel *= m_mModel;
		m_pProgram->setUniform("model", tmpModel);

		// set colors
		m_pProgram->setUniform("ambientColor", ambient);
		m_pProgram->setUniform("diffuseColor", diffuse);
		m_pProgram->setUniform("specularColor", specular);

		// set the "normalMatrix" uniform in the vertex shader
		m_pProgram->setUniform("normalMatrix", glm::mat3(glm::transpose(glm::inverse(tmpModel))));

		// bind the texture and set the "tex" uniform in the fragment shader
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_iTextureIDs[i]);
		m_pProgram->setUniform("tex", 0); //set to 0 because the texture is bound to GL_TEXTURE0

		// bind the VAO 
		glBindVertexArray(m_iTreeVAOs[i]);
		
		glDrawArrays(GL_TRIANGLES, 0, m_vNumberOfVertices[i]);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		m_pProgram->stopUsing();
	}
}

void Tree::drawBillBoard()
{
	glm::vec3 specular = glm::vec3( 0.f, 0.f, 0.f);
	glm::vec3 ambient = glm::vec3( 0.1f, 0.1f, 0.1f);
	glm::vec3 diffuse = glm::vec3( 0.13f, 0.13f, 0.13f);

	// bind the program (the shaders)
	m_pProgram->use();

	// set the "model" uniform in the vertex shader
	glm::mat4 tmpModel = glm::translate(glm::mat4(), m_vTranslationVector);
	m_pProgram->setUniform("model", tmpModel);

	// set colors
	m_pProgram->setUniform("ambientColor", ambient);
	m_pProgram->setUniform("diffuseColor", diffuse);
	m_pProgram->setUniform("specularColor", specular);

	// set the "normalMatrix" uniform in the vertex shader
	m_pProgram->setUniform("normalMatrix", glm::mat3(glm::transpose(glm::inverse(tmpModel))));

	// bind the texture and set the "tex" uniform in the fragment shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textures.back());
	m_pProgram->setUniform("tex", 0); //set to 0 because the texture is bound to GL_TEXTURE0

									  // bind the VAO 
	glBindVertexArray(m_uiBillBoardVAO);

	glDrawArrays(GL_TRIANGLES, 0, 12); // 2 squares made up of 6-6 vertices
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	m_pProgram->stopUsing();

}

void Tree::translate(glm::vec3 vector)
{
	m_vTranslationVector = vector;
}
