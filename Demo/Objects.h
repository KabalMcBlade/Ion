#pragma once

#include "../Ion/Ion.h"

class RotatingEntity : public ion::Entity
{
    virtual void OnUpdate(ionFloat _deltaTime) override;
};