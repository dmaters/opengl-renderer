#include "Application.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui.h>

#include <filesystem>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>

#include "Camera.h"
#include "Node.h"
#include "Renderer.h"
#include "Scene.h"
#include "SceneLoader.h"
#include "UI/UI.h"
#include "scene/LightDescription.h"

void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void GLAPIENTRY debugCallback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam
);

Application::Application() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	m_window = glfwCreateWindow(m_width, m_height, "OpenGL+", NULL, NULL);
	if (m_window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		throw;
	}

	glfwMakeContextCurrent(m_window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		throw;
	}

	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glDebugMessageCallback(debugCallback, 0);
	glDebugMessageControl(
		GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE
	);

	glfwSwapInterval(0);
	glfwSetInputMode(m_window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
	glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	m_resourceManager = std::make_shared<ResourceManager>();
	m_renderer =
		std::make_unique<Renderer>(m_width, m_height, m_resourceManager);
}

Application::~Application() { glfwTerminate(); }

void Application::run(std::filesystem::path scenePath) {
	double previousTime = glfwGetTime();
	bool toggleSpeed = false;

	Scene scene = SceneLoader::Load(scenePath, *m_resourceManager);
	/*
	LightDescription light {
	    .type = LightDescription::Type::Directional,
	    .position = glm::vec3(0, 300, 0),
	    .direction = glm::vec3(0, -1, 0),
	    .color = glm::vec3(1, 1, 1),
	    .intensity = 50,
	};
	scene.addLight(light);
	*/

	Camera& camera = scene.getCamera();
	camera.setPosition(glm::vec3(0, 0, 20));
	glm::mat3 orientation = glm::mat3(1);
	orientation[0] = -orientation[0];
	orientation[2] = -orientation[2];
	camera.setOrientation(orientation);

	while (!glfwWindowShouldClose(m_window)) {
		int width = 0, height = 0;
		glfwGetWindowSize(m_window, &width, &height);

		if (width != m_width || height != m_height) {
			if (width != 0 && height != 0)
				m_renderer->setResolution(width, height);
			m_width = width;
			m_height = height;
		}

		if (m_width == 0 || m_height == 0) {
			glfwPollEvents();
			continue;
		}

		double currentTime = glfwGetTime();

		float delta = (float)(currentTime - previousTime);

		processInput(delta);

		glm::vec3 direction = glm::vec3(0);
		glm::vec2 rotation = glm::vec2(0);

		processMovement(delta, &direction, &rotation, &toggleSpeed);

		camera.movementInput(direction, delta);
		camera.rotationInput(rotation / glm::vec2(m_width, m_height), delta);

		m_renderer->render(scene);

		// UI::Render(scene);

		glfwSwapBuffers(m_window);

		glfwPollEvents();

		previousTime = currentTime;
	}
}

void Application::processInput(double deltaTime) {
	int state = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT);

	if (state == GLFW_PRESS && !m_cameraControl) {
		glfwSetCursorPos(m_window, m_width / 2, m_height / 2);
		m_cameraControl = true;
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	if (state != GLFW_PRESS && m_cameraControl) {
		m_cameraControl = false;
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(m_window, true);
}

void Application::processMovement(
	double deltaTime,
	glm::vec3* direction,
	glm::vec2* rotation,
	bool* toggleSpeed
) {
	if (!m_cameraControl) return;

	double xpos, ypos;
	glfwGetCursorPos(m_window, &xpos, &ypos);

	*rotation = glm::vec2((xpos - m_width / 2), (ypos - m_height / 2));

	glfwSetCursorPos(m_window, m_width / 2, m_height / 2);

	if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
		*direction += glm::vec3(0, 0, 1);

	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
		*direction += glm::vec3(0, 0, -1);

	if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
		*direction += glm::vec3(-1, 0, 0);

	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
		*direction += glm::vec3(1, 0, 0);

	if (glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		*toggleSpeed = true;
	else
		*toggleSpeed = false;
}

static void GLAPIENTRY debugCallback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam
) {
	if (type == GL_DEBUG_TYPE_ERROR) {
		std::cerr << "Error:" << message << std::endl;
	} else {
		std::cout << message << std::endl;
	}
}