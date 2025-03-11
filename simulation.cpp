#include "utilities.hpp"
#include "simulation.hpp"

namespace Simulation {

GameOfLife2D::GameOfLife2D(int32_t rows, int32_t cols) :
    gridSize{rows,cols,1}
{
    this->voxels.resize(rows * cols);
    size_t i = 0;
    const float voxel_size = 1.0f;

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            uint32_t index = row*cols + col; 
            this->voxels[index].color = utils::black;
            this->voxels[index].position = {row,col,0};
        }
    }
    this->InitRandomState();
}

// (Re)initialize to random state
void GameOfLife2D::InitRandomState()  {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution dis(0.5);

    auto [rows, cols, _] = this->GetGridSize().elements;
    this->cells.resize(rows*cols);
    for (uint32_t index = 0; index < rows*cols; index++) {
        this->cells[index] = dis(gen);
    }
}

double GameOfLife2D::Step(double dt)  {
    auto [rows, cols, _] = this->gridSize.elements; 

    for (uint32_t i = 0; i < rows*cols; i++) {
        auto neighbours_alive = this->SumNeighbouringCells(i);
        if (neighbours_alive < 2) {
            this->cells[i] = 0; // dies from underpopulation
        } else if (neighbours_alive == 3) {
            this->cells[i] = 1; // either stays alive or gets created
        } else if (neighbours_alive > 3) {
            this->cells[i] = 0; // dies from overpopulation
        }
    }

    for (uint32_t index = 0; index < rows*cols; index++) {
        this->voxels[index].color = this->cells[index] ? utils::white : utils::black;
    }
    return dt;
}

const utils::Vec<int32_t, 3>& GameOfLife2D::GetGridSize()  {
    return this->gridSize;
}

const std::vector<Voxel>& GameOfLife2D::GetVoxels()  {
    return this->voxels;
}

uint32_t GameOfLife2D::SumNeighbouringCells(uint32_t index) {
    auto [rows, cols, _] = this->GetGridSize().elements;
    int32_t row = index / cols;
    int32_t col = index % cols;
    uint32_t sum_alive = 0;
    int32_t up_row = row-1;
    bool up_exists = up_row >= 0;
    int32_t down_row = row+1;
    bool down_exist = down_row < rows;
    int32_t left_col = col-1;
    bool left_exists = left_col >= 0;
    int32_t right_col = col+1;
    bool right_exists = right_col < cols;

    if (up_exists) {
        if (left_exists) {
            sum_alive += cells[up_row*cols + left_col];
        }
        sum_alive += cells[up_row*cols + col];
        if (right_exists) {
            sum_alive += cells[up_row*cols + right_col];
        }
    }
    if (left_exists) {
        sum_alive += cells[row*cols + left_col];
    }
    if (right_exists) {
        sum_alive += cells[row*cols + right_col];
    }
    if (down_exist) {
        if (left_exists) {
            sum_alive += cells[down_row*cols + left_col];
        }
        sum_alive += cells[down_row*cols];
        if (right_exists) {
            sum_alive += cells[down_row*cols + right_col];
        }
    }
    // TODO tohle je fakt bida, urcite to jde lip

    return sum_alive;
}

}
