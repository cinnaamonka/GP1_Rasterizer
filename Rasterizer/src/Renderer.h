#pragma once

#include <cstdint>
#include <vector>

#include "Camera.h"

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Texture;
	struct Mesh;
	struct Vertex;
	class Timer;
	class Scene;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(Timer* pTimer);
		void Render_W1_Part1();
		void Render_W1_Part2();
		void Render_W1_Part3();
		void Render_W1_Part4();
		void Render_W2_Part1();

		bool SaveBufferToImage() const;
		void VertexTransformationFunction(const std::vector<Mesh>& meshes_in, std::vector<Mesh>& meshes_out) const;
		Vector2 ConvertNDCtoScreen(const Vector3& ndc, int screenWidth, int screenHeight)const;
		bool IsPixelInsideTriangle(const Vector2& pixel, const Vector2& v0, const Vector2& v1, const Vector2& v2);
		
	private:
		SDL_Window* m_pWindow{};

		SDL_Surface* m_pFrontBuffer{ nullptr };
		SDL_Surface* m_pBackBuffer{ nullptr };
		uint32_t* m_pBackBufferPixels{};

		//float* m_pDepthBufferPixels{};
		std::vector<std::vector<Vector2>> m_pTriangles;
		Camera m_Camera{};
		float m_AspectRatio;

		std::vector<Mesh> m_Meshes;

		int m_Width{};
		int m_Height{};

		std::vector<float> m_pDepthBuffer;
	};
}
