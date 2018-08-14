#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include "Renderer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexBufferlayout.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "imgui\imgui.h"
#include "imgui\imgui_impl_glfw.h"
#include "imgui\imgui_impl_opengl3.h"

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"

// Window size
#define WIDTH 1280.0f
#define HEIGHT 720.0f


int main(void) {

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
		/*
		std::vector<float> grid_v = {};
		for (int i = -100; i <= 100; i++) {
			for (int j = -100; j <= 100; j++) {
				grid_v.push_back((float)(j) / 100);
				grid_v.push_back((float)(i) / 100);
			}
		}
		float* grid = &grid_v[0];
		int grid_s = grid_v.size();
		std::vector<unsigned int> idices_v = {};
		unsigned int k = 200;
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
		*/

		float grid[] = {
			0.0f, 0.0f, 0.0f, 0.0f,
			200.0f, 0.0f, 1.0f, 0.0f,
			200.0f, 200.0f, 1.0f, 1.0f,
			0.0f, 200.0f, 0.0f, 1.0f
		};
		int grid_s = (sizeof(grid) / sizeof(*grid));
		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};
		int indices_s = (sizeof(indices) / sizeof(*indices));

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		VertexArray va;
		VertexBuffer vb(grid, grid_s * sizeof(float));
		VertexBufferlayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);
		IndexBuffer ib(indices, indices_s);

		glm::mat4 proj = glm::ortho(0.0f, WIDTH, 0.0f, HEIGHT, -1.0f, 1.0f);
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

		Shader shader("res/shaders/Basic.shader");
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.2f, 0.3f, 0.8f, 1.0f);

		Texture texture("res/textures/test.png");
		texture.Bind();
		shader.SetUniform1i("u_Texture", 0);

		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();

		Renderer renderer;

		ImGui::CreateContext();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init();
		ImGui::StyleColorsDark();

		glm::vec3 translation(0, 0, 0);
		float r = 0.0f;
		float increment = 0.05f;
		while (!glfwWindowShouldClose(window)) {

			renderer.Clear();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
			glm::mat4 mvp = proj * view * model;

			shader.Bind();
			shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
			shader.SetUniformMat4f("u_MVP", mvp);
			renderer.Draw(va, ib, shader);

			if (r > 1.0f) increment = -0.05f;
			else if (r < 0.0f) increment = 0.05f;
			r += increment;

			{
				ImGui::SliderFloat3("Translation Model", &translation.x, 0.0f, WIDTH);
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}
