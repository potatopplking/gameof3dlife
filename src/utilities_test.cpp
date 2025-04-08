#include <iostream>
#include <cassert>

#include "utilities.hpp"
#include "log.hpp"

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
    CSVec<CoordinateSystem::SPHERICAL, double, 3> cv4;
    assert(cv4 != cv1); // different coord sys, therefore not equal
    //CSVec<CoordinateSystem::SPHERICAL, double, 3> cv5(cv4);
    //assert(cv5 == cv4);

    std::cout << std::endl << std::endl;
        
    Log::debug("debug");
    Log::info("info");
    Log::warning("warning");
    Log::error("error");
    Log::critical("critical");

//    CSVec<CoordinateSystem::CARTESIAN, double, 3> lol{1.0,2.0,3.0};
//    CSVec<CoordinateSystem::CARTESIAN, double, 3> rofl(lol);
//    CSVec<CoordinateSystem::SPHERICAL, double, 3> roflmao(lol);
    return 0;

}
