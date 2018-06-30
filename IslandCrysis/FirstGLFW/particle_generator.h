
#pragma once
#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Camera.h"
#include "Shader.h"
#include <glm\gtc\matrix_transform.hpp>

#include "texture.h"

struct Particle {
	glm::vec3 Position;
	glm::vec3 Velocity = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec4 Color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	GLfloat Life;
	GLfloat gravityEffect;
	GLfloat rotation;
	GLfloat scale;
	GLfloat elapsedTime = 0;
};


class ParticleGenerator
{
public:
	// Constructor
	ParticleGenerator(Shader shader, GLuint amount);
	// Update all particles
	//void Update(GLfloat dt, GLuint newParticles, glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f));
	void Update(GLfloat dt, GLuint newParticles);
	// Render all particles
	void Draw(Camera& camera, float& deltaTime, glm::mat4 model, glm::mat4 view, glm::mat4 projection);
private:
	// State
	std::vector<Particle> particles;
	GLuint amount;
	// Render state
	Shader shader;
	GLuint VAO;
	// Initializes buffer and vertex attributes
	void init();
	// Returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
	GLuint firstUnusedParticle();
	// Respawns particle
	void respawnParticle(Particle &particle);
};







#endif
