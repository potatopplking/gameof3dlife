#include <cassert>
#include <iostream>
#include <array>
#include <vector>
#include <memory>
#include <cmath>
#include <thread>
#include <chrono>
#include <random>

#include <SDL2/SDL.h>
#include <GL/glew.h>

#include "ui.hpp"
#include "utils.hpp"

namespace UI {

SphericCoords::SphericCoords(double phi, double theta, double r): coords{phi, theta, r} {
    std::cout << "SphericCoords initalized: " << coords << std::endl;
}

utils::Vec<double, 3> SphericCoords::toCartesian() {
    auto phi = coords[0] * M_PI / 180;
    auto theta = coords[1] * M_PI / 180;
    auto r = coords[2];

    auto x = r * sin(phi) * cos(theta);
    auto y = r * sin(phi) * sin(theta);
    auto z = r * cos(phi);

    return utils::Vec<double, 3>({x,y,z});
}

}
