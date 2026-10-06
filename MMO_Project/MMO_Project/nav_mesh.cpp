#pragma once
#include "pch.h"
#include "nav_mesh.h"

#include <fstream>
#include <random>

#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"
#include "DetourCommon.h"

namespace
{
	constexpr int navmesh_set_magic = 'M' << 24 | 'S' << 16 | 'E' << 8 | 'T';	// 'MSET'
	constexpr int navmesh_set_version = 1;

	struct navmesh_set_header
	{
		int magic;
		int version;
		int num_tiles;
		dtNavMeshParams params;
	};

	struct navmesh_tile_header
	{
		dtTileRef tile_ref;
		int data_size;
	};

	// 언리얼 좌표 변환
	constexpr float ue_to_recast_scale = 0.01f;
	constexpr float recast_to_ue_scale = 100.f;

	void to_recast(const vec3& ue, float* out)
	{
		out[0] = -ue.x * ue_to_recast_scale;
		out[1] = ue.z * ue_to_recast_scale;
		out[2] = -ue.y * ue_to_recast_scale;
	}

	vec3 to_ue(const float* recast)
	{
		return vec3{ -recast[0] * recast_to_ue_scale, -recast[2] * recast_to_ue_scale, recast[1] * recast_to_ue_scale };
	}

	constexpr float search_extents[3] = { 2.f, 5.f, 2.f };

	const dtQueryFilter& default_filter()
	{
		static const dtQueryFilter filter;
		return filter;
	}

	float thread_random()
	{
		thread_local std::mt19937 engine{ std::random_device{}() };
		thread_local std::uniform_real_distribution<float> dist(0.f, 1.f);
		return dist(engine);
	}

	uint64_t next_instance_id()
	{
		static std::atomic<uint64_t> id{ 1 };
		return id.fetch_add(1, std::memory_order_relaxed);
	}
}

nav_mesh::nav_mesh()
	: instance_id_(next_instance_id())
{
}

nav_mesh::~nav_mesh()
{
	if (nav_ != nullptr)
	{
		dtFreeNavMesh(nav_);
		nav_ = nullptr;
	}
}

auto nav_mesh::load(const std::filesystem::path& file_path) -> fw::error
{
	ASSERT_RETURN_VALUE(nav_ == nullptr, error::code::map_load_fail);

	std::ifstream file(file_path, std::ios::binary);
	if (!file.is_open())
	{
		FLOG_ERROR("nav_mesh :: file not found ({})", file_path.string());
		return error::code::map_load_fail;
	}

	navmesh_set_header header{};
	file.read(reinterpret_cast<char*>(&header), sizeof(header));
	if (!file || header.magic != navmesh_set_magic || header.version != navmesh_set_version)
	{
		FLOG_ERROR("nav_mesh :: invalid file header ({})", file_path.string());
		return error::code::map_load_fail;
	}

	dtNavMesh* nav = dtAllocNavMesh();
	ASSERT_RETURN_VALUE(nav != nullptr, error::code::map_load_fail);

	if (dtStatusFailed(nav->init(&header.params)))
	{
		dtFreeNavMesh(nav);
		FLOG_ERROR("nav_mesh :: dtNavMesh init failed ({})", file_path.string());
		return error::code::map_load_fail;
	}

	int poly_count = 0;
	for (int i = 0; i < header.num_tiles; ++i)
	{
		navmesh_tile_header tile{};
		file.read(reinterpret_cast<char*>(&tile), sizeof(tile));
		if (!file || tile.tile_ref == 0 || tile.data_size <= 0)
		{
			dtFreeNavMesh(nav);
			FLOG_ERROR("nav_mesh :: invalid tile header index({}) ({})", i, file_path.string());
			return error::code::map_load_fail;
		}

		unsigned char* data = static_cast<unsigned char*>(dtAlloc(tile.data_size, DT_ALLOC_PERM));
		file.read(reinterpret_cast<char*>(data), tile.data_size);
		if (!file || dtStatusFailed(nav->addTile(data, tile.data_size, DT_TILE_FREE_DATA, tile.tile_ref, nullptr)))
		{
			dtFree(data);
			dtFreeNavMesh(nav);
			FLOG_ERROR("nav_mesh :: addTile failed index({}) ({})", i, file_path.string());
			return error::code::map_load_fail;
		}

		if (const dtMeshTile* loaded = nav->getTileByRef(tile.tile_ref); loaded && loaded->header)
		{
			poly_count += loaded->header->polyCount;
		}
	}

	nav_ = nav;
	tile_count_ = header.num_tiles;
	poly_count_ = poly_count;
	return fw::error{};
}

auto nav_mesh::get_query() const -> dtNavMeshQuery*
{
	struct query_deleter
	{
		void operator()(dtNavMeshQuery* query) const { dtFreeNavMeshQuery(query); }
	};

	thread_local std::unordered_map<uint64_t, std::unique_ptr<dtNavMeshQuery, query_deleter>> queries;

	auto& query = queries[instance_id_];
	if (query == nullptr)
	{
		query.reset(dtAllocNavMeshQuery());
		if (query == nullptr || dtStatusFailed(query->init(nav_, max_search_nodes)))
		{
			query.reset();
			FLOG_ERROR("nav_mesh :: dtNavMeshQuery init failed");
			return nullptr;
		}
	}

	return query.get();
}

auto nav_mesh::find_nearest_poly(dtNavMeshQuery* query, const vec3& pos, uint64_t& out_ref, float* out_nearest) const -> bool
{
	float center[3];
	to_recast(pos, center);

	dtPolyRef ref = 0;
	if (dtStatusFailed(query->findNearestPoly(center, search_extents, &default_filter(), &ref, out_nearest)) || ref == 0)
	{
		return false;
	}

	out_ref = ref;
	return true;
}

auto nav_mesh::find_nearest_point(const vec3& pos, vec3& out_pos) const -> bool
{
	dtNavMeshQuery* query = is_loaded() ? get_query() : nullptr;
	if (query == nullptr)
	{
		return false;
	}

	uint64_t ref = 0;
	float nearest[3];
	if (!find_nearest_poly(query, pos, ref, nearest))
	{
		return false;
	}

	out_pos = to_ue(nearest);
	return true;
}

auto nav_mesh::find_path(const vec3& start, const vec3& end, std::vector<vec3>& out_points, bool* out_partial) const -> bool
{
	out_points.clear();
	if (out_partial != nullptr)
	{
		*out_partial = false;
	}

	dtNavMeshQuery* query = is_loaded() ? get_query() : nullptr;
	if (query == nullptr)
	{
		return false;
	}

	uint64_t start_ref = 0, end_ref = 0;
	float start_pos[3], end_pos[3];
	if (!find_nearest_poly(query, start, start_ref, start_pos) || !find_nearest_poly(query, end, end_ref, end_pos))
	{
		return false;
	}

	dtPolyRef polys[max_path_polys];
	int poly_count = 0;
	const dtStatus status = query->findPath(static_cast<dtPolyRef>(start_ref), static_cast<dtPolyRef>(end_ref), start_pos, end_pos,
		&default_filter(), polys, &poly_count, max_path_polys);
	if (dtStatusFailed(status) || poly_count == 0)
	{
		return false;
	}

	float target[3];
	dtVcopy(target, end_pos);
	if (polys[poly_count - 1] != static_cast<dtPolyRef>(end_ref))
	{
		query->closestPointOnPoly(polys[poly_count - 1], end_pos, target, nullptr);
		if (out_partial != nullptr)
		{
			*out_partial = true;
		}
	}

	float straight[max_path_polys * 3];
	int straight_count = 0;
	if (dtStatusFailed(query->findStraightPath(start_pos, target, polys, poly_count, straight, nullptr, nullptr, &straight_count, max_path_polys)))
	{
		return false;
	}

	out_points.reserve(straight_count);
	for (int i = 0; i < straight_count; ++i)
	{
		out_points.push_back(to_ue(&straight[i * 3]));
	}

	return !out_points.empty();
}

auto nav_mesh::find_random_point_around(const vec3& center, float radius, vec3& out_pos) const -> bool
{
	dtNavMeshQuery* query = is_loaded() ? get_query() : nullptr;
	if (query == nullptr)
	{
		return false;
	}

	uint64_t center_ref = 0;
	float center_pos[3];
	if (!find_nearest_poly(query, center, center_ref, center_pos))
	{
		return false;
	}

	dtPolyRef random_ref = 0;
	float random_pos[3];
	if (dtStatusFailed(query->findRandomPointAroundCircle(static_cast<dtPolyRef>(center_ref), center_pos, radius * ue_to_recast_scale,
		&default_filter(), &thread_random, &random_ref, random_pos)))
	{
		return false;
	}

	out_pos = to_ue(random_pos);
	return true;
}
