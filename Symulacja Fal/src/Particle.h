#pragma once

#include <vector>

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"

class Particle {
private:
	std::vector<unsigned int> poly;
	glm::vec3 start;
	float angle;
	float alpha = 1;
	int x = 0;
	int y = 0;
public:
	Particle(float angle, int cx, int cy);

	void PolUpdate(int r, int cx, int cy);
	float Alpha(int r);

	inline float GetAlpha() const { return alpha; }
	inline float GetAngle() const { return angle; }
	inline std::vector<unsigned int> GetPoly() const { return poly; }
};
