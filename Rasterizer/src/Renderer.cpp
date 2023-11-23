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

	m_AspectRatio = m_AspectRatio = static_cast<float>(m_Width) / static_cast<float>(m_Height);
	m_Camera.Initialize(m_AspectRatio, 60.f, { .0f,.0f,-10.f });
	m_pDepthBuffer.resize(m_Height * m_Width);

	m_Texture1 = Texture::LoadFromFile("Resources/uv_grid_2.png");
}

Renderer::~Renderer()
{
	delete m_Texture1;
}

void Renderer::Update(Timer* pTimer)
{
	m_Camera.Update(pTimer);
}

void Renderer::Render()
{
	SDL_LockSurface(m_pBackBuffer);

	std::vector<Mesh> meshes_worldList
	{
		Mesh
		{
			{
				Vertex_Out{{-3, 3, -2,333},{},{0,0}},
				Vertex_Out{{ 0,  3, -2,434},{},{0.5,0}},
				Vertex_Out{{ 3,  3, -2,43},{},{1,0}},
				Vertex_Out{{-3,  0, -2,0},{},{0,0.5}},
				Vertex_Out{{ 0,  0, -2,0},{},{0.5,0.5}},
				Vertex_Out{{ 3,  0, -2,4},{},{1,0.5}},
				Vertex_Out{{-3, -3, -2,0},{},{0,1}},
				Vertex_Out{{ 0, -3, -2,0},{},{0.5,1}},
				Vertex_Out{{ 3, -3, -2,0},{},{1,1}}
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
				Vertex_Out{{-3, 3, -2,0},ColorRGB{colors::White},{0,0}},
				Vertex_Out{{ 0,  3, -2,0},ColorRGB{colors::White},{0.5,0}},
				Vertex_Out{{ 3,  3, -2,0},ColorRGB{colors::White},{1,0}},
				Vertex_Out{{-3,  0, -2,0},ColorRGB{colors::White},{0,0.5}},
				Vertex_Out{{ 0,  0, -2,0},ColorRGB{colors::White},{0.5,0.5}},
				Vertex_Out{{ 3,  0, -2,0},ColorRGB{colors::White},{1,0.5}},
				Vertex_Out{{-3, -3, -2,0},ColorRGB{colors::White},{0,1}},
				Vertex_Out{{ 0, -3, -2,0},ColorRGB{colors::White},{0.5,1}},
				Vertex_Out{{ 3, -3, -2,0},ColorRGB{colors::White},{1,1}}
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

	SDL_FillRect(m_pBackBuffer, NULL, SDL_MapRGB(m_pBackBuffer->format,
		static_cast<uint8_t>(100.f * 255),
		static_cast<uint8_t>(100.f * 255),
		static_cast<uint8_t>(100.f * 255))
	);

	// depth buffer is initialized with the maximum value of float
	std::fill(m_pDepthBuffer.begin(), m_pDepthBuffer.end(), std::numeric_limits<float>::max());

	const SDL_Rect clearRect = { 0, 0, m_Width, m_Height };
	//clearing the back buffer
	ColorRGB finalColor;
	Triangle currentTriangle;

	for (int i = 0; i < m_Meshes[0].indices.size() - 2; i++)
	{
		if (m_Meshes[0].primitiveTopology == PrimitiveTopology::TriangleList)
		{
			currentTriangle =
			{
				meshes_screen[0].vertices_out[m_Meshes[0].indices[i]],
				meshes_screen[0].vertices_out[m_Meshes[0].indices[i + 1]],
				meshes_screen[0].vertices_out[m_Meshes[0].indices[i + 2]]
			};
		}
		else if (i % 2 == 0)
		{
			currentTriangle =
			{
				meshes_screen[0].vertices_out[m_Meshes[0].indices[i]],
				meshes_screen[0].vertices_out[m_Meshes[0].indices[i + 1]],
				meshes_screen[0].vertices_out[m_Meshes[0].indices[i + 2]]
			};
		}
		else
		{
			currentTriangle =
			{
				meshes_screen[0].vertices_out[m_Meshes[0].indices[i]],
				meshes_screen[0].vertices_out[m_Meshes[0].indices[i + 2]],
				meshes_screen[0].vertices_out[m_Meshes[0].indices[i + 1]]
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

				float cross1 = Vector2::Cross(pointToVertex2, edge2);	if (cross1 >= 0) continue;

				float cross0 = Vector2::Cross(pointToVertex1, edge1);	if (cross0 >= 0) continue;

				float cross2 = Vector2::Cross(pointToVertex, edge);	if (cross2 >= 0) continue;


				// Check the signs of the cross products

				const float totalParallelogramArea = cross0 + cross1 + cross2;

				const float W0 = cross0 / totalParallelogramArea;
				const float W1 = cross1 / totalParallelogramArea;
				const float W2 = cross2 / totalParallelogramArea;


				//interpolate through the depth values
				float pixelDepth = 1 /
					(W0 / currentTriangle.vertex0.position.z +
						W1 / currentTriangle.vertex1.position.z +
						W2 / currentTriangle.vertex2.position.z);
				
				const int pixelIndex = { px + py * m_Width };

				if (pixelDepth > m_pDepthBuffer[pixelIndex]) continue;

				m_pDepthBuffer[pixelIndex] = pixelDepth;
				float test = Remap(pixelDepth, 0.98f, 1.f);
				//interpolate through the depth values
				const Vector2 uvInterp = (currentTriangle.vertex0.uv * W0 / currentTriangle.vertex0.position.z +
					currentTriangle.vertex1.uv * W1 / currentTriangle.vertex1.position.z +
					currentTriangle.vertex2.uv * W2 / currentTriangle.vertex2.position.z) * test;

	
				finalColor = m_Texture1->Sample(uvInterp);

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
		std::vector<Vertex_Out> vertices_out;

		for (const auto& vertex : meshes_in[i].vertices_out)
		{
			Vertex_Out newVertex
			{
				vertex.position,
				vertex.color
			};

			newVertex.position = m_Camera.worldViewProectionMatrix.TransformPoint(newVertex.position);

			//positive Z-axis is pointing into the screen
			newVertex.position.x = newVertex.position.x / newVertex.position.w;
			newVertex.position.y = newVertex.position.y / newVertex.position.w;

			// Convert from NDC to screen
			newVertex.position.x = ConvertNDCtoScreen(newVertex.position, m_Width, m_Height).x;
			newVertex.position.y = ConvertNDCtoScreen(newVertex.position, m_Width, m_Height).y;

			newVertex.uv = vertex.uv;

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

