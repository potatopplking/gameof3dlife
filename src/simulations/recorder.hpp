#pragma once

#include <memory>

#include "utilities.hpp"
#include "simulations/base.hpp"

namespace Simulation {

/**
 * Can either directly run and save the simulation,
 * or may act as a passthrough to Window, saving
 * the simulation while it is being run and displayed
 */
class Recorder : public BaseSimulation
{
public:
    Recorder(std::unique_ptr<Simulation::BaseSimulation> sim, std::string filename) :
        m_Filename(filename)
    {
        m_Simulation = std::move(sim);
    }
    ~Recorder() {}

    /*
     * Passthrough methods
     * 
     * Used when the recorder is passed as BaseSimulation
     * to the Window (rendering results and recording
     * at the same time)
     */
    void InitRandomState() override
    {
        m_Simulation->InitRandomState();
    }

    double Step(double dt) override {
        // TODO record
        return m_Simulation->Step(dt);
    }



    void Simulate(uint64_t steps, std::string filename = "") {

    }

    /*
     * Low-level functions, but might still be useful to user
     */
    void Step() {

    }

    void Save() {

    }

private:
    std::unique_ptr<Simulation::BaseSimulation> m_Simulation;
    std::string m_Filename;
};

}
