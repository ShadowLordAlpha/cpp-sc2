/**
 * @file sc2_common.h
 * @brief Common data types for StarCraft II API including points, rectangles, colors, and utility functions.
 *
 * This header provides a complete geometric library with:
 * - N-dimensional Point class with arithmetic operations
 * - N-dimensional Rect class for bounding boxes
 * - Comprehensive color palette for game development
 * - Thread-safe random number generation utilities
 *
 * All types are designed for performance with constexpr support, move semantics,
 * and modern C++20 features including concepts and three-way comparison.
 *
 * @author StarCraft II API Team
 * @version 2.0
 * @date 2025/05/31
 */
#pragma once

#include <concepts>
#include <limits>
#include <functional>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cassert>
#include <random>

namespace sc2
{

    //! Position constants for accessing Point components by name
    inline constexpr std::size_t POS_X = 0; //!< X coordinate / Red component index
    inline constexpr std::size_t POS_R = 0; //!< Red component index (same as X)
    inline constexpr std::size_t POS_Y = 1; //!< Y coordinate / Green component index
    inline constexpr std::size_t POS_G = 1; //!< Green component index (same as Y)
    inline constexpr std::size_t POS_Z = 2; //!< Z coordinate / Blue component index
    inline constexpr std::size_t POS_B = 2; //!< Blue component index (same as Z)
    inline constexpr std::size_t POS_W = 3; //!< W coordinate / Alpha component index
    inline constexpr std::size_t POS_A = 3; //!< Alpha component index (same as W)

    //! Concept ensuring T is an arithmetic type (int, float, double, etc.)
    template<typename T>
    concept Arithmetic = std::is_arithmetic_v<T>;

    //! Concept for floating point operations requiring precise math
    template<typename T>
    concept FloatingPoint = std::is_floating_point_v<T>;

    /**
     * @class Point
     * @brief N-dimensional point/vector class with comprehensive arithmetic support
     *
     * A highly optimized, generic point class that can represent:
     * - 2D/3D coordinates for game positions
     * - RGB/RGBA colors
     * - N-dimensional vectors for mathematical operations
     *
     * Features:
     * - Constexpr support for compile-time calculations
     * - Cross-dimensional/precision conversions
     * - Named accessors (x/y/z/w and r/g/b/a)
     * - Complete arithmetic operator set
     * - Vector operations (dot, cross, norm, normalize)
     * - STL compatibility with iterators
     *
     * @tparam T Arithmetic type (int, float, double, etc.)
     * @tparam N Number of dimensions (2, 3, 4, etc.)
     *
     * @example
     * ```cpp
     * Point2f position{10.5f, 20.3f};
     * Point3i color{255, 128, 64};
     * auto distance = sc2::distance(pos1, pos2);
     * auto normalized = vector.normalized();
     * ```
     */
    template<Arithmetic T, std::size_t N>
    class Point {
        std::array<T, N> v; //!< Internal storage for components

    public:

        //! @name Construction
        //! @{

        //! Default constructor - initializes all components to zero
        constexpr Point() noexcept : v{} {}

        //! Explicit construction from std::array
        //! @param arr Array of N components
        constexpr explicit Point(const std::array<T, N>& arr) noexcept : v(arr) {}

        //! Variadic constructor for direct component initialization
        //! @param args Exactly N arguments convertible to T
        //! @note Only available for N >= 2 to avoid ambiguity with copy constructor
        template <typename... Args>
        requires (sizeof...(Args) == N) && (std::conjunction_v<std::is_convertible<Args, T>...>) && (N >= 2)
        constexpr Point(Args... args) noexcept : v{ static_cast<T>(args)... } {}

        //! Cross-dimensional and precision conversion constructor
        //! @param other Point with potentially different type or dimensions
        //! @note Copies min(N, M) components, fills extras with zero
        template<Arithmetic U, std::size_t M>
        requires (!std::same_as<T, U> || N != M) // Avoid ambiguity with copy constructor
        constexpr Point(const Point<U, M>& other) noexcept {
            constexpr std::size_t min_dim = std::min(N, M);
            for (std::size_t i = 0; i < min_dim; ++i)
                v[i] = static_cast<T>(other[i]);
            for (std::size_t i = min_dim; i < N; ++i)
                v[i] = T{}; // fill extras with zero (if any)
        }
        //! @}

        //! @name Component Access
        //! @{

//! Macro for generating swizzle accessors to reduce code repetition
#define DEFINE_SWIZZLE_ACCESSOR(name, pos, req_dim) \
        /*! @brief Access to component by name */ \
        constexpr       T& name()       noexcept requires (N >= req_dim) { return v[pos]; } \
        /*! @brief Const access to component by name */ \
        constexpr const T& name() const noexcept requires (N >= req_dim) { return v[pos]; }

        // Spatial coordinate accessors
        DEFINE_SWIZZLE_ACCESSOR(x, POS_X, 1) //!< X coordinate (first component)
        DEFINE_SWIZZLE_ACCESSOR(y, POS_Y, 2) //!< Y coordinate (second component)
        DEFINE_SWIZZLE_ACCESSOR(z, POS_Z, 3) //!< Z coordinate (third component)
        DEFINE_SWIZZLE_ACCESSOR(w, POS_W, 4) //!< W coordinate (fourth component)

        // Color component accessors
        DEFINE_SWIZZLE_ACCESSOR(r, POS_R, 1) //!< Red component (first component)
        DEFINE_SWIZZLE_ACCESSOR(g, POS_G, 2) //!< Green component (second component)
        DEFINE_SWIZZLE_ACCESSOR(b, POS_B, 3) //!< Blue component (third component)
        DEFINE_SWIZZLE_ACCESSOR(a, POS_A, 4) //!< Alpha component (fourth component)

#undef DEFINE_SWIZZLE_ACCESSOR

        //! Array-style access with bounds checking
        //! @param i Component index [0, N)
        //! @return Reference to component
        constexpr       T& operator[](std::size_t i)       { assert(i < N && "Point index out of bounds"); return v[i]; }

        //! Const array-style access with bounds checking
        //! @param i Component index [0, N)
        //! @return Const reference to component
        constexpr const T& operator[](std::size_t i) const { assert(i < N && "Point index out of bounds"); return v[i]; }
        //! @}

        //! @name Comparison
        //! @{

        //! Three-way comparison operator (C++20)
        //! @param rhs Point to compare with
        //! @return Ordering result for lexicographic comparison
        constexpr auto operator<=>(const Point& rhs) const noexcept = default;
        //! @}


        // Unary operators
        [[nodiscard]] constexpr Point operator-() const noexcept {
            Point result;
            for (std::size_t i = 0; i < N; ++i)
                result.v[i] = -v[i];
            return result;
        }

        [[nodiscard]] constexpr Point operator+() const noexcept {
            return *this;
        }

        //! @name Assignment Operators
        //! @{

        //! Macro for generating assignment operators to reduce code repetition
#define DEFINE_POINT_ASSIGNMENT_OP(OP) \
        /*! @brief Component-wise assignment operation with another point */ \
        constexpr Point& operator OP##=(const Point& rhs) noexcept { \
            for (std::size_t i = 0; i < N; ++i) v[i] OP##= rhs.v[i]; \
            return *this; \
        }

#define DEFINE_SCALAR_ASSIGNMENT_OP(OP) \
        /*! @brief Assignment operation with scalar applied to all components */ \
        constexpr Point& operator OP##=(const T& scalar) noexcept { \
            for (std::size_t i = 0; i < N; ++i) v[i] OP##= scalar; \
            return *this; \
        }

        // Point-Point assignment operators
        DEFINE_POINT_ASSIGNMENT_OP(+) //!< Add another point component-wise
        DEFINE_POINT_ASSIGNMENT_OP(-) //!< Subtract another point component-wise
        DEFINE_POINT_ASSIGNMENT_OP(*) //!< Multiply by another point component-wise
        DEFINE_POINT_ASSIGNMENT_OP(/) //!< Divide by another point component-wise

        // Scalar assignment operators
        DEFINE_SCALAR_ASSIGNMENT_OP(+) //!< Add scalar to all components
        DEFINE_SCALAR_ASSIGNMENT_OP(-) //!< Subtract scalar from all components
        DEFINE_SCALAR_ASSIGNMENT_OP(*) //!< Multiply all components by scalar
        DEFINE_SCALAR_ASSIGNMENT_OP(/) //!< Divide all components by scalar

#undef DEFINE_POINT_ASSIGNMENT_OP
#undef DEFINE_SCALAR_ASSIGNMENT_OP
        //! @}

        //! @name Vector Operations
        //! @{

        //! Calculate squared magnitude (length²) of vector
        //! @return Sum of squared components
        //! @note More efficient than norm() when you only need relative comparisons
        [[nodiscard]] constexpr T normSquared() const noexcept {
            T sum = T{};
            for (std::size_t i = 0; i < N; ++i) sum += v[i] * v[i];
            return sum;
        }

        //! Calculate magnitude (length) of vector
        //! @return Euclidean length of vector
        //! @note Uses appropriate precision for integer types
        [[nodiscard]] T norm() const noexcept {
            if constexpr (FloatingPoint<T>)
                return std::sqrt(normSquared());
            else
                return static_cast<T>(std::sqrt(static_cast<long double>(normSquared())));
        }

        //! Calculate dot product with another vector
        //! @param rhs Other vector
        //! @return Scalar dot product
        [[nodiscard]] constexpr T dot(const Point& rhs) const noexcept {
            T sum = T{};
            for (std::size_t i = 0; i < N; ++i) sum += v[i] * rhs.v[i];
            return sum;
        }

        //! Calculate cross product (3D vectors only)
        //! @param rhs Other 3D vector
        //! @return Cross product vector perpendicular to both inputs
        [[nodiscard]] constexpr Point cross(const Point& rhs) const noexcept requires (N == 3) {
            return Point(
                    v[1] * rhs.v[2] - v[2] * rhs.v[1],
                    v[2] * rhs.v[0] - v[0] * rhs.v[2],
                    v[0] * rhs.v[1] - v[1] * rhs.v[0]
            );
        }

        //! Create normalized copy (unit vector) with epsilon check
        //! @return Normalized vector, or original if length < epsilon
        //! @note Only available for floating-point types
        [[nodiscard]] Point normalized() const noexcept requires FloatingPoint<T> {
            T n = norm();
            if (n < std::numeric_limits<T>::epsilon()) [[unlikely]]
                return *this;
            return *this / n;
        }

        //! Linear interpolation between this point and another
        //! @param other Target point
        //! @param t Interpolation parameter [0.0, 1.0]
        //! @return Interpolated point
        //! @note Only available for floating-point types
        [[nodiscard]] constexpr Point lerp(const Point& other, T t) const noexcept requires FloatingPoint<T> {
            Point result;
            for (std::size_t i = 0; i < N; ++i) {
                result.v[i] = std::lerp(v[i], other.v[i], t);
            }
            return result;
        }

        //! Check if all components are finite (not NaN or infinite)
        //! @return true if all components are finite
        //! @note Only available for floating-point types
        [[nodiscard]] constexpr bool isFinite() const noexcept requires FloatingPoint<T> {
            for (std::size_t i = 0; i < N; ++i) {
                if (!std::isfinite(v[i])) [[unlikely]]
                    return false;
            }
            return true;
        }
        //! @}

        //! @name Array Conversion
        //! @{

        //! Get const reference to underlying array
        //! @return Const reference to internal std::array
        [[nodiscard]] constexpr const std::array<T, N>& array() const noexcept { return v; }

        //! Get mutable reference to underlying array
        //! @return Mutable reference to internal std::array
        [[nodiscard]] constexpr std::array<T, N>& array() noexcept { return v; }
        //! @}

        //! @name Iterator Support
        //! @{

        //! Begin iterator for range-based loops
        constexpr auto begin() noexcept { return v.begin(); }
        //! End iterator for range-based loops
        constexpr auto end() noexcept { return v.end(); }
        //! Const begin iterator
        constexpr auto begin() const noexcept { return v.begin(); }
        //! Const end iterator
        constexpr auto end() const noexcept { return v.end(); }
        //! Const begin iterator (explicit)
        constexpr auto cbegin() const noexcept { return v.cbegin(); }
        //! Const end iterator (explicit)
        constexpr auto cend() const noexcept { return v.cend(); }
        //! @}
    };

    //! @name Implementation Details
    //! Internal helper structures for operator implementations
    namespace detail
    {
        //! Generic binary operation helper for Point-Point operations
        template<typename Op>
        struct point_op {
            template<Arithmetic T, Arithmetic U, std::size_t N>
            [[nodiscard]] constexpr auto operator()(const Point<T, N>& a, const Point<U, N>& b) const {
                using ResultType = std::common_type_t<T, U>;
                Point<ResultType, N> result;
                for (std::size_t i = 0; i < N; ++i)
                    result[i] = Op{}(static_cast<ResultType>(a[i]), static_cast<ResultType>(b[i]));
                return result;
            }
        };

        //! Generic scalar operation helper for Point-Scalar operations
        template<typename Op>
        struct scalar_op {
            template<Arithmetic S, Arithmetic T, std::size_t N>
            [[nodiscard]] constexpr auto operator()(S scalar, const Point<T, N>& point) const {
                using ResultType = std::common_type_t<T, S>;
                Point<ResultType, N> result;
                for (std::size_t i = 0; i < N; ++i)
                    result[i] = Op{}(static_cast<ResultType>(scalar), static_cast<ResultType>(point[i]));
                return result;
            }

            template<Arithmetic T, Arithmetic S, std::size_t N>
            [[nodiscard]] constexpr auto operator()(const Point<T, N>& point, S scalar) const {
                using ResultType = std::common_type_t<T, S>;
                Point<ResultType, N> result;
                for (std::size_t i = 0; i < N; ++i)
                    result[i] = Op{}(static_cast<ResultType>(point[i]), static_cast<ResultType>(scalar));
                return result;
            }
        };
    }

    //! @name Binary Operators
    //! Free functions providing arithmetic operations between Points and scalars
    //! @{

    //! Macro to define Point-Point binary operators
#define DEFINE_POINT_POINT_OP(OP, FUNC) \
    /*! @brief Component-wise operation between two points */ \
    template<Arithmetic T, Arithmetic U, std::size_t N> \
    [[nodiscard]] constexpr auto operator OP(const Point<T, N>& a, const Point<U, N>& b) { \
        return detail::point_op<std::FUNC<>>{}(a, b); \
    }

    //! Macro to define both scalar-point and point-scalar operators
#define DEFINE_SCALAR_POINT_OP(OP, FUNC) \
    /*! @brief Operation between scalar and point */ \
    template<Arithmetic S, Arithmetic T, std::size_t N> \
    [[nodiscard]] constexpr auto operator OP(S s, const Point<T, N>& a) { \
        return detail::scalar_op<std::FUNC<>>{}(s, a); \
    }                                    \
    /*! @brief Operation between point and scalar */ \
    template<Arithmetic S, Arithmetic T, std::size_t N> \
    [[nodiscard]] constexpr auto operator OP(const Point<T, N>& a, S s) { \
        return detail::scalar_op<std::FUNC<>>{}(a, s); \
    }

    // Point-Point operations
    DEFINE_POINT_POINT_OP(+, plus) //!< Component-wise addition
    DEFINE_POINT_POINT_OP(-, minus) //!< Component-wise subtraction
    DEFINE_POINT_POINT_OP(*, multiplies)//!< Component-wise multiplication
    DEFINE_POINT_POINT_OP(/, divides) //!< Component-wise division

    // Scalar operations
    DEFINE_SCALAR_POINT_OP(+, plus) //!< Add scalar to all components
    DEFINE_SCALAR_POINT_OP(-, minus) //!< Subtract scalar from all components
    DEFINE_SCALAR_POINT_OP(*, multiplies) //!< Multiply all components by scalar
    DEFINE_SCALAR_POINT_OP(/, divides) //!< Divide all components by scalar

#undef DEFINE_POINT_POINT_OP
#undef DEFINE_SCALAR_POINT_OP
    //! @}

    //! @name Template Argument Deduction
    //! @{

    //! Deduction guide for constructor with multiple arguments
    //! Automatically determines common type and dimension count
    template<Arithmetic T, Arithmetic... Args>
    Point(T, Args...) -> Point<std::common_type_t<T, Args...>, 1 + sizeof...(Args)>;
    //! @}

    //! @name Distance Functions
    //! Cross-dimensional distance calculations with automatic type promotion
    //! @{

    //! Calculate squared distance between two points (more efficient)
    //! @param a First point
    //! @param b Second point
    //! @return Squared Euclidean distance
    //! @note Works with different dimensions - uses minimum dimension count
    template<Arithmetic T, std::size_t N, Arithmetic U, std::size_t M>
    [[nodiscard]] constexpr auto distanceSquared(const Point<T, N>& a, const Point<U, M>& b) {
        using ResultType = std::common_type_t<T, U, double>;
        constexpr std::size_t min_dim = std::min(N, M);
        ResultType sum = ResultType{};
        for (std::size_t i = 0; i < min_dim; ++i) {
            ResultType diff = static_cast<ResultType>(a[i]) - static_cast<ResultType>(b[i]);
            sum += diff * diff;
        }
        return sum;
    }

    //! Calculate Euclidean distance between two points
    //! @param a First point
    //! @param b Second point
    //! @return Euclidean distance
    //! @note Works with different dimensions - uses minimum dimension count
    template<Arithmetic T, std::size_t N, Arithmetic U, std::size_t M>
    [[nodiscard]] inline auto distance(const Point<T, N>& a, const Point<U, M>& b) {
        using ResultType = decltype(distanceSquared(a, b));
        return static_cast<ResultType>(std::sqrt(distanceSquared(a, b)));
    }
    //! @}

    /**
     * @class Rect
     * @brief N-dimensional axis-aligned rectangle/bounding box class
     *
     * Represents rectangular regions in N-dimensional space defined by
     * minimum and maximum corner points. Provides comprehensive geometric
     * operations for collision detection, spatial queries, and region management.
     *
     * Features:
     * - Constexpr support for compile-time calculations
     * - Intersection and overlap testing
     * - Volume/area calculations for any dimension
     * - Point containment testing
     * - Region expansion and manipulation
     *
     * @tparam T Arithmetic type for coordinates
     * @tparam N Number of dimensions
     *
     * @example
     * ```cpp
     * Rect2f bounds{{0, 0}, {100, 50}};
     * bool contains = bounds.contains({25, 25});
     * auto intersection = rect1.intersect(rect2);
     * auto area = bounds.volume(); // 2D: area, 3D: volume
     * ```
     */
    template<Arithmetic T, std::size_t N>
    class Rect {
    public:
        using point_type = Point<T, N>; //!< Point type for corners

        point_type min; //!< Minimum corner (bottom-left in 2D)
        point_type max; //!< Maximum corner (top-right in 2D)

        //! @name Construction
        //! @{

        //! Default constructor - creates zero-sized rectangle at origin
        constexpr Rect() noexcept : min(), max() {}

        //! Constructor from corner points
        //! @param minimum Minimum corner point
        //! @param maximum Maximum corner point
        constexpr Rect(const point_type& minimum, const point_type& maximum) noexcept : min(minimum), max(maximum) {}

        //! Create rectangle from center point and half-size
        //! @param center Center point of rectangle
        //! @param halfsize Half-size in each dimension
        //! @return Rectangle centered at given point
        [[nodiscard]] static constexpr Rect fromCenter(const point_type& center, const point_type& halfsize) noexcept {
            return Rect(center - halfsize, center + halfsize);
        }
        //! @}

        //! @name Properties
        //! @{

        //! Get size (width/height/depth) of rectangle
        //! @return Size vector (max - min)
        [[nodiscard]] constexpr point_type size() const noexcept { return max - min; }

        //! Calculate volume (area in 2D, volume in 3D, hypervolume in N-D)
        //! @return Product of all dimensions
        [[nodiscard]] constexpr T volume() const noexcept {
            point_type s = size();
            T result = T{1};
            for (std::size_t i = 0; i < N; ++i) result *= s[i];
            return result;
        }

        //! Get center point of rectangle
        //! @return Geometric center point
        [[nodiscard]] constexpr point_type center() const noexcept { return (min + max) * T{0.5}; }

        //! Check if rectangle is valid (min <= max for all dimensions)
        //! @return true if rectangle has non-negative size in all dimensions
        [[nodiscard]] constexpr bool isValid() const noexcept {
            for (std::size_t i = 0; i < N; ++i)
                if (min[i] > max[i]) [[unlikely]]
                    return false;
            return true;
        }
        //! @}

        //! @name Geometric Queries
        //! @{

        //! Test if rectangle contains a point
        //! @param pt Point to test
        //! @return true if point is inside or on boundary
        [[nodiscard]] constexpr bool contains(const point_type& pt) const noexcept {
            for (std::size_t i = 0; i < N; ++i)
                if (pt[i] < min[i] || pt[i] > max[i]) [[unlikely]]
                    return false;
            return true;
        }

        //! Test if this rectangle overlaps with another
        //! @param other Other rectangle to test
        //! @return true if rectangles overlap or touch
        [[nodiscard]] constexpr bool overlaps(const Rect& other) const noexcept {
            for (std::size_t i = 0; i < N; ++i)
                if (max[i] < other.min[i] || min[i] > other.max[i]) [[unlikely]]
                    return false;
            return true;
        }
        //! @}

        //! @name Modification
        //! @{

        //! Expand rectangle to include a point
        //! @param pt Point to include
        //! @return Reference to this rectangle (for chaining)
        constexpr Rect& expand(const point_type& pt) noexcept {
            for (std::size_t i = 0; i < N; ++i) {
                min[i] = std::min(min[i], pt[i]);
                max[i] = std::max(max[i], pt[i]);
            }
            return *this;
        }

        //! Calculate intersection with another rectangle
        //! @param other Rectangle to intersect with
        //! @return Intersection rectangle (empty if no intersection)
        [[nodiscard]] constexpr Rect intersect(const Rect& other) const noexcept {
            point_type new_min, new_max;
            for (std::size_t i = 0; i < N; ++i) {
                new_min[i] = std::max(min[i], other.min[i]);
                new_max[i] = std::min(max[i], other.max[i]);
                if (new_min[i] > new_max[i]) [[unlikely]]
                    return Rect(); // No intersection - return empty rectangle
            }
            return Rect(new_min, new_max);
        }
        //! @}

        //! @name Comparison
        //! @{

        //! Three-way comparison operator (C++20)
        //! @param other Rectangle to compare with
        //! @return Ordering result based on min point, then max point
        constexpr auto operator<=>(const Rect& other) const noexcept = default;
        //! @}
    };

    //! @name Common Type Aliases
    //! Convenient aliases for frequently used Point and Rect types
    //! @{

    // Float precision points
    using Point2f = Point<float, 2>; //!< 2D point with float precision
    using Point3f = Point<float, 3>; //!< 3D point with float precision
    using Point4f = Point<float, 4>; //!< 4D point with float precision

    // Integer points
    using Point2i = Point<int, 2>; //!< 2D point with integer coordinates
    using Point3i = Point<int, 3>; //!< 3D point with integer coordinates
    using Point4i = Point<int, 4>; //!< 4D point with integer coordinates

    // Color type
    using Color = Point<std::uint8_t, 3>; //!< RGB color with 8-bit components

    // Rectangle types
    using Rect2f = Rect<float, 2>; //!< 2D rectangle with float precision
    using Rect2i = Rect<int, 2>; //!< 2D rectangle with integer coordinates
    //! @}

    //! @name Deprecated Aliases
    //! Legacy type names maintained for backward compatibility
    //! @{

    using Point2DI [[deprecated("Use Point2i instead")]] = Point2i; //!< @deprecated Use Point2i
    using Point2D [[deprecated("Use Point2f instead")]] = Point2f; //!< @deprecated Use Point2f
    using Point3DI [[deprecated("Use Point3i instead")]] = Point3i; //!< @deprecated Use Point3i
    using Point3D [[deprecated("Use Point3f instead")]] = Point3f; //!< @deprecated Use Point3f
    using Rect2DI [[deprecated("Use Rect2i instead")]] = Rect2i; //!< @deprecated Use Rect2i
    //! @}

    /**
     * @brief Predefined color constants for game development
     *
     * Comprehensive collection of RGB colors organized by category:
     * - Basic colors (black, white, gray variations)
     * - Primary and secondary colors
     * - Extended color palette
     * - Nature-inspired colors
     * - StarCraft II player colors
     * - UI and debug colors
     *
     * All colors use 8-bit RGB components (0-255 range).
     */
    namespace Colors {
        //! @name Basic Colors
        //! Fundamental colors and grayscale values
        //! @{
        inline constexpr Color White{255, 255, 255}; //!< Pure white
        inline constexpr Color Black{0, 0, 0}; //!< Pure black
        inline constexpr Color Gray{128, 128, 128}; //!< Medium gray
        inline constexpr Color Silver{192, 192, 192}; //!< Light gray (silver)
        inline constexpr Color DarkGray{64, 64, 64}; //!< Dark gray
        inline constexpr Color LightGray{211, 211, 211}; //!< Very light gray
        //! @}

        //! @name Primary Colors
        //! RGB primary colors
        //! @{
        inline constexpr Color Red{255, 0, 0}; //!< Pure red
        inline constexpr Color Green{0, 255, 0}; //!< Pure green (lime)
        inline constexpr Color Blue{0, 0, 255}; //!< Pure blue
        //! @}

        //! @name Secondary Colors
        //! RGB secondary and tertiary colors
        //! @{
        inline constexpr Color Yellow{255, 255, 0}; //!< Yellow (red + green)
        inline constexpr Color Magenta{255, 0, 255}; //!< Magenta (red + blue)
        inline constexpr Color Cyan{0, 255, 255}; //!< Cyan (green + blue)
        inline constexpr Color Purple{128, 0, 128}; //!< Dark purple
        inline constexpr Color Teal{0, 128, 128}; //!< Dark cyan
        //! @}

        //! @name Extended Colors
        //! Additional commonly used colors
        //! @{
        inline constexpr Color Orange{255, 165, 0}; //!< Orange
        inline constexpr Color Pink{255, 192, 203}; //!< Light pink
        inline constexpr Color Brown{165, 42, 42}; //!< Brown
        inline constexpr Color Maroon{128, 0, 0}; //!< Dark red
        inline constexpr Color Navy{0, 0, 128}; //!< Dark blue
        inline constexpr Color Olive{128, 128, 0}; //!< Dark yellow
        inline constexpr Color Lime{0, 255, 0}; //!< Bright green
        inline constexpr Color Aqua{0, 255, 255}; //!< Bright cyan
        inline constexpr Color Fuchsia{255, 0, 255}; //!< Bright magenta
        //! @}

        //! @name Nature Colors
        //! Colors inspired by nature
        //! @{
        inline constexpr Color ForestGreen{34, 139, 34}; //!< Deep forest green
        inline constexpr Color SkyBlue{135, 206, 235}; //!< Light sky blue
        inline constexpr Color Gold{255, 215, 0}; //!< Golden yellow
        inline constexpr Color Coral{255, 127, 80}; //!< Coral pink-orange
        inline constexpr Color Salmon{250, 128, 114}; //!< Salmon pink
        inline constexpr Color Violet{238, 130, 238}; //!< Violet purple
        inline constexpr Color Indigo{75, 0, 130}; //!< Deep indigo
        inline constexpr Color Crimson{220, 20, 60}; //!< Deep red
        inline constexpr Color Turquoise{64, 224, 208}; //!< Turquoise blue-green
        inline constexpr Color Khaki{240, 230, 140}; //!< Light brown
        //! @}

        //! @name StarCraft II Player Colors
        //! Official player colors from StarCraft II
        //! @{
        inline constexpr Color PlayerBlue{0, 66, 255}; //!< Player 1 - Blue
        inline constexpr Color PlayerRed{180, 20, 30}; //!< Player 2 - Red
        inline constexpr Color PlayerTeal{28, 167, 234}; //!< Player 3 - Teal
        inline constexpr Color PlayerPurple{84, 0, 129}; //!< Player 4 - Purple
        inline constexpr Color PlayerYellow{235, 225, 41}; //!< Player 5 - Yellow
        inline constexpr Color PlayerOrange{254, 138, 14}; //!< Player 6 - Orange
        inline constexpr Color PlayerGreen{22, 128, 0}; //!< Player 7 - Green
        inline constexpr Color PlayerPink{204, 166, 252}; //!< Player 8 - Pink
        //! @}

        //! @name UI and Debug Colors
        //! Colors for user interface and debugging
        //! @{
        inline constexpr Color Warning{255, 140, 0}; //!< Warning orange
        inline constexpr Color Error{220, 20, 60}; //!< Error red
        inline constexpr Color Success{50, 205, 50}; //!< Success green
        inline constexpr Color Info{70, 130, 180}; //!< Information blue
        inline constexpr Color Debug{138, 43, 226}; //!< Debug purple
        //! @}
    };

    //! @name Random Number Generation
    //! Thread-safe random number utilities for game development
    //! @{

    /**
     * @brief Generate random floating-point number in range [-1.0, 1.0]
     *
     * Thread-safe random number generator using Mersenne Twister.
     * Each thread maintains its own generator state for optimal performance.
     *
     * @return Random float in range [-1.0f, 1.0f]
     *
     * @example
     * ```cpp
     * float randomDirection = GetRandomScalar(); // -1.0 to 1.0
     * Point2f randomOffset{GetRandomScalar(), GetRandomScalar()};
     * ```
     */
    [[nodiscard]] inline float GetRandomScalar()
    {
        static thread_local std::mt19937 gen{std::random_device{}()};
        static thread_local std::uniform_real_distribution<float> dist{-1.0f, 1.0f};
        return dist(gen);
    }

    /**
     * @brief Generate random floating-point number in range [0.0, 1.0]
     *
     * Thread-safe random number generator using Mersenne Twister.
     * Each thread maintains its own generator state for optimal performance.
     *
     * @return Random float in range [0.0f, 1.0f]
     *
     * @example
     * ```cpp
     * float probability = GetRandomFraction(); // 0.0 to 1.0
     * if (probability < 0.5f) { . . . }
     * auto randomColor = Colors::Red * GetRandomFraction(); // Dim red
     * ```
     */
    [[nodiscard]] inline float GetRandomFraction()
    {
        static thread_local std::mt19937 gen{std::random_device{}()};
        static thread_local std::uniform_real_distribution<float> dist{0.0f, 1.0f};
        return dist(gen);
    }
    //! @}
}
