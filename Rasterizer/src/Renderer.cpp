//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Maths.h"
#include "Texture.h"
#include "Utils.h"

using namespace dae;

Renderer::Renderer(SDL_Window* pWindow) :
	m_pWindow(pWindow)
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

	//Create Buffers
	m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
	m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
	m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;

	//m_pDepthBufferPixels = new float[m_Width * m_Height];

	//Initialize Camera
	m_Camera.Initialize(60.f, { .0f,.0f,-10.f });
	m_AspectRatio = m_AspectRatio = static_cast<float>(m_Width) / static_cast<float>(m_Height);
	m_pDepthBuffer.resize(m_Height * m_Width);
}

Renderer::~Renderer()
{

}

void Renderer::Update(Timer* pTimer)
{
	m_Camera.Update(pTimer);
}

void Renderer::Render_W1_Part1()
{
	//@START
	//Lock BackBuffer
	SDL_LockSurface(m_pBackBuffer);

	std::vector<Vector3> vertices_ndc
	{
		{0.f,0.5f,1.f},
		{0.5f,-0.5f,1.f},
		{-0.5f,-0.5f,1.f},
	};

	// Converting NDC coordinates to screen space
	std::vector<Vector2> vertices_screen;
	for (const auto& vertex_ndc : vertices_ndc)
	{
		Vector2 vertex_screen = ConvertNDCtoScreen(vertex_ndc, m_Width, m_Height);
		vertices_screen.push_back(vertex_screen);
	}
	m_pTriangles.push_back(vertices_screen);

	const float aspectRatio = m_Width / static_cast<float>(m_Height);

	ColorRGB finalColor;

	//RENDER LOGIC
	/*for (const auto& triangle : m_pTriangles)
	{*/
	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{
			Vector2 P = { static_cast<float>(px) + 0.5f, static_cast<float>(py) + 0.5f };

			if (IsPixelInsideTriangle(P, vertices_screen[0], vertices_screen[1], vertices_screen[2]))
			{
				finalColor = ColorRGB{ 1.0f, 1.0f, 1.0f };
			}
			else
			{
				/*float gradient = px / static_cast<float>(m_Width);
				gradient += py / static_cast<float>(m_Width);
				gradient /= 2.0f;

				finalColor = { gradient, gradient, gradient };*/

				finalColor = ColorRGB{ .0f, .0f, .0f };
			}

			//Update Color in Buffer
			finalColor.MaxToOne();

			m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
				static_cast<uint8_t>(finalColor.r * 255),
				static_cast<uint8_t>(finalColor.g * 255),
				static_cast<uint8_t>(finalColor.b * 255));
		}
	}
	/*}*/


	//@END
	//Update SDL Surface
	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}
void Renderer::Render_W1_Part2()
{
	SDL_LockSurface(m_pBackBuffer);

	std::vector<Vertex> vertices_world
	{
		{{0.f,2.f,1.f}},
		{{1.f,0.f,1.f}},
		{{-1.f,0.f,0.f}},
	};

	std::vector<Vertex> vertices_screen;


	//VertexTransformationFunction(vertices_world, vertices_screen);


	ColorRGB finalColor;

	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{
			Vector2 P = { static_cast<float>(px) + 0.5f, static_cast<float>(py) + 0.5f };

			if (IsPixelInsideTriangle(P, vertices_screen[0].position.GetXY(), vertices_screen[1].position.GetXY(), vertices_screen[2].position.GetXY()))
			{
				finalColor = ColorRGB{ 1.0f, 1.0f, 1.0f };
			}
			else
			{

				finalColor = ColorRGB{ .0f, .0f, .0f };
			}


			finalColor.MaxToOne();

			m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
				static_cast<uint8_t>(finalColor.r * 255),
				static_cast<uint8_t>(finalColor.g * 255),
				static_cast<uint8_t>(finalColor.b * 255));
		}
	}

	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}
void Renderer::Render_W1_Part3()
{
	SDL_LockSurface(m_pBackBuffer);

	std::vector<Vertex> vertices_world
	{
		{{0.f,4.f,2.f},{1,0,0}},
		{{3.f,-2.f,2.f},{0,1,0}},
		{{-3.f,-2.f,2.f},{0,0,1}}
	};

	std::vector<Vertex> vertices_screen;


	//VertexTransformationFunction(vertices_world, vertices_screen);


	ColorRGB finalColor;

	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{
			Vector2 P = { static_cast<float>(px) + 0.5f, static_cast<float>(py) + 0.5f };

			// Calculate vectors from vertices to the pixel
			const Vector2 edge = { vertices_screen[1].position.x - vertices_screen[0].position.x,  vertices_screen[1].position.y - vertices_screen[0].position.y };// V1 - V0
			const Vector2 edge1 = { vertices_screen[2].position.x - vertices_screen[1].position.x,  vertices_screen[2].position.y - vertices_screen[1].position.y };// V2 - V1
			const Vector2 edge2 = { vertices_screen[0].position.x - vertices_screen[2].position.x,  vertices_screen[0].position.y - vertices_screen[2].position.y };//V0 - V2

			const Vector2 pointToVertex = { P.x - vertices_screen[0].position.x, P.y - vertices_screen[0].position.y };// P - V0
			const Vector2 pointToVertex1 = { P.x - vertices_screen[1].position.x, P.y - vertices_screen[1].position.y };// P - V1
			const Vector2 pointToVertex2 = { P.x - vertices_screen[2].position.x, P.y - vertices_screen[2].position.y };// P - V2

			// Calculate 2D cross products (signed areas)
			float cross0 = Vector2::Cross(edge, pointToVertex);
			float cross1 = Vector2::Cross(edge1, pointToVertex1);
			float cross2 = Vector2::Cross(edge2, pointToVertex2);

			// Check the signs of the cross products
			if (cross0 > 0 && cross1 > 0 && cross2 > 0)
			{
				const float totalParallelogramArea = cross0 + cross1 + cross2;
				const float W0 = cross0 / totalParallelogramArea;
				const float W1 = cross1 / totalParallelogramArea;
				const float W2 = cross2 / totalParallelogramArea;

				finalColor = vertices_screen[0].color * W0 + vertices_screen[1].color * W1 + vertices_screen[2].color * W2;
			}
			else
			{
				finalColor = ColorRGB{ .0f, .0f, .0f };
			}

			finalColor.MaxToOne();

			m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
				static_cast<uint8_t>(finalColor.r * 255),
				static_cast<uint8_t>(finalColor.g * 255),
				static_cast<uint8_t>(finalColor.b * 255));
		}
	}

	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}
void Renderer::Render_W1_Part4()
{
	SDL_LockSurface(m_pBackBuffer);

	std::vector<Vertex> vertices_world
	{
		// Triangle 0
		{{0.f,2.f,0.f},{1,0,0}},
		{{1.5f,-1.f,0.f},{1,0,0}},
		{{-1.5f,-1.f,0.f},{1,0,0}},

		// Triangle 1
		{{0.f,4.f,2.f},{1,0,0}},
		{{3.f,-2.f,2.f},{0,1,0}},
		{{-3.f,-2.f,2.f},{0,0,1}},

	};

	std::vector<Vertex> vertices_screen;

	///VertexTransformationFunction(vertices_world, vertices_screen);

	// depth buffer is initialized with the maximum value of float
	std::fill(m_pDepthBuffer.begin(), m_pDepthBuffer.end(), std::numeric_limits<float>::max());

	const SDL_Rect clearRect = { 0, 0, m_Width, m_Height };
	//clearing the back buffer
	SDL_FillRect(m_pBackBuffer, &clearRect, Uint32(100));

	ColorRGB finalColor;

	for (int i = 0; i < vertices_screen.size(); i += 3)
	{
		const Triangle currentTriangle
		{
			vertices_screen[i],
			vertices_screen[i + 1],
			vertices_screen[i + 2]
		};

		//optimization

		int minX = static_cast<int>(std::min(currentTriangle.vertex0.position.x, std::min(currentTriangle.vertex1.position.x, currentTriangle.vertex2.position.x)));
		int maxX = static_cast<int>(std::max(currentTriangle.vertex0.position.x, std::max(currentTriangle.vertex1.position.x, currentTriangle.vertex2.position.x)));
		int minY = static_cast<int>(std::min(currentTriangle.vertex0.position.y, std::min(currentTriangle.vertex1.position.y, currentTriangle.vertex2.position.y)));
		int maxY = static_cast<int>(std::max(currentTriangle.vertex0.position.y, std::max(currentTriangle.vertex1.position.y, currentTriangle.vertex2.position.y)));

		// if statement of std::clamp from C++ 20
		minX = std::ranges::clamp(minX, 0, m_Width);
		maxX = std::ranges::clamp(maxX, 0, m_Width);
		minY = std::ranges::clamp(minY, 0, m_Height);
		maxY = std::ranges::clamp(maxY, 0, m_Height);

		for (int px = minX; px < maxX; ++px)
		{
			for (int py = minY; py < maxY; ++py)
			{
				Vector2 P = { static_cast<float>(px) + 0.5f, static_cast<float>(py) + 0.5f };

				// Calculate vectors from vertices to the pixel
				const Vector2 edge = currentTriangle.vertex1.position.GetXY() - currentTriangle.vertex0.position.GetXY();// V1 - V0
				const Vector2 edge1 = currentTriangle.vertex2.position.GetXY() - currentTriangle.vertex1.position.GetXY();// V2 - V1
				const Vector2 edge2 = currentTriangle.vertex0.position.GetXY() - currentTriangle.vertex2.position.GetXY();

				const Vector2 pointToVertex = P - currentTriangle.vertex0.position.GetXY();// P - V0
				const Vector2 pointToVertex1 = P - currentTriangle.vertex1.position.GetXY();// P - V1
				const Vector2 pointToVertex2 = P - currentTriangle.vertex2.position.GetXY();// P - V2

				// Calculate 2D cross products (signed areas)
				float cross0 = Vector2::Cross(pointToVertex, edge);	if (cross0 > 0) continue;

				float cross1 = Vector2::Cross(pointToVertex1, edge1);	if (cross1 > 0) continue;

				float cross2 = Vector2::Cross(pointToVertex2, edge2);	if (cross2 > 0) continue;


				// Check the signs of the cross products

				const float totalParallelogramArea = cross0 + cross1 + cross2;
				const float W0 = cross0 / totalParallelogramArea;
				const float W1 = cross1 / totalParallelogramArea;
				const float W2 = cross2 / totalParallelogramArea;

				//interpolate through the depth values
				const float pixelDepth =
					currentTriangle.vertex0.position.z * W0 +
					currentTriangle.vertex1.position.z * W1 +
					currentTriangle.vertex2.position.z * W2;

				const int pixelIndex{ px + py * m_Width };

				if (pixelDepth > m_pDepthBuffer[pixelIndex]) continue;

				m_pDepthBuffer[pixelIndex] = pixelDepth;

				finalColor = currentTriangle.vertex0.color * W0 + currentTriangle.vertex1.color * W1 + currentTriangle.vertex2.color * W2;

				finalColor.MaxToOne();

				m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
					static_cast<uint8_t>(finalColor.r * 255),
					static_cast<uint8_t>(finalColor.g * 255),
					static_cast<uint8_t>(finalColor.b * 255));
			}
		}
	}
	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

void Renderer::Render_W2_Part1()
{
	SDL_LockSurface(m_pBackBuffer);

	std::vector<Mesh> meshes_worldList
	{
		Mesh
		{
			{
				Vertex{{-3, 3, -2}},
				Vertex{{ 0,  3, -2}},
				Vertex{{ 3,  3, -2}},
				Vertex{{-3,  0, -2}},
				Vertex{{ 0,  0, -2}},
				Vertex{{ 3,  0, -2}},
				Vertex{{-3, -3, -2}},
				Vertex{{ 0, -3, -2}},
				Vertex{{ 3, -3, -2}}
			},
			{
				3, 0, 1,    1, 4, 3,    4, 1, 2,
				2, 5, 4,    6, 3, 4,    4, 7, 6,
				7, 4, 5,    5, 8, 7
			},
			PrimitiveTopology::TriangleList
		}
	};

	std::vector<Mesh> meshes_worldStrip
	{
		Mesh
		{
			{
				Vertex{{-3, 3, -2}},
				Vertex{{ 0,  3, -2}},
				Vertex{{ 3,  3, -2}},
				Vertex{{-3,  0, -2}},
				Vertex{{ 0,  0, -2}},
				Vertex{{ 3,  0, -2}},
				Vertex{{-3, -3, -2}},
				Vertex{{ 0, -3, -2}},
				Vertex{{ 3, -3, -2}}
			},
			{
				3, 0, 4, 1, 5, 2,
				2, 6,
				6, 3, 7, 4, 8, 5
			},
			PrimitiveTopology::TriangleStrip
		}
	};
	std::vector<Mesh> meshes_screen;
	m_Meshes.push_back(meshes_worldList[0]);
	m_Meshes.push_back(meshes_worldStrip[0]);

	VertexTransformationFunction(m_Meshes, meshes_screen);


	// depth buffer is initialized with the maximum value of float
	std::fill(m_pDepthBuffer.begin(), m_pDepthBuffer.end(), std::numeric_limits<float>::max());

	const SDL_Rect clearRect = { 0, 0, m_Width, m_Height };
	//clearing the back buffer
	SDL_FillRect(m_pBackBuffer, &clearRect, Uint32(100));

	ColorRGB finalColor;
	Triangle currentTriangle;
	for (int i = 0; i < m_Meshes[1].indices.size() - 2; i++)
	{
		if (m_Meshes[1].primitiveTopology == PrimitiveTopology::TriangleList)
		{
			currentTriangle =
			{
				meshes_screen[1].vertices[m_Meshes[1].indices[i]],
				meshes_screen[1].vertices[m_Meshes[1].indices[i + 1]],
				meshes_screen[1].vertices[m_Meshes[1].indices[i + 2]]
			};
		}
		else if(i % 2 == 0)
		{
			currentTriangle =
			{
				meshes_screen[1].vertices[m_Meshes[1].indices[i]],
				meshes_screen[1].vertices[m_Meshes[1].indices[i + 1]],
				meshes_screen[1].vertices[m_Meshes[1].indices[i + 2]]
			};
		}
		else
		{
			currentTriangle =
			{
				meshes_screen[1].vertices[m_Meshes[1].indices[i]],
				meshes_screen[1].vertices[m_Meshes[1].indices[i + 2]],
				meshes_screen[1].vertices[m_Meshes[1].indices[i + 1]]
			};
		}

		//optimization

		int minX = static_cast<int>(std::min(currentTriangle.vertex0.position.x, std::min(currentTriangle.vertex1.position.x, currentTriangle.vertex2.position.x)) - 1);
		int maxX = static_cast<int>(std::max(currentTriangle.vertex0.position.x, std::max(currentTriangle.vertex1.position.x, currentTriangle.vertex2.position.x)) + 1);
		int minY = static_cast<int>(std::min(currentTriangle.vertex0.position.y, std::min(currentTriangle.vertex1.position.y, currentTriangle.vertex2.position.y)) - 1);
		int maxY = static_cast<int>(std::max(currentTriangle.vertex0.position.y, std::max(currentTriangle.vertex1.position.y, currentTriangle.vertex2.position.y)) + 1);

		// if statement of std::clamp from C++ 20
		minX = std::ranges::clamp(minX, 0, m_Width);
		maxX = std::ranges::clamp(maxX, 0, m_Width);
		minY = std::ranges::clamp(minY, 0, m_Height);
		maxY = std::ranges::clamp(maxY, 0, m_Height);

		for (int px = minX; px < maxX; ++px)
		{
			for (int py = minY; py < maxY; ++py)
			{
				Vector2 P = { static_cast<float>(px) + 0.5f, static_cast<float>(py) + 0.5f };

				// Calculate vectors from vertices to the pixel
				const Vector2 edge = currentTriangle.vertex1.position.GetXY() - currentTriangle.vertex0.position.GetXY();// V1 - V0
				const Vector2 edge1 = currentTriangle.vertex2.position.GetXY() - currentTriangle.vertex1.position.GetXY();// V2 - V1
				const Vector2 edge2 = currentTriangle.vertex0.position.GetXY() - currentTriangle.vertex2.position.GetXY();

				const Vector2 pointToVertex = P - currentTriangle.vertex0.position.GetXY();// P - V0
				const Vector2 pointToVertex1 = P - currentTriangle.vertex1.position.GetXY();// P - V1
				const Vector2 pointToVertex2 = P - currentTriangle.vertex2.position.GetXY();// P - V2

				// Calculate 2D cross products (signed areas)
				float cross0 = Vector2::Cross(pointToVertex, edge);	if (cross0 > 0) continue;

				float cross1 = Vector2::Cross(pointToVertex1, edge1);	if (cross1 > 0) continue;

				float cross2 = Vector2::Cross(pointToVertex2, edge2);	if (cross2 > 0) continue;


				// Check the signs of the cross products

				const float totalParallelogramArea = cross0 + cross1 + cross2;
				const float W0 = cross0 / totalParallelogramArea;
				const float W1 = cross1 / totalParallelogramArea;
				const float W2 = cross2 / totalParallelogramArea;

				//interpolate through the depth values
				const float pixelDepth =
					currentTriangle.vertex0.position.z * W0 +
					currentTriangle.vertex1.position.z * W1 +
					currentTriangle.vertex2.position.z * W2;

				const int pixelIndex{ px + py * m_Width };

				if (pixelDepth > m_pDepthBuffer[pixelIndex]) continue;

				m_pDepthBuffer[pixelIndex] = pixelDepth;

				finalColor = currentTriangle.vertex0.color * W0 + currentTriangle.vertex1.color * W1 + currentTriangle.vertex2.color * W2;

				finalColor.MaxToOne();

				m_pBackBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBackBuffer->format,
					static_cast<uint8_t>(finalColor.r * 255),
					static_cast<uint8_t>(finalColor.g * 255),
					static_cast<uint8_t>(finalColor.b * 255));
			}
		}
	}
	SDL_UnlockSurface(m_pBackBuffer);
	SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
	SDL_UpdateWindowSurface(m_pWindow);
}

void Renderer::VertexTransformationFunction(const std::vector<Mesh>& meshes_in, std::vector<Mesh>& meshes_out) const
{
	for (int i = 0; i < meshes_in.size(); i++)
	{
		std::vector<Vertex> vertices_out;

		for (const auto& vertex : meshes_in[i].vertices)
		{
			Vertex newVertex
			{
				vertex.position,
				vertex.color
			};

			newVertex.position = m_Camera.invViewMatrix.TransformPoint(newVertex.position);

			//positive Z-axis is pointing into the screen
			newVertex.position.x = newVertex.position.x / newVertex.position.z;
			newVertex.position.y = newVertex.position.y / newVertex.position.z;

			newVertex.position.x = newVertex.position.x / (m_AspectRatio * m_Camera.fov);
			newVertex.position.y = newVertex.position.y / m_Camera.fov;

			// Convert from NDC to screen
			newVertex.position.x = ConvertNDCtoScreen(newVertex.position, m_Width, m_Height).x;
			newVertex.position.y = ConvertNDCtoScreen(newVertex.position, m_Width, m_Height).y;

			vertices_out.push_back(newVertex);
		}
		const Mesh newMesh
		{
			vertices_out,
			meshes_in[i].indices,
			meshes_in[i].primitiveTopology
		};
		meshes_out.push_back(newMesh);
	}
}
bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBackBuffer, "Rasterizer_ColorBuffer.bmp");
}
Vector2 Renderer::ConvertNDCtoScreen(const Vector3& ndc, int screenWidth, int screenHeight)const
{
	float screenSpaceX = (ndc.x + 1.0f) / 2.0f * screenWidth;
	float screenSpaceY = (1.0f - ndc.y) / 2.0f * screenHeight;
	return Vector2{ screenSpaceX, screenSpaceY };
}
bool Renderer::IsPixelInsideTriangle(const Vector2& pixel, const Vector2& v0, const Vector2& v1, const Vector2& v2)
{
	// Calculate vectors from vertices to the pixel
	Vector2 edge = { v1.x - v0.x, v1.y - v0.y };
	Vector2 edge1 = { v2.x - v1.x, v2.y - v1.y };
	Vector2 edge2 = { v0.x - v2.x, v0.y - v2.y };

	Vector2 pointToVertex = { pixel.x - v0.x, pixel.y - v0.y };
	Vector2 pointToVertex1 = { pixel.x - v1.x, pixel.y - v1.y };
	Vector2 pointToVertex2 = { pixel.x - v2.x, pixel.y - v2.y };

	// Calculate 2D cross products (signed areas)
	float cross0 = Vector2::Cross(edge, pointToVertex);
	float cross1 = Vector2::Cross(edge1, pointToVertex1);
	float cross2 = Vector2::Cross(edge2, pointToVertex2);

	// Check the signs of the cross products
	bool isInside = (cross0 > 0 && cross1 > 0 && cross2 > 0);

	return isInside;
}

