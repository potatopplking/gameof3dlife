#pragma once

#include <cstdint>

#include "voxel.hpp"
#include "utilities.hpp"
#include "simulations/base.hpp"

namespace Simulation {
  
class GameOfLife2D : public BaseSimulation {
public:
    GameOfLife2D(uint32_t rows, uint32_t cols);

    void InitRandomState() override;
    double Step(double dt) override;

private:
    uint32_t SumNeighbouringCells(int32_t row, int32_t col);

    std::vector<uint8_t> cells;
};

}

