#pragma once

#include <cstdint>

#include "utilities.hpp"


struct Voxel {
    // coordinates of the voxel are given by its position in Voxel array
    utils::Color color;
    utils::SimCoords position;
    // TODO 
    // create VBO (or something else) when creating Simulation,
    // then during Updates only pass color array to the Window to draw them
    Voxel() : color({0,0,0,0}), position({0,0,0}) {} 

};
