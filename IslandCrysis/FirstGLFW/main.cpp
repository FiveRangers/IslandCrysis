#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include <iostream>
#include "imconfig.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw_gl3.h"
#include <vector>
#include <math.h>
#include <glm\glm.hpp>
#include "Shader.h"
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include "Camera.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "model.h"
#include "particle_generator.h"
#include "explosion.h"

using namespace std;

//GLFW窗口大小
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

unsigned int MAX_BUFFER = SCR_WIDTH * SCR_HEIGHT;

//初始化相机
Camera camera(glm::vec3(0.0f, 10.0f, 65.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//计时
float deltaTime = 0.0f;	//当前帧和最后一帧之间的时间
float lastFrame = 0.0f;

unsigned int quadVAO = 0;
unsigned int quadVBO;

void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

//鼠标移动时调用
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset * 3, yoffset * 3);
}

//鼠标滚轮移动时调用
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}

//窗口大小变化时调用
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

//定义键盘操作输入
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime * 3);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime * 3);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime * 3);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime * 3);
}

unsigned int loadCubemap(vector<string> faces) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++) {
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			cout << "Cubemap texture failed to load at path:" << faces[i] << endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

int main() {
	//初始化GLFW
	if (!glfwInit()) {
		return -1;
	}
	//配置GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//核心模式
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* Mywindow = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "IslandCrysis", NULL, NULL);
	
	//设定指针和滚轮的回调函数
	glfwSetCursorPosCallback(Mywindow, mouse_callback);
	glfwSetScrollCallback(Mywindow, scroll_callback);

	//告诉GLFW捕捉鼠标，隐藏鼠标指针图标
	glfwSetInputMode(Mywindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//设定当前窗口并设定窗口大小变化时的回调函数
	glfwMakeContextCurrent(Mywindow);
	glfwSetFramebufferSizeCallback(Mywindow, framebuffer_size_callback);

	//初始化GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//初始化ImGui
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfwGL3_Init(Mywindow, true);
	ImGui::StyleColorsDark();

	//打开深度测试
	glEnable(GL_DEPTH_TEST);

	//Shader ModelShader("ModelShader.vert", "ModelShader.frag");

	Shader MoonShader("./Shader/MoonShader.vert", "./Shader/MoonShader.frag");

	//阴影映射shader
	Shader shader("./Shader/shadow_mappingVS.glsl", "./Shader/shadow_mappingFS.glsl");
	Shader islandShader("./Shader/shadow_mappingVS.glsl", "./Shader/shadow_mappingFS_dolphin.glsl");
	//深度shader
	Shader DepthShader("./Shader/DepthVS.glsl", "./Shader/DepthFS.glsl");
	// debug
	Shader debugShader("Shader/DebugVS.glsl", "Shader/DebugFS.glsl");

	Model island("./resources/Small_Tropical_Island/Small_Tropical_Island.obj");
	Model fire("./resources/fire/fire.obj");
	Model moon("./resources/moon/Moon.obj");
	Model seabird("./resources/seabird/seabird.obj");
	Model fence("./resources/fence/fence.obj");
	Model fossil("./resources/fossil/fossil.obj");
	Model rabbit("./resources/rabbit/rabbit.obj");
	Model treasure("./resources/treasure/treasure.obj");
	Model dolphin("./resources/dolphin/dolphin.obj");
	Model ship("./resources/ship/ship.obj");
	Model whale("./resources/whale/whale.obj");

	float skyboxVertices[] = {
		// positions          
		-1000.0f,  1000.0f, -1000.0f,
		-1000.0f, -1000.0f, -1000.0f,
		1000.0f, -1000.0f, -1000.0f,
		1000.0f, -1000.0f, -1000.0f,
		1000.0f,  1000.0f, -1000.0f,
		-1000.0f,  1000.0f, -1000.0f,

		-1000.0f, -1000.0f,  1000.0f,
		-1000.0f, -1000.0f, -1000.0f,
		-1000.0f,  1000.0f, -1000.0f,
		-1000.0f,  1000.0f, -1000.0f,
		-1000.0f,  1000.0f,  1000.0f,
		-1000.0f, -1000.0f,  1000.0f,

		1000.0f, -1000.0f, -1000.0f,
		1000.0f, -1000.0f,  1000.0f,
		1000.0f,  1000.0f,  1000.0f,
		1000.0f,  1000.0f,  1000.0f,
		1000.0f,  1000.0f, -1000.0f,
		1000.0f, -1000.0f, -1000.0f,

		-1000.0f, -1000.0f,  1000.0f,
		-1000.0f,  1000.0f,  1000.0f,
		1000.0f,  1000.0f,  1000.0f,
		1000.0f,  1000.0f,  1000.0f,
		1000.0f, -1000.0f,  1000.0f,
		-1000.0f, -1000.0f,  1000.0f,

		-1000.0f,  1000.0f, -1000.0f,
		1000.0f,  1000.0f, -1000.0f,
		1000.0f,  1000.0f,  1000.0f,
		1000.0f,  1000.0f,  1000.0f,
		-1000.0f,  1000.0f,  1000.0f,
		-1000.0f,  1000.0f, -1000.0f,

		-1000.0f, -1000.0f, -1000.0f,
		-1000.0f, -1000.0f,  1000.0f,
		1000.0f, -1000.0f, -1000.0f,
		1000.0f, -1000.0f, -1000.0f,
		-1000.0f, -1000.0f,  1000.0f,
		1000.0f, -1000.0f,  1000.0f
	};

	// 初始化一块15*15的布料
	cloth flag(15, 15);
	// 获取布料的顶点数组指针
	float * flagVertices = new float[flag.getRow()*flag.getCol() * 3];
	flag.getVerticeArray(flagVertices);

	// flag VAO and VBO
	unsigned int flagVAO, flagVBO;
	glGenVertexArrays(1, &flagVAO);
	glGenBuffers(1, &flagVBO);
	glBindVertexArray(flagVAO);
	glBindBuffer(GL_ARRAY_BUFFER, flagVBO);
	glBufferData(GL_ARRAY_BUFFER, flag.getRow()*flag.getCol() * 3 * sizeof(float), flagVertices, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// 获取布料的索引数组指针
	shared_ptr<unsigned int> indexPtr = flag.getIndexArray();
	unsigned int *index = new unsigned int[(flag.getRow() - 1)*(flag.getCol() - 1) * 2 * 3];
	flag.getIndexArray(index);

	// flag EBO
	unsigned int flagEBO;
	glGenBuffers(1, &flagEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, flagEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, flag.getRow()*flag.getCol() * 6 * sizeof(unsigned int), index, GL_STATIC_DRAW);

	Shader flagShader("Shader/FlagVS.glsl", "Shader/FlagFS.glsl");

	Shader skyboxShader("Shader/skybox.vert", "Shader/skybox.frag");

	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// load textures
	vector<string> faces = {
		"./resources/img/ame_nebula/purplenebula_lf.tga",
		"./resources/img/ame_nebula/purplenebula_rt.tga",
		"./resources/img/ame_nebula/purplenebula_up.tga",
		"./resources/img/ame_nebula/purplenebula_dn.tga",
		"./resources/img/ame_nebula/purplenebula_ft.tga",
		"./resources/img/ame_nebula/purplenebula_bk.tga"
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	//创建深度贴图
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//深度纹理作为帧缓冲的深度缓冲
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//配置着色器
	//shader.use();
	//shader.setInt("diffuseTexture", 0);
	//shader.setInt("shadowMap", 8);


	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	//particle system
	Shader ParticleShader("Shader/ParticleShader.vert", "Shader/ParticleShader.frag");

	ParticleGenerator* Particles;
	Particles = new ParticleGenerator(ParticleShader, 20);

	ExplosionGenerator* explosions;
	explosions = new ExplosionGenerator(ParticleShader, 1);

	while (!glfwWindowShouldClose(Mywindow)) {
		//定义变量
		//光源初始位置
		static float Light_X = 100.0f;
		static float Light_Y = 95.0f;
		static float Light_Z = -35.0f;
		static int ProjectionMode = 0;

		//获取帧间时间
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//处理输入
		processInput(Mywindow);
		//cout << camera.Position.x << ", "<<camera.Position.y <<", " << camera.Position.z<<endl;

		//清屏为白色
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//------------------------------------new------------------------------------//
		//从光源角度渲染
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 1.0f, far_plane = 500.0f;
		if (ProjectionMode == 0) {
			shader.setBool("isOrtho", true);
			shader.setFloat("near_plane", near_plane);
			shader.setFloat("far_plane", far_plane);
			lightProjection = glm::ortho(-250.0f, 250.0f, -250.0f, 250.0f, near_plane, far_plane);
		}
		else {
			shader.setBool("isOrtho", false);
			shader.setFloat("near_plane", near_plane);
			shader.setFloat("far_plane", far_plane);
			lightProjection = glm::perspective(45.0f, 1.0f, 0.1f, 1000.0f);
		}
		lightView = glm::lookAt(glm::vec3(Light_X, Light_Y, Light_Z), glm::vec3(0.0f), glm::vec3(0.0f,1.0f, 0.0f));
		lightSpaceMatrix = lightProjection * lightView;

		DepthShader.use();
		DepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		//fire
		glm::mat4 model;
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(5.0f, 1.0f, 20.0f));
		model = glm::rotate(model, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		DepthShader.setMat4("model", model);
		fire.Draw(DepthShader);
		// seabird
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(sin(glfwGetTime()) * 32.0f, 22.0f + sin(glfwGetTime() / 2.0) * 17.0f, sin(glfwGetTime() * 1.5) * 25.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		DepthShader.setMat4("model", model);
		seabird.Draw(DepthShader);
		// fence
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(3.0f, 8.5f, -2.0f));
		model = glm::rotate(model, -15.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0005f, 0.0005f, 0.0005f));
		DepthShader.setMat4("model", model);
		fence.Draw(DepthShader);
		// fossil
		//model = glm::mat4();
		model = glm::translate(model, glm::vec3(10.0f, 0.0f, 27.5f));
		model = glm::rotate(model, -15.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.018f, 0.018f, 0.018f));
		DepthShader.setMat4("model", model);
		fossil.Draw(DepthShader);
		// rabbit
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(1.52f, 8.5f, -0.5f));
		model = glm::rotate(model, 60.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.011f, 0.011f, 0.011f));
		DepthShader.setMat4("model", model);
		rabbit.Draw(DepthShader);
		// treasure
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-17.0f, 0.5f, 31.0f));
		model = glm::rotate(model, -100.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -15.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.003f, 0.003f, 0.003f));
		DepthShader.setMat4("model", model);
		treasure.Draw(DepthShader);
		// dolphin
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-100.0f, 1.5f, 32.0f));
		model = glm::rotate(model, 35.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(7.0f, 5.0f, 5.0f));
		DepthShader.setMat4("model", model);
		dolphin.Draw(DepthShader);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-80.0f, 2.5f, 15.0f));
		model = glm::rotate(model, -35.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		DepthShader.setMat4("model", model);
		dolphin.Draw(DepthShader);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-110.0f, 0.5f, 2.0f));
		model = glm::rotate(model, -20.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(7.0f, 7.0f, 7.0f));
		DepthShader.setMat4("model", model);
		dolphin.Draw(DepthShader);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-90.0f, 1.5f, -13.0f));
		model = glm::rotate(model, 15.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(6.0f, 3.0f, 5.0f));
		DepthShader.setMat4("model", model);
		dolphin.Draw(DepthShader);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-120.0f, 2.5f, -23.0f));
		model = glm::rotate(model, -15.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(6.0f, 5.0f, 4.0f));
		DepthShader.setMat4("model", model);
		dolphin.Draw(DepthShader);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-100.0f, 2.5f, -32.0f));
		model = glm::rotate(model, -45.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		DepthShader.setMat4("model", model);
		dolphin.Draw(DepthShader);
		// ship
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(10.0f, -2.0f, -182.0f));
		model = glm::rotate(model, 75.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.025f, 0.025f, 0.025f));
		DepthShader.setMat4("model", model);
		ship.Draw(DepthShader);
		// whale
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(160.0f, -10.0f, 2.0f));
		model = glm::rotate(model, -85.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		DepthShader.setMat4("model", model);
		whale.Draw(DepthShader);
		// island
		model = glm::mat4();
		model = glm::scale(model, glm::vec3(0.28f, 0.28f, 0.28f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		DepthShader.setMat4("model", model);
		island.Draw(DepthShader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//重新设置视口
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//debugShader.use();
		//debugShader.setInt("depthMap", 8);
		//debugShader.setFloat("near_plane", near_plane);
		//debugShader.setFloat("far_plane", far_plane);
		//glActiveTexture(GL_TEXTURE8);
		//glBindTexture(GL_TEXTURE_2D, depthMap);
		//renderQuad();

		//利用深度贴图渲染场景
		shader.use();
		glm::mat4 projection = glm::perspective(camera.Zoom, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		//设定光照变量
		shader.setVec3("viewPos", camera.Position);
		shader.setVec3("lightPos", glm::vec3(Light_X, Light_Y, Light_Z));
		shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		shader.setInt("shadowMap", 8);
		//fire
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(5.0f, 1.0f, 20.0f));
		model = glm::rotate(model, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		shader.setMat4("model", model);
		fire.Draw(shader);
		// seabird
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(sin(glfwGetTime()) * 32.0f, 22.0f + sin(glfwGetTime() / 2.0) * 17.0f, sin(glfwGetTime() * 1.5) * 25.0f));
		// model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		shader.setMat4("model", model);
		seabird.Draw(shader);
		// fence
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(3.0f, 8.5f, -2.0f));
		model = glm::rotate(model, -15.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0005f, 0.0005f, 0.0005f));
		shader.setMat4("model", model);
		fence.Draw(shader);
		// fossil
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(10.0f, 0.0f, 27.5f));
		model = glm::rotate(model, -15.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.018f, 0.018f, 0.018f));
		shader.setMat4("model", model);
		fossil.Draw(shader);
		// rabbit
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(1.52f, 8.5f, -0.5f));
		model = glm::rotate(model, 60.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.011f, 0.011f, 0.011f));
		shader.setMat4("model", model);
		rabbit.Draw(shader);
		// treasure
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-17.0f, 0.5f, 31.0f));
		model = glm::rotate(model, -100.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -15.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.003f, 0.003f, 0.003f));
		shader.setMat4("model", model);
		treasure.Draw(shader);
		// dolphin
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-100.0f, 1.5f, 32.0f));
		model = glm::rotate(model, 35.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(7.0f, 5.0f, 5.0f));
		shader.setMat4("model", model);
		dolphin.Draw(shader);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-80.0f, 2.5f, 15.0f));
		model = glm::rotate(model, -35.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		shader.setMat4("model", model);
		dolphin.Draw(shader);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-110.0f, 0.5f, 2.0f));
		model = glm::rotate(model, -20.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(7.0f, 7.0f, 7.0f));
		shader.setMat4("model", model);
		dolphin.Draw(shader);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-90.0f, 1.5f, -13.0f));
		model = glm::rotate(model, 15.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(6.0f, 3.0f, 5.0f));
		shader.setMat4("model", model);
		dolphin.Draw(shader);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-120.0f, 2.5f, -23.0f));
		model = glm::rotate(model, -15.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(6.0f, 5.0f, 4.0f));
		shader.setMat4("model", model);
		dolphin.Draw(shader);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-100.0f, 2.5f, -32.0f));
		model = glm::rotate(model, -45.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		shader.setMat4("model", model);
		dolphin.Draw(shader);

		// ship
		shader.use();
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(10.0f, -2.0f, -182.0f));
		model = glm::rotate(model, 75.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.025f, 0.025f, 0.025f));
		shader.setMat4("model", model);
		ship.Draw(shader);

		// whale
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(160.0f, -10.0f, 2.0f));
		model = glm::rotate(model, -85.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		shader.setMat4("model", model);
		whale.Draw(shader);

		// island
		islandShader.use();
		islandShader.use();
		islandShader.setFloat("near_plane", near_plane);
		islandShader.setFloat("far_plane", far_plane);
		islandShader.setMat4("projection", projection);
		islandShader.setMat4("view", view);
		islandShader.setVec3("viewPos", camera.Position);
		islandShader.setVec3("lightPos", glm::vec3(Light_X, Light_Y, Light_Z));
		islandShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		islandShader.setInt("shadowMap", 8);
		model = glm::mat4();
		model = glm::scale(model, glm::vec3(0.28f, 0.28f, 0.28f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		islandShader.setMat4("model", model);
		island.Draw(islandShader);

		// moon
		MoonShader.use();
		projection = glm::perspective(camera.Zoom, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		view = camera.GetViewMatrix();
		MoonShader.setMat4("projection", projection);
		MoonShader.setMat4("view", view);
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(200.0f, 200.0f, -100.0f));
		//model = glm::rotate(model, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		MoonShader.setMat4("model", model);
		moon.Draw(MoonShader);

		//particle
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(4.9f, 1.5f, 28.1f));
		model = glm::scale(model, glm::vec3(0.08f, 0.08f, 0.08f));
		Particles->Update(0.05f, 1000);
		Particles->Draw(camera, deltaTime, model, view, projection);

		//explosion
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(60.0f, 50.0f, 0.f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		explosions->Update(0.01f, 1);
		explosions->Draw(camera, deltaTime, model, view, projection);

		// draw skybox
		glDepthFunc(GL_LEQUAL);
		skyboxShader.use();
	    view = camera.GetViewMatrix();
		//view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		skyboxShader.setMat4("view", view);
		projection = glm::perspective(camera.Zoom, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		skyboxShader.setMat4("projection", projection);

		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);

		// cloth simulation
		flagShader.use();
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(8.0f, 10.0f, 13.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		flagShader.setMat4("model", model);
		flagShader.setMat4("view", view);
		flagShader.setMat4("projection", projection);
		flagShader.setVec4("color", glm::vec4(1.0f, 0.0f,0.0f, 0.0f));
		glBindVertexArray(flagVAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, flagEBO);
		glDrawElements(GL_TRIANGLES, (flag.getRow()- 1)*(flag.getCol() - 1) * 6, GL_UNSIGNED_INT, 0);

		flag.Update(flagVertices);
		// 更新缓存中的织物顶顶点数据
		glBindBuffer(GL_ARRAY_BUFFER, flagVBO);
		glBufferData(GL_ARRAY_BUFFER, flag.getRow()*flag.getCol() * 3 * sizeof(float), flagVertices, GL_DYNAMIC_DRAW);

		glfwSwapBuffers(Mywindow);
		glfwPollEvents();
	}

	glfwTerminate();
	
	return 0;
}

