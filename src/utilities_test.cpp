#include <iostream>
#include <cassert>
#include <cmath>

#include "utilities.hpp"
#include "log.hpp"

using namespace utils;

// implement operator== for double
bool double_equal(double a, double b) {
    return std::abs(a - b) < 1e-6;
}

void test_CSVec(
        CSVec<CoordinateSystem::SPHERICAL, double, 3> spherical,
        CSVec<CoordinateSystem::CARTESIAN, double, 3> cartesian
) {
    CSVec<CoordinateSystem::CARTESIAN, double, 3> cartesian_converted(spherical);
    CSVec<CoordinateSystem::SPHERICAL, double, 3> spherical_converted(cartesian);
    Log::debug(spherical, " -> ", cartesian_converted, " (correct value is ", cartesian, ")");
    assert(spherical == spherical_converted);
    Log::debug(cartesian, " -> ", spherical_converted, " (correct value is ", spherical, ")");
    assert(cartesian == cartesian_converted);
    // TODO wip nekde tady
    Log::debug(cartesian, " -> ", spherical_converted, " (correct value is ", spherical, ")");
    CSVec<CoordinateSystem::SPHERICAL, double, 3> spherical_backconverted(cartesian_converted);
    assert(spherical_backconverted == spherical);
    CSVec<CoordinateSystem::CARTESIAN, double, 3> cartesian_backconverted(spherical_converted);
    assert(cartesian_backconverted == cartesian);
}

int main(void)
{
    // Vec class
    Vec<int, 5> v1{1,2,3,4,5};
    Vec<int, 5> v2 = v1;
    assert(v2 == v1);
    assert(v1*2 == (v1 + v1));
    Vec<int, 5> v3; // should be null
    assert((v2-v1) == v3);
    v2[0] = 10;
    assert(v1 != v2);
    Vec<int, 5> v4{1.0,2.0,3.0,4.0,5.0}; // implicit conversion
    assert(v4 == v1);
    // CSVec class
    CSVec<CoordinateSystem::CARTESIAN, double, 3> cv1;
    CSVec<CoordinateSystem::CARTESIAN, double, 3> cv2{0.0,0.0,0.0};
    assert(cv1 == cv2);
    auto cv3 = CSVec(cv2);
    CSVec<CoordinateSystem::SPHERICAL, double, 3> cv4;
    assert(cv4 != cv1); // different coord sys, therefore not equal
    CSVec<CoordinateSystem::SPHERICAL, double, 3> cv5(cv4);
    assert(cv5 == cv4);
    // null vector converted between coordinate systems should be equal
    CSVec<CoordinateSystem::SPHERICAL, double, 3> null_spherical;
    CSVec<CoordinateSystem::CARTESIAN, double, 3> null_cartesian_from_spherical(null_spherical);
    assert(null_cartesian_from_spherical == null_cartesian_from_spherical);
    // other way around
    CSVec<CoordinateSystem::CARTESIAN, double, 3> null_cartesian;
    CSVec<CoordinateSystem::SPHERICAL, double, 3> null_spherical_from_cartesian(null_cartesian);
    assert(null_spherical_from_cartesian == null_spherical_from_cartesian);
    //
    // X Y Z verification
    // See https://en.wikipedia.org/wiki/Spherical_coordinate_system, image with physical convention
    Log::debug("Spherical to cartesian test");
    // R = 1, both angles zero -> [0,0,1]
    test_CSVec(
        CSVec<CoordinateSystem::SPHERICAL, double, 3>{1.0, 0.0, 0.0},
        CSVec<CoordinateSystem::CARTESIAN, double, 3>{0.0, 0.0, 1.0}
    );
    // R = 1, theta = pi/2 -> [1,0,0]
    test_CSVec(
        CSVec<CoordinateSystem::SPHERICAL, double, 3>{1.0, M_PI/2.0, 0.0},
        CSVec<CoordinateSystem::CARTESIAN, double, 3>{1.0, 0.0, 0.0}
    );
    // R = 1, theta = pi/2, phi = pi/2 -> [0, 1, 0]
    test_CSVec(
        CSVec<CoordinateSystem::SPHERICAL, double, 3>{1.0, M_PI/2.0, M_PI/2.0},
        CSVec<CoordinateSystem::CARTESIAN, double, 3>{0.0, 1.0, 0.0}
    );
    //

//    CSVec<CoordinateSystem::CARTESIAN, double, 3> lol{1.0,2.0,3.0};
//    CSVec<CoordinateSystem::CARTESIAN, double, 3> rofl(lol);
//    CSVec<CoordinateSystem::SPHERICAL, double, 3> roflmao(lol);
    return 0;

}
