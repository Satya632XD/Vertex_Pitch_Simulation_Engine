#include <cmath>
#include <algorithm>

struct CharacterKineticNode {
    float mass;
    float velX; float velY;
    float headingX; float headingY;
    float centerOfGravityZ;
    float stamina;
    float aggression;
};

struct MechanicalDuelOutput {
    float knockbackVelX;
    float knockbackVelY;
    float balanceDamage;
    bool breakSkeletalAnchor;
};

MechanicalDuelOutput ResolveKineticBarge(CharacterKineticNode attacker, CharacterKineticNode defender) {
    MechanicalDuelOutput output{0.0f, 0.0f, 0.0f, false};

    // Calculate angular vector alignment using absolute dot product scaling
    float contactAngleNorm = (attacker.headingX * defender.headingX) + (attacker.headingY * defender.headingY);

    // Dynamic Kinetic Energy Matrix Calculations: KE = 0.5 * m * v^2
    float vAttackerSq = (attacker.velX * attacker.velX) + (attacker.velY * attacker.velY);
    float vDefenderSq = (defender.velX * defender.velX) + (defender.velY * defender.velY);
    
    float forceAttacker = 0.5f * attacker.mass * vAttackerSq * attacker.aggression * attacker.stamina;
    float forceDefender = 0.5f * defender.mass * vDefenderSq * 1.25f * defender.stamina; // Protection bonus for set shielding anchor

    float netImpactForce = forceAttacker - (forceDefender * std::abs(contactAngleNorm));

    if (netImpactForce > 0.0f) {
        // Attacker wins physical space displacement
        float targetNormalX = -defender.headingX;
        float targetNormalY = -defender.headingY;
        
        output.knockbackVelX = (netImpactForce / defender.mass) * targetNormalX;
        output.knockbackVelY = (netImpactForce / defender.mass) * targetNormalY;
        
        // Apply leverage multiplication via Center of Gravity structural offsets
        float leverageDisparity = attacker.centerOfGravityZ / defender.centerOfGravityZ;
        output.balanceDamage = netImpactForce * leverageDisparity * 0.55f;

        if (output.balanceDamage > (75.0f * defender.stamina)) {
            output.breakSkeletalAnchor = true; // Forces physics thread breakdown into ragdoll tumble state
        }
    } else {
        // Defender successfully holds spatial position; attacker receives recoil feedback
        output.knockbackVelX = (std::abs(netImpactForce) / attacker.mass) * attacker.headingX;
        output.knockbackVelY = (std::abs(netImpactForce) / attacker.mass) * attacker.headingY;
        output.balanceDamage = std::abs(netImpactForce) * 0.30f;
    }

    return output;
}
