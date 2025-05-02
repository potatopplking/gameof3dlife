#pragma once

#include <cstdint>

#include "voxel.hpp"
#include "utilities.hpp"
#include "simulations/base.hpp"

namespace Simulation {
  
class GameOfLife3D : public BaseSimulation {
public:
    GameOfLife3D(int32_t rows, int32_t cols);

    void InitRandomState() override;
    double Step(double dt) override;

    const utils::Vec<int32_t, 3>& GetGridSize() override;
    const std::vector<Voxel>& GetVoxels() override;

private:
    uint32_t SumNeighbouringCells(int32_t row, int32_t col);

    utils::Vec<int32_t, 3> gridSize;
    std::vector<uint8_t> cells;
    std::vector<Voxel> voxels;
};

}

