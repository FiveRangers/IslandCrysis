#pragma once
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <iostream>
#include <vector>
#include <math.h>
#include <memory>

using std::vector;
using std::shared_ptr;
using std::unique_ptr;
using std::cout;
using std::endl;

// cloth 类表示n*n的布
class cloth {
public:
	cloth();
	cloth(int mrow, int mcol, float mmass = 1.0, float *mk = nullptr, float *ml = nullptr, float g = 9.8f, float v = 1.0f, float d = 1.0f, glm::vec3 muFluid = glm::vec3(10.0f, 10.0f, 0.0f));
	cloth(const cloth& c);
	// 执行布料模拟,返回更新之后的布料顶点
	shared_ptr<float> Simulation();
	// 跟其他类统一一下接口，做的东西跟simualtion差不多
	void Update(float *);

	int getRow();
	int getCol();

	// 返回位置矩阵
	vector<vector<glm::vec3>> getAllPositon();
	// 返回速度矩阵
	vector<vector<glm::vec3>> getAllVelocity();
	// 返回法向量矩阵
	vector<vector<glm::vec3>> getAllNormal();

	// 返回点（i，j）的位置
	glm::vec3 getPosition(int i, int j);
	// 设置点（i，j）的位置
	void setPosition(int i, int j, glm::vec3 p);

	// 返回点（i，j）的法向量
	glm::vec3 getNormal(int i, int j);

	// 返回点（i，j）的速度
	glm::vec3 getVelocity(int i, int j);
	// 设置点（i，j）的速度
	void setVelocity(int i, int j, glm::vec3 v);

	float getCg();
	void setCg(const float& g);

	float getCv();
	void setCv(const float& v);

	float getCd();
	void setCd(const float& d);

	glm::vec3 getUFluid();
	void setUFluid(glm::vec3);

	// 将二维vector中的数据转换到一维数组中
	shared_ptr<float> getVerticeArray(const vector<vector<glm::vec3>>&);
	shared_ptr<float> getVerticeArray();
	void getVerticeArray(float *);
	// 获得索引数组
	shared_ptr<unsigned int> getIndexArray();
	void getIndexArray(unsigned int*);

	// 将布料的原点平移到某个点,布料原点为左下角点
	glm::vec3 translate(glm::vec3);
	// 将网格中的质点坐标映射到一个新的范围
	void posMapping(float minX=-1, float minY=-1, float minZ=-1,float maxX=1, float maxY=1,float maxZ=1);

	// 以下这些成员函数应该设置为private
private:
	// 计算点p和点q的弹簧力
	glm::vec3 getSpringF(int pi, int pj, int qi,int qj,float k, float l);
	// 计算点的结构弹簧力
	glm::vec3 getStructuralSpringF(int i, int j);
	// 计算点的剪切弹簧力
	glm::vec3 getShearSpringF(int i, int j);
	// 计算点的弯曲弹簧力
	glm::vec3 getFlexionSpringF(int i, int j);
	// 计算总的弹簧力
	glm::vec3 getJoinSpringF(int i, int j);
	// 计算重力
	glm::vec3 getGravity(int i, int j);
	// 计算damping force
	glm::vec3 getDampingF(int i, int j);
	// 计算某个顶点的法向量
	glm::vec3 getNormalVec(int i, int j);
	// 计算viscous force 
	glm::vec3 getViscousFluidF(int i, int j);
	// 计算合力
	glm::vec3 getJoinForce(int i, int j);
	// 更新速度
	glm::vec3 updateVelocity(int i, int j,float deltaT);
	// 更新位置
	glm::vec3 updatePosition(int i, int j,float deltaT,glm::vec3);
private:
	vector<vector<glm::vec3>> position;
	vector<vector<glm::vec3>> velocity;
	vector<vector<glm::vec3>> normal;
	int row;
	int col;
	// 每个点的质量
	float mass;
	// 每个质点的重力
	// 弹簧的刚性系数
	float K[3];
	// 每根弹簧的自然长度
	float L[3];
	// 重力常数
	float cg;
	// 缓冲力常量系数
	float cd;
	// 流体力常量系数
	float cv;
	// 流体速度
	glm::vec3 uFluid;
};

