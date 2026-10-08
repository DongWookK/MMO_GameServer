#pragma once
#include "pch.h"
#include "enum_common_generated.h"
#include "vec3.h"

class gobject : public std::enable_shared_from_this<gobject>
{
public:
	using pool_index_t = uint32_t;
	using reuse_count_t = uint32_t;
	using map_no_t = uint32_t;
	using sector_id_t = uint32_t;

	union object_id_t
	{
		static constexpr uint32_t index_bits = 20;
		static constexpr uint32_t reuse_bits = 8;
		static constexpr uint32_t type_bits = 4;
		static constexpr pool_index_t max_index = (1u << index_bits) - 1;
		static constexpr reuse_count_t reuse_mask = (1u << reuse_bits) - 1;

		int32_t value;
		struct
		{
			uint32_t index       : index_bits;	// [19..0]
			uint32_t reuse_count : reuse_bits;	// [27..20]
			uint32_t type        : type_bits;	// [31..28]
		};

		constexpr object_id_t() : value(0) {}
		explicit constexpr object_id_t(int32_t raw) : value(raw) {}
		object_id_t(common::object_type object_type, reuse_count_t reuse, pool_index_t pool_index)
			: value(0)
		{
			index = pool_index & max_index;
			reuse_count = reuse & reuse_mask;
			type = std::to_underlying(object_type);
		}

		auto is_valid() const -> bool { return value != 0; }
		auto get_type() const -> common::object_type { return static_cast<common::object_type>(type); }

		friend auto operator==(object_id_t lhs, object_id_t rhs) -> bool { return lhs.value == rhs.value; }
		friend auto operator<=>(object_id_t lhs, object_id_t rhs) { return lhs.value <=> rhs.value; }
		friend auto hash_value(object_id_t id) -> size_t { return std::hash<int32_t>{}(id.value); }	// boost::multi_index hashed index
	};
	static_assert(sizeof(object_id_t) == sizeof(int32_t));
	static_assert(std::to_underlying(common::object_type::MAX) < (1u << object_id_t::type_bits), "object_type does not fit in object_id type bits");

	static const object_id_t invalid_object_id;	// 정의는 클래스 밖 (중첩 타입이 완성된 뒤)
	static constexpr map_no_t invalid_map_id = 0;
	static constexpr sector_id_t invalid_sector_id = (std::numeric_limits<sector_id_t>::max)();

public:
	explicit gobject(common::object_type type);
	virtual ~gobject() = default;

	gobject(const gobject&) = delete;
	gobject& operator=(const gobject&) = delete;
	gobject(gobject&&) = delete;
	gobject& operator=(gobject&&) = delete;

public:
	auto set_pool_index(pool_index_t index) -> void;
	virtual auto on_release() -> void;

	auto get_pool_index() const -> pool_index_t { return object_id_.index; }
	auto get_reuse_count() const -> reuse_count_t { return object_id_.reuse_count; }

public:
	auto spawn(map_no_t map_id, const vec3& pos, float heading) -> fw::error;
	auto despawn() -> fw::error;

	virtual auto is_movable() const -> bool { return false; }

public:
	auto get_object_id() const -> object_id_t { return object_id_; }
	auto get_object_type() const -> common::object_type { return object_type_; }
	auto is_spawned() const -> bool { return spawned_; }

	auto get_map_id() const -> map_no_t { return map_id_; }
	auto get_pos() const -> const vec3& { return pos_; }
	auto get_heading() const -> float { return heading_; }
	auto get_sector_id() const -> sector_id_t { return sector_id_; }

	auto set_pos(const vec3& pos) -> void { pos_ = pos; }
	auto set_heading(float heading) -> void { heading_ = heading; }
	auto set_sector_id(sector_id_t sector_id) -> void { sector_id_ = sector_id; }

protected:
	virtual auto on_spawn() -> void {}
	virtual auto on_despawn() -> void {}

private:
	const common::object_type object_type_;
	object_id_t   object_id_{};

	bool        spawned_ = false;
	map_no_t    map_id_ = invalid_map_id;
	vec3        pos_{};
	float       heading_ = 0.f;
	sector_id_t sector_id_ = invalid_sector_id;
};

inline const gobject::object_id_t gobject::invalid_object_id{};

template <>
struct std::hash<gobject::object_id_t>
{
	auto operator()(gobject::object_id_t id) const noexcept -> size_t { return std::hash<int32_t>{}(id.value); }
};
