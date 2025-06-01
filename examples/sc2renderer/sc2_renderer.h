#pragma once

#include "sc2api/sc2_common.h"
#include <cstdint>
#include <string_view>

/**
 * @brief Simple StarCraft II Renderer using SDL3
 *
 * Provides basic rendering functionality for SC2 related graphics.
 * Focuses on simplicity and maintainability.
 */
namespace sc2::renderer {

    /**
     * @brief Rendering parameters for matrix-based data
     */
    struct MatrixParams {
        std::int32_t matrixWidth;
        std::int32_t matrixHeight;
        std::int32_t offsetX;
        std::int32_t offsetY;
        std::int32_t pixelWidth;
        std::int32_t pixelHeight;
    };

    /**
     * @brief Rendering parameters for image data
     */
    struct ImageParams {
        std::int32_t width;
        std::int32_t height;
        std::int32_t offsetX;
        std::int32_t offsetY;
    };

    /**
     * @brief Initialize the renderer
     */
    void initialize(std::string_view title,
                    std::int32_t x, std::int32_t y,
                    std::int32_t width, std::int32_t height,
                    bool resizable = true);

    /**
     * @brief Shutdown the renderer and cleanup resources
     */
    void shutdown() noexcept;

    /**
     * @brief Render a 1-bit per pixel binary matrix
     */
    void renderMatrix1BPP(const std::byte* data, const MatrixParams& params) noexcept;

    /**
     * @brief Render an 8-bit height map in grayscale
     */
    void renderHeightMap(const std::byte* data, const MatrixParams& params) noexcept;

    /**
     * @brief Render an 8-bit player overlay with predefined colors
     */
    void renderPlayerMap(const std::byte* data, const MatrixParams& params) noexcept;

    /**
     * @brief Render an RGB image
     */
    void renderImageRGB(const std::byte* data, const ImageParams& params) noexcept;

    /**
     * @brief Process events and present the frame
     * @return true if should continue rendering, false to quit
     */
    [[nodiscard]] bool processFrame() noexcept;

    /**
     * @brief Check if renderer is initialized
     */
    [[nodiscard]] bool isInitialized() noexcept;

    // Legacy interface for backward compatibility
    [[deprecated("Use initialize instead")]]
    void Initialize(const char* title, int x, int y, int w, int h, unsigned int flags = 0);

    [[deprecated("Use shutdown instead")]]
    void Shutdown();

    [[deprecated("Use renderMatrix1BPP instead")]]
    void Matrix1BPP(const char* bytes, int w_mat, int h_mat, int off_x, int off_y, int px_w, int px_h);

    [[deprecated("Use renderHeightMap instead")]]
    void Matrix8BPPHeightMap(const char* bytes, int w_mat, int h_mat, int off_x, int off_y, int px_w, int px_h);

    [[deprecated("Use renderPlayerMap instead")]]
    void Matrix8BPPPlayers(const char* bytes, int w_mat, int h_mat, int off_x, int off_y, int px_w, int px_h);

    [[deprecated("Use renderImageRGB instead")]]
    void ImageRGB(const char* bytes, int width, int height, int off_x, int off_y);

    [[deprecated("Use processFrame instead")]]
    void Render();

} // namespace sc2::renderer
