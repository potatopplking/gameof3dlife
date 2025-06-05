#pragma once

#include "simulations/base.hpp"
#include "utilities.hpp"

namespace Simulation {
namespace FDTD {
/**
 * YeeCell
 *
 */
class Cell {
public:
    
    utils::Vec<double,3> E, H, P; // P for Poynting vector
    double permittivity;
    double permeability;
    double conductivity;

};

class Maxwell: public BaseSimulation {
public:
    void InitRandomState() override
    {

    }

    double Step(double dt) override
    {
        return dt;
    }



};

}
}
