//
// Created on 2024/4/18.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef HARMONYOSSKIATEST_DRAWPOINT_H
#define HARMONYOSSKIATEST_DRAWPOINT_H
#include <vector>

template <typename T, typename Predicate> int findLastIndex(const std::vector<T> &vec, Predicate predicate) {
    int count = 1;
    for (auto it = vec.rbegin(); it != vec.rend(); ++it) {
        if (predicate(*it)) {
            return vec.size() - count;
        }
        count++;
    }
    return -1;
}

class DrawPoint {
public:
    enum class DrawPointType { START, LINE };
    DrawPoint(float x, float y, DrawPointType type) : x_(x), y_(y), type_(type) {}

    float x() const { return x_; }
    float y() const { return y_; }
    DrawPointType type() const { return type_; }

private:
    float x_;
    float y_;
    DrawPointType type_;
};

#endif // HARMONYOSSKIATEST_DRAWPOINT_H
