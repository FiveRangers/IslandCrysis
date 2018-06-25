
#pragma once
#ifndef EXPLOSION_GENERATOR_H
#define EXPLOSION_GENERATOR_H
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include "Camera.h"
#include "Shader.h"
#include <glm\glm\gtc\matrix_transform.hpp>
#include "explosion.h"
#include "texture.h"

struct Explosion {
	glm::vec3 Position;
	glm::vec3 Velocity = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec4 Color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	GLfloat Life;
	GLfloat gravityEffect;
	GLfloat rotation;
	GLfloat scale;
	GLfloat elapsedTime = 0;
};


class ExplosionGenerator
{
public:
	// Constructor
	ExplosionGenerator(Shader shader, GLuint amount);
	// Update all explosions
	//void Update(GLfloat dt, GLuint newExplosions, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
	void Update(GLfloat dt, GLuint newExplosions);
	// Render all explosions
	void Draw(Camera& camera, float& deltaTime, glm::mat4 model, glm::mat4 view, glm::mat4 projection);
	GLfloat getRand();
private:
	// State
	std::vector<Explosion> explosions;
	std::vector<Explosion> fireworks;
	bool hasExplosion;
	GLuint amount;
	// Render state
	Shader shader;
	GLuint ExplosionVAO;
	GLuint FireworkVAO;
	// Initializes buffer and vertex attributes
	void init();
	// Returns the first Explosion index that's currently unused e.g. Life <= 0.0f or 0 if no explosion is currently inactive
	GLuint firstUnusedExplosion();
	// Respawns explosion
	void respawnExplosion(Explosion &explosion);
};







#endif
