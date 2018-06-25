
#include "explosion.h"

ExplosionGenerator::ExplosionGenerator(Shader shader, GLuint amount)
	: shader(shader), amount(amount)
{
	this->init();
}

void ExplosionGenerator::Update(GLfloat dt, GLuint newExplosions)
{
	// Add new explosions 


	for (GLuint i = 0; i < newExplosions; ++i)
	{

		if (explosions[i].Life < 0.0f) {
			hasExplosion = true;
			for (int j = 0; j < 100; j++) {
				fireworks[j].Position = explosions[i].Position;
				fireworks[j].Color = glm::vec4(glm::vec3(1.0f, 0.2f, 0.0f), 1.0f);
				fireworks[j].Life = 0.75f;
				fireworks[j].Velocity = glm::vec3(100 * getRand(), 100 * getRand(), 100 * getRand());
			}



			explosions[i].Position = glm::vec3(0.0f + getRand() * 50, 0.0f + getRand() * 50, 0.0f + getRand() * 50);
			explosions[i].Color = glm::vec4(glm::vec3(1.0f), 1.0f);
			explosions[i].Life = (getRand() + 3);
			explosions[i].Velocity = glm::vec3(200.0f + 100 * getRand(), 8.0f, 200.0f + 100 * getRand());
		}

	}

	// Update all explosions
	for (GLuint i = 0; i < this->amount; ++i)
	{
		Explosion &p = this->explosions[i];
		p.Life -= dt; // reduce life
		if (p.Life > 0.0f)
		{	// explosion is alive, thus update
			p.Position -= p.Velocity * dt;
			p.Color.a += dt * 2.5;
		}
	}
	for (GLuint j = 0; j < 100; ++j)
	{
		Explosion &p = this->fireworks[j];
		p.Life -= dt; // reduce life
		if (p.Life > 0.0f)
		{	// explosion is alive, thus update
			p.Position -= p.Velocity * dt;
			p.Color.a -= dt * 10000;
		}
	}

}

// Render all explosions
void ExplosionGenerator::Draw(Camera& camera, float& deltaTime, glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{
	// Use additive blending to give it a 'glow' effect
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	this->shader.use();
	for (Explosion explosion : this->explosions)
	{
		if (explosion.Life > 0.0f)
		{

			this->shader.setMat4("model", model);
			this->shader.setMat4("projection", projection);
			this->shader.setMat4("view", view);
			this->shader.setVec4("color", explosion.Color);
			this->shader.setVec3("offset", explosion.Position);
			glBindVertexArray(this->ExplosionVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}
	}


	for (Explosion firework : this->fireworks)
	{
		if (firework.Life > 0.0f)
		{

			this->shader.setMat4("model", model);
			this->shader.setMat4("projection", projection);
			this->shader.setMat4("view", view);
			this->shader.setVec4("color", firework.Color);
			this->shader.setVec3("offset", firework.Position);
			glBindVertexArray(this->FireworkVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}
	}
	

	// Don't forget to reset to default blending mode
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ExplosionGenerator::init()
{
	// Set up mesh and attribute properties
	GLuint ExplosionVBO;
	GLuint FireWorkVBO;
	GLfloat explosion_quad[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	glGenVertexArrays(1, &this->ExplosionVAO);
	glGenBuffers(1, &ExplosionVBO);
	glBindVertexArray(this->ExplosionVAO);
	// Fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, ExplosionVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(explosion_quad), explosion_quad, GL_STATIC_DRAW);
	// Set mesh attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	glGenVertexArrays(1, &this->FireworkVAO);
	glGenBuffers(1, &FireWorkVBO);
	glBindVertexArray(this->FireworkVAO);
	// Fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, FireWorkVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(explosion_quad), explosion_quad, GL_STATIC_DRAW);
	// Set mesh attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);


	// Create this->amount default explosion instances
	for (GLuint i = 0; i < this->amount; ++i)
		this->explosions.push_back(Explosion());

	for (GLuint i = 0; i < 100; ++i)
		this->fireworks.push_back(Explosion());

}

// Stores the index of the last explosion used (for quick access to next dead explosion)
GLuint lastUsedExplosion = 0;
GLuint ExplosionGenerator::firstUnusedExplosion()
{
	// First search from last used explosion, this will usually return almost instantly
	for (GLuint i = lastUsedExplosion; i < this->amount; ++i) {
		if (this->explosions[i].Life <= 0.0f) {
			lastUsedExplosion = i;
			return i;
		}
	}
	// Otherwise, do a linear search
	for (GLuint i = 0; i < lastUsedExplosion; ++i) {
		if (this->explosions[i].Life <= 0.0f) {
			lastUsedExplosion = i;
			return i;
		}
	}
	// All explosions are taken, override the first one (note that if it repeatedly hits this case, more explosions should be reserved)
	lastUsedExplosion = 0;
	return 0;
}

GLfloat ExplosionGenerator::getRand() {
	return ((rand() % 100) - 50) / 50.0f;
}

