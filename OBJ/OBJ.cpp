#include "OBJ.hpp"
#include <string>
#include <iostream>

static struct TexVertex
{
	float  x, y, z;
	float nx, ny, nz;
	float  u, v;
};

OBJ::Info::Info()
	:positionsCount(0U), uvsCount(0U), normalsCount(0U), verticesCount(0U), facesCount(0U), arr_verticesPerFace(nullptr) {}

OBJ::Info::~Info()
{
	if (arr_verticesPerFace)
	{
		delete[] arr_verticesPerFace;
		arr_verticesPerFace = nullptr;
	}
}

void OBJ::LoadInfo(std::ifstream& file)
{
	std::string line;

	while (getline(file, line))
	{
		if (line[0] == 'v')
		{
			switch (line[1])
			{
			case ' ':
				m_info.positionsCount++;
				break;
			case 'n':
				m_info.normalsCount++;
				break;
			case 't':
				m_info.uvsCount++;
				break;
			}
		}
		else if (line[0] == 'f')
		{
			for (int i = 1; i < line.size(); i++)
			{
				if (line[i] == ' ')
				{
					m_info.verticesCount++;
				}
			}
			m_info.facesCount++;
		}
	}

	m_info.arr_verticesPerFace = new unsigned int[m_info.facesCount];
	for (int i = 0; i < m_info.facesCount; i++)
		m_info.arr_verticesPerFace[i] = 1;
}

void OBJ::LoadPositions(std::ifstream& file, GLfloat* positions)
{
	std::string line;
	int posIndex = 0;

	while (getline(file, line))
	{
		if (!(line[0] == 'v' and line[1] == ' '))
			continue;
		int elem = 0;
		int elemIndex = 0;

		char posBuffer[3][10];
		for (int i = 0; i < 3; i++)
		{
			memset(posBuffer, 0, 30); // Sets all bytes to zero
		}

		for (int i = 2; i < line.size(); i++)
		{
			if (line[i] == ' ')
			{
				elem++;
				elemIndex = 0;
			}
			else
			{
				posBuffer[elem][elemIndex] = line[i];
				elemIndex++;
			}
		}
		positions[posIndex + 0] = std::stof(posBuffer[0]);
		positions[posIndex + 1] = std::stof(posBuffer[1]);
		positions[posIndex + 2] = std::stof(posBuffer[2]);
		posIndex += 3;
	}
}

void OBJ::LoadNormals(std::ifstream& file, GLfloat* normals)
{
	std::string line;
	int normalIndex = 0;

	while (getline(file, line))
	{
		if (!(line[0] == 'v' and line[1] == 'n'))
			continue;
		int elem = 0;
		int elemIndex = 0;

		char normalBuffer[3][10];
		for (int i = 0; i < 3; i++)
		{
			memset(normalBuffer, 0, 30);
		}

		for (int i = 3; i < line.size(); i++)
		{
			if (line[i] == ' ')
			{
				elem++;
				elemIndex = 0;
			}
			else
			{
				normalBuffer[elem][elemIndex] = line[i];
				elemIndex++;
			}
		}
		normals[normalIndex + 0] = std::stof(normalBuffer[0]);
		normals[normalIndex + 1] = std::stof(normalBuffer[1]);
		normals[normalIndex + 2] = std::stof(normalBuffer[2]);
		normalIndex += 3;
	}
}

void OBJ::LoadUVs(std::ifstream& file, GLfloat* uvs)
{
	std::string line;
	int uvIndex = 0;

	while (getline(file, line))
	{
		if (!(line[0] == 'v' and line[1] == 't'))
			continue;
		int elem = 0;
		int elemIndex = 0;

		char uvBuffer[2][10];
		for (int i = 0; i < 3; i++)
		{
			memset(uvBuffer, 0, 20);
		}

		for (int i = 3; i < line.size(); i++)
		{
			if (line[i] == ' ')
			{
				elem++;
				elemIndex = 0;
			}
			else
			{
				uvBuffer[elem][elemIndex] = line[i];
				elemIndex++;
			}
		}
		uvs[uvIndex + 0] = std::stof(uvBuffer[0]);
		uvs[uvIndex + 1] = std::stof(uvBuffer[1]);
		uvIndex += 2;
	}
}

void OBJ::LoadBuffers(std::ifstream& file, GLfloat* data)
{
	GLfloat* positions = data;
	GLfloat* normals = positions + m_info.positionsCount * 3;
	GLfloat* uvs = normals + m_info.normalsCount * 3;

	GLfloat* vertices = new GLfloat[m_info.verticesCount * 8];

	unsigned long* indices = new unsigned long[m_info.verticesCount * 3];
	for (int i = 0; i < m_info.verticesCount * 3; i++)
		indices[i] = 1;
	unsigned long indicesIndex = 0U;

	std::string line;

	int faceCountIndex = 0;

	while (getline(file, line))
	{
		if (line[0] != 'f')
			continue;
		
		char elemBuffer[10]; // 10 because INT_MAX has 10 digits (each stored in a byte)
		memset(elemBuffer, 0, 10);
		unsigned long elemIndex = 0U;

		for (int i = 2; i < line.size() + 1; i++)
		{
			if (line[i] >= 48 and line[i] <= 57)
			{
				elemBuffer[elemIndex] = line[i];
				elemIndex++;
			}
			else if ((line[i] == '/' or line[i] == ' ' or line[i] == '\0') and elemIndex > 0)
			{
				indices[indicesIndex] = std::stoul(elemBuffer);
				indicesIndex++;
				memset(elemBuffer, 0, 10);
				elemIndex = 0;
				if (line[i] == '\0')
					faceCountIndex++;
				else if (line[i] == ' ')
					m_info.arr_verticesPerFace[faceCountIndex]++;
			}
		}
	}

	for (int i = 0; i < m_info.verticesCount; i++) // 8 elements, that being X Y Z NX NY NZ U V
	{
		vertices[(i * 8) + 0] = positions[((indices[i * 3] - 1) * 3) + 0];
		vertices[(i * 8) + 1] = positions[((indices[i * 3] - 1) * 3) + 1];
		vertices[(i * 8) + 2] = positions[((indices[i * 3] - 1) * 3) + 2];
		vertices[(i * 8) + 3] = normals[((indices[(i * 3) + 2] - 1) * 3) + 0];
		vertices[(i * 8) + 4] = normals[((indices[(i * 3) + 2] - 1) * 3) + 1];
		vertices[(i * 8) + 5] = normals[((indices[(i * 3) + 2] - 1) * 3) + 2];
		vertices[(i * 8) + 6] = uvs[((indices[(i * 3) + 1] - 1) * 2) + 0];
		vertices[(i * 8) + 7] = uvs[((indices[(i * 3) + 1] - 1) * 2) + 1];
	}

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 32 * m_info.verticesCount, vertices, GL_STATIC_DRAW);
	
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glVertexAttribPointer(0U, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
	glVertexAttribPointer(1U, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)12);
	glVertexAttribPointer(2U, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)24);
	glEnableVertexAttribArray(0U);
	glEnableVertexAttribArray(1U);
	glEnableVertexAttribArray(2U);

	delete[] indices;
	delete[] vertices;
}

OBJ::OBJ(const char* filepath)
	:m_info(Info()), m_vbo(0U), m_vao(0U)
{
	if (filepath)
		Load(filepath);
}

void OBJ::Load(const char* filepath)
{
	std::ifstream file = std::ifstream(filepath);

	LoadInfo(file);
	GLfloat* data = new GLfloat[m_info.positionsCount * 3 + m_info.normalsCount * 3 + m_info.uvsCount * 2];

	file.clear();
	file.seekg(0U);
	LoadPositions(file, data);
	file.clear();
	file.seekg(0U);
	LoadNormals(file, data + m_info.positionsCount * 3);
	file.clear();
	file.seekg(0U);
	LoadUVs(file, data + m_info.positionsCount * 3 + m_info.normalsCount * 3);
	file.clear();
	file.seekg(0U);
	LoadBuffers(file, data);

	file.close();
	delete[] data;
}

void OBJ::Render() const
{
	glBindVertexArray(m_vao);
	for (int i = 0, j = 0; i < m_info.facesCount; j += m_info.arr_verticesPerFace[i], i++)
	{
		glDrawArrays(GL_TRIANGLE_FAN, j, m_info.arr_verticesPerFace[i]);
	}
}

OBJ::~OBJ()
{
	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);
}