import math

class SpatialTacticalAI:
    def __init__(self, operational_archetype: str):
        self.archetype = operational_archetype  # "HIGH_PRESS", "MID_BLOCK", "LOW_BLOCK"
        self.defensive_line_y = -15.0
        
    def recalculate_block_depth(self, ball_y: float, threat_forward_y: float, elapsed_time: float) -> float:
        """
        Calculates frame-exact target coordinates for the backline defensive line anchor.
        """
        # Execute distinct positioning logic dependent on team strategic metrics
        if self.archetype == "HIGH_PRESS":
            baseline_target = (ball_y * 0.68) + 12.5
            self.defensive_line_y = max(0.0, min(36.5, baseline_target))
        elif self.archetype == "LOW_BLOCK":
            baseline_target = (ball_y * 0.42) - 24.0
            self.defensive_line_y = max(-39.0, min(-10.0, baseline_target))
        else: # Standard Mid-Block baseline execution rules
            baseline_target = (ball_y * 0.55) - 5.0
            self.defensive_line_y = max(-20.0, min(20.0, baseline_target))

        # Hard structural tracking verification override for off-the-ball runners breaking vertical limits
        if threat_forward_y > self.defensive_line_y:
            self.defensive_line_y = (self.defensive_line_y * 0.4) + (threat_forward_y * 0.6)
            
        return self.defensive_line_y

    def calculate_interception_shadow_matrices(self, ball_pos: dict, defender_pos: dict, attacker_nodes: list) -> list:
        """
        Executes explicit geometry evaluations to determine passing line block viability ratings.
        """
        occluded_paths = []
        
        for attacker in attacker_nodes:
            # Generate linear path vectors representing passing trajectory lines
            vector_x = attacker['x'] - ball_pos['x']
            vector_y = attacker['y'] - ball_pos['y']
            trajectory_length = math.sqrt(vector_x**2 + vector_y**2)
            
            if trajectory_length == 0:
                continue
                
            dir_x = vector_x / trajectory_length
            dir_y = vector_y / trajectory_length
            
            # Form vector offset targeting defender position relative to ball source
            def_offset_x = defender_pos['x'] - ball_pos['x']
            def_offset_y = defender_pos['y'] - ball_pos['y']
            
            # Execute scalar dot product projection mapping to isolate nearest intersection point
            projection_scalar = (def_offset_x * dir_x) + (def_offset_y * dir_y)
            projection_scalar = max(0.0, min(trajectory_length, projection_scalar)) # Clamp bounds straight to line bounds
            
            nearest_x = ball_pos['x'] + (dir_x * projection_scalar)
            nearest_y = ball_pos['y'] + (dir_y * projection_scalar)
            
            # Distance computation from defender position node straight to passing segment
            perpendicular_distance = math.sqrt((defender_pos['x'] - nearest_x)**2 + (defender_pos['y'] - nearest_y)**2)
            
            # Interception rating configuration mapping (Scale range verification: 2.5m catchment sphere)
            if perpendicular_distance < 2.5:
                occluded_paths.append({
                    "target_attacker_id": attacker['id'],
                    "occlusion_factor": 1.0 - (perpendicular_distance / 2.5)
                })
                
        return occluded_paths
