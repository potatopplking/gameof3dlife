#include <iostream>
#include <cassert>

#include "utilities.hpp"

using namespace utils;

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
    assert(cv3 == cv2);
    CSVec<CoordinateSystem::SPHERICAL, double, 3> cv4;
    CSVec<CoordinateSystem::SPHERICAL, double, 3> cv5(cv4);
    std::cout << cv3 << std::endl;
    std::cout << cv4 << std::endl;
    assert(cv4 == cv3); // different coord sys, therefore not equal


//    CSVec<CoordinateSystem::CARTESIAN, double, 3> lol{1.0,2.0,3.0};
//    CSVec<CoordinateSystem::CARTESIAN, double, 3> rofl(lol);
//    CSVec<CoordinateSystem::SPHERICAL, double, 3> roflmao(lol);
    return 0;

}
