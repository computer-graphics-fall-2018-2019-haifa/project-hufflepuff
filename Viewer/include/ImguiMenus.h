#pragma once
#include <imgui/imgui.h>
#include "Scene.h"

void DrawImguiMenus(ImGuiIO& io, Scene& scene, float& scale, float& rotation, float translation[2]);
const glm::vec4& GetClearColor();
