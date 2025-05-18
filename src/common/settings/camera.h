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
