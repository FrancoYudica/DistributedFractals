#pragma once
#include "common/number.h"

class Camera {

public:
    number x, y;
    number zoom;

    Camera()
        : x(0.0)
        , y(0.0)
        , zoom(1.0)
    {
    }

    /// @brief Transforms screen coordinates to world
    /// @param screen_normalized Are in range [-0.5, 0.5]
    /// this is done to avoid mapping [-1, 1] and then to [-0.5, 0.5]
    /// therefore, this aren't NDC
    void to_world(
        double screen_normalized_x,
        double screen_normalized_y,
        number& world_x,
        number& world_y) const
    {
        world_x = (number)screen_normalized_x / zoom + x;
        world_y = (number)screen_normalized_y / zoom + y;
    }
};
