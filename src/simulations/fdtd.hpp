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
    uint8_t R = static_cast<uint8_t>( c * 255 );
    uint8_t G = static_cast<uint8_t>( (1 - c) * 255 );
    uint8_t B = static_cast<uint8_t>( 0 );
    uint8_t alpha = static_cast<uint8_t>( c * 255 );

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

        // square function
        //double integral_part;
        //double xx = std::modf(dt*T * freq_in, &integral_part);
        //carrier = xx > 0.5 ? 0.0 : 1.0;

        carrier = sin(2.0*M_PI*freq_in*dt*T);
        enveloppe = 1.0;//exp( -0.5*pow((t0-T)/spread,2.0) );
        dz[1][1] += carrier * enveloppe * sourceAmplification;
        //dz[ic][jc] += carrier*enveloppe;
        //dz[IE/4][JE/4] += carrier*enveloppe;

        Log::info("dt*T = ", dt*T, "\t\tcarrier = ", carrier, "\t\tenvelope = ", enveloppe);
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

    void TriggerSource() override {
      sourceAmplification = sourceAmplification > 0.01 ? 0.0 : 1.0;
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
    
    double sourceAmplification = 1.0;

};


class FDTD_3D : public BaseSimulation {
public:

    FDTD_3D(uint32_t rows, uint32_t cols, uint32_t stacks) :
        BaseSimulation(rows, cols, stacks)
    {
        using vd = std::vector<double>;
        IE = rows;
        JE = cols;
        KE = stacks;
        ia = ja = ka = 7; // here we should probably assert < rows

        dx.resize(IE, std::vector<vd>(JE, vd(KE)));
        dy.resize(IE, std::vector<vd>(JE, vd(KE)));
        dz.resize(IE, std::vector<vd>(JE, vd(KE)));
        ex.resize(IE, std::vector<vd>(JE, vd(KE)));
        ey.resize(IE, std::vector<vd>(JE, vd(KE)));
        ez.resize(IE, std::vector<vd>(JE, vd(KE)));
        hx.resize(IE, std::vector<vd>(JE, vd(KE)));
        hy.resize(IE, std::vector<vd>(JE, vd(KE)));
        hz.resize(IE, std::vector<vd>(JE, vd(KE)));
        ix.resize(IE, std::vector<vd>(JE, vd(KE)));
        iy.resize(IE, std::vector<vd>(JE, vd(KE)));
        iz.resize(IE, std::vector<vd>(JE, vd(KE)));
        gax.resize(IE, std::vector<vd>(JE, vd(KE)));
        gay.resize(IE, std::vector<vd>(JE, vd(KE)));
        gaz.resize(IE, std::vector<vd>(JE, vd(KE)));
        gbx.resize(IE, std::vector<vd>(JE, vd(KE)));
        gby.resize(IE, std::vector<vd>(JE, vd(KE)));
        gbz.resize(IE, std::vector<vd>(JE, vd(KE)));
        
        ez_inc.resize(JE);
        hx_inc.resize(JE);

        idxl.resize(ia, std::vector<vd>(JE, vd(KE)));
        // TODO tady

        InitRandomState();
    }

    void InitRandomState() override
    {
        // TODO tady
        auto [rows, cols, stacks] = this->gridSize.elements;
        for (int32_t row = 0; row < rows; row++) {
            for (int32_t col = 0; col < cols; col++) {
                for (int32_t stack = 0; stack < stacks; stack++) {
                    dx[row][col][stack] = 0.0;
                    dy[row][col][stack] = 0.0;
                    dz[row][col][stack] = 0.0;
                    dx[row][col][stack] = 0.0;
                    dy[row][col][stack] = 0.0;
                    dz[row][col][stack] = 0.0;
                    hx[row][col][stack] = 0.0;
                    hy[row][col][stack] = 0.0;
                    hz[row][col][stack] = 0.0;
                    ix[row][col][stack] = 0.0;
                    iy[row][col][stack] = 0.0;
                    iz[row][col][stack] = 0.0;
                    gax[row][col][stack] = 0.0;
                    gay[row][col][stack] = 0.0;
                    gaz[row][col][stack] = 0.0;
                    gbx[row][col][stack] = 0.0;
                    gby[row][col][stack] = 0.0;
                    gbz[row][col][stack] = 0.0;

                    uint32_t index = IndexFromSimCoords(row, col, stack);
                    voxels[index].position = {row, col, stack};
                    voxels[index].color = utils::black;
                }
            }
        }
//        IE = rows;
//        JE = cols;
//        ic = IE / 2;
//        jc = JE / 2;
//        T = 0.0;
//        NSTEPS = 50;
//        t0 = 20.0;
//        spread = 3.0;
//
//        dx = 0.01;      // Cell size [m]
//        dt = dx/(2*utils::constants::C0); // Time step [s]
//        freq_in = 2.0e9;// Signal Frequency [Hz]
//
//
//        //freq_in = 2 * 0.5 * utils::constants::C0 / (rows * dx);
    }

    double Step(double _dt) override
    {
//        T += 1.0;   // T keeps track of the number of times FDTD loop
//                        // is executed.
//                           
//        // Calculate the Dz field
//        for (int j = 1; j < JE; j++) {
//            for (int i = 1; i < IE; i++) {
//                dz[i][j] += 0.5*(   hy[i][j] - hy[i-1][j]
//                                  - hx[i][j] + hx[i][j-1] ); 
//            }
//        }
//        
//        // Put a Gaussian pulse in the middle
//        //dz[ic][jc] = exp( -0.5*pow((t0-T)/spread,2.0) );
//
//        // square function
//        //double integral_part;
//        //double xx = std::modf(dt*T * freq_in, &integral_part);
//        //carrier = xx > 0.5 ? 0.0 : 1.0;
//
//        carrier = sin(2.0*M_PI*freq_in*dt*T);
//        enveloppe = 1.0;//exp( -0.5*pow((t0-T)/spread,2.0) );
//        dz[1][1] += carrier * enveloppe * sourceAmplification;
//        //dz[ic][jc] += carrier*enveloppe;
//        //dz[IE/4][JE/4] += carrier*enveloppe;
//
//        Log::info("dt*T = ", dt*T, "\t\tcarrier = ", carrier, "\t\tenvelope = ", enveloppe);
//        Log::info("f = ", freq_in);
//
//        // Calculate the Ez field
//        for (int j = 1; j < JE; j++) {
//            for (int i = 1; i < IE; i++) {
//                ez[i][j] = ga[i][j]*dz[i][j]; 
//                }
//            }
//
//        // Calculate the Hx field
//        for (int j = 0; j < JE-1; j++) {
//            for (int i = 0; i < IE-1; i++) {
//                hx[i][j] += 0.5*( ez[i][j] - ez[i][j+1] );
//            }
//        }
//        
//        // Calculate the Hy field
//        for (int j = 0; j < JE-1; j++) {
//            for (int i = 0; i < IE-1; i++) {
//                hy[i][j] += 0.5*( ez[i+1][j] - ez[i][j] );
//            }
//        }
//        VoxelToColor();
//
//        return dt; // TODO
    }

    void VoxelToColor() {
        auto [rows, cols, stacks] = this->gridSize.elements;
        uint32_t stack = 0;
        for (int32_t row = 0; row < rows; row++) {
            for (int32_t col = 0; col < cols; col++) {
                // TODO try something else other than ez
                uint32_t index = IndexFromSimCoords(row, col, stack);
                //this->voxels[index].color = FieldStrengthToColor(ez[row][col]);
            }
        }
    }

    void TriggerSource() override {
      sourceAmplification = sourceAmplification > 0.01 ? 0.0 : 1.0;
    }


private:

    std::vector<
        std::vector<
            std::vector<
                double
            >
        >
    > dx, dy, dz,
      ex, ey, ez,
      hx, hy, hz,
      ix, iy, iz,
      gax, gay, gaz,
      gbx, gby, gbz,
      //
      idxl, idxh,
      ihxl, ihxh,
      idyl, idyh,
      ihyl, ihyh,
      idzl, idzh,
      ihzl, ihzh,
      //
      real_pt, imag_pt;

    std::vector<
        std::vector<
            double
        >
    > amp, phase;

    std::vector<
        double
    >  gi1, gi2, gi3, 
       gj1, gj2, gj3, 
       gk1, gk2, gk3, 
       fi1, fi2, fi3, 
       fj1, fj2, fj3, 
       fk1, fk2, fk3,
       ez_inc, hx_inc;



    int IE, JE, KE;
    int ia, ja, ka, ib, jb, kb;
    int l,m,n,i,j,k,ic,jc,kc,nsteps,n_pml;
    double ddx,dt,T,epsz,muz,pi,eaf,npml;
    double xn,xxn,xnum,xd,curl_e;
    double t0,spread,pulse;
    int ixh, jyh, kzh;
    int NSTEPS;
    double curl_h,curl_d;
    double radius[10],epsilon[10],sigma[10],eps,cond;
    int numsph;
    float dist,xdist,ydist,zdist;

    static constexpr int NFREQS = 3;
    double freq[NFREQS],arg[NFREQS];
    double real_in[5],imag_in[5],amp_in[5],phase_in[5];

    double sourceAmplification = 1.0;

};


};
