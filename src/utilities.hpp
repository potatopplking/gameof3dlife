#pragma once

#include <array>
#include <cmath>
#include <cassert>
#include <cstdint>
#include <iostream>

#ifdef _WIN32
  #include <numbers>
  #define M_PI std::numbers::pi
#endif

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

/*
 * Classes
 */

// generic vector
template<typename T, int size>
class Vec
{
    public:
        std::array<T, size> elements;

        template <class U>
        Vec(std::initializer_list<U> list) {
            //std::cout <<"Vec{initializer_list} constructor called" << std::endl; // TODO debug output
            assert(size == list.size());
            //std:: cout << "Using std::initializer_list constructor" << std::endl;
            size_t i = 0;
            for (auto& l : list) {
                this->elements[i++] = static_cast<T>(l);
            }
        }

        Vec() {
            //std::cout <<"Vec() constructor called" << std::endl;
            for (auto& element : this->elements) {
                element = static_cast<T>(0);
            }
        }

        Vec operator+(const Vec& other) {
            auto result = other;
            for (int i = 0; i < size; i++) {
                result.elements[i] += this->elements[i];
            }
            return result;
        }

        Vec operator-(const Vec& other) {
            auto result = other;
            for (int i = 0; i < size; i++) {
                result.elements[i] -= this->elements[i];
            }
            return result;
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
            Vec result = *this;
            for (auto& element : result.elements) {
                element *= static_cast<T>(scalar);
            }
            return result;
        }
        
        bool operator==(const Vec& other) {
            return this->elements == other.elements;
        }

        bool operator!=(const Vec& other) {
            return !(this->operator==(other));
        }

        T& operator[](int index) {
            return this->elements[index];
        }

        const T& operator[](int index) const {
            return this->elements[index];
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
        //std::cout <<"CSVec() constructor called" << std::endl;
    }
    CSVec(std::initializer_list<double> list) : Vec<ElementT,dimension>(list) {
        //std::cout <<"CSVec{initializer_list} constructor called" << std::endl;
    }

    CSVec(Vec<ElementT, dimension>& vec) {
        std::cout << "CSVec(Vec) called" << std::endl;
        this->elements = vec.elements;
    }

    template <CoordinateSystem SourceCS>
    CSVec(const CSVec<SourceCS, ElementT, dimension>& from_vec) {
        Convert<SourceCS, CS>(*this, from_vec);
    }

    // different coord system should return false, although elements
    // are equal
    template <CoordinateSystem OtherCS,
             typename OtherElementT,
             int OtherDimension>
    bool operator==(const CSVec<OtherCS, OtherElementT, OtherDimension>& other) const {
        if constexpr (
                OtherCS == CS             &&
                typeid(OtherElementT) == typeid(ElementT) &&
                OtherDimension == dimension
                ) {
            // passthrough only if types are identical,
            // explicit conversion is needed for comparison
            // between coordinate systems
            // TODO tady
            return Vec<ElementT,dimension>::operator==(other);
        }
        return false;
    }

    // modify in-place
    template <CoordinateSystem SourceCS, CoordinateSystem TargetCS>
    static void Convert(      CSVec<TargetCS, ElementT, dimension>& target,
                        const CSVec<SourceCS, ElementT, dimension>& source) {
        if constexpr (dimension != 3) {
            // only 3D currently supported
            // TODO use static_assert? But win for some reason doesn't even compile
            assert(false);
        }

        if constexpr (TargetCS == SourceCS) {
            return;
        }
        if constexpr (
            TargetCS == CoordinateSystem::SPHERICAL &&
            SourceCS == CoordinateSystem::CARTESIAN
        ) {
            // convert CARTESIAN to SPHERICAL
            assert(false); // TODO
            return;
        } else if constexpr (
            TargetCS == CoordinateSystem::CARTESIAN &&
            SourceCS == CoordinateSystem::SPHERICAL
        ) {
            // convert SPHERICAL to CARTESIAN 
            auto phi = target.elements[0] * M_PI / 180;
            auto theta = target.elements[1] * M_PI / 180;
            auto r = target.elements[2];

            auto x = r * sin(phi) * cos(theta);
            auto y = r * sin(phi) * sin(theta);
            auto z = r * cos(phi);

            target.elements[0] = x;
            target.elements[1] = y;
            target.elements[2] = z;

            return;
        }
    }

};


 

template<class T, int dim>
Vec<T,dim> CrossProduct(Vec<T,dim>& a, Vec<T,dim>& b) {
    
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

/*
 * Helper classes and utils
 */


using Color = Vec<uint8_t, 4>;
using SimCoords = utils::Vec<int32_t, 3>;
const auto black = Color{0,0,0,255};
const auto white = Color{255,255,255,255};


}
