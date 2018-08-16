//inicjalizacja:
//opneGL
//glue
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//standardowe bib
#include <iostream>
#include <vector>
#include <set>
#include <math.h>

//w³asne definicje
#include "Renderer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexBufferlayout.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "Particle.h"

//okno debugowania
#include "imgui\imgui.h"
#include "imgui\imgui_impl_glfw.h"
#include "imgui\imgui_impl_opengl3.h"

//bib matematyczna do opneGL
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"

#define WIDTH	1600.0f
#define HEIGHT	1200.0f
#define PI		3.14159265358979323846

int main(void) {

	//Inicjalizacja onka
	GLFWwindow* window;
	if (!glfwInit()) return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(WIDTH, HEIGHT, "Hello World", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	if (glewInit() != GLEW_OK) std::cout << "Error!" << std::endl;
	std::cout << glGetString(GL_VERSION) << std::endl;
	
	{
		//Generacja punktów dla VertexBuffer
		unsigned int k = 200;
		unsigned int count = 0;
		bool state = false;
		std::vector<float> grid_v = {};
		for (int i = -100; i <= 100; i++) {
			for (int j = -100; j <= 100; j++) {
				grid_v.push_back((float)(j) / 100);
				grid_v.push_back((float)(i) / 100);
				if (count != 0) if (count % k == 0) {
					state = !state;
					k += 201;
				}
				//Inicjalizacja rogów kwadratów
				if (!state) {
					if (count % 2 == 0) {
						grid_v.push_back(0.0f);
						grid_v.push_back(0.0f);
					}
					else {
						grid_v.push_back(1.0f);
						grid_v.push_back(0.0f);
					}
				}
				else {
					if (count % 2 == 0) {
						grid_v.push_back(1.0f);
						grid_v.push_back(1.0f);
					}
					else {
						grid_v.push_back(0.0f);
						grid_v.push_back(1.0f);
					}
				}
				count++;
			}
		}
		//Generacja indexów dla IndexBuffer
		float* grid = &grid_v[0];
		int grid_s = grid_v.size();
		std::vector<unsigned int> idices_v = {};
		k = 200;
		for (unsigned int i = 0; i < 40200; i++) {
			if (i != 0) if (i % k == 0) {
				k += 201;
				continue;
			}
			idices_v.push_back(i);
			idices_v.push_back(i+1);
			idices_v.push_back(i+201);
			idices_v.push_back(i+1);
			idices_v.push_back(i+201);
			idices_v.push_back(i+202);
		}
		unsigned int* indices = &idices_v[0];
		int indices_s = idices_v.size();
		/*
		Przyk³ad dzialania
		float grid[] = {
			-50.0f, -50.0f, 0.0f, 0.0f,
			50.0f, -50.0f, 1.0f, 0.0f,
			50.0f, 50.0f, 1.0f, 1.0f,
			-50.0f, 50.0f, 0.0f, 1.0f
		};
		int grid_s = (sizeof(grid) / sizeof(*grid));
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};
		int indices_s = (sizeof(indices) / sizeof(*indices));
		*/
		//W³¹czenie blendingu dla kanaów alpha (nie potrzebne)
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		//Inicjalizacja klass
		VertexArray va;
		VertexBuffer vb(grid, grid_s * sizeof(float));
		VertexBufferlayout layout;
		//layout VertexBuffer 2flota to pozycja, 2float to róg
		layout.Push<float>(2);
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);
		IndexBuffer ib(0);
		ib.Update(indices, indices_s);

		//Macierze przekszta³ceñ dla kamery i projekcji
		glm::mat4 proj = glm::ortho(0.0f, WIDTH, 0.0f, HEIGHT, -1.0f, 1.0f);
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

		//Inicjalizacja shaderów (program rysuj¹cy)
		Shader shader("res/shaders/Basic.shader");
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

		//Inicjalizacja tekstur (nie potrzebne)
		Texture texture0("res/textures/test.png");
		shader.SetUniform1i("u_Texture", 0);

		//Wycztszczenie na wszelki wypadek
		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();

		//Inicjalizacja klasy rysuj¹cej
		Renderer renderer;

		//Inicjalizacja ImGui
		ImGui::CreateContext();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init();
		ImGui::StyleColorsDark();

		//Lista naszych punktów
		std::list<Particle> particles;

		//Zmienne startowe i do debugowania
		glm::vec3 translationB(WIDTH/2, HEIGHT/2, 0);
		glm::vec1 rotationB(0.0f);
		glm::vec3 scaleB(100.0f, 100.0f, 0.0f);
		glm::vec3 test(0, 0, 10);
		float scale = 500;
		int switch_tex = 0;
		int switch_ind = 1;
		bool spawned = false;
		int r = 0;
		float alpha = 1;

		//pêtla g³ówna
		while (!glfwWindowShouldClose(window)) {

			//czyszczenie ekranu
			renderer.Clear();
				
				//generacja punktów okrêgu
				std::vector<unsigned int> idices_circle_v = {};
				int cx = test[0];
				int cy = test[1];
				r = test[2];
				test[2] ++;
				if (test[2] > 400) test[2] = 400;
				if (!spawned) {
					for (float a = 0; a < 2 * PI; a += PI / 180) {
						particles.push_back(Particle(a, cx, cy));
					}
					spawned = !spawned;
				}
				//update punktów dopuki nie wygaœnie fala
				if (r < 300) {
					for (Particle p : particles) {
						p.PolUpdate(r, cx, cy);
						idices_circle_v.push_back(p.GetPoly()[0]);
						idices_circle_v.push_back(p.GetPoly()[1]);
						idices_circle_v.push_back(p.GetPoly()[2]);
						idices_circle_v.push_back(p.GetPoly()[3]);
						idices_circle_v.push_back(p.GetPoly()[4]);
						idices_circle_v.push_back(p.GetPoly()[5]);
						alpha = p.Alpha(r);
					}
				}
				else {
					idices_circle_v.push_back(0);
					idices_circle_v.push_back(1);
					idices_circle_v.push_back(201);
					idices_circle_v.push_back(1);
					idices_circle_v.push_back(201);
					idices_circle_v.push_back(202);
				}
				//Normalizacja alpha do przedzia³u 0-1
				alpha = alpha / 1000;
				if (alpha < 0) alpha *= -1;
				if (alpha > 1) alpha = 1;
				alpha -= 1;
				alpha *= -1;

				//castowanie na typ unsi int i update IndexBuffera
				unsigned int* indices_circle = &idices_circle_v[0];
				int indices_circle_s = idices_circle_v.size();
				if (switch_ind == 1) ib.Update(indices_circle, indices_circle_s);
				else ib.Update(indices, indices_s);			

			//Inicjalizacja okna debugowania
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			{
				//Slajdery i przyciski do debugowania
				ImGui::SliderFloat3("Translation Model B", &translationB.x, 0.0f, WIDTH);
				ImGui::SliderFloat("Rotation Model B", &rotationB[0], 0.0f, 360);
				ImGui::SliderFloat("Scale Model B", &scale, -100, 1000);
				ImGui::SliderFloat("test of r", &test[2], -700, 700);
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS) \nScale: %.1f", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate, scaleB[0]);
				ImGui::RadioButton("Red texture", &switch_tex, 0);
				ImGui::RadioButton("White texture", &switch_tex, 1);
				ImGui::RadioButton("Indices square", &switch_ind, 0);
				ImGui::RadioButton("Indices crirle", &switch_ind, 1);
			}
			{
				//Przekszta³cenie macierzy mvp (model view projection/model kamera projekcja)
				glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
				model = glm::rotate(model, glm::radians(rotationB[0]), glm::vec3(0.f, 0.f, 1.f));
				model = glm::scale(model, scaleB);
				scaleB[0] = scale;
				scaleB[1] = scale;
				glm::mat4 mvp = proj * view * model;
				shader.SetUniformMat4f("u_MVP", mvp);
				//update kolorów wed³ug mocy fali i rysowanie na ekran modelu
				if (switch_tex == 0) { shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, alpha); }
				else { shader.SetUniform1i("u_Texture", 0); }
				shader.Bind();
				//pobiera indexbuffer, vertexarray, shader
				renderer.Draw(va, ib, shader);
			}

			//Rysowanie okna debugowania
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			//wyczyszczenie bufferów jakby co sie zjeba³o
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	//Czyszczenie pamiêci
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}