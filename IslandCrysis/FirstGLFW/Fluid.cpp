#include "Fluid.h"

Fluid::Fluid(int n,Shader mshader) :shader(mshader)
{
	vertices = new float[n*n * 3];
	size = n;
	//fluid.assign(n, vector<glm::vec3>(n, glm::vec3()));
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			//fluid[i][j]=glm::vec3(j, 0.0f, i);
			vertices[i*n * 3 + j * 3 + 0] = j;
			vertices[i*n * 3 + j * 3 + 1] = 0.0f;
			vertices[i*n * 3 + j * 3 + 2] = i;
		}
	}
	//getVerticesArray();
	index = new unsigned int[(n - 1)*(n - 1) * 6];
	for (int i = 0; i < (n - 1); i++)
	{
		for (int j = 0; j < (n - 1); j++)
		{
			index[i*(n - 1) * 6 + j * 6 + 0] = i*n + j;
			index[i*(n - 1) * 6 + j * 6 + 1] = i*n + j + 1;
			index[i*(n - 1) * 6 + j * 6 + 2] = (i + 1)*n + j;
			index[i*(n - 1) * 6 + j * 6 + 3] = i*n + j + 1;
			index[i*(n - 1) * 6 + j * 6 + 4] = (i + 1)*n + j;
			index[i*(n - 1) * 6 + j * 6 + 5] = (i + 1)*n + j + 1;
		}
	}
}

Fluid::~Fluid()
{
	delete[] vertices;
	delete[] index;
}

void Fluid::Update(float deltaT, unsigned int)
{
	int row = fluid.size(), col = fluid[0].size();
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			float x = fluid[i][j].x;
			float y = fluid[i][j].y;
			float z = fluid[i][j].z;
			for (int m = 0; m < 3; m++)
			{
				for (int n = 0; n < 2; n++)
				{
					x = x - cos(thetas[n])*amplitudes[m * 2 + n] * sin(ks[m] * (fluid[i][j].x*cos(thetas[n]) +
						fluid[i][j].z*sin(thetas[n])) - omegas[m] * deltaT);
					y = y + amplitudes[m * 2 + n] * cos(ks[m] * (fluid[i][j].x*cos(thetas[n]) + fluid[i][j].z*sin(thetas[n])) - omegas[m] * deltaT);
					z = z - sin(thetas[n])*amplitudes[m * 2 + n] * sin(ks[m] * (fluid[i][j].x*cos(thetas[n]) + fluid[i][j].z*sin(thetas[n])) - omegas[m] * deltaT);
				}
			}
			fluid[i][j] = glm::vec3(x, y, z);
		}
	}
	getVerticesArray();
}

void Fluid::Draw(Camera & camera, float & deltaTime, glm::mat4 model, glm::mat4 view, glm::mat4 projection,glm::vec4 color)
{
	shader.use();
	shader.setMat4("model", model);
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);
	shader.setVec4("color", color);
	//shader.setFloat("time", deltaTime);
	int n = fluid.size();
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, n*n* 3 * sizeof(float), vertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, n*n * 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

float * Fluid::getVertices()
{
	return vertices;
}

unsigned int * Fluid::getIndex()
{
	return index;
}

int Fluid::getSize()
{
	return size;
}

void Fluid::getVerticesArray()
{
	int row = fluid.size(), col = fluid[0].size();
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j <col; j++)
		{
			vertices[i*col * 3 + j * 3 + 0] = fluid[i][j].x;
			vertices[i*col * 3 + j * 3 + 1] = fluid[i][j].y;
			vertices[i*col * 3 + j * 3 + 2] = fluid[i][j].z;
		}
	}
}
