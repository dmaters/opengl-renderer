

#include "UI.h"

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui.h>

#include <format>
#include <string>

#include "Primitive.h"
#include "State.h"

void UI::Render(Scene& scene) {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	const State& state = State::Get();
	ImGui::Begin("Rendering Informations");
	ImGui::Text(
		"FPS: %i",
		(int)(1e3 / (std::max(state.averageTimeCPU, state.averageTimeGPU)))
	);

	ImGui::PlotLines(
		"CPU",
		state.frameTimesCPU.data(),
		state.frameTimesCPU.size(),
		state.frameIndex % 128,
		std::format("CPU Average : {:.2f}ms", state.averageTimeCPU).c_str(),
		0,
		16,
		ImVec2(0, 40)
	);

	ImGui::PlotLines(
		"GPU",
		state.frameTimesGPU.data(),
		state.frameTimesGPU.size(),
		state.frameIndex % 128,
		std::format("GPU Average : {:.2f}ms", state.averageTimeGPU).c_str(),
		0,
		16,
		ImVec2(0, 40)
	);

	ImGui::Text(
		"Primitive Count (Total/Visible): %zu/%u",
		scene.m_primitives.size(),
		state.primitiveCount
	);

	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}