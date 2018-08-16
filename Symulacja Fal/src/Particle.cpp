#include "Particle.h"

#include <math.h>
#include <map>
#include <iostream>

#define PI		3.14159265358979323846

Particle::Particle(float angle, int cx, int cy) {
	for (int i = 0; i < 6; i++) { poly.push_back(0); }
	this->angle = angle;
	start.x = cx;
	start.y = cy;
	start.z = 0;
}

void Particle::PolUpdate(int r, int cx, int cy) {
	x = start.x + cx + (r - start.z)*cos(angle);
	y = start.y + cy + (r - start.z)*sin(angle);
	if ((x > 20 && x <22) || (x < -20 && x > -22) || (y > 20 && y < 22) || (y < -20 && y > -22) ) {
		if (start.z == 0) {
			start.z = r;
			start.x = x;
			start.y = y;
			angle = (-1 * angle) + PI;
		}
	}
	unsigned int indice = (x + 100) + (y + 100) * 201;
	if ((indice + 1) % 201 == 0 || indice > 40200) { }
	else {
		poly[0] = (indice);
		poly[1] = (indice + 1);
		poly[2] = (indice + 201);
		poly[3] = (indice + 1);
		poly[4] = (indice + 201);
		poly[5] = (indice + 202);
	}
}

void Particle::Alpha(int r) {

}
