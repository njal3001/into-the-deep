#pragma once
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>
#include "calc.h"

namespace ITD {

template <class T>
struct Rect;
template <class T>
struct Quad;

using Rectf = Rect<float>;
using Recti = Rect<int>;
using Quadf = Quad<float>;

template <class T>
struct Rect {
    glm::tvec2<T> bl;
    glm::tvec2<T> tr;

    Rect()
        : bl(glm::tvec2<T>())
        , tr(glm::tvec2<T>())
    {
    }

    Rect(const glm::tvec2<T> &bl, const glm::tvec2<T> &tr)
        : bl(bl)
        , tr(tr)
    {
    }

    glm::tvec2<T> bot_right() const
    {
        return glm::tvec2<T>(tr.x, bl.y);
    }

    glm::tvec2<T> top_left() const
    {
        return glm::tvec2<T>(bl.x, tr.y);
    }

    glm::tvec2<T> center() const
    {
        return bl + (tr - bl) / (T)2;
    }

    bool contains(const glm::tvec2<T> &point) const
    {
        return bl.x <= point.x && tr.x >= point.x && bl.y <= point.y &&
               tr.y >= point.y;
    }

    T width() const
    {
        return tr.x - bl.x + 1;
    }

    T height() const
    {
        return tr.y - bl.y + 1;
    }

    Rect operator+(const glm::tvec2<T> &rhs) const
    {
        return Rect(bl + rhs, tr + rhs);
    }

    Rect &operator+=(const glm::tvec2<T> &rhs)
    {
        bl += rhs;
        tr += rhs;
        return *this;
    }
};

template <class T>
struct Quad {
    glm::tvec2<T> a;
    glm::tvec2<T> b;
    glm::tvec2<T> c;
    glm::tvec2<T> d;

    Quad()
        : a(glm::tvec2<T>())
        , b(glm::tvec2<T>())
        , c(glm::tvec2<T>())
        , d(glm::tvec2<T>())
    {
    }

    Quad(const T a, const T b, const T c, const T d)
        : a(a)
        , b(b)
        , c(c)
        , d(d)
    {
    }

    Quad(const Rect<T> rect, const float rotation)
        : a(rect.bl)
        , b(rect.top_left())
        , c(rect.tr)
        , d(rect.bot_right())
    {
        rotate(rotation, rect.center());
    }

    void rotate(const T amount, const glm::tvec2<T> &pivot)
    {
        if (amount == 0)
            return;

        const glm::mat4 mat = Calc::rotate(amount, pivot);

        a = glm::tvec2<T>(mat * glm::tvec4<T>(a, 0, 1));
        b = glm::tvec2<T>(mat * glm::tvec4<T>(b, 0, 1));
        c = glm::tvec2<T>(mat * glm::tvec4<T>(c, 0, 1));
        d = glm::tvec2<T>(mat * glm::tvec4<T>(d, 0, 1));
    }

    glm::vec2 center() const
    {
        return (a + b + c + d) / (T)4;
    }

    Quad operator+(const glm::tvec2<T> &rhs) const
    {
        return Quad(a + rhs, b + rhs, c + rhs, d + rhs);
    }

    Quad &operator+=(const glm::tvec2<T> &rhs)
    {
        a += rhs;
        b += rhs;
        c += rhs;
        d += rhs;
        return *this;
    }

};

}  // namespace ITD
