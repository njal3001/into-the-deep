#pragma once
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

namespace Uboat
{
    template<class T> struct Rect;
    template<class T> struct Quad;

    using Rectf = Rect<float>;
    using Quadf = Quad<float>;

    template<class T>
    struct Rect
    {
        glm::tvec2<T> pos;
        glm::tvec2<T> size;

        Rect()
            : pos(glm::tvec2<T>()), size(glm::tvec2<T>())
        {}

        Rect(const glm::tvec2<T>& pos, const glm::tvec2<T>& size)
            : pos(pos), size(size)
        {}

        Rect(const T x, const T y, const T w, const T h)
            : pos(glm::tvec2<T>(x, y)), size(glm::tvec2<T>(w, h))
        {}

        glm::tvec2<T> bot_left() const
        {
            return pos;
        }

        glm::tvec2<T> bot_right() const
        {
            return pos + glm::tvec2<T>(size.x, 0);
        }

        glm::tvec2<T> top_left() const
        {
            return pos + glm::tvec2<T>(0, size.y);
        }

        glm::tvec2<T> top_right() const
        {
            return pos + size;
        }

        glm::tvec2<T> center() const
        {
            return pos + (size / 2.0f);
        }
    };

    template<class T>
    struct Quad
    {
        glm::tvec2<T> a;
        glm::tvec2<T> b;
        glm::tvec2<T> c;
        glm::tvec2<T> d;

        Quad()
            : a(glm::tvec2<T>()), b(glm::tvec2<T>()), c(glm::tvec2<T>()), d(glm::tvec2<T>())
        {}

        Quad(const T a, const T b, const T c, const T d)
            : a(a), b(b), c(c), d(d)
        {}

        Quad(const Rect<T> rect, const float rotation)
            : a(rect.bot_left()), b(rect.top_left()), c(rect.top_right()), d(rect.bot_right())
        {
            rotate(rotation, rect.center());
        }

        void rotate(const T amount, const glm::tvec2<T>& pivot)
        {
            if (amount  == 0) return;

            const glm::tvec3<T> pivot3 = glm::tvec3<T>(pivot, 0);
            const glm::tmat4x4<T> rotate = glm::translate(
                    glm::rotate(glm::translate(glm::tmat4x4<T>(1), pivot3),
                        glm::radians(amount), glm::tvec3<T>(0, 0, 1)), -pivot3);

            a = glm::tvec2<T>(rotate * glm::tvec4<T>(a, 0, 1));
            b = glm::tvec2<T>(rotate * glm::tvec4<T>(b, 0, 1));
            c = glm::tvec2<T>(rotate * glm::tvec4<T>(c, 0, 1));
            d = glm::tvec2<T>(rotate * glm::tvec4<T>(d, 0, 1));
        }

        void offset(const glm::tvec2<T>& off)
        {
            a += off;
            b += off;
            c += off;
            d += off;
        }
    };
}
