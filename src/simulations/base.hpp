#pragma once

#include <vector>
#include <cstdint>

#include "voxel.hpp"

namespace Simulation {

class BaseSimulation {
public:
    virtual ~BaseSimulation() = default;

    BaseSimulation(uint32_t rows, uint32_t cols, uint32_t stacks) :
      gridSize{rows, cols, stacks}
    {}

    virtual void InitRandomState() = 0;
    virtual double Step(double dt) = 0;

    const utils::Vec<int32_t, 3>& GetGridSize() const { return gridSize; }
    const std::vector<Voxel>& GetVoxels() const { return voxels; }
    double GetSimulationTime() const { return simulation_time; }
    double GetStepSize() const { return step; }
    uint32_t IndexFromSimCoords(int32_t row, int32_t col, int32_t stack) const // TODO order
    {
        auto [ rows, cols, stacks ] = GetGridSize().elements;
        if (row < 0 || row >= rows ||
            col < 0 || col >= cols ||
            stack < 0 || stack >= stacks)
        {
            throw std::out_of_range("Wrong index for voxel");
        }
        return (stack * (rows*cols)) + (row * cols) + col;
    }

protected:
    utils::Vec<int32_t, 3> gridSize;
    std::vector<Voxel> voxels;
    double simulation_time = 0.0;
    double step = 1.0;
};

} // namespace Simulation
