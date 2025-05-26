#pragma once

#include <cstdint>

#include "voxel.hpp"
#include "utilities.hpp"
#include "simulations/base.hpp"

namespace Simulation {
  
class GameOfLife3D : public BaseSimulation {
public:
    GameOfLife3D(uint32_t rows, uint32_t cols, uint32_t stacks);

    void InitRandomState() override;
    double Step(double dt) override;

private:
    uint32_t SumNeighbouringCells(int32_t row, int32_t col, int32_t stack);
    void VoxelToColor();

    std::vector<uint8_t, utils::TrackingAllocator<uint8_t>> cells_current;
    std::vector<uint8_t, utils::TrackingAllocator<uint8_t>> cells_next;
};

}

