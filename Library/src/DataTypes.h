#pragma once
#include "Maths.h"
#include "vector"

namespace dae
{
	struct Vertex
	{
		Vector3 position{};
		ColorRGB color{colors::White};
		Vector2 uv{};
		Vector3 normal{}; //W4
		Vector3 tangent{}; //W4
		Vector3 viewDirection{}; //W4
	};
	struct Vertex_Out
	{
		Vector4 position{};
		ColorRGB color{ colors::White };
		Vector2 uv{};
		Vector3 normal{};
		Vector3 tangent{};
		Vector3 viewDirection{};
	};
	struct Triangle
	{
		Vertex vertex0;
		Vertex vertex1;
		Vertex vertex2;
	};
	struct Triangle4
	{
		Vertex_Out vertex0;
		Vertex_Out vertex1;
		Vertex_Out vertex2;
	};

	enum class PrimitiveTopology
	{
		TriangleList,
		TriangleStrip
	};

	struct Mesh
	{
		std::vector<Vertex_Out> vertices_out{};
		std::vector<uint32_t> indices{};
		PrimitiveTopology primitiveTopology{ PrimitiveTopology::TriangleStrip };

		std::vector<Vertex> vertices{};
		Matrix worldMatrix{};
	};
	struct Mesh4
	{
		std::vector<Vertex_Out> vertices_out{};
		std::vector<uint32_t> indices{};
		PrimitiveTopology primitiveTopology{ PrimitiveTopology::TriangleStrip };

		Matrix worldMatrix{};
	};
}
