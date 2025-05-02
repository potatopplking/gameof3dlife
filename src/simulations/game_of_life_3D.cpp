#include <random>

#include "utilities.hpp"
#include "simulations/game_of_life_3D.hpp"

namespace Simulation {

// locally redefining color to be more transparent
const auto black = utils::Color{0,0,0,100};
const auto white = utils::Color{255,255,255,100};

// TODO indexing!!!
// TODO move index from sim coords to base

GameOfLife3D::GameOfLife3D(uint32_t rows, uint32_t cols, uint32_t stacks) :
    BaseSimulation(rows,cols,stacks)
{
    this->voxels.resize(rows * cols * stacks); // TODO move to base
    size_t i = 0;
    const float voxel_size = 1.0f;

    // TODO order
    for (int32_t stack = 0; stack < static_cast<int32_t>(stacks); stack++) {
        for (int32_t row = 0; row < static_cast<int32_t>(rows); row++) {
            for (int32_t col = 0; col < static_cast<int32_t>(cols); col++) {
                uint32_t index = IndexFromSimCoords(row, col, stack);
                this->voxels[index].color = black;
                this->voxels[index].position = { row, col, stack };
            }
        }
    }
    this->InitRandomState();
}

// (Re)initialize to random state
void GameOfLife3D::InitRandomState() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution dis(0.5);

    auto [rows, cols, _] = this->GetGridSize().elements;
    this->cells.resize(rows * cols);
    for (auto &cell : this->cells) {
        cell = dis(gen);
    }
    
    this->simulation_time = 0.0;
}

double GameOfLife3D::Step(double dt) {
    auto [rows, cols, stacks] = this->gridSize.elements;
    std::vector<uint8_t> next_cells = this->cells; // Ensure consistent type

    // TODO order
    for (int32_t stack = 0; stack < static_cast<int32_t>(stacks); stack++) {
        for (int32_t row = 0; row < rows; ++row) {
            for (int32_t col = 0; col < cols; ++col) {
                uint32_t index = IndexFromSimCoords(row, col, stack);
                auto neighbours_alive = this->SumNeighbouringCells(row, col); // Pass row and col as arguments

                if (this->cells[index] == 1) {
                    next_cells[index] = (neighbours_alive == 2 || neighbours_alive == 3) ? 1 : 0;
                } else {
                    next_cells[index] = (neighbours_alive == 3) ? 1 : 0;
                }
            }
        }
    }

    this->cells = std::move(next_cells); // Ensure consistent type

    for (int index = 0; index < rows * cols; ++index) {
        this->voxels[index].color = this->cells[index] ? white : black;
    }

    this->simulation_time += dt;
    return dt;
}

uint32_t GameOfLife3D::SumNeighbouringCells(int32_t row, int32_t col) {
    auto [rows, cols, _] = this->GetGridSize().elements;
    uint32_t sum_alive = 0;

    // done by our AI overlords
    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            if (dr == 0 && dc == 0) continue; // Skip the current cell

            int32_t neighbor_row = row + dr;
            int32_t neighbor_col = col + dc;

            if (neighbor_row >= 0 && neighbor_row < rows && neighbor_col >= 0 && neighbor_col < cols) {
                sum_alive += this->cells[neighbor_row * cols + neighbor_col];
            }
        }
    }

    return sum_alive;
}

}
