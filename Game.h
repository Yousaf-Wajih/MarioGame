#pragma once
#include <SFML/Graphics.hpp>
#include "Renderer.h"
#include "Object.h"
#include "Camera.h"

extern Camera camera;
extern bool paused;

void Begin();
void Update(float deltaTime);
void Render(Renderer& renderer);
void RenderUI(Renderer& renderer);

void DeleteObject(Object* object);
