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
    // generic vector
    template<typename T, int size>
    class Vec
    {
        public:
            std::array<T, size> elements;

            template <class U>
            Vec(std::initializer_list<U> list) {
                std::cout <<"Vec{initializer_list} constructor called" << std::endl;
                assert(size >= list.size());
                //std:: cout << "Using std::initializer_list constructor" << std::endl;
                size_t i = 0;
                for (auto& l : list) {
                    this->elements[i++] = static_cast<T>(l);
                }
            }

            Vec() {
                std::cout <<"Vec() constructor called" << std::endl;
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

        using Color = Vec<uint8_t, 4>;
        const auto black = Color{0,0,0,255};
        const auto white = Color{255,255,255,255};

        enum class CoordinateSystem {
            SPHERICAL,
            CARTESIAN,
            POLAR,
        };

        
        template <CoordinateSystem T, class ElementT, int dimension>
        class NewCSVec : public Vec<ElementT,dimension> {
            public:
            NewCSVec() {
                std::cout <<"NewCSVec() constructor called" << std::endl;
            }
            NewCSVec(std::initializer_list<double> list) : Vec<ElementT,dimension>(list) {
                std::cout <<"NewCSVec{initializer_list} constructor called" << std::endl;
            }

            template <CoordinateSystem Dest_T>
            inline NewCSVec<Dest_T, ElementT, dimension> Convert() {
                if constexpr (Dest_T == T) {
                    return *this;
                }
                if constexpr (Dest_T == CoordinateSystem::SPHERICAL) {
                    // convert CARTESIAN to SPHERICAL
                    assert(false);
                    return NewCSVec<Dest_T, ElementT, dimension>();
                } else if constexpr (dimension == 3) {
                    // convert SPHERICAL to CARTESIAN 
                    auto phi = this->elements[0] * M_PI / 180;
                    auto theta = this->elements[1] * M_PI / 180;
                    auto r = this->elements[2];

                    auto x = r * sin(phi) * cos(theta);
                    auto y = r * sin(phi) * sin(theta);
                    auto z = r * cos(phi);

                    return NewCSVec<Dest_T, ElementT, dimension>{x,y,z};
                }
            }
        };

        // a vector that holds information about coordinate system
        template <CoordinateSystem T>
        class CSVec {

            public:
                CSVec(std::initializer_list<double> x) : pos(x) {}
                CSVec() : pos{.0, .0, .0} {}

                inline double operator[](int index) const {
                    return this->pos[index];
                }

                inline double& operator[](int index) {
                    return this->pos[index];
                }

                // TODO add cout

                template <CoordinateSystem Dest_T>
                inline CSVec<Dest_T> Convert() {
                    if constexpr (Dest_T == T) {
                        return *this;
                    }
                    if constexpr (Dest_T == CoordinateSystem::SPHERICAL) {
                        // convert CARTESIAN to SPHERICAL
                        assert(false);
                        return CSVec<Dest_T>();
                    } else {
                        // convert SPHERICAL to CARTESIAN 
                        auto phi = pos[0] * M_PI / 180;
                        auto theta = pos[1] * M_PI / 180;
                        auto r = pos[2];

                        auto x = r * sin(phi) * cos(theta);
                        auto y = r * sin(phi) * sin(theta);
                        auto z = r * cos(phi);

                        return CSVec<Dest_T>{x,y,z};
                    }
                }

                // TODO this should be private, but kept public due to missing cout
                Vec<double,3> pos;
        };

        using SimCoords = utils::Vec<int32_t, 3>;

template<class T, int dim>
Vec<T,dim> CrossProduct(Vec<T,dim>& a, Vec<T,dim>& b) {
    
    Vec<T,dim> result;
    
    if constexpr (dim == 2) {
        static_assert(false, "pls implement");

    } else if constexpr (dim == 3) {
        result[0] = a[1] * b[2] - a[2] * b[1];
        result[1] = a[2] * b[0] - a[0] * b[2];
        result[2] = a[0] * b[1] - a[1] * b[0];
    } else {
        static_assert(false, "Cross product arguments can have dimension 2 or 3");
    }
    return result;
}
}
