#pragma once
#include "pch.h"
#include <filesystem>
#include "vec3.h"

class dtNavMesh;
class dtNavMeshQuery;
class nav_mesh
{
public:
	static constexpr int max_path_polys = 256;
	static constexpr int max_search_nodes = 2048;

public:
	nav_mesh();
	~nav_mesh();

	nav_mesh(const nav_mesh&) = delete;
	nav_mesh& operator=(const nav_mesh&) = delete;

public:
	auto load(const std::filesystem::path& file_path) -> fw::error;
	auto is_loaded() const -> bool { return nav_ != nullptr; }

	auto get_tile_count() const -> int { return tile_count_; }
	auto get_poly_count() const -> int { return poly_count_; }
	auto find_nearest_point(const vec3& pos, vec3& out_pos) const -> bool;

	//  끝까지 갈 수 없으면 도달 가능한 가장 가까운 지점까지의 경로를 돌려준다 (out_partial = true)
	auto find_path(const vec3& start, const vec3& end, std::vector<vec3>& out_points, bool* out_partial = nullptr) const -> bool;
	auto find_random_point_around(const vec3& center, float radius, vec3& out_pos) const -> bool;

private:
	auto get_query() const -> dtNavMeshQuery*;
	auto find_nearest_poly(dtNavMeshQuery* query, const vec3& pos, uint64_t& out_ref, float* out_nearest) const -> bool;

private:
	dtNavMesh* nav_ = nullptr;
	const uint64_t instance_id_; // 스레드별 쿼리 캐시 키
	int tile_count_ = 0;
	int poly_count_ = 0;
};
