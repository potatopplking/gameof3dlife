#pragma once

#include <algorithm>
#include <cmath>

#include "simulations/base.hpp"
#include "utilities.hpp"

namespace Simulation {

class FDTD_1D : public BaseSimulation {
public:

    FDTD_1D(uint32_t rows) :
        BaseSimulation(rows, 1, 1)
    {
        ex = std::unique_ptr<double[]>(new double[rows]);
        hy = std::unique_ptr<double[]>(new double[rows]);
        cb = std::unique_ptr<double[]>(new double[rows]);

        const uint32_t col = 1, stack = 1;
        for (uint32_t row = 0; row < rows; row++)
        {
            this->voxels[row].position = { row, col, stack };
        }
        InitRandomState();
    }

    void InitRandomState() override
    {
        auto [rows, cols, stacks] = this->gridSize.elements;
        for (int32_t row = 0; row < rows; row++)
        {
            this->ex[row] = 0.0;
            this->hy[row] = 0.0;
            this->cb[row] = 1.0;
        }

        KE = rows;      // Number of cells
        kc = KE/2;      // Middle of the grid
        dx = 0.01,      // Cell size [m]
        dt = dx/(2*utils::constants::C0); // Time step [s]
        T = 0.0;        // Time
    
        kstart  = KE/2;  // Boundary between mediums 1 and 2
        epsilon = 4.0;  /* Relative dielectric constant of 
                                    medium 2 */
    
        t0 = 80.0;      // Center of the incident pulse
        spread = 40.0;  // Width of the incident pulse
        freq_in = 2.0e9;// Signal Frequency [Hz]
        carrier = 0.0;  // Signal carrier
        enveloppe = 0.0;// Signal enveloppe
    
        ex_low_1  = 0.0;// Temp variables for
        ex_low_2  = 0.0;// absorbing boundaries
        ex_high_1 = 0.0;
        ex_high_2 = 0.0;
        ex_high_3 = 0.0; 
        ex_high_4 = 0.0;

        // Initialize the medium 2        
        for (int k = kstart; k < KE; k++){ cb[k] = 1.0/epsilon; }
    }

    double Step(double _dt) override
    {
        Log::info("Time: ", dt*T);

        T += 1.0;   // T keeps track of the number of times FDTD loop
                    // is executed.
                           
        // Calculate the Ex field
        for (int k=1; k < KE; k++) { 
            ex[k] += cb[k]*0.5*( hy[k-1] - hy[k] ); 
        }
            
        // Put a Gaussian pulse in the middle
        carrier = sin(2.0*M_PI*freq_in*dt*T);
        enveloppe = exp( -0.5*pow((t0-T)/spread,2.0) );
        ex[5] += carrier*enveloppe;
            
        // Absorbing boundary conditions for Ex
        ex[0]     = ex_low_2;
        ex_low_2  = ex_low_1;
        ex_low_1  = ex[1];
            
        ex[KE-1]  = ex_high_4;
        ex_high_4 = ex_high_3;
        ex_high_3 = ex_high_2;
        ex_high_2 = ex_high_1;
        ex_high_1 = ex[KE-2];
            
        // Calculate the Hy field
        for (int k=0; k < KE-1; k++){ 
            hy[k] += 0.5*( ex[k] - ex[k+1] ); 
        }

        VoxelToColor();

        return dt;
    }

    void AddSource(utils::Vec<uint32_t,3> &position)
    {

    }

private:

    int    KE,
           kc;
    double dx,
           dt;
    double T;
    
    int    kstart;
    double epsilon;
    
    double t0;
    double spread;
    double freq_in ;
    double carrier ;
    double enveloppe ;
    
    double ex_low_1  ,
           ex_low_2  ,
           ex_high_1 ,
           ex_high_2 ,
           ex_high_3 ,
           ex_high_4;



    std::unique_ptr<double[]> ex, hy, cb;

    void update_E()
    {

    }

    void update_H()
    {

    }

    void update_P()
    {

    }

    inline utils::Color FieldStrengthToColor(double fieldValue)
    {
        double c = std::clamp(std::abs(fieldValue), 0.0, 1.0);
        uint8_t R = c * 255;
        uint8_t G = (1 - c) * 255;
        uint8_t B = 0;
        uint8_t alpha = 255;

        return utils::Color{R, G, B, alpha};
    }

    void VoxelToColor() {
      auto [rows, cols, stacks] = this->gridSize.elements;
      for (int index = 0; index < rows * cols * stacks; ++index) {
          this->voxels[index].color = FieldStrengthToColor(this->ex[index]);

      }   
    }
};


};
