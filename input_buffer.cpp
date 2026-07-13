#include <cstdint>
#include <cmath>

struct ControllerSnapshot {
    uint32_t buttonBitmask;
    float leftStickX;  float leftStickY;
    float rightStickX; float rightStickY;
    float frameDurationHeld;
};

enum PitchInputFlags : uint32_t {
    INPUT_PASS        = 1 << 0,
    INPUT_STRIKE      = 1 << 1,
    INPUT_MOD_L1      = 1 << 2,
    INPUT_MOD_R1      = 1 << 3,
    INPUT_SPRINT_R2   = 1 << 4
};

class InputInterpreterPipeline {
public:
    uint32_t ParseFrameComboStrings(ControllerSnapshot current, ControllerSnapshot historical) {
        // 1. Verify Power Shot Override (L1 + R1 + Strike Button Flag state)
        if ((current.buttonBitmask & INPUT_STRIKE) && 
            (current.buttonBitmask & INPUT_MOD_L1) && 
            (current.buttonBitmask & INPUT_MOD_R1)) {
            return 991; // State Machine ID: Trigger Custom Manual Power Shot execution loop
        }

        // 2. Verify Knock-On Sprint (Sprint Trigger active + high-velocity stick delta displacement)
        float historicalStickMag = std::sqrt(historical.rightStickX * historical.rightStickX + historical.rightStickY * historical.rightStickY);
        float currentStickMag = std::sqrt(current.rightStickX * current.rightStickX + current.rightStickY * current.rightStickY);
        
        if ((current.buttonBitmask & INPUT_SPRINT_R2) && (historicalStickMag < 0.15f) && (currentStickMag > 0.85f)) {
            return 882; // State Machine ID: Break dribble tracking sphere for long-space sprint
        }

        // 3. Process Skill Execution Rotations (e.g., Ball Roll check via continuous angular constraints)
        if (currentStickMag > 0.80f && historicalStickMag > 0.80f) {
            float angleCurrent = std::atan2(current.rightStickY, current.rightStickX);
            float angleHistorical = std::atan2(historical.rightStickY, historical.rightStickX);
            float deltaAngle = std::abs(angleCurrent - angleHistorical);

            if (deltaAngle < 0.1f && std::abs(current.rightStickX) > 0.75f && std::abs(current.rightStickY) < 0.25f) {
                return 101; // State Machine ID: Continuous Lateral Ball Roll execution loop
            }
        }

        if (current.buttonBitmask & INPUT_STRIKE) return 1;
        if (current.buttonBitmask & INPUT_PASS)   return 2;

        return 0; // Baseline standard running locomotion state
    }
};
