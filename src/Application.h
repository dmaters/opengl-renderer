#pragma once
// clang-format off
#include "Renderer.h"
#include "ResourceManager.h"
#include <GLFW/glfw3.h>
// clang-format on

#include <memory>

class Application {
private:
	uint32_t m_width = 800;
	uint32_t m_height = 600;

	GLFWwindow* m_window;

	std::unique_ptr<Renderer> m_renderer;
	std::shared_ptr<ResourceManager> m_resourceManager;

	bool m_cameraControl = false;

	void processInput(double delta);
	void processMovement(
		double delta, glm::vec3* direction, glm::vec2* rotation, bool* speeding
	);

public:
	Application();
	~Application();
	void run(std::filesystem::path scene);
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};