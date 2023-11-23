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
		void Render();

		bool SaveBufferToImage() const;
		void VertexTransformationFunction(const std::vector<Mesh>& meshes_in, std::vector<Mesh>& meshes_out) const;
		Vector2 ConvertNDCtoScreen(const Vector3& ndc, int screenWidth, int screenHeight)const;
		static float Remap(float value, float inRangeMin, float inRangeMax)
		{
			// Remap color values to [0, 1] range
			float normalizedR = static_cast<float>(value) / inRangeMax;
		
			    
			return normalizedR;
		
		}
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

		Texture* m_Texture1;

		int m_Width{};
		int m_Height{};

		std::vector<float> m_pDepthBuffer;
	};
}
