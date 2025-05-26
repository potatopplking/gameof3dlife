#pragma once

#include <array>
#include <cmath>
#include <chrono>
#include <ranges>
#include <cassert>
#include <cstdint>
#include <iostream>

#ifdef _WIN32
  #include <numbers>
  #define M_PI std::numbers::pi
#endif

#include "log.hpp"

namespace utils
{

/*
 * Enums and types
 */

enum class CoordinateSystem {
    SPHERICAL,
    CARTESIAN,
    POLAR,
};


enum class SphericalIndex {
  // physics convention (ISO 80000-2:2019)
  // https://en.wikipedia.org/wiki/Spherical_coordinate_system
  R = 0,
  THETA,
  PHI
};

enum class CartesianIndex {
  X = 0,
  Y,
  Z
};


/*
 * Functions
 */

template<typename T, int N>
constexpr int len(T (&array)[N])
{
    return N;
}

/*
 * Classes
 */

// generic vector
template<typename T, int size>
class Vec
{
    private:
        inline bool equal(T a, T b) {
            constexpr T MAX_DIFF = static_cast<T>(1e-6);
            return std::abs(a-b) <= MAX_DIFF;
        }

    public:
        std::array<T, size> elements;

        template <class U>
        Vec(std::initializer_list<U> list) {
            Log::profiling_debug("Vec{initializer_list} constructor called" );
            assert(size == list.size());
            size_t i = 0;
            for (auto& l : list) {
                this->elements[i++] = static_cast<T>(l);
            }
        }

        Vec() {
            Log::profiling_debug("Vec() constructor called" );
            for (auto& element : this->elements) {
                element = static_cast<T>(0);
            }
        }

        Vec operator+(const Vec& other) {
//            Log::profiling_debug("Vec::operator+" );
            auto result = other;
            for (int i = 0; i < size; i++) {
                result.elements[i] += this->elements[i];
            }
            return result;
        }

        Vec& operator+=(const Vec& other) {
            for (int i = 0; i < size; i++) {
                this->elements[i] += other.elements[i];
            }
            return *this;
        }

        Vec operator-(const Vec& other) {
//            Log::profiling_debug("Vec::operator+" );
            auto result = *this;
            for (int i = 0; i < size; i++) {
                result.elements[i] -= other.elements[i];
            }
            return result;
        }

        Vec operator-() {
          Vec<T,size> result;
          return result - *this;
        }

        /*template <class U>
        Vec operator*(U& other)
        {
            auto result = *this;
            for (auto& element : result.elements) {
                element *= static_cast<T>(other);
            }
            return result;
        }*/

        Vec operator*(double scalar) {
            //Log::profiling_debug("Vec::operator*" );
            Vec result = *this;
            for (auto& element : result.elements) {
                element *= static_cast<T>(scalar);
            }
            return result;
        }
        
        bool operator==(const Vec& other) {
            //Log::profiling_debug("Vec::operator==" );
            auto zipped = std::ranges::views::zip(this->elements, other.elements);
            // TODO how well is this optimized? Is it vectorized?
            for (auto [a,b] : zipped) {
                if (!this->equal(a, b)) {
                    return false;
                }
            }
            return true;
            //return this->elements == other.elements;
        }

        bool operator!=(const Vec& other) {
            //Log::profiling_debug("Vec::operator!=" );
            return !(this->operator==(other));
        }

        T& operator[](int index) {
            //Log::profiling_debug("Vec::operator[]" );
            return this->elements[index];
        }

        const T& operator[](int index) const {
            //Log::profiling_debug("Vec::operator[] const" );
            return this->elements[index];
        }

        void Normalize() {
          T sum = 0;
          for (auto& elem : this->elements) {
            sum += elem*elem;
          }
          T coeff = sqrt(sum);

          for (auto& elem : this->elements) {
            elem /= coeff;
          }
        }

        friend std::ostream& operator<<(std::ostream& os, const Vec& obj) {
            std::cout << "{ ";
            for (const auto& element : obj.elements) {
                // C++17 constexpr if
                if constexpr (std::is_integral_v<T>) {
                    std::cout << int(element) << " ";
                } else if constexpr (std::is_floating_point_v<T>) {
                    std::cout << float(element) << " ";
                }
            }
            std::cout << "}";
            return os;
        }
    };



// a vector that holds information about coordinate system
template <CoordinateSystem CS, typename ElementT, int dimension>
class CSVec : public Vec<ElementT,dimension> {
    public:
    CSVec() {
        Log::profiling_debug("CSVec() constructor called" );
    }
    CSVec(std::initializer_list<double> list) : Vec<ElementT,dimension>(list) {
        // order must be as given in SphericalIndex or CartesianIndex
        Log::profiling_debug("CSVec{initializer_list} constructor called" );
    }

    CSVec(Vec<ElementT, dimension>& vec) {
        // order must be as given in SphericalIndex or CartesianIndex
        Log::profiling_debug( "CSVec(Vec) called" );
        this->elements = vec.elements;
    }

    CSVec(const Vec<ElementT, dimension>& other) {
        this->elements = other.elements;
    }

    template <CoordinateSystem SourceCS>
    CSVec(const CSVec<SourceCS, ElementT, dimension>& from_vec) {
        Log::profiling_debug( "CSVec(CSVec)" );
        // Log::profiling_debug( "CSVec(CSVec): CS = ", CS );
        // Log::profiling_debug( "CSVec(CSVec): SourceCS = ", SourceCS );
        // Log::profiling_debug( "CSVec(CSVec): dimension = ", dimension );
        Convert<SourceCS, CS>(*this, from_vec);
        Log::profiling_debug( "CSVec(CSVec): Convert done, src = ",  from_vec, " dst = ", *this );
    }

    // different coord system should return false, although elements
    // are equal
    template <CoordinateSystem OtherCS,
             typename OtherElementT,
             int OtherDimension>
    bool operator==(const CSVec<OtherCS, OtherElementT, OtherDimension>& other) {
//        Log::profiling_debug( "CSVec::operator==" );
        if constexpr (
                OtherCS == CS &&
                std::is_same_v<OtherElementT, ElementT> &&
                OtherDimension == dimension
                ) {
            // passthrough only if types are identical,
            // explicit conversion is needed for comparison
            // between coordinate systems
            return Vec<OtherElementT,OtherDimension>::operator==(other);
        }
        return false;
    }

    template<typename IndexT>
    ElementT& operator[](IndexT index) {
        return const_cast<ElementT&>(static_cast<const CSVec&>(*this)[index]);
    }

    template <typename IndexT>
    const ElementT& operator[](IndexT index) const {
        if constexpr (std::is_same<IndexT, CartesianIndex>::value) {
            static_assert(CS == CoordinateSystem::CARTESIAN, "Attempted to access cartesian vector using non-cartesian index");
        } else if constexpr (std::is_same<IndexT, SphericalIndex>::value) {
            static_assert(CS == CoordinateSystem::SPHERICAL, "Attempted to access spherical vector using non-spherical index");
        }
        return this->elements[static_cast<uint32_t>(index)];
    }

    template <CoordinateSystem OtherCS,
             typename OtherElementT,
             int OtherDimension>
    bool operator!=(const CSVec<OtherCS, OtherElementT, OtherDimension>& other) {
        return !this->operator==<OtherCS, OtherElementT, OtherDimension>(other);
    }

    // modify in-place
    template <CoordinateSystem SourceCS, CoordinateSystem TargetCS>
    static void Convert(      CSVec<TargetCS, ElementT, dimension>& target,
                        const CSVec<SourceCS, ElementT, dimension>& source) {
        Log::profiling_debug( "CSVec::Convert" );
        if constexpr (dimension != 3) {
            // only 3D currently supported
            // TODO use static_assert? But win for some reason doesn't even compile
            //      maybe because we need 2 arguments, second is string with error description?
            Log::critical("CSVec::Convert: dimension != 3");
            assert(false);
        }

        if constexpr (TargetCS == SourceCS) {
            Log::profiling_debug( "CSVec::Convert: TargetCS == SourceCS" );
            // no conversion needed
            return;
        }
        if constexpr (
            TargetCS == CoordinateSystem::SPHERICAL &&
            SourceCS == CoordinateSystem::CARTESIAN
        ) {
            // convert CARTESIAN to SPHERICAL
            Log::profiling_debug( "CSVec::Convert: from CARTESIAN to SPHERICAL" );

            auto x = source[CartesianIndex::X];
            auto y = source[CartesianIndex::Y];
            auto z = source[CartesianIndex::Z];

            auto r = std::sqrt(x * x + y * y + z * z);
            auto phi = std::atan2(std::sqrt(x * x + y * y), z);
            auto theta = std::atan2(y, x);

            target[SphericalIndex::R] = r;
            target[SphericalIndex::THETA] = theta;
            target[SphericalIndex::PHI] = phi;
        } else if constexpr (
            TargetCS == CoordinateSystem::CARTESIAN &&
            SourceCS == CoordinateSystem::SPHERICAL
        ) {
            Log::profiling_debug( "CSVec::Convert: from SPHERICAL to CARTESIAN" );
            // convert SPHERICAL to CARTESIAN 
            auto phi = source[SphericalIndex::PHI];
            auto theta = source[SphericalIndex::THETA];
            auto r = source[SphericalIndex::R];

            auto x = r * sin(theta) * cos(phi);
            auto y = r * sin(theta) * sin(phi);
            auto z = r * cos(theta);

            target[CartesianIndex::X] = x;
            target[CartesianIndex::Y] = y;
            target[CartesianIndex::Z] = z;
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const CSVec& obj) {
        std::cout << "{ ";
        for (const auto& element : obj.elements) {
            if constexpr (std::is_integral_v<ElementT>) {
                std::cout << int(element) << " ";
            } else if constexpr (std::is_floating_point_v<ElementT>) {
                std::cout << float(element) << " ";
            }
        }
        std::cout << "}";
        return os;
    }

};

template<class T, int dim>
Vec<T,dim> CrossProduct(const Vec<T,dim>& a, const Vec<T,dim>& b) {
    
    Vec<T,dim> result;
    
    if constexpr (dim == 2) {
        // TODO uncomment - windows compiler was failing here
        //static_assert(false, "pls implement");

    } else if constexpr (dim == 3) {
        result[0] = a[1] * b[2] - a[2] * b[1];
        result[1] = a[2] * b[0] - a[0] * b[2];
        result[2] = a[0] * b[1] - a[1] * b[0];
    } else {
        //static_assert(false, "Cross product arguments can have dimension 2 or 3");
    }
    return result;
}

class TimeStats
{
  using clock = std::chrono::high_resolution_clock;

  public:
    TimeStats() :
        count(0),
        mean(0.0),
        min(0.0),
        max(0.0)
    {
        this->start = clock::now();
    }

    void Start()
    {
        this->start = clock::now();
    }

    void Stop()
    {
        auto stop = clock::now();
        auto diff_us = std::chrono::duration<double>(stop - start);
        Add(diff_us.count());   
    }
      
    double Min(void)  const { return this->min;  };
    double Max(void)  const { return this->max;  };
    double Mean(void) const { return this->mean; };
    double FPS(void) const  { return 1.0 / this->mean; };

    friend std::ostream& operator<<(std::ostream& os, const TimeStats& obj) {
        std::cout << "{ ";
        std::cout << "max: "     <<  1000.0 * obj.Max()
                  << "ms, min: "  << 1000.0 * obj.Min()
                  << "ms, mean: " << 1000.0 * obj.Mean()
                  << "ms, FPS: "  << obj.FPS();
        std::cout << " }";
        return os;
    }


  private:
    double min;
    double max;
    double mean;
    size_t count;
    std::chrono::time_point<clock> start;

    void Add(double value)
    {
      if (value > this->max) {
        this->max = value;
      } else if (value < this->min) {
        this->min = value;
      }
      auto& N = this->count;
      auto& m = this->mean;
      
      m = (m * N + value) / (N+1);
      N++;
    }
};

template <class T>
  class TrackingAllocator : public std::allocator<T>
  {
    public:
      inline T* allocate(size_t count)  {
        this->allocations++;
        Log::debug("Allocating type ", typeid(T).name());
        return static_cast<T*>(::operator new(sizeof(T) * count));
      }

      inline void deallocate(T* ptr, size_t count)  {
        this->allocations--;
        Log::debug("Deallocating type ", typeid(T).name());
        ::operator delete(ptr);
      }

    private:
      size_t allocations = 0;
  };

/*
 * Helper classes and utils
 */


using Color = Vec<uint8_t, 4>;
using SimCoords = utils::Vec<int32_t, 3>;
const auto black = Color{0,0,0,255};
const auto white = Color{255,255,255,255};


}
