#pragma once

#include "Core.h"

namespace KEngine {
    class KE_API TimeStep {
    public:
        TimeStep(float ts) :timeStep(ts) {}
        inline float GetTimeStep() const { return timeStep; }
        inline void SetTimeStep(float ts) { timeStep = ts; }
    private:
        float timeStep;

    };
}
