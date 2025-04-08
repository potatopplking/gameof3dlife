#include <iostream>
#include <cassert>

#include "utilities.hpp"
#include "log.hpp"

using namespace utils;

// implement operator== for double
bool double_equal(double a, double b) {
    return std::abs(a - b) < 1e-6;
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
    // conversion between coordinate systems
    CSVec<CoordinateSystem::CARTESIAN, double, 3> cartesian{12.0, 34.0, 56.0};
    CSVec<CoordinateSystem::SPHERICAL, double, 3> spherical(cartesian);
    assert(double_equal(spherical[0], 32.77539683)); // phi
    assert(double_equal(spherical[1], 70.55996517)); // theta
    assert(double_equal(spherical[2], 66.60330322)); // r
    // other way around
    CSVec<CoordinateSystem::CARTESIAN, double, 3> cartesian2(spherical);
    // std::cout << Vec(cartesian2) << std::endl;
    assert(double_equal(cartesian2[0], 12.0)); // x
    assert(double_equal(cartesian2[1], 34.0)); // y
    assert(double_equal(cartesian2[2], 56.0)); // z

//    CSVec<CoordinateSystem::CARTESIAN, double, 3> lol{1.0,2.0,3.0};
//    CSVec<CoordinateSystem::CARTESIAN, double, 3> rofl(lol);
//    CSVec<CoordinateSystem::SPHERICAL, double, 3> roflmao(lol);
    return 0;

}
