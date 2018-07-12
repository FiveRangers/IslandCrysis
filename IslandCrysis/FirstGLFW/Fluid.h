#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <iostream>
#include <vector>
#include <math.h>
#include <memory>

#include "Camera.h"
#include "Shader.h"

using std::vector;
using std::cout;
using std::endl;

class Fluid {
public:
	Fluid(int n,Shader mshader);
	~Fluid();
	void Update(float deltaT,unsigned int);
	void Draw(Camera& camera,float& deltaTime,glm::mat4 model,glm::mat4 view,glm::mat4 projection,glm::vec4 color);
	float* getVertices();
	unsigned int* getIndex();
	int getSize();
private:
	void getVerticesArray();
private:
	vector<vector<glm::vec3>> fluid;
	int size;
	float *vertices;
	unsigned int *index;
	unsigned int VAO, VBO, EBO;
	Shader shader;

	// 传播方向
	float thetas[2] = { 0.38f,1.42f };
	// 角频率
	float omegas[3] = { 3.27f,3.31f,3.42f };
	// 波数
	float ks[3] = { 1.091f,1.118f,1.1935f };
	// 振幅
	float amplitudes[6] = {
		0.2f,0.2f,
		0.3f,0.5f,
		0.2f,0.6f
	};

};
