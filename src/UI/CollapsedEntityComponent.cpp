#include "CollapsedEntityComponent.h"

#include <imgui/imgui.h>

void CollapsedEntityComponent::Render(std::string name, bool& expand) {
	ImGui::Text(name.c_str());
	expand = ImGui::Button("Open Details");
}