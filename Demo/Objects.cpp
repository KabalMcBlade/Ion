#include "Objects.h"

void RotatingEntity::OnUpdate(ionFloat _deltaTime)
{
    static const ionFloat radPerFrame = 0.0174533f;     // 1 deg
    static const Vector axis(0.0f, 1.0f, 0.0f, 1.0f);
    static ionFloat radRotated = 0.0f;

    radRotated += radPerFrame;
    while (radRotated > 6.283185307f) radRotated -= 6.283185307f;   // 360 deg

    GetTransformHandle()->SetRotation(radRotated, axis);
}