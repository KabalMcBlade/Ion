#pragma once

#include "../Ion/Ion.h"


void Test_ColoredTriangle(ion::Entity& _entity);
void Test_ColoredQuad(ion::Entity& _entity);
void Test_TexturedQuad(ion::Entity& _entity);
void Test_TexturedQuadEx(ion::Entity& _entity, ion::Texture* _texture);
void Test_ColoredCube(ion::Entity& _entity);
void Test_ColoredSphere(ion::Entity& _entity);
void Test_Model_Ambient(ion::Entity& _entity);
void Test_ModelPBR(ion::Entity& _entity, ion::Texture* _brdf, ion::Texture* _irradiance, ion::Texture* _prefiltered);

void Test_DrawBoundingBox(ion::Entity& _entity);