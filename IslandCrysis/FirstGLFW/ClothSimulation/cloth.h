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

// cloth ���ʾn*n�Ĳ�
class cloth {
public:
	cloth();
	cloth(int mrow, int mcol, float mmass = 1.0, float *mk = nullptr, float *ml = nullptr, float g = 9.8f, float v = 1.0f, float d = 1.0f, glm::vec3 muFluid = glm::vec3(10.0f, 10.0f, 0.0f));
	cloth(const cloth& c);
	// ִ�в���ģ��,���ظ���֮��Ĳ��϶���
	shared_ptr<float> Simulation();
	// ��������ͳһһ�½ӿڣ����Ķ�����simualtion���
	void Update(float *);

	int getRow();
	int getCol();

	// ����λ�þ���
	vector<vector<glm::vec3>> getAllPositon();
	// �����ٶȾ���
	vector<vector<glm::vec3>> getAllVelocity();
	// ���ط���������
	vector<vector<glm::vec3>> getAllNormal();

	// ���ص㣨i��j����λ��
	glm::vec3 getPosition(int i, int j);
	// ���õ㣨i��j����λ��
	void setPosition(int i, int j, glm::vec3 p);

	// ���ص㣨i��j���ķ�����
	glm::vec3 getNormal(int i, int j);

	// ���ص㣨i��j�����ٶ�
	glm::vec3 getVelocity(int i, int j);
	// ���õ㣨i��j�����ٶ�
	void setVelocity(int i, int j, glm::vec3 v);

	float getCg();
	void setCg(const float& g);

	float getCv();
	void setCv(const float& v);

	float getCd();
	void setCd(const float& d);

	glm::vec3 getUFluid();
	void setUFluid(glm::vec3);

	// ����άvector�е�����ת����һά������
	shared_ptr<float> getVerticeArray(const vector<vector<glm::vec3>>&);
	shared_ptr<float> getVerticeArray();
	void getVerticeArray(float *);
	// �����������
	shared_ptr<unsigned int> getIndexArray();
	void getIndexArray(unsigned int*);

	// �����ϵ�ԭ��ƽ�Ƶ�ĳ����,����ԭ��Ϊ���½ǵ�
	glm::vec3 translate(glm::vec3);
	// �������е��ʵ�����ӳ�䵽һ���µķ�Χ
	void posMapping(float minX=-1, float minY=-1, float minZ=-1,float maxX=1, float maxY=1,float maxZ=1);

	// ������Щ��Ա����Ӧ������Ϊprivate
private:
	// �����p�͵�q�ĵ�����
	glm::vec3 getSpringF(int pi, int pj, int qi,int qj,float k, float l);
	// �����Ľṹ������
	glm::vec3 getStructuralSpringF(int i, int j);
	// �����ļ��е�����
	glm::vec3 getShearSpringF(int i, int j);
	// ����������������
	glm::vec3 getFlexionSpringF(int i, int j);
	// �����ܵĵ�����
	glm::vec3 getJoinSpringF(int i, int j);
	// ��������
	glm::vec3 getGravity(int i, int j);
	// ����damping force
	glm::vec3 getDampingF(int i, int j);
	// ����ĳ������ķ�����
	glm::vec3 getNormalVec(int i, int j);
	// ����viscous force 
	glm::vec3 getViscousFluidF(int i, int j);
	// �������
	glm::vec3 getJoinForce(int i, int j);
	// �����ٶ�
	glm::vec3 updateVelocity(int i, int j,float deltaT);
	// ����λ��
	glm::vec3 updatePosition(int i, int j,float deltaT,glm::vec3);
private:
	vector<vector<glm::vec3>> position;
	vector<vector<glm::vec3>> velocity;
	vector<vector<glm::vec3>> normal;
	int row;
	int col;
	// ÿ���������
	float mass;
	// ÿ���ʵ������
	// ���ɵĸ���ϵ��
	float K[3];
	// ÿ�����ɵ���Ȼ����
	float L[3];
	// ��������
	float cg;
	// ����������ϵ��
	float cd;
	// ����������ϵ��
	float cv;
	// �����ٶ�
	glm::vec3 uFluid;
};

