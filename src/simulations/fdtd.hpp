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
        idxh.resize(ia, std::vector<vd>(JE, vd(KE)));
        ihxl.resize(ia, std::vector<vd>(JE, vd(KE)));
        ihxh.resize(ia, std::vector<vd>(JE, vd(KE)));
        idyl.resize(IE, std::vector<vd>(ja, vd(KE)));
        idyh.resize(IE, std::vector<vd>(ja, vd(KE)));
        ihyl.resize(IE, std::vector<vd>(ja, vd(KE)));
        ihyh.resize(IE, std::vector<vd>(ja, vd(KE)));
        idzl.resize(IE, std::vector<vd>(JE, vd(ka)));
        idzh.resize(IE, std::vector<vd>(JE, vd(ka)));
        ihzl.resize(IE, std::vector<vd>(JE, vd(ka)));
        ihzh.resize(IE, std::vector<vd>(JE, vd(ka)));

        gi1.resize(IE);        
        gi2.resize(IE);        
        gi3.resize(IE);        
        gj1.resize(IE);        
        gj2.resize(IE);        
        gj3.resize(IE);        
        gk1.resize(IE);        
        gk2.resize(IE);        
        gk3.resize(IE);        
        fi1.resize(IE);        
        fi2.resize(IE);        
        fi3.resize(IE);        
        fj1.resize(IE);        
        fj2.resize(IE);        
        fj3.resize(IE);        
        fk1.resize(IE);        
        fk2.resize(IE);        
        fk3.resize(IE);        

        real_pt.resize(NFREQS, std::vector<vd>(IE, vd(JE)));
        imag_pt.resize(NFREQS, std::vector<vd>(IE, vd(JE)));

        amp.resize(IE, vd(JE));
        phase.resize(IE, vd(JE));

        InitRandomState();
    }

    void InitRandomState() override
    {
        ic = IE/2;
        jc = JE/2;
        kc = KE/2;
        ib = IE - ia - 1;
        jb = JE - ja - 1;
        kb = KE - ka - 1;
        epsz = 8.8e-12;
        muz  = 4 * M_PI * 1.e-7;
        ddx = 0.01;                 /* Cell size */
        dt = ddx / 6e8;             /* Time steps */

        for (int32_t j = 0; j < JE; j++) {
            ez_inc[j] = 0.0;
            hx_inc[j] = 0.0;
            for (int32_t i = 0; i < IE; i++) {
                for (int32_t k = 0; k < KE; k++) {
                    dx[i][j][k] = 0.0;
                    dy[i][j][k] = 0.0;
                    dz[i][j][k] = 0.0;
                    dx[i][j][k] = 0.0;
                    dy[i][j][k] = 0.0;
                    dz[i][j][k] = 0.0;
                    hx[i][j][k] = 0.0;
                    hy[i][j][k] = 0.0;
                    hz[i][j][k] = 0.0;
                    ix[i][j][k] = 0.0;
                    iy[i][j][k] = 0.0;
                    iz[i][j][k] = 0.0;
                    gax[i][j][k] = 1.0;
                    gay[i][j][k] = 1.0;
                    gaz[i][j][k] = 1.0;
                    gbx[i][j][k] = 0.0;
                    gby[i][j][k] = 0.0;
                    gbz[i][j][k] = 0.0;

                    uint32_t index = IndexFromSimCoords(i, j, k);
                    voxels[index].position = {i, j, k};
                    voxels[index].color = utils::black;
                }
            }
        }
        for (int32_t n = 0; n < NFREQS; n++) {
            real_in[n] = 0.0;
            imag_in[n] = 0.0;
            for (int32_t i = 0; i < IE; i++) {
                for (int32_t j = 0; j < JE; j++) {
                    real_pt[n][i][j] = 0.0;
                    imag_pt[n][i][j] = 0.0;
                }
            }
        }

        /* Parameters for the Fourier Transforms */
        freq[0] =  10.e6;
        freq[1] = 100.e6;
        freq[2] = 433.e6;

        for (int32_t n = 0; n < NFREQS; n++) {
            arg[n] = 2 * M_PI * freq[n] * dt;
        }

        for (int32_t i=0; i < ia; i++ ) {
               for (int32_t j=0; j < JE; j++ ) {
                  for (int32_t k=0; k < KE; k++ ) {
                   idxl[i][j][k] = 0.0;
                   idxh[i][j][k] = 0.0;
                   ihxl[i][j][k] = 0.0;
                   ihxh[i][j][k] = 0.0;
            }   }  }

        for (int32_t i=0; i < IE; i++ ) {
           for (int32_t j=0; j < ja; j++ ) {
              for (int32_t k=0; k < KE; k++ ) {
               idyl[i][j][k] = 0.0;
               idyh[i][j][k] = 0.0;
               ihyl[i][j][k] = 0.0;
               ihyh[i][j][k] = 0.0;
        }   }  }

        for (int32_t i=0; i < IE; i++ ) {
           for (int32_t j=0; j < JE; j++ ) {
              for (int32_t k=0; k < ka; k++ ) {
               idzl[i][j][k] = 0.0;
               idzh[i][j][k] = 0.0;
               ihzl[i][j][k] = 0.0;
               ihzh[i][j][k] = 0.0;
        }   }  }
        
        /*   Boundary Conditions */

        for ( i=0; i < IE; i++ ) {
            gi1[i] = 0.;
            fi1[i] = 0.;
            gi2[i] = 1.;
            fi2[i] = 1.;
            gi3[i] = 1.;
            fi3[i] = 1.;
        }

        for ( j=0; j < JE; j++ ) {
            gj1[j] = 0.;
            fj1[j] = 0.;
            gj2[j] = 1.;
            fj2[j] = 1.;
            gj3[j] = 1.;
            fj3[j] = 1.;
        }

        for ( k=0; k < IE; k++ ) {
            gk1[k] = 0.;
            fk1[k] = 0.;
            gk2[k] = 1.;
            fk2[k] = 1.;
            gk3[k] = 1.;
            fk3[k] = 1.;
        }

        // here the original source code asks the user for for n_pml value,
        // we choose some dummy value
        n_pml = 2; // TODO what value should this be?
        
        for ( i=0; i < n_pml; i++ ) {
            xxn = (npml-i)/npml;
            xn  = .33*pow(xxn,3.);

            fi1[i] = xn;
            fi1[IE-i-1] = xn;
            gi2[i] = 1./(1.+xn);
            gi2[IE-i-1] = 1./(1.+xn);
            gi3[i] = (1.-xn)/(1.+xn);
            gi3[IE-i-1] = (1.-xn)/(1.+xn);
            xxn = (npml-i-.5)/npml;
            xn  = .33*pow(xxn,3.);
            gi1[i] = xn;
            gi1[IE-i-2] = xn;
            fi2[i] = 1./(1.+xn);
            fi2[IE-i-2] = 1./(1.+xn);
            fi3[i] = (1.-xn)/(1.+xn);
            fi3[IE-i-2] = (1.-xn)/(1.+xn);
        }

        for ( j=0; j < n_pml; j++ ) {
            xxn = (npml-j)/npml;
            xn  = .33*pow(xxn,3.);
            fj1[j] = xn;
            fj1[JE-j-1] = xn;
            gj2[j] = 1./(1.+xn);
            gj2[JE-j-1] = 1./(1.+xn);
            gj3[j] = (1.-xn)/(1.+xn);
            gj3[JE-j-1] = (1.-xn)/(1.+xn);
            xxn = (npml-j-.5)/npml;
            xn  = .33*pow(xxn,3.);
            gj1[j] = xn;
            gj1[JE-j-2] = xn;
            fj2[j] = 1./(1.+xn);
            fj2[JE-j-2] = 1./(1.+xn);
            fj3[j] = (1.-xn)/(1.+xn);
            fj3[JE-j-2] = (1.-xn)/(1.+xn);
        }

        for ( k=0; k < n_pml; k++ ) {
            xxn = (npml-k)/npml;
            xn  = .33*pow(xxn,3.);
            fk1[k] = xn;
            fk1[KE-k-1] = xn;
            gk2[k] = 1./(1.+xn);
            gk2[KE-k-1] = 1./(1.+xn);
            gk3[k] = (1.-xn)/(1.+xn);
            gk3[KE-k-1] = (1.-xn)/(1.+xn);
            xxn = (npml-k-.5)/npml;
            xn  = .33*pow(xxn,3.);
            gk1[k] = xn;
            gk1[KE-k-2] = xn;
            fk2[k] = 1./(1.+xn);
            fk2[KE-k-2] = 1./(1.+xn);
            fk3[k] = (1.-xn)/(1.+xn);
            fk3[KE-k-2] = (1.-xn)/(1.+xn);
        }


        /* Specify the dielectric sphere */

        epsilon[0] = 1.;
        sigma[0]   = 0;

        // here the original source code asks the user for number of spheres
        numsph = 2;

        for ( i = ia; i < ib; i++ ) {
            for ( j = ja; j < jb; j++ ) {
                for ( k = ka; k < kb; k++ ) {
                    eps = epsilon[0];
                    cond = sigma[0];
                    ydist = static_cast<double>(jc-j);
                    xdist = static_cast<double>(ic-i-.5);
                    zdist = static_cast<double>(kc-k);
                    dist = sqrt(pow(xdist,2.) + pow(ydist,2.) + pow(zdist,2.));
                    for (n=1; n<= numsph; n++) {
                        if( dist <= radius[n]) {
                            eps  =  epsilon[n];
                            cond =  sigma[n] ;
                        }
                    }
                    gax[i][j][k] = 1./(eps + (cond*dt/epsz));
                    gbx[i][j][k] = cond*dt/epsz;
                }
            }
        }

        /* Calculate gax,gbx  */
        for ( i = ia; i < ib; i++ ) {
            for ( j = ja; j < jb; j++ ) {
                for ( k = ka; k < kb; k++ ) {
                    eps = epsilon[0];
                    cond = sigma[0];
                    ydist = (jc-j);
                    xdist = (ic-i-.5);
                    zdist = (kc-k);
                    dist = sqrt(pow(xdist,2.) + pow(ydist,2.) + pow(zdist,2.));
                    for (n=1; n<= numsph; n++) {
                        if( dist <= radius[n]) {
                            eps  =  epsilon[n];
                            cond =  sigma[n] ;
                        }
                    }
                    gax[i][j][k] = 1./(eps + (cond*dt/epsz));
                    gbx[i][j][k] = cond*dt/epsz;
                }
            }
        }
        /* Calculate gay,gby  */
        for ( i = ia; i < ib; i++ ) {
            for ( j = ja; j < jb; j++ ) {
                for ( k = ka; k < kb; k++ ) {
                   eps = epsilon[0];
                   cond = sigma[0];
                   xdist = (ic-i);
                   ydist = (jc-j-.5);
                   zdist = (kc-k);
                   dist = sqrt(pow(xdist,2.) + pow(ydist,2.) + pow(zdist,2.));
                   for (n=1; n<= numsph; n++) {
                       if( dist <= radius[n]) {
                           eps  = epsilon[n] ;
                           cond = sigma[n] ;
                       }
                   }
                   gay[i][j][k] = 1./(eps + (cond*dt/epsz));
                   gby[i][j][k] = cond*dt/epsz;
                }
            }
        }

        /* Calculate gaz,gbz  */
        for ( i = ia; i < ib; i++ ) {
            for ( j = ja; j < jb; j++ ) {
                for ( k = ka; k < kb; k++ ) {
                    eps = epsilon[0];
                    cond = sigma[0];
                    xdist = (ic-i);
                    ydist = (jc-j);
                    zdist = (kc-k-.5);
                    dist = sqrt(pow(xdist,2.) + pow(ydist,2.) + pow(zdist,2.));
                    for (n=1; n<= numsph; n++) {
                        if( dist <= radius[n]) {
                            eps  =  epsilon[n] ;
                            cond =  sigma[n];
                        }
                    }
                    gaz[i][j][k] = 1./(eps + (cond*dt/epsz));
                    gbz[i][j][k] = cond*dt/epsz;
                }
            }
        }

        t0 = 40.0;
        spread = 10.0;
        T = 0;
        nsteps = 1;
    }

    double Step(double _dt) override
    {

        T = T + 1;

        /*  ----   Start of the Main FDTD loop ----  */

        /* Calculate the incident buffer */

        for ( j=1; j < JE; j++ ) {
        ez_inc[j] = ez_inc[j] + .5*( hx_inc[j-1] - hx_inc[j] );
        }

        /* Fourier Tramsform of the incident field */
        for ( m=0; m < NFREQS ; m++ )
        {
            real_in[m] = real_in[m] + cos(arg[m]*T)*ez_inc[ja-1] ;
            imag_in[m] = imag_in[m] - sin(arg[m]*T)*ez_inc[ja-1] ;
        }

        /*  Source */

        /* pulse =  sin(2*pi*400*1e6*dt*T);  */
        pulse =  exp(-.5*(pow((t0-T)/spread,2.0) ));
        ez_inc[3]  = pulse;
        printf("%4.0f  %6.2f \n ",T,pulse);

        /* Boundary conditions for the incident buffer*/

        ez_inc[0] = ez_low_m2;
        ez_low_m2 = ez_low_m1;
        ez_low_m1 = ez_inc[1];

        ez_inc[JE-1]  = ez_high_m2;
        ez_high_m2 = ez_high_m1;
        ez_high_m1 = ez_inc[JE-2];

        /* Calculate the Dx field */

        for ( i=1; i < ia; i++ ) {
            for ( j=1; j < JE; j++ ) {
                for ( k=1; k < KE; k++ ) {
	                  curl_h = ( hz[i][j][k] - hz[i][j-1][k]
	                           - hy[i][j][k] + hy[i][j][k-1]);
                    idxl[i][j][k] = idxl[i][j][k] + curl_h;
                    dx[i][j][k] = gj3[j]*gk3[k]*dx[i][j][k]
	                      + gj2[j]*gk2[k]*.5*(curl_h + gi1[i]*idxl[i][j][k]);
                }
            }
        }

        for ( i=ia; i <= ib; i++ ) {
            for ( j=1; j < JE; j++ ) {
                for ( k=1; k < KE; k++ ) {
                    curl_h = ( hz[i][j][k] - hz[i][j-1][k] 
                             - hy[i][j][k] + hy[i][j][k-1]);
                    dx[i][j][k] = gj3[j]*gk3[k]*dx[i][j][k]
                                + gj2[j]*gk2[j]*.5*curl_h ;
                }
            }
        }

        for ( i=ib+1; i < IE; i++ ) {
            ixh = i - ib - 1;
            for ( j=1; j < JE; j++ ) {
                for ( k=1; k < KE; k++ ) {
                    curl_h = ( hz[i][j][k] - hz[i][j-1][k]
                             - hy[i][j][k] + hy[i][j][k-1]) ;
                    idxh[ixh][j][k] = idxh[ixh][j][k] + curl_h;
                    dx[i][j][k] = gj3[j]*gk3[k]*dx[i][j][k]
                                + gj2[j]*gk2[k]*.5*(curl_h + gi1[i]*idxh[ixh][j][k]);
                }
            }
        }

        /* Calculate the Dy field */

        for ( i=1; i < IE; i++ ) {
            for ( j=1; j < ja; j++ ) {
                for ( k=1; k < KE; k++ ) {
                    curl_h = ( hx[i][j][k] - hx[i][j][k-1]
                             - hz[i][j][k] + hz[i-1][j][k]) ;
                    idyl[i][j][k] = idyl[i][j][k] + curl_h;
                    dy[i][j][k] = gi3[i]*gk3[k]*dy[i][j][k]
                                + gi2[i]*gk2[k]*.5*( curl_h + gj1[j]*idyl[i][j][k]);
                }
            }
        }

        for ( i=1; i < IE; i++ ) {
            for ( j=ja; j <= jb; j++ ) {
                for ( k=1; k < KE; k++ ) {
                    curl_h = ( hx[i][j][k] - hx[i][j][k-1]
                             - hz[i][j][k] + hz[i-1][j][k]) ;
                    dy[i][j][k] = gi3[i]*gk3[k]*dy[i][j][k]
                                + gi2[i]*gk2[k]*.5* curl_h ;
                }
            }
        }

        for ( i=1; i < IE; i++ ) {
            for ( j=jb+1; j < JE; j++ ) {
                jyh = j - jb - 1;
                for ( k=1; k < KE; k++ ) {
                    curl_h = ( hx[i][j][k] - hx[i][j][k-1]
                             - hz[i][j][k] + hz[i-1][j][k]) ;
                    idyh[i][jyh][k] = idyh[i][jyh][k] + curl_h;
                    dy[i][j][k] = gi3[i]*gk3[k]*dy[i][j][k]
                                + gi2[i]*gk2[k]*.5*( curl_h + gj1[j]*idyh[i][jyh][k]);
                }
            }
        }

        /* Incident Dy */
        for ( i=ia; i <= ib; i++ ) {
            for ( j=ja; j <= jb-1; j++ ) {
                dy[i][j][ka]   = dy[i][j][ka]   - .5*hx_inc[j];
                dy[i][j][kb+1] = dy[i][j][kb+1] + .5*hx_inc[j];
            }
        }


        /* Calculate the Dz field */

        for ( i=1; i < IE; i++ ) {
            for ( j=1; j < JE; j++ ) {
                for ( k=0; k < ka; k++ ) {
                    curl_h = ( hy[i][j][k] - hy[i-1][j][k]
                             - hx[i][j][k] + hx[i][j-1][k]) ;
                    idzl[i][j][k] = idzl[i][j][k] + curl_h;
                    dz[i][j][k] = gi3[i]*gj3[j]*dz[i][j][k]
                                + gi2[i]*gj2[j]*.5*( curl_h + gk1[k]*idzl[i][j][k] );
                }
            }
        }

        for ( i=1; i < IE; i++ ) {
            for ( j=1; j < JE; j++ ) {
                for ( k=ka; k <= kb; k++ ) {
                    curl_h = ( hy[i][j][k] - hy[i-1][j][k]
                             - hx[i][j][k] + hx[i][j-1][k]) ;
                    dz[i][j][k] = gi3[i]*gj3[j]*dz[i][j][k]
                                + gi2[i]*gj2[j]*.5* curl_h ;
                }
            }
        }

        for ( i=1; i < IE; i++ ) {
            for ( j=1; j < JE; j++ ) {
                for ( k=kb+1; k < KE; k++ ) {
                    kzh = k - kb - 1;
                    curl_h = ( hy[i][j][k] - hy[i-1][j][k]
                             - hx[i][j][k] + hx[i][j-1][k]) ;
                    idzh[i][j][kzh] = idzh[i][j][kzh] + curl_h;
                    dz[i][j][k] = gi3[i]*gj3[j]*dz[i][j][k]
                                + gi2[i]*gj2[j]*.5*( curl_h + gk1[k]*idzh[i][j][kzh] );
                }
            }
        }

        /* Incident Dz */
        for ( i=ia; i <= ib; i++ ) {
            for ( k=ka; k <= kb; k++ ) {
                dz[i][ja][k] = dz[i][ja][k] + .5*hx_inc[ja-1];
                dz[i][jb][k] = dz[i][jb][k] - .5*hx_inc[jb];
            }
        }

        /*  Source */

        pulse =  sin(2*pi*400*1e6*dt*T);
        for ( k=kc-6; k <= kc+6; k++ ) {
            dz[ic][jc][k] = 0.;
        }
        pulse =  exp(-.5*(pow((t0-T)/spread,2.0) ));
        dz[ic][jc][kc] = pulse;


        /* Calculate the E from D field */
        /* Remember: part of the PML is E=0 at the edges */
        for ( i=1; i < IE-1; i++ ) {
            for ( j=1; j < JE-1; j++ ) {
                for ( k=1; k < KE-1; k++ ) {
                    ex[i][j][k] = gax[i][j][k]*(dx[i][j][k] - ix[i][j][k]);
                    ix[i][j][k] = ix[i][j][k] + gbx[i][j][k]*ex[i][j][k];
                    ey[i][j][k] = gay[i][j][k]*(dy[i][j][k] - iy[i][j][k]);
                    iy[i][j][k] = iy[i][j][k] + gby[i][j][k]*ey[i][j][k];
                    ez[i][j][k] = gaz[i][j][k]*(dz[i][j][k] - iz[i][j][k]);
                    iz[i][j][k] = iz[i][j][k] + gbz[i][j][k]*ez[i][j][k];
                }
            }
        }

        /* Calculate the Fourier transform of Ex. */
        for ( j=0; j < JE; j++ ) {
            for ( i=0; i < JE; i++ ) {
                for ( m=0; m < NFREQS; m++ ) {
                    real_pt[m][i][j] = real_pt[m][i][j] + cos(arg[m]*T)*ez[i][j][kc] ;
                    imag_pt[m][i][j] = imag_pt[m][i][j] + sin(arg[m]*T)*ez[i][j][kc] ;
                }
            }
        }

        /* Calculate the incident field */

        for ( j=0; j < JE-1; j++ ) {
            hx_inc[j] = hx_inc[j] + .5*( ez_inc[j] - ez_inc[j+1] );
        }

        /* Calculate the Hx field */

        for ( i=0; i < ia; i++ ) {
            for ( j=0; j < JE-1; j++ ) {
                for ( k=0; k < KE-1; k++ ) {
                    curl_e = ( ey[i][j][k+1] - ey[i][j][k]
                             - ez[i][j+1][k] + ez[i][j][k]) ;
                    ihxl[i][j][k] = ihxl[i][j][k]  + curl_e;
                    hx[i][j][k] = fj3[j]*fk3[k]*hx[i][j][k]
                                + fj2[j]*fk2[k]*.5*( curl_e + fi1[i]*ihxl[i][j][k] );
                }
            }
        }

        for ( i=ia; i <= ib; i++ ) {
            for ( j=0; j < JE-1; j++ ) {
                for ( k=0; k < KE-1; k++ ) {
                    curl_e = ( ey[i][j][k+1] - ey[i][j][k]
                             - ez[i][j+1][k] + ez[i][j][k]) ;
                    hx[i][j][k] = fj3[j]*fk3[k]*hx[i][j][k]
                                + fj2[j]*fk2[k]*.5*curl_e ;
                }
            }
        }

        for ( i=ib+1; i < IE; i++ ) {
            ixh = i - ib-1;
                for ( j=0; j < JE-1; j++ ) {
                    for ( k=0; k < KE-1; k++ ) {
                        curl_e = ( ey[i][j][k+1] - ey[i][j][k]
                                 - ez[i][j+1][k] + ez[i][j][k]) ;
                        ihxh[ixh][j][k] = ihxh[ixh][j][k]  + curl_e;
                        hx[i][j][k] = fj3[j]*fk3[k]*hx[i][j][k]
                                    + fj2[j]*fk2[k]*.5*( curl_e + fi1[i]*ihxh[ixh][j][k] );
                    }
                }
        }

        /* Incident Hx */
        for ( i=ia; i <= ib; i++ ) {
            for ( k=ka; k <= kb; k++ ) {
                hx[i][ja-1][k] = hx[i][ja-1][k] + .5*ez_inc[ja];
                hx[i][jb][k]   = hx[i][jb][k]   - .5*ez_inc[jb];
            }
        }


        /* Calculate the Hy field */

        for ( i=0; i < IE-1; i++ ) {
            for ( j=0; j < ja; j++ ) {
                for ( k=0; k < KE-1; k++ ) {
                    curl_e = ( ez[i+1][j][k] - ez[i][j][k]
                             - ex[i][j][k+1] + ex[i][j][k]) ;
                    ihyl[i][j][k] = ihyl[i][j][k] + curl_e ;
                    hy[i][j][k] = fi3[i]*fk3[k]*hy[i][j][k]
                                + fi2[i]*fk3[k]*.5*( curl_e + fj1[j]*ihyl[i][j][k] );
                }
            }
        }

        for ( i=0; i < IE-1; i++ ) {
            for ( j=ja; j <= jb; j++ ) {
                for ( k=0; k < KE-1; k++ ) {
                    curl_e = ( ez[i+1][j][k] - ez[i][j][k]
                             - ex[i][j][k+1] + ex[i][j][k]) ;
                    hy[i][j][k] = fi3[i]*fk3[k]*hy[i][j][k]
                                + fi2[i]*fk3[k]*.5*curl_e ;
                }
            }
        }

        for ( i=0; i < IE-1; i++ ) {
            for ( j=jb+1; j < JE; j++ ) {
                jyh = j - jb-1;
                for ( k=0; k < KE-1; k++ ) {
                    curl_e = ( ez[i+1][j][k] - ez[i][j][k]
                             - ex[i][j][k+1] + ex[i][j][k]) ;
                    ihyh[i][jyh][k] = ihyh[i][jyh][k] + curl_e ;
                    hy[i][j][k] = fi3[i]*fk3[k]*hy[i][j][k]
                                + fi2[i]*fk3[k]*.5*( curl_e + fj1[j]*ihyh[i][jyh][k] );
                }
            }
        }

        /* Incident Hy */

        for ( j=ja; j <= jb; j++ ) {
            for ( k=ka; k <= kb; k++ ) {
                hy[ia-1][j][k] = hy[ia-1][j][k] - .5*ez_inc[j];
                hy[ib][j][k]   = hy[ib][j][k]   + .5*ez_inc[j];
            }
        }

        /* Calculate the Hz field */

        for ( i=0; i < IE-1; i++ ) {
            for ( j=0; j < JE-1; j++ ) {
                for ( k=0; k < ka; k++ ) {
                    curl_e = ( ex[i][j+1][k] - ex[i][j][k]
                           -   ey[i+1][j][k] + ey[i][j][k] );
                    ihzl[i][j][k] = ihzl[i][j][k] + curl_e;
                    hz[i][j][k] = fi3[i]*fj3[j]*hz[i][j][k]
                                + fi2[i]*fj2[j]*.5*( curl_e + fk1[k]*ihzl[i][j][k] );
                }
            }
        }

        for ( i=0; i < IE-1; i++ ) {
            for ( j=0; j < JE-1; j++ ) {
                for ( k=ka; k <= kb; k++ ) {
                    curl_e = ( ex[i][j+1][k] - ex[i][j][k]
                            -  ey[i+1][j][k] + ey[i][j][k] );
                    hz[i][j][k] = fi3[i]*fj3[j]*hz[i][j][k]
                                + fi2[i]*fj2[j]*.5*curl_e ;
                }
            }
        }

        for ( i=0; i < IE-1; i++ ) {
            for ( j=0; j < JE-1; j++ ) {
                for ( k=kb+1; k < KE; k++ ) {
                    kzh = k - kb - 1;
                    curl_e = ( ex[i][j+1][k] - ex[i][j][k]
                           -   ey[i+1][j][k] + ey[i][j][k] );
                    ihzh[i][j][kzh] = ihzh[i][j][kzh] + curl_e;
                    hz[i][j][k] = fi3[i]*fj3[j]*hz[i][j][k]
                                + fi2[i]*fj2[j]*.5*( curl_e + fk1[k]*ihzh[i][j][kzh] );
                }
            }
        }

        return dt; // TODO
    }

    void VoxelToColor() {
        auto [rows, cols, stacks] = this->gridSize.elements;
        for (int32_t row = 0; row < rows; row++) {
            for (int32_t col = 0; col < cols; col++) {
                for (int32_t stack = 0; stack < stacks; stack++) {
                    // TODO try something else other than ez
                    uint32_t index = IndexFromSimCoords(row, col, stack);
                    this->voxels[index].color = FieldStrengthToColor(ez[row][col][stack]);
                }
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

    double ez_low_m1, ez_low_m2, ez_high_m1, ez_high_m2;

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
    double dist,xdist,ydist,zdist;

    static constexpr int NFREQS = 3;
    double freq[NFREQS],arg[NFREQS];
    double real_in[5],imag_in[5],amp_in[5],phase_in[5]; // TODO omg...

    double sourceAmplification = 1.0;

};


};
