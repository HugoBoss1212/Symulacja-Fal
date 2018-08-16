#include "Particle.h"

#include <math.h>
#include <map>
#include <iostream>

#define PI		3.14159265358979323846

Particle::Particle(float angle, int cx, int cy) {
	//zmienne pocz¹tkowe
	for (int i = 0; i < 6; i++) { poly.push_back(0); }
	this->angle = angle;
	start.x = cx;
	start.y = cy;
	start.z = 0;
}

void Particle::PolUpdate(int r, int cx, int cy) {
	//obliczanie pozycji pojedynczego punktu
	x = start.x + cx + (r - start.z)*cos(angle);
	y = start.y + cy + (r - start.z)*sin(angle);
	//odbicie od œciany (zmiana k¹ta i nowy œrodek rysowanego ko³a)
	if ((x > 20 && x <22) || (x < -20 && x > -22) || (y > 20 && y < 22) || (y < -20 && y > -22) ) {
		if (start.z == 0) {
			start.z = r;
			start.x = x;
			start.y = y;
			angle = (-1 * angle) + PI;
		}
	}
	//normalizacja dla index buffera
	unsigned int indice = (x + 100) + (y + 100) * 201;
	//wstawianie kradratu do vektora (czyli dwóch poligonów 2X3punkty)
	if ((indice + 1) % 201 == 0 || indice > 40200) { }
	else {
		/*
		201     201 202      --> 401
		.         .__.
		| \        \ |
		|  \        \|
		.----.       .
		0   1       1        --> 200
		*/
		poly[0] = (indice);
		poly[1] = (indice + 1);
		poly[2] = (indice + 201);
		poly[3] = (indice + 1);
		poly[4] = (indice + 201);
		poly[5] = (indice + 202);
	}
}

float Particle::Alpha(int r) {
	//obliczanie drgañ jako alpha dla koloru
	alpha = 5 * r * sin(((2 * PI) / 10*PI)*r - ((2*PI)/PI)*x + 500);
	return alpha;
}
