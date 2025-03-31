#pragma once

#include <vector>

#include "voxel.hpp"

namespace Simulation {

  class BaseSimulation {
    public:
      // tries to conform to dt, but may differ - returns time that actually passed in the simulation
      virtual double Step(double dt) = 0;
      virtual void InitRandomState() = 0;
      virtual const utils::Vec<int32_t, 3>& GetGridSize() = 0;
      virtual const std::vector<Voxel>& GetVoxels() = 0;
      virtual ~BaseSimulation() = default;
  };

  class GameOfLife2D : public BaseSimulation {
    public:
      GameOfLife2D(int32_t rows, int32_t cols);
      ~GameOfLife2D() = default;
      // (Re)initialize to random state
      void InitRandomState() override;
      double Step(double dt) override;
      const utils::Vec<int32_t, 3>& GetGridSize() override;
      const std::vector<Voxel>& GetVoxels() override;

    private:
      SimCoords gridSize;
      std::vector<Voxel> voxels;
      std::vector<bool> cells;

      uint32_t SumNeighbouringCells(uint32_t index);
  };
}
