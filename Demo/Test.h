#pragma once

#include "../Ion/Ion.h"


void Test_ColoredTriangle(ion::ObjectHandler& _entity);
void Test_ColoredQuad(ion::ObjectHandler& _entity);
void Test_TexturedQuad(ion::ObjectHandler& _entity);
void Test_TexturedQuadEx(ion::ObjectHandler& _entity, const ion::Texture* _texture);
void Test_ColoredCube(ion::ObjectHandler& _entity);
void Test_ColoredSphere(ion::ObjectHandler& _entity);

void Test_DrawBoundingBox(ion::ObjectHandler& _entity);