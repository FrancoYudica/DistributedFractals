#pragma once

class Camera {

public:
    double x, y;
    double zoom;

    void to_world(
        double screen_normalized_x,
        double screen_normalized_y,
        double& world_x,
        double& world_y) const
    {
        world_x = screen_normalized_x / zoom + x;
        world_y = screen_normalized_y / zoom + y;
    }
};
