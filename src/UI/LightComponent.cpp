#include "UI/LightComponent.h"

#include <imgui/imgui.h>

bool LightComponent::Render(Light& light, Node& tranform) {
	bool keepOpen = true;

	ImGui::Begin("Light", &keepOpen);

	ImGui::DragFloat("Light Intensity", &light.m_intensity, 0.01f, 0.0f, 10.0f);
	ImGui::ColorPicker3("Light Color", &light.m_color[0]);

	ImGui::End();
	return keepOpen;
}