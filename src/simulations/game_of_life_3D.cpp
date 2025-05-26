#include <random>

#include "utilities.hpp"
#include "simulations/game_of_life_3D.hpp"

namespace Simulation {

// locally redefining color to more transparent version
const auto black = utils::Color{  0,   0,   0, 200};
const auto white = utils::Color{255, 255, 255, 200};
const auto transparent = utils::Color{0, 0, 0, 0};

// TODO indexing!!!
// TODO move index from sim coords to base

GameOfLife3D::GameOfLife3D(uint32_t rows, uint32_t cols, uint32_t stacks) :
    BaseSimulation(rows,cols,stacks)
{
    auto size = rows * cols * stacks;
    this->cells_current.resize(size);
    this->cells_next.resize(size);

    for (int32_t row = 0; row < static_cast<int32_t>(rows); row++) {
        for (int32_t col = 0; col < static_cast<int32_t>(cols); col++) {
            for (int32_t stack = 0; stack < static_cast<int32_t>(stacks); stack++) {
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

    for (auto &cell : this->cells_current) {
        cell = dis(gen);
    }
    
    this->simulation_time = 0.0;

    VoxelToColor();
}

void GameOfLife3D::VoxelToColor() {
    auto [rows, cols, stacks] = this->gridSize.elements;
    for (int index = 0; index < rows * cols * stacks; ++index) {
        this->voxels[index].color = this->cells_current[index] ? white : transparent;
    }   
}

double GameOfLife3D::Step(double dt) {
    auto [rows, cols, stacks] = this->gridSize.elements;

    for (int32_t row = 0; row < rows; ++row) {
        for (int32_t col = 0; col < cols; ++col) {
            for (int32_t stack = 0; stack < stacks; stack++) {

                uint32_t index = IndexFromSimCoords(row, col, stack);
                auto neighbours_alive = this->SumNeighbouringCells(row, col, stack); // Pass row and col as arguments

                if (this->cells_current[index] == 1) { // TODO radeji != 0
                    this->cells_next[index] = (neighbours_alive == 2 || neighbours_alive == 3) ? 1 : 0;
                } else {
                    this->cells_next[index] = (neighbours_alive == 3) ? 1 : 0;
                }
            }
        }
    }
    std::swap(this->cells_current, this->cells_next);

    VoxelToColor();

    this->simulation_time += dt;
    return dt;
}

uint32_t GameOfLife3D::SumNeighbouringCells(int32_t row, int32_t col, int32_t stack) {
    auto [rows, cols, stacks] = this->GetGridSize().elements;
    uint32_t sum_alive = 0;

    // done by our AI overlords
    for (int dr = -1; dr <= 1; ++dr) {
        for (int dc = -1; dc <= 1; ++dc) {
            for (int ds = -1; ds <= 1; ++ds) {
                if (dr == 0 && dc == 0 && ds == 0)
                    continue;

                int32_t neighbor_row = row + dr;
                int32_t neighbor_col = col + dc;
                int32_t neighbor_stack = stack + ds;

                if (neighbor_row >= 0 && neighbor_row < rows &&
                    neighbor_col >= 0 && neighbor_col < cols &&
                    neighbor_stack >= 0 && neighbor_stack < stacks) {
                    //sum_alive += this->cells_current[neighbor_row * cols + neighbor_col];
                    sum_alive += this->cells_current[IndexFromSimCoords(neighbor_row, neighbor_col, neighbor_stack)];
                }
            }
        }
    }

    return sum_alive;
}

}
