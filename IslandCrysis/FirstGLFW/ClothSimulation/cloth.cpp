#include "cloth.h"

cloth::cloth()
{

	row = 0;
	col = 0;
	mass = 0;
	K[0] = 0;
	K[1] = 0;
	K[2] = 0;
	L[0] = 0;
	L[1] = 0;
	L[2] = 0;
}

cloth::cloth(int mrow, int mcol, float mmass, float *mk, float *ml, float g, float v, float d, glm::vec3 muFluid)
{
	position.assign(mrow, vector<glm::vec3>(mcol));
	velocity.assign(mrow, vector<glm::vec3>(mcol));
	normal.assign(mrow, vector<glm::vec3>(mcol));
	row = mrow;
	col = mcol;
	mass = mmass;

	// 初始化位置矩阵、速度矩阵和法向量矩阵
	for (int i=0; i < row; i++)
	{
		for (int j=0; j < col; j++)
		{
			position[i][j] = glm::vec3(j, i, 0.0f);
			velocity[i][j] = glm::vec3(0.0f, 0.0f, 0.0f);
		}
	}
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			normal[i][j] = getNormalVec(i, j);
		}
	}
	if (mk == nullptr)
	{
		K[0] = 0.3f;
		K[1] = 0.3f;
		K[2] = 0.3f;
	}
	else
	{
		K[0] = mk[0];
		K[1] = mk[1];
		K[2] = mk[2];
	}
	if (ml == nullptr)
	{
		L[0] = (float)row/(float)(row-1);
		L[1] = sqrt(2.0f)*(float)row/(float)(row-1);
		L[2] = 2.0f*L[0];
		//L[0] = 1.0f;
		//L[1] = sqrt(2.0f);
		//L[2] = 2.0f*L[0];
	}
	else
	{
		L[0] = ml[0];
		L[1] = ml[1];
		L[2] = ml[2];
	}

	cg = g;
	cv = v;
	cd = d;

	uFluid = muFluid ;
}

cloth::cloth(const cloth & c)
{
}

shared_ptr<float> cloth::Simulation()
{
	vector<vector<glm::vec3>> tempPosition(position);
	vector<vector<glm::vec3>> tempVelocity(velocity);
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			// 固定左边一列的点位置不变化
			if (j==0)
			{
				continue;
			}
			else
			{
				tempVelocity[i][j] = updateVelocity(i, j, 1);
				tempPosition[i][j] = updatePosition(i, j, 1, tempVelocity[i][j]);
			}
		}
	}
	position = tempPosition;
	velocity = tempVelocity;
	return getVerticeArray(position);
}

void cloth::Update(float *vertices)
{
	Simulation();
	translate(glm::vec3(-3, -3, 0));
	getVerticeArray(vertices);
	for (int i = 0; i < row*col * 3 && row>1; i++)
	{
		vertices[i] = vertices[i] / (row - 1);
		if (vertices[i] > 1) vertices[i] = 1;
		else if (vertices[i] < -1) vertices[i] = -1;
	}
}

int cloth::getRow()
{
	return row;
}

int cloth::getCol()
{
	return col;
}


vector<vector<glm::vec3>> cloth::getAllPositon()
{
	return position;
}

vector<vector<glm::vec3>> cloth::getAllVelocity()
{
	return velocity;
}

vector<vector<glm::vec3>> cloth::getAllNormal()
{
	return normal;
}


glm::vec3 cloth::getPosition(int i, int j)
{
	if (i < row&&i >= 0&&j<col&&j>=0)
	{
		return position[i][j];
	}
	return glm::vec3();
}

void cloth::setPosition(int i, int j, glm::vec3 p)
{
	if (i < row&&i >= 0 && j<col&&j >= 0)
	{
		position[i][j]=p;
	}
}

glm::vec3 cloth::getNormal(int i, int j)
{
	if (i < row&&i >= 0 && j<col&&j >= 0)
	{
		return normal[i][j];
	}
	return glm::vec3();
}

glm::vec3 cloth::getVelocity(int i, int j)
{
	if (i < row&&i >= 0 && j<col&&j >= 0)
	{
		return velocity[i][j];
	}
	return glm::vec3();
}

void cloth::setVelocity(int i, int j, glm::vec3 v)
{
	if (i < row&&i >= 0 && j<col&&j >= 0)
	{
		velocity[i][j]=v;
	}
}

float cloth::getCg()
{
	return cg;
}

void cloth::setCg(const float & g)
{
	cg = g;
}

float cloth::getCv()
{
	return cv;
}

void cloth::setCv(const float & v)
{
	cv = v;
}

float cloth::getCd()
{
	return cd;
}

void cloth::setCd(const float & d)
{
	cd = d;
}

shared_ptr<float> cloth::getVerticeArray(const vector<vector<glm::vec3>>& pos)
{
	// 利用智能指针来管理数组
	// 使用时必须保证row和col值与pos的row和col相等，否则会造成内存越界访问
	shared_ptr<float> vertices(new float[row*col * 3], [](float *p) {delete[] p; }); 
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j <col; j++)
		{
			*(vertices.get() + i*col*3+ j * 3 + 0) = pos[i][j].x;
			*(vertices.get() + i*col*3 + j * 3 + 1) = pos[i][j].y;
			*(vertices.get() + i*col*3 + j * 3 + 2) = pos[i][j].z;
		}
	}
	return vertices;
}


shared_ptr<float> cloth::getVerticeArray()
{
	// 利用智能指针来管理数组
	// 使用时必须保证row和col值与pos的row和col相等，否则会造成内存越界访问
	shared_ptr<float> vertices(new float[row*col * 3], [](float *p) {delete[] p; });
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j <col; j++)
		{
			*(vertices.get() + i*col * 3 + j * 3 + 0) = position[i][j].x;
			*(vertices.get() + i*col * 3 + j * 3 + 1) = position[i][j].y;
			*(vertices.get() + i*col * 3 + j * 3 + 2) = position[i][j].z;
		}
	}
	return vertices;
}

void cloth::getVerticeArray(float * vertices)
{
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j <col; j++)
		{
			vertices[i*col * 3 + j * 3 + 0] = position[i][j].x;
			vertices[i*col * 3 + j * 3 + 1] = position[i][j].y;
			vertices[i*col * 3 + j * 3 + 2] = position[i][j].z;
		}
	}
}

shared_ptr<unsigned int> cloth::getIndexArray()
{
	unsigned int* index(new unsigned int[(row - 1)*(col - 1) * 2 * 3]); 
	for (int i = 0; i < (row - 1); i++)
	{
		for (int j = 0; j < (col - 1); j++)
		{
			index[i*(col - 1) * 6 + j * 6 + 0] = i*col + j;
			index[i*(col - 1) * 6 + j * 6 + 1] = i*col + j + 1;
			index[i*(col - 1) * 6 + j * 6 + 2] = (i + 1)*col + j;
			index[i*(col - 1) * 6 + j * 6 + 3] = i*col + j + 1;
			index[i*(col - 1) * 6 + j * 6 + 4] = (i + 1)*col + j;
			index[i*(col - 1) * 6 + j * 6 + 5] = (i + 1)*col + j + 1;
		}
	}
	return shared_ptr<unsigned int>(index);
}

void cloth::getIndexArray(unsigned int * index)
{
	for (int i = 0; i < (row - 1); i++)
	{
		for (int j = 0; j < (col - 1); j++)
		{
			index[i*(col - 1) * 6 + j * 6 + 0] = i*col + j;
			index[i*(col - 1) * 6 + j * 6 + 1] = i*col + j + 1;
			index[i*(col - 1) * 6 + j * 6 + 2] = (i + 1)*col + j;
			index[i*(col - 1) * 6 + j * 6 + 3] = i*col + j + 1;
			index[i*(col - 1) * 6 + j * 6 + 4] = (i + 1)*col + j;
			index[i*(col - 1) * 6 + j * 6 + 5] = (i + 1)*col + j + 1;
		}
	}
}

glm::vec3 cloth::translate(glm::vec3 newOrigin)
{
	glm::vec3 origin;
	if (row != 0 && col != 0)
	{
		origin = position[0][0];
		glm::vec3 direct = newOrigin - origin;
		for (int i = 0; i < row; i++)
		{
			for (int j = 0; j < col; j++)
			{
				position[i][j] += direct;
			}
		}
	}
	return origin;
}

void cloth::posMapping(float minX, float minY, float maxX, float maxY, float minZ, float maxZ)
{
	if (minX == maxX || minY == maxY || minZ==maxZ)
	{
		cout << "ERROR: RANGE ERROR MINX==MAXX OR MINY==MAXY";
		return;
	}
	float originalWidth = col, originalHeight = row,originalDepth=2, 
		  newWidth = maxX - minX, newHeight = maxY - minY,newDepth=maxZ-minZ;
	float widthScale = originalWidth / newWidth, 
		  heightScale = originalHeight / newHeight,
		  depthScale=originalDepth/newDepth;
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			position[i][j].x = position[i][j].x / widthScale*newWidth+minX;
			position[i][j].y = position[i][j].y / heightScale*newHeight + minY;
			position[i][j].z = position[i][j].z / depthScale*newDepth + minZ;
		}
	}
}

glm::vec3 cloth::getSpringF(int pi, int pj, int qi, int qj, float k, float l)
{
	float distance = glm::distance(position[pi][pj], position[qi][qj]);
	glm::vec3 f = k*(l - distance)*((position[pi][pj] - position[qi][qj]) / distance);
	//测试
	
	//cout << "(" << pi << "," << pj << ") k: " << k<< endl;
	//cout << "(" << pi << "," << pj << ") l: " << l << endl;
	//cout << "(" << pi << "," << pj << ") distance: " << distance << endl;
	//cout << "(" << pi << "," << pj << ") and "<< "(" << qi << "," << qj << ") diff: " << (position[pi][pj] - position[qi][qj]).x<<" "<< (position[pi][pj] - position[qi][qj]).y <<" "<< (position[pi][pj] - position[qi][qj]).z << endl;
	

	return f;
}

glm::vec3 cloth::getStructuralSpringF(int i, int j)
{
	glm::vec3 f(0.0f,0.0f,0.0f);
	if (i - 1 >= 0)
	{
		f += getSpringF(i, j, i - 1, j, K[0], L[0]);
	}
	if (i + 1 < row)
	{
		f += getSpringF(i, j, i + 1, j, K[0], L[0]);
	}
	if (j - 1 >= 0)
	{
		f += getSpringF(i, j, i , j-1, K[0], L[0]);
	}
	if (j + 1 <col)
	{
		f += getSpringF(i, j, i, j + 1, K[0], L[0]);
	}
	return f;
}

glm::vec3 cloth::getShearSpringF(int i, int j)
{
	glm::vec3 f(0.0f, 0.0f, 0.0f);
	if (i - 1 >=0&&j-1>=0)
	{
		f += getSpringF(i, j, i - 1, j-1, K[1], L[1]);
	}
	if (i + 1 < row&&j+1<col)
	{
		f += getSpringF(i, j, i + 1, j+1, K[1], L[1]);
	}
	if (i+1<row&&j - 1 >= 0)
	{
		f += getSpringF(i, j, i+1, j - 1, K[1], L[1]);
	}
	if (i-1>=0&&j + 1 <col)
	{
		f += getSpringF(i, j, i-1, j + 1, K[1], L[1]);
	}
	return f;
}

glm::vec3 cloth::getFlexionSpringF(int i, int j)
{
	glm::vec3 f(0.0f, 0.0f, 0.0f);
	if (i - 2 >= 0)
	{
		f += getSpringF(i, j, i - 2, j, K[2], L[2]);
	}
	if (i + 2 < row)
	{
		f += getSpringF(i, j, i + 2, j, K[2], L[2]);
	}
	if (j - 2 >= 0)
	{
		f += getSpringF(i, j, i, j - 2, K[2], L[2]);
	}
	if (j + 2 <col)
	{
		f += getSpringF(i, j, i, j + 2, K[2], L[2]);
	}
	return f;
}

glm::vec3 cloth::getJoinSpringF(int i, int j)
{
	glm::vec3 f(0.0f, 0.0f, 0.0f);
	//测试
	glm::vec3 f1= getStructuralSpringF(i, j),f2= getShearSpringF(i, j),f3= getFlexionSpringF(i, j);
	f = f + getStructuralSpringF(i, j) + getShearSpringF(i, j) + getFlexionSpringF(i, j);
	//cout << "(" << i << "," << j << ") structural spring force: " << f1.x << " " << f1.y << " " << f1.z << endl;
	//cout << "(" << i << "," << j << ") shear spring force: " << f2.x << " " << f2.y << " " << f2.z << endl;
	//cout << "(" << i << "," << j << ") flexion spring force: " << f3.x << " " << f3.y << " " << f3.z << endl;
	return f;
}

glm::vec3 cloth::getGravity(int i=0, int j=0)
{
	return glm::vec3(0.0f,-1*mass*cg,0);
}

glm::vec3 cloth::getDampingF(int i, int j)
{
	glm::vec3 f = -1 * cd*velocity[i][j];
	return f;
}

glm::vec3 cloth::getNormalVec(int i, int j)
{
	// 顶点法向量等于与其相连的面的法向量之和
	// 这里设（i，j）的法向量等于他与上下左右四个顶点组成的面的法向量之和
	glm::vec3 normal1(0.0f,0.0f,0.0f), normal2(0.0f, 0.0f, 0.0f), normal3(0.0f, 0.0f, 0.0f), normal4(0.0f, 0.0f, 0.0f);
	glm::vec3 normal(0.0f, 0.0f, 0.0f);
	glm::vec3 direct1, direct2;
	// 法向量必须向外，根据右手定则确定叉乘的顺序
	if (i - 1 >= 0 && j + 1 < col)
	{
		direct1 = position[i - 1][j] - position[i][j];
		direct2 = position[i][j + 1] - position[i][j];
		normal1 = glm::cross(direct1, direct2);
		// 将法向量归一化
		normal1 = glm::normalize(normal1);
	}
	if (i + 1 < row&&j + 1 < col)
	{
		direct2 = position[i + 1][j] - position[i][j];
		direct1 = position[i][j + 1] - position[i][j];
		normal2 = glm::cross(direct1, direct2);
		normal2 = glm::normalize(normal2);
	}
	if (i + 1 < row&&j - 1 >= 0)
	{
		direct1 = position[i + 1][j] - position[i][j];
		direct2 = position[i][j - 1] - position[i][j];
		normal3 = glm::cross(direct1, direct2);
		normal3 = glm::normalize(normal3);
	}
	if (i - 1 >= 0 && j - 1 >= 0)
	{
		direct2 = position[i - 1][j] - position[i][j];
		direct1 = position[i][j - 1] - position[i][j];
		normal4 = glm::cross(direct1, direct2);
		normal4 = glm::normalize(normal4);
	}
	normal = normal + normal1 + normal2 + normal3 + normal4;
	return glm::normalize(normal);
}

glm::vec3 cloth::getViscousFluidF(int i, int j)
{
	glm::vec3 f(0.0f, 0.0f, 0.0f);
	glm::vec3 verticeNormal = getNormalVec(i, j);
	f = cv*glm::dot(glm::vec3(verticeNormal),(uFluid - velocity[i][j]))*verticeNormal;
	//cout << "(" << i << "," << j << ") normal: " << verticeNormal.x << " " << verticeNormal.y << " " << verticeNormal.z << endl;
	return f;
}

glm::vec3 cloth::getJoinForce(int i, int j)
{
	glm::vec3 f(0.0f, 0.0f, 0.0f);
	// 测试
	glm::vec3 jf= getJoinSpringF(i, j),g= getGravity(),d= getDampingF(i, j),v= getViscousFluidF(i, j);
	f = f + getJoinSpringF(i, j)  + getViscousFluidF(i, j);
	//f = f + getJoinSpringF(i,j);
	//cout << "(" << i << "," << j << ") join spring force: " << jf.x<<" "<<jf.y<<" "<<jf.z << endl;
	//cout << "(" << i << "," << j << ") gravity force: " << g.x << " " << g.y << " " << g.z << endl;
	//cout << "(" << i << "," << j << ") damping force: " << d.x << " " << d.y << " " << d.z << endl;
	//cout << "(" << i << "," << j << ") viscousfluid force: " << v.x << " " << v.y << " " << v.z << endl;
	return f;
}

glm::vec3 cloth::updateVelocity(int i, int j,float deltaT)
{
	glm::vec3 f = getJoinForce(i, j);
	glm::vec3 a = f / mass;
	glm::vec3 newV(velocity[i][j] + a*deltaT);
	//cout << "(" << i << "," << j << ") join force: " << f.x << " " << f.y << " " << f.z << endl;
	//cout << "(" << i << "," << j << ") a: " << a.x << " " << a.y << " " << a.z << endl;
	//cout << "(" << i << "," << j << ") new v: " << newV.x << " " << newV.y << " " << newV.z << endl;
	//velocity[i][j] = newV;
	return newV;
}

glm::vec3 cloth::updatePosition(int i, int j,float deltaT,glm::vec3 tempV)
{
	glm::vec3 newPos(position[i][j] + tempV * deltaT);
	//cout << "(" << i << "," << j << ") new p: " << newPos.x << " " << newPos.y << " " << newPos.z << endl<<endl;
	//position[i][j] = newPos;
	return newPos;
}
