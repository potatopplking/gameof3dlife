#include <cmath>

#include "utilities.hpp"

namespace utils {

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
