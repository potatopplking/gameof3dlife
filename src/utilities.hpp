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
    template<typename T, int size>
    class Vec
    {
        public:
            std::array<T, size> elements;

            template <class U>
            Vec(std::initializer_list<U> list) {
                assert(size >= list.size());
                //std:: cout << "Using std::initializer_list constructor" << std::endl;
                size_t i = 0;
                for (auto& l : list) {
                    this->elements[i++] = static_cast<T>(l);
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

        // tracks position on a sphere. Used for camera
        class SphericCoords
        {
            public:
                // pitch, yaw, radius
                utils::Vec<double, 3> coords;

                SphericCoords(): coords{.0,.0,.0} {}
                SphericCoords(double phi, double theta, double r): coords{phi, theta, r} {}

                utils::Vec<double, 3> toCartesian();
        };

        enum class CoordinateSystem {
            SPHERICAL,
            CARTESIAN
        };

        template <CoordinateSystem T>
        class Pos3D {

            using Pos_t = Vec<double, 3>;

            public:
                Pos3D(std::initializer_list<double> x) : pos(x) {}
                Pos3D() : pos{.0, .0, .0} {}

                inline double operator[](int index) const {
                    return this->pos[index];
                }

                inline double& operator[](int index) {
                    return this->pos[index];
                }

                // TODO add cout

                template <CoordinateSystem Dest_T>
                inline Pos3D<Dest_T> Convert() {
                    if constexpr (Dest_T == T) {
                        return *this;
                    }
                    if constexpr (Dest_T == CoordinateSystem::SPHERICAL) {
                        // convert CARTESIAN to SPHERICAL
                        assert(false);
                        return Pos3D<Dest_T>();
                    } else {
                        // convert SPHERICAL to CARTESIAN 
                        auto phi = pos[0] * M_PI / 180;
                        auto theta = pos[1] * M_PI / 180;
                        auto r = pos[2];

                        auto x = r * sin(phi) * cos(theta);
                        auto y = r * sin(phi) * sin(theta);
                        auto z = r * cos(phi);

                        return Pos3D<Dest_T>{x,y,z};
                    }
                }

                // TODO this should be private, but kept public due to missing cout
                Pos_t pos;
        };

        using SimCoords = utils::Vec<int32_t, 3>;
}
