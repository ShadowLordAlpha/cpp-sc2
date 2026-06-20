#include "sc2renderer/sc2_renderer.h"

#include <SDL3/SDL.h>
#include <array>
#include <cassert>
#include <iostream>
#include <memory>
#include <stdexcept>

namespace {
    /// Simple renderer state
    struct RendererState {
        std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window{nullptr, SDL_DestroyWindow};
        std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)> renderer{nullptr, SDL_DestroyRenderer};
        bool initialized{false};
    };

    RendererState g_state;

    /// Player colors using existing sc2::Color convention
    constexpr std::array<sc2::Color, 8> kPlayerColors{{
        sc2::Colors::Black,  // 0: Black (empty)
        sc2::Colors::Green,  // 1: Green (self/ally)
        sc2::Colors::Red,    // 2: Red (enemy)
        sc2::Colors::Blue,   // 3: Blue (neutral)
        sc2::Colors::Yellow, // 4: Yellow
        sc2::Colors::Cyan,   // 5: Cyan
        sc2::Colors::White,  // 6: White
        sc2::Colors::White   // 7: White (fallback)
    }};

    [[nodiscard]] constexpr SDL_FRect createRect(float x, float y, float w, float h) noexcept {
        return SDL_FRect{x, y, w, h};
    }

    void setColor(SDL_Renderer* renderer, const sc2::Color& color) noexcept {
        SDL_SetRenderDrawColor(renderer, color.r(), color.g(), color.b(), 255);
    }

    void renderMatrixGeneric(const std::byte* data, const sc2::renderer::MatrixParams& params,
                             auto colorFunction) noexcept {
        assert(g_state.initialized && "Renderer must be initialized");
        assert(data && "Data cannot be null");

        if (!g_state.renderer) return;

        auto rect = createRect(0.0f, 0.0f,
                               static_cast<float>(params.pixelWidth),
                               static_cast<float>(params.pixelHeight));

        for (std::int32_t y = 0; y < params.matrixHeight; ++y) {
            for (std::int32_t x = 0; x < params.matrixWidth; ++x) {
                rect.x = static_cast<float>(params.offsetX + (x * params.pixelWidth));
                rect.y = static_cast<float>(params.offsetY + (y * params.pixelHeight));

                const auto index = static_cast<std::size_t>(x + y * params.matrixWidth);
                colorFunction(g_state.renderer.get(), data, index);
                SDL_RenderFillRect(g_state.renderer.get(), &rect);
            }
        }
    }
}

namespace sc2::renderer {

    void initialize(std::string_view title,
                    std::int32_t x, std::int32_t y,
                    std::int32_t width, std::int32_t height,
                    bool resizable) {

        if (g_state.initialized) {
            throw std::runtime_error("Renderer already initialized");
        }

        if (!SDL_Init(SDL_INIT_VIDEO)) {
            throw std::runtime_error(std::string{"SDL_Init failed: "} + SDL_GetError());
        }

        const auto flags = resizable ? SDL_WINDOW_RESIZABLE : 0;
        auto* rawWindow = SDL_CreateWindow(title.data(), width, height, flags);
        if (!rawWindow) {
            SDL_Quit();
            throw std::runtime_error(std::string{"SDL_CreateWindow failed: "} + SDL_GetError());
        }

        if (x != SDL_WINDOWPOS_CENTERED && x != SDL_WINDOWPOS_UNDEFINED &&
            y != SDL_WINDOWPOS_CENTERED && y != SDL_WINDOWPOS_UNDEFINED) {
            SDL_SetWindowPosition(rawWindow, x, y);
        }

        g_state.window.reset(rawWindow);

        auto* rawRenderer = SDL_CreateRenderer(g_state.window.get(), nullptr);
        if (!rawRenderer) {
            g_state.window.reset();
            SDL_Quit();
            throw std::runtime_error(std::string{"SDL_CreateRenderer failed: "} + SDL_GetError());
        }

        g_state.renderer.reset(rawRenderer);

        // Clear to black
        setColor(g_state.renderer.get(), Colors::Black);
        SDL_RenderClear(g_state.renderer.get());

        g_state.initialized = true;
    }

    void shutdown() noexcept {
        if (!g_state.initialized) return;

        g_state.renderer.reset();
        g_state.window.reset();
        SDL_Quit();
        g_state.initialized = false;
    }

    void renderMatrix1BPP(const std::byte* data, const MatrixParams& params) noexcept {
        renderMatrixGeneric(data, params, [](SDL_Renderer* renderer, const std::byte* data, std::size_t index) {
            const auto byteIndex = index / 8;
            const auto bitIndex = 7 - (index % 8);
            const auto byteValue = static_cast<std::uint8_t>(data[byteIndex]);
            const bool isSet = (byteValue & (1 << bitIndex)) != 0;

            const auto color = isSet ? Colors::White : Colors::Black;
            setColor(renderer, color);
        });
    }

    void renderHeightMap(const std::byte* data, const MatrixParams& params) noexcept {
        renderMatrixGeneric(data, params, [](SDL_Renderer* renderer, const std::byte* data, std::size_t index) {
            const auto heightValue = static_cast<std::uint8_t>(data[index]);
            SDL_SetRenderDrawColor(renderer, heightValue, heightValue, heightValue, 255);
        });
    }

    void renderPlayerMap(const std::byte* data, const MatrixParams& params) noexcept {
        renderMatrixGeneric(data, params, [](SDL_Renderer* renderer, const std::byte* data, std::size_t index) {
            const auto playerId = static_cast<std::uint8_t>(data[index]);
            const auto colorIndex = std::min(playerId, static_cast<std::uint8_t>(kPlayerColors.size() - 1));
            setColor(renderer, kPlayerColors[colorIndex]);
        });
    }

    void renderImageRGB(const std::byte* data, const ImageParams& params) noexcept {
        assert(g_state.initialized && "Renderer must be initialized");
        assert(data && "Data cannot be null");

        if (!g_state.renderer) return;

        auto* surface = SDL_CreateSurfaceFrom(params.width, params.height,
                                              SDL_PIXELFORMAT_RGB24,
                                              const_cast<std::byte*>(data),
                                              params.width * 3);
        if (!surface) {
            std::cerr << "SDL_CreateSurfaceFrom failed: " << SDL_GetError() << std::endl;
            return;
        }

        auto* texture = SDL_CreateTextureFromSurface(g_state.renderer.get(), surface);
        SDL_DestroySurface(surface);

        if (!texture) {
            std::cerr << "SDL_CreateTextureFromSurface failed: " << SDL_GetError() << std::endl;
            return;
        }

        const auto dstRect = createRect(static_cast<float>(params.offsetX),
                                        static_cast<float>(params.offsetY),
                                        static_cast<float>(params.width),
                                        static_cast<float>(params.height));

        SDL_RenderTexture(g_state.renderer.get(), texture, nullptr, &dstRect);
        SDL_DestroyTexture(texture);
    }

    [[nodiscard]] bool processFrame() noexcept {
        assert(g_state.initialized && "Renderer must be initialized");

        if (!g_state.renderer) return false;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    return false;
                case SDL_EVENT_KEY_DOWN:
                    if (event.key.key == SDLK_ESCAPE) {
                        return false;
                    }
                    break;
                default:
                    break;
            }
        }

        SDL_RenderPresent(g_state.renderer.get());
        SDL_RenderClear(g_state.renderer.get());
        return true;
    }

    [[nodiscard]] bool isInitialized() noexcept {
        return g_state.initialized;
    }

// Legacy interface implementations
    void Initialize(const char* title, int x, int y, int w, int h, unsigned int flags) {
        initialize(std::string_view{title}, x, y, w, h, (flags & SDL_WINDOW_RESIZABLE) != 0);
    }

    void Shutdown() {
        shutdown();
    }

    void Matrix1BPP(const char* bytes, int w_mat, int h_mat, int off_x, int off_y, int px_w, int px_h) {
        const MatrixParams params{w_mat, h_mat, off_x, off_y, px_w, px_h};
        renderMatrix1BPP(reinterpret_cast<const std::byte*>(bytes), params);
    }

    void Matrix8BPPHeightMap(const char* bytes, int w_mat, int h_mat, int off_x, int off_y, int px_w, int px_h) {
        const MatrixParams params{w_mat, h_mat, off_x, off_y, px_w, px_h};
        renderHeightMap(reinterpret_cast<const std::byte*>(bytes), params);
    }

    void Matrix8BPPPlayers(const char* bytes, int w_mat, int h_mat, int off_x, int off_y, int px_w, int px_h) {
        const MatrixParams params{w_mat, h_mat, off_x, off_y, px_w, px_h};
        renderPlayerMap(reinterpret_cast<const std::byte*>(bytes), params);
    }

    void ImageRGB(const char* bytes, int width, int height, int off_x, int off_y) {
        const ImageParams params{width, height, off_x, off_y};
        renderImageRGB(reinterpret_cast<const std::byte*>(bytes), params);
    }

    void Render() {
        processFrame();
    }

} // namespace sc2::renderer
