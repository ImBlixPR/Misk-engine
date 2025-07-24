#pragma once

namespace misk
{
    struct ivec2 {
        int x;
        int y;
        ivec2() = default;
        template <typename T>
        ivec2(T x_, T y_) : x(static_cast<int>(x_)), y(static_cast<int>(y_)) {}
        ivec2(const ivec2& other_) : x(other_.x), y(other_.y) {}
        ivec2(int x_, int y_) : x(x_), y(y_) {}
        ivec2 operator+(const ivec2& other) const {
            return ivec2(x + other.x, y + other.y);
        }
        ivec2 operator-(const ivec2& other) const {
            return ivec2(x - other.x, y - other.y);
        }
        ivec2& operator=(const ivec2& other) {
            if (this != &other) {
                x = other.x;
                y = other.y;
            }
            return *this;
        }
    };


}
