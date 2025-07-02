#pragma once

#include <vector>
#include <cstdint>

#include "voxel.hpp"
#include "utilities.hpp"

namespace Simulation {

class BaseSimulation {
public:
    virtual ~BaseSimulation() = default;

    BaseSimulation(uint32_t rows, uint32_t cols, uint32_t stacks) :
      gridSize{rows, cols, stacks}
    {
        ResizeVoxels();
    }

    BaseSimulation() :
      gridSize{0,0,0}
    {

    }

    virtual void InitRandomState() = 0;
    virtual double Step(double dt) = 0;

    virtual void TriggerSource()
    {
      // test function, only applicable to FDTD simulations
    }

    virtual inline const utils::Vec<int32_t, 3>& GetGridSize() const
    {
        return gridSize;
    }

    virtual inline const std::vector<Voxel, utils::TrackingAllocator<Voxel>>& GetVoxels() const
    {
        return voxels;
    }

    virtual inline double GetSimulationTime() const
    {
        return simulation_time;
    }

    virtual inline double GetStepSize() const
    {
        return step;
    }

    uint32_t IndexFromSimCoords(int32_t row, int32_t col, int32_t stack) const
    {
        auto [ rows, cols, stacks ] = GetGridSize().elements;
        if (  row < 0 || row >= rows ||
              col < 0 || col >= cols ||
            stack < 0 || stack >= stacks)
        {
            throw std::out_of_range("Wrong index for voxel");
        }
        return (stack * (rows*cols)) + (row * cols) + col;
    }

    utils::Vec<double,3> GetCenter() const
    {
        auto [ rows, cols, stacks ] = GetGridSize().elements;
        return utils::Vec<double,3>{rows/2, cols/2, stacks/2};
    }

protected:
    utils::Vec<int32_t, 3> gridSize;
    std::vector<Voxel, utils::TrackingAllocator<Voxel>> voxels;
    double simulation_time = 0.0;
    double step = 1.0;

    void ResizeVoxels()
    {
        auto [rows, cols, stacks] = this->gridSize.elements;
        auto size = rows * cols * stacks;
        this->voxels.resize(size);
    }
};

} // namespace Simulation
