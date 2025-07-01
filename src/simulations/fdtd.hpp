#pragma once

#include <algorithm>
#include <cmath>

#include "simulations/base.hpp"
#include "utilities.hpp"

namespace Simulation {

inline utils::Color FieldStrengthToColor(double fieldValue)
{
    constexpr double amplification = 5.0;
    double c = std::clamp(std::abs(amplification * fieldValue), 0.0, 1.0);
    uint8_t R = c * 255;
    uint8_t G = (1 - c) * 255;
    uint8_t B = 0;
    uint8_t alpha = 255;

    return utils::Color{R, G, B, alpha};
}

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

    void VoxelToColor() {
      auto [rows, cols, stacks] = this->gridSize.elements;
      for (int index = 0; index < rows * cols * stacks; ++index) {
          this->voxels[index].color = FieldStrengthToColor(this->ex[index]);

      }   
    }
};

class FDTD_2D : public BaseSimulation {
public:

    FDTD_2D(uint32_t rows, uint32_t cols) :
        BaseSimulation(rows, cols, 1)
    {
        dz.resize(rows, std::vector<double>(cols));
        ez.resize(rows, std::vector<double>(cols));
        hx.resize(rows, std::vector<double>(cols));
        hy.resize(rows, std::vector<double>(cols));
        ga.resize(rows, std::vector<double>(cols));
        InitRandomState();
    }

    void InitRandomState() override
    {
        auto [rows, cols, _] = this->gridSize.elements;
        int32_t stack = 0;
        for (int32_t row = 0; row < rows; row++) {
            for (int32_t col = 0; col < cols; col++) {
                dz[row][col] = 0.0;
                ez[row][col] = 0.0;
                hx[row][col] = 0.0;
                hy[row][col] = 0.0;
                ga[row][col] = 1.0;
                uint32_t index = IndexFromSimCoords(row, col, stack);
                voxels[index].position = {row, col, stack};
                voxels[index].color = utils::black;
            }
        }
        IE = rows;
        JE = cols;
        ic = IE / 2;
        jc = JE / 2;
        T = 0.0;
        NSTEPS = 50;
        t0 = 20.0;
        spread = 3.0;

        dx = 0.01;      // Cell size [m]
        dt = dx/(2*utils::constants::C0); // Time step [s]
        freq_in = 2.0e9;// Signal Frequency [Hz]


        //freq_in = 2 * 0.5 * utils::constants::C0 / (rows * dx);
    }

    double Step(double _dt) override
    {
        T += 1.0;   // T keeps track of the number of times FDTD loop
                        // is executed.
                           
        // Calculate the Dz field
        for (int j = 1; j < JE; j++) {
            for (int i = 1; i < IE; i++) {
                dz[i][j] += 0.5*(   hy[i][j] - hy[i-1][j]
                                  - hx[i][j] + hx[i][j-1] ); 
            }
        }
        
        // Put a Gaussian pulse in the middle
        //dz[ic][jc] = exp( -0.5*pow((t0-T)/spread,2.0) );

        double integral_part;
        double xx = std::modf(dt*T * freq_in, &integral_part);

        carrier = sin(2.0*M_PI*freq_in*dt*T);
        //carrier = xx > 0.5 ? 0.0 : 1.0;
        enveloppe = 1.0;//exp( -0.5*pow((t0-T)/spread,2.0) );
        dz[ic][jc] += carrier*enveloppe;
        //dz[IE/4][JE/4] += carrier*enveloppe;

        Log::info("dt*T = ", dt*T, "\t\tcarrier = ", carrier, "\t\tenvelope = ", enveloppe, "\t\txx = ", xx);
        Log::info("f = ", freq_in);

        // Calculate the Ez field
        for (int j = 1; j < JE; j++) {
            for (int i = 1; i < IE; i++) {
                ez[i][j] = ga[i][j]*dz[i][j]; 
                }
            }

        // Calculate the Hx field
        for (int j = 0; j < JE-1; j++) {
            for (int i = 0; i < IE-1; i++) {
                hx[i][j] += 0.5*( ez[i][j] - ez[i][j+1] );
            }
        }
        
        // Calculate the Hy field
        for (int j = 0; j < JE-1; j++) {
            for (int i = 0; i < IE-1; i++) {
                hy[i][j] += 0.5*( ez[i+1][j] - ez[i][j] );
            }
        }
        VoxelToColor();

        return dt; // TODO
    }

    void VoxelToColor() {
        auto [rows, cols, stacks] = this->gridSize.elements;
        uint32_t stack = 0;
        for (int32_t row = 0; row < rows; row++) {
            for (int32_t col = 0; col < cols; col++) {
                // TODO try something else other than ez
                uint32_t index = IndexFromSimCoords(row, col, stack);
                this->voxels[index].color = FieldStrengthToColor(ez[row][col]);
            }
        }
    }   


private:

    std::vector<std::vector<double>> dz, ez, hx, hy, ga;
    int IE, JE, ic, jc;
    double T;
    int NSTEPS;
    double t0, spread;

    double dt, dx;
    double freq_in;
    double carrier;
    double enveloppe;
    


};


};
