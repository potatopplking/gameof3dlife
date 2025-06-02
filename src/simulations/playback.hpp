#pragma once

#include <iostream>
#include <fstream>

#include "log.hpp"
#include "simulations/base.hpp"

namespace Simulation {

class Playback : public BaseSimulation
{
public:
    Playback(std::string filename)
    {
        m_File = std::ifstream(filename);
        if (!m_File.is_open()) {
            Log::critical("Failed to open playback file ", filename);
            throw std::runtime_error("Failed to open file");
        }
        Log::info("Opened file ", filename, " for playback");
        LoadHeader();
        ResizeVoxels();

        auto [rows, cols, stacks] = gridSize.elements;
        for (int32_t row = 0; row < static_cast<int32_t>(rows); row++) {
            for (int32_t col = 0; col < static_cast<int32_t>(cols); col++) {
                for (int32_t stack = 0; stack < static_cast<int32_t>(stacks); stack++) {
                    uint32_t index = IndexFromSimCoords(row, col, stack);
                    this->voxels[index].color = utils::black;
                    this->voxels[index].position = { row, col, stack };
                }
            }
        }

    }

    ~Playback()
    {
        m_File.close();
    }

    void InitRandomState()
    {
        throw std::runtime_error("Cannot init random state for playback");
    }

    double Step(double dt)
    {
        double actual_dt = LoadStep();
        simulation_time += actual_dt;
        return actual_dt;
    }

private:
    std::ifstream m_File;

    void LoadHeader()
    {
        uint32_t rows, cols, stacks;
        m_File.read(reinterpret_cast<char*>(&rows), sizeof(rows));
        m_File.read(reinterpret_cast<char*>(&cols), sizeof(cols));
        m_File.read(reinterpret_cast<char*>(&stacks), sizeof(stacks));
        if (m_File.fail()) {
            const char* msg = "Failed to read sim dimensions from file";
            Log::critical(msg);
            throw std::runtime_error(msg);
        }
        Log::info("Playback sim dimensions: ",
                rows, " ", cols, " ", stacks
        );
        constexpr uint32_t MAX = 4096;
        assert(rows < MAX && cols < MAX && stacks < MAX);
        this->gridSize.elements[0] = rows;
        this->gridSize.elements[1] = cols;
        this->gridSize.elements[2] = stacks;
    }

    double LoadStep()
    {
        double dt;

        m_File.read(reinterpret_cast<char*>(&dt), sizeof(dt));
        if (m_File.fail()) {
            // nothing more to load or other error
            return 0.0;
        }
        auto [ rows, cols, stacks ] = this->gridSize.elements;
        for (auto row = 0; row < rows; row++) {
            for (auto col = 0; col < cols; col++) {
                for (auto stack = 0; stack < stacks; stack++) {
                    uint8_t R, G, B, A;
                    m_File.read(reinterpret_cast<char*>(&R), 1);
                    m_File.read(reinterpret_cast<char*>(&G), 1);
                    m_File.read(reinterpret_cast<char*>(&B), 1);
                    m_File.read(reinterpret_cast<char*>(&A), 1);
                    if (m_File.fail()) {
                        // nothing more to load or other error
                        Log::info("Nothing more to load (or some error)");
                        return 0.0;
                    }
                    auto index = IndexFromSimCoords(row, col, stack);
                    auto& voxel = voxels[index];
                    voxel.color[0] = R;
                    voxel.color[1] = G;
                    voxel.color[2] = B;
                    voxel.color[3] = A;
                }
            }
        }
        Log::info("Loaded step");

        return dt;
    }
};

}
