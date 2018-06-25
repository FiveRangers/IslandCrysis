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

	Shader ModelShader("ModelShader.vert", "ModelShader.frag");

	Shader MoonShader("./MoonShader.vert", "./MoonShader.frag");

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

	Shader skyboxShader("skybox.vert", "skybox.frag");

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

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	//particle system
	Shader ParticleShader("ParticleShader.vert", "ParticleShader.frag");

	ParticleGenerator* Particles;
	Particles = new ParticleGenerator(ParticleShader, 20);

	ExplosionGenerator* explosions;
	explosions = new ExplosionGenerator(ParticleShader, 1);


	while (!glfwWindowShouldClose(Mywindow)) {
		//定义变量
		//光源初始位置
		static float Light_X = -2.0f;
		static float Light_Y = 4.0f;
		static float Light_Z = -1.0f;
		static int ProjectionMode = 0;

		//获取帧间时间
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//处理输入
		processInput(Mywindow);

		//清屏为白色
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// moon
		MoonShader.use();
		glm::mat4 projection = glm::perspective(camera.Zoom, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		MoonShader.setMat4("projection", projection);
		MoonShader.setMat4("view", view);
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(200.0f, 200.0f, -100.0f));
		//model = glm::rotate(model, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		MoonShader.setMat4("model", model);
		moon.Draw(MoonShader);
		
		// fire
		ModelShader.use();
		projection = glm::perspective(camera.Zoom, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		view = camera.GetViewMatrix();
		ModelShader.setMat4("projection", projection);
		ModelShader.setMat4("view", view);
		ModelShader.setVec3("viewPos", camera.Position);
		ModelShader.setFloat("ambient_str", 0.35f);
		ModelShader.setFloat("diffuse_str", 0.6f);
		ModelShader.setFloat("specular_str", 0.0f);
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(5.0f, 1.0f, 20.0f));
		model = glm::rotate(model, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		ModelShader.setMat4("model", model);
		fire.Draw(ModelShader);

		// seabird
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(sin(glfwGetTime()) * 32.0f, 22.0f + sin(glfwGetTime() / 2.0) * 17.0f, sin(glfwGetTime() * 1.5) * 25.0f));
		model = glm::rotate(model, 5.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
		ModelShader.setMat4("model", model);
		seabird.Draw(ModelShader);

		// fence
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(3.0f, 8.5f, -2.0f));
		model = glm::rotate(model, -15.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.0005f, 0.0005f, 0.0005f));
		ModelShader.setMat4("model", model);
		fence.Draw(ModelShader);

		// fossil
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(10.0f, 0.0f, 27.5f));
		model = glm::rotate(model, -15.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.018f, 0.018f, 0.018f));
		ModelShader.setMat4("model", model);
		fossil.Draw(ModelShader);

		// rabbit
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(1.52f, 8.5f, -0.5f));
		model = glm::rotate(model, 60.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.011f, 0.011f, 0.011f));
		ModelShader.setMat4("model", model);
		rabbit.Draw(ModelShader);

		// treasure
		ModelShader.setFloat("specular_str", 3.0f);
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-17.0f, 0.5f, 31.0f));
		model = glm::rotate(model, -100.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -15.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.003f, 0.003f, 0.003f));
		ModelShader.setMat4("model", model);
		treasure.Draw(ModelShader);

		// dolphin
		ModelShader.setFloat("specular_str", 0.4f);
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-100.0f, 1.5f, 32.0f));
		model = glm::rotate(model, 35.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(7.0f, 5.0f, 5.0f));
		ModelShader.setMat4("model", model);
		dolphin.Draw(ModelShader);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-80.0f, 2.5f, 15.0f));
		model = glm::rotate(model, -35.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		ModelShader.setMat4("model", model);
		dolphin.Draw(ModelShader);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-110.0f, 0.5f, 2.0f));
		model = glm::rotate(model, -20.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(7.0f, 7.0f, 7.0f));
		ModelShader.setMat4("model", model);
		dolphin.Draw(ModelShader);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-90.0f, 1.5f, -13.0f));
		model = glm::rotate(model, 15.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(6.0f, 3.0f, 5.0f));
		ModelShader.setMat4("model", model);
		dolphin.Draw(ModelShader);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-120.0f, 2.5f, -23.0f));
		model = glm::rotate(model, -15.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(6.0f, 5.0f, 4.0f));
		ModelShader.setMat4("model", model);
		dolphin.Draw(ModelShader);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-100.0f, 2.5f, -32.0f));
		model = glm::rotate(model, -45.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		ModelShader.setMat4("model", model);
		dolphin.Draw(ModelShader);

		// ship
		ModelShader.setFloat("specular_str", 0.6f);
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(10.0f, -2.0f, -182.0f));
		model = glm::rotate(model, 75.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.025f, 0.025f, 0.025f));
		ModelShader.setMat4("model", model);
		ship.Draw(ModelShader);

		// whale
		ModelShader.setFloat("specular_str", 0.3f);
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(160.0f, -10.0f, 2.0f));
		model = glm::rotate(model, -85.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		ModelShader.setMat4("model", model);
		whale.Draw(ModelShader);
		
		// island
		ModelShader.setFloat("specular_str", 0.0f);
		model = glm::mat4();
		model = glm::scale(model, glm::vec3(0.28f, 0.28f, 0.28f));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		ModelShader.setMat4("model", model);
		island.Draw(ModelShader);

		//particle
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(4.9f, 1.5f, 28.1f));
		model = glm::scale(model, glm::vec3(0.08f, 0.08f, 0.08f));
		Particles->Update(0.05f, 1000);
		Particles->Draw(camera, deltaTime, model, view, projection);

		//explosion

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(200.0f, 400.0f, 200.f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		explosions->Update(0.01f, 1);
		explosions->Draw(camera, deltaTime, model, view, projection);


		// draw skybox
		glDepthFunc(GL_LEQUAL);
		skyboxShader.use();
		view = camera.GetViewMatrix();
		//view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);

		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);

		glfwSwapBuffers(Mywindow);
		glfwPollEvents();
	}

	glfwTerminate();
	
	return 0;
}

