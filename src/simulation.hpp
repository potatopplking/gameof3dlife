#pragma once

#include <vector>

#include "voxel.hpp"

namespace Simulation {

class BaseSimulation {
public:
    virtual ~BaseSimulation() = default;

    // Pure virtual methods to be implemented by derived classes
    virtual void InitRandomState() = 0;
    virtual double Step(double dt) = 0;
    virtual const utils::Vec<int32_t, 3>& GetGridSize() = 0;
    virtual const std::vector<Voxel>& GetVoxels() = 0;
};

class GameOfLife2D : public BaseSimulation {
public:
    GameOfLife2D(int32_t rows, int32_t cols);

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

} // namespace Simulation
