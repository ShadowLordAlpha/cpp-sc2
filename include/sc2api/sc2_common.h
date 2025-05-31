/*! \file sc2_common.h
    \brief Common data types, including points, rectangles and colors.
*/
#pragma once

#include <concepts>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cassert>

namespace sc2 {

    #define POS_X 0
    #define POS_R 0
    #define POS_Y 1
    #define POS_G 1
    #define POS_Z 2
    #define POS_B 2
    #define POS_W 3
    #define POS_A 3

    // Ensures T is an arithmetic type (int, float, double, etc.)
    template<typename T>
    concept Arithmetic = std::is_arithmetic_v<T>;

    template<Arithmetic T, size_t N>
    class Point {
        std::array<T, N> v;

    public:
        using value_type = T;

        // Construction
        constexpr Point() noexcept : v{} {}
        constexpr explicit Point(const std::array<T, N>& arr) noexcept : v(arr) {}

        // Variadic construction for N elements
        template <typename... Args>
        requires (sizeof...(Args) == N) && (std::conjunction_v<std::is_convertible<Args, T>...>) && (N >= 2)
        constexpr Point(Args... args) noexcept : v{ static_cast<T>(args)... } {}

        // Generic dimension/precision conversion constructor
        template<Arithmetic U, size_t M>
        constexpr Point(const Point<U, M>& other) noexcept {
            constexpr size_t min_dim = (N < M) ? N : M;
            for (size_t i = 0; i < min_dim; ++i)
                v[i] = static_cast<T>(other[i]);
            for (size_t i = min_dim; i < N; ++i)
                v[i] = T{}; // fill extras with zero (if any)
        }


        // Named (swizzle) accessors
        constexpr       T& x()       noexcept { static_assert(N >= 1); return v[POS_X]; }
        constexpr const T& x() const noexcept { static_assert(N >= 1); return v[POS_X]; }
        constexpr       T& y()       noexcept { static_assert(N >= 2); return v[POS_Y]; }
        constexpr const T& y() const noexcept { static_assert(N >= 2); return v[POS_Y]; }
        constexpr       T& z()       noexcept { static_assert(N >= 3); return v[POS_Z]; }
        constexpr const T& z() const noexcept { static_assert(N >= 3); return v[POS_Z]; }
        constexpr       T& w()       noexcept { static_assert(N >= 4); return v[POS_W]; }
        constexpr const T& w() const noexcept { static_assert(N >= 4); return v[POS_W]; }

        constexpr       T& r()       noexcept { static_assert(N >= 1); return v[POS_R]; }
        constexpr const T& r() const noexcept { static_assert(N >= 1); return v[POS_R]; }
        constexpr       T& g()       noexcept { static_assert(N >= 2); return v[POS_G]; }
        constexpr const T& g() const noexcept { static_assert(N >= 2); return v[POS_G]; }
        constexpr       T& b()       noexcept { static_assert(N >= 3); return v[POS_B]; }
        constexpr const T& b() const noexcept { static_assert(N >= 3); return v[POS_B]; }
        constexpr       T& a()       noexcept { static_assert(N >= 4); return v[POS_A]; }
        constexpr const T& a() const noexcept { static_assert(N >= 4); return v[POS_A]; }

        // Index access
        constexpr       T& operator[](size_t i)       { assert(i < N); return v[i]; }
        constexpr const T& operator[](size_t i) const { assert(i < N); return v[i]; }

        // Comparison
        constexpr bool operator==(const Point& rhs) const noexcept {
            for (size_t i = 0; i < N; ++i)
                if (v[i] != rhs.v[i]) return false;
            return true;
        }
        constexpr bool operator!=(const Point& rhs) const noexcept {
            return !(*this == rhs);
        }

        // Arithmetic assignment operators
        constexpr Point& operator+=(const Point& rhs) noexcept {
            for (size_t i = 0; i < N; ++i) v[i] += rhs.v[i];
            return *this;
        }
        constexpr Point& operator-=(const Point& rhs) noexcept {
            for (size_t i = 0; i < N; ++i) v[i] -= rhs.v[i];
            return *this;
        }
        constexpr Point& operator*=(const Point& rhs) noexcept {
            for (size_t i = 0; i < N; ++i) v[i] *= rhs.v[i];
            return *this;
        }
        constexpr Point& operator/=(const Point& rhs) noexcept {
            for (size_t i = 0; i < N; ++i) v[i] /= rhs.v[i];
            return *this;
        }
        constexpr Point& operator*=(const T& scalar) noexcept {
            for (size_t i = 0; i < N; ++i) v[i] *= scalar;
            return *this;
        }
        constexpr Point& operator/=(const T& scalar) noexcept {
            for (size_t i = 0; i < N; ++i) v[i] /= scalar;
            return *this;
        }

        // Norm (length) & dot
        [[nodiscard]] constexpr T normSquared() const noexcept {
            T sum = T{};
            for (size_t i = 0; i < N; ++i) sum += v[i] * v[i];
            return sum;
        }
        [[nodiscard]] T norm() const noexcept {
            if constexpr (std::is_floating_point_v<T>)
                return std::sqrt(normSquared());
            else
                return static_cast<T>(std::sqrt(static_cast<double>(normSquared())));
        }
        [[nodiscard]] constexpr T dot(const Point& rhs) const noexcept {
            T sum = T{};
            for (size_t i = 0; i < N; ++i) sum += v[i] * rhs.v[i];
            return sum;
        }

        // Normalized copy (only enabled for floating-point types)
        [[nodiscard]] Point normalized() const noexcept
        requires std::is_floating_point_v<T> {
            T n = norm();
            if (n == T{}) return *this;
            return *this / n;
        }

        // Conversion to array
        [[nodiscard]] constexpr std::array<T, N> array() const noexcept { return v; }
    };

    // Addition
    template<typename T, typename U, size_t N>
    requires (Arithmetic<T> && Arithmetic<U>)
    constexpr auto operator+(const Point<T, N>& a, const Point<U, N>& b) {
        using ResultType = std::common_type_t<T, U>;
        Point<ResultType, N> result;
        for (size_t i = 0; i < N; ++i)
            result[i] = static_cast<ResultType>(a[i]) + static_cast<ResultType>(b[i]);
        return result;
    }

    // Subtraction
    template<typename T, typename U, size_t N>
    requires (Arithmetic<T> && Arithmetic<U>)
    constexpr auto operator-(const Point<T, N>& a, const Point<U, N>& b) {
        using ResultType = std::common_type_t<T, U>;
        Point<ResultType, N> result;
        for (size_t i = 0; i < N; ++i)
            result[i] = static_cast<ResultType>(a[i]) - static_cast<ResultType>(b[i]);
        return result;
    }

    // Component-wise multiplication
    template<typename T, typename U, size_t N>
    requires (Arithmetic<T> && Arithmetic<U>)
    constexpr auto operator*(const Point<T, N>& a, const Point<U, N>& b) {
        using ResultType = std::common_type_t<T, U>;
        Point<ResultType, N> result;
        for (size_t i = 0; i < N; ++i)
            result[i] = static_cast<ResultType>(a[i]) * static_cast<ResultType>(b[i]);
        return result;
    }

    // Component-wise division
    template<typename T, typename U, size_t N>
    requires (Arithmetic<T> && Arithmetic<U>)
    constexpr auto operator/(const Point<T, N>& a, const Point<U, N>& b) {
        using ResultType = std::common_type_t<T, U>;
        Point<ResultType, N> result;
        for (size_t i = 0; i < N; ++i)
            result[i] = static_cast<ResultType>(a[i]) / static_cast<ResultType>(b[i]);
        return result;
    }

    // --- Scalar <op> Point ---
    template<typename S, typename T, size_t N>
    requires (Arithmetic<T> && Arithmetic<S>)
    constexpr auto operator+(S s, const Point<T, N>& a) {
        using ResultType = std::common_type_t<T, S>;
        Point<ResultType, N> result;
        for (size_t i = 0; i < N; ++i)
            result[i] = static_cast<ResultType>(s) + static_cast<ResultType>(a[i]);
        return result;
    }

    template<typename S, typename T, size_t N>
    requires (Arithmetic<T> && Arithmetic<S>)
    constexpr auto operator+(const Point<T, N>& a, S s) {
        using ResultType = std::common_type_t<T, S>;
        Point<ResultType, N> result;
        for (size_t i = 0; i < N; ++i)
            result[i] = static_cast<ResultType>(s) + static_cast<ResultType>(a[i]);
        return result;
    }

    template<typename S, typename T, size_t N>
    requires (Arithmetic<T> && Arithmetic<S>)
    constexpr auto operator-(S s, const Point<T, N>& a) {
        using ResultType = std::common_type_t<T, S>;
        Point<ResultType, N> result;
        for (size_t i = 0; i < N; ++i)
            result[i] = static_cast<ResultType>(s) - static_cast<ResultType>(a[i]);
        return result;
    }

    template<typename S, typename T, size_t N>
    requires (Arithmetic<T> && Arithmetic<S>)
    constexpr auto operator-(const Point<T, N>& a, S s) {
        using ResultType = std::common_type_t<T, S>;
        Point<ResultType, N> result;
        for (size_t i = 0; i < N; ++i)
            result[i] = static_cast<ResultType>(a[i]) - static_cast<ResultType>(s);
        return result;
    }

    template<typename S, typename T, size_t N>
    requires (Arithmetic<T> && Arithmetic<S>)
    constexpr auto operator*(S s, const Point<T, N>& a) {
        using ResultType = std::common_type_t<T, S>;
        Point<ResultType, N> result;
        for (size_t i = 0; i < N; ++i)
            result[i] = static_cast<ResultType>(s) * static_cast<ResultType>(a[i]);
        return result;
    }

    template<typename S, typename T, size_t N>
    requires (Arithmetic<T> && Arithmetic<S>)
    constexpr auto operator*(const Point<T, N>& a, S s) {
        using ResultType = std::common_type_t<T, S>;
        Point<ResultType, N> result;
        for (size_t i = 0; i < N; ++i)
            result[i] = static_cast<ResultType>(s) * static_cast<ResultType>(a[i]);
        return result;
    }

    template<typename S, typename T, size_t N>
    requires (Arithmetic<T> && Arithmetic<S>)
    constexpr auto operator/(S s, const Point<T, N>& a) {
        using ResultType = std::common_type_t<T, S>;
        Point<ResultType, N> result;
        for (size_t i = 0; i < N; ++i)
            result[i] = static_cast<ResultType>(s) / static_cast<ResultType>(a[i]);
        return result;
    }

    template<typename S, typename T, size_t N>
    requires (Arithmetic<T> && Arithmetic<S>)
    constexpr auto operator/(const Point<T, N>& a, S s) {
        using ResultType = std::common_type_t<T, S>;
        Point<ResultType, N> result;
        for (size_t i = 0; i < N; ++i)
            result[i] = static_cast<ResultType>(a[i]) / static_cast<ResultType>(s);
        return result;
    }

    // Squared Euclidean distance for points of possibly different size
    template<typename T, size_t N, typename U, size_t M>
    constexpr auto distanceSquared(const Point<T, N>& a, const Point<U, M>& b) {
        using ResultType = std::common_type_t<T, U, float>;
        constexpr size_t min_dim = (N < M) ? N : M;
        ResultType sum = ResultType{};
        for (size_t i = 0; i < min_dim; ++i) {
            ResultType diff = static_cast<ResultType>(a[i]) - static_cast<ResultType>(b[i]);
            sum += diff * diff;
        }
        return sum;
    }

    // Euclidean distance
    template<typename T, size_t N, typename U, size_t M>
    inline auto distance(const Point<T, N>& a, const Point<U, M>& b) {
        using std::sqrt;
        using ResultType = decltype(distanceSquared(a, b));
        return static_cast<ResultType>(sqrt(distanceSquared(a, b)));
    }

    template<Arithmetic T, size_t N>
    class Rect {
    public:
        using point_type = Point<T, N>;

        point_type min;
        point_type max;

        // Construction
        // Default: zero rectangle/box
        constexpr Rect() noexcept : min(), max() {}

        // From min and max points
        constexpr Rect(const point_type& min_, const point_type& max_) noexcept : min(min_), max(max_) {}

        // From array (to match Point’s constructors)
        // constexpr explicit Rect(const std::array<T, N>& a, const std::array<T, N>& b) noexcept : min(a), max(b) {}

        // From center and half-size
        static constexpr Rect fromCenter(const point_type& center, const point_type& halfsize) noexcept {
            return Rect(center - halfsize, center + halfsize);
        }

        // Size (width/height/depth)
        constexpr point_type size() const noexcept { return max - min; }

        // Area (N==2) or volume (N==3)
        constexpr T area_or_volume() const noexcept {
            point_type s = size();
            if constexpr (N == 2)
                return s.x() * s.y();
            else if constexpr (N == 3)
                return s.x() * s.y() * s.z();
        }

        // Center point
        constexpr point_type center() const noexcept { return (min + max) * T(0.5); }

        // Check if contains a point
        constexpr bool contains(const point_type& pt) const noexcept {
            for (size_t i = 0; i < N; ++i)
                if (pt[i] < min[i] || pt[i] > max[i]) return false;
            return true;
        }

        // Check if overlaps with another Rect
        constexpr bool overlaps(const Rect& other) const noexcept {
            for (size_t i = 0; i < N; ++i)
                if (max[i] < other.min[i] || min[i] > other.max[i]) return false;
            return true;
        }

        // Expand to include a point
        constexpr Rect& expand(const point_type& pt) noexcept {
            for (size_t i = 0; i < N; ++i) {
                if (pt[i] < min[i]) min[i] = pt[i];
                if (pt[i] > max[i]) max[i] = pt[i];
            }
            return *this;
        }

        // Intersection (returns empty rect if no intersection)
        constexpr Rect intersect(const Rect& other) const noexcept {
            point_type new_min, new_max;
            for (size_t i = 0; i < N; ++i) {
                new_min[i] = std::max(min[i], other.min[i]);
                new_max[i] = std::min(max[i], other.max[i]);
            }
            // If invalid, return empty rect
            for (size_t i = 0; i < N; ++i)
                if (new_min[i] > new_max[i])
                    return Rect(); // or optionally a special 'invalid' Rect
            return Rect(new_min, new_max);
        }
    };

    // Aliases for common types
    using Point2f = Point<float, 2>;
    using Point3f = Point<float, 3>;
    using Point4f = Point<float, 4>;

    using Point2i = Point<int, 2>;
    using Point3i = Point<int, 3>;
    using Point4i = Point<int, 4>;

    using Color = Point<uint8_t, 3>;

    using Rect2f = Rect<float, 2>;
    using Rect2i = Rect<int, 2>;

    // Old methods
    using Point2DI [[deprecated]] = Point2i;
    using Point2D [[deprecated]] = Point2f;
    using Point3DI [[deprecated]] = Point3f;
    using Point3D [[deprecated]] = Point3i;
    using Rect2DI [[deprecated]] = Rect2i;

    namespace Colors {
        static const Color White = Color(255, 255, 255);
        static const Color Red = Color(255, 0, 0);
        static const Color Green = Color(0, 255, 0);
        static const Color Yellow = Color(255, 255, 0);
        static const Color Blue = Color(0, 0, 255);
        static const Color Teal = Color(0, 255, 255);
        static const Color Purple = Color(255, 0, 255);
        static const Color Black = Color(0, 0, 0);
        static const Color Gray = Color(128, 128, 128);
    };

//! Gets a random floating-point number between -1.0 and 1.0.
//!< \return Random floating-point number between -1.0 and 1.0.
    float GetRandomScalar();

//! Gets a random floating-point number between 0.0 and 1.0.
//!< \return Random floating-point number between 0.0 and 1.0.
    float GetRandomFraction();

}
