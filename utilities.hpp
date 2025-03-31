#pragma once

#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>

namespace utils
{
    #define VEC_FROM_XY(obj) utils::Vec<int,2>({static_cast<int>(obj.x), static_cast<int>(obj.y)})

    template<typename T, int size>
    class Vec
    {
        public:
            std::array<T, size> elements;
            Vec(std::initializer_list<T> list) {
                assert(size >= list.size());
                //std:: cout << "Using std::initializer_list constructor" << std::endl;
                size_t i = 0;
                for (auto& l : list) {
                    this->elements[i++] = l;
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

        enum class CoordType {
            SPHERICAL,
            CARTESIAN 
        };
        
        // TODO udelat efektivni reprezentaci pozice, aby umela kartezske i sfericke
        template <class T>
        class Pos3D {
            public:

            private:
                T lol;
        };

        using SimCoords = utils::Vec<int32_t, 3>;

        constexpr auto Ahohj = static_cast<int>(CoordType::SPHERICAL);
}
