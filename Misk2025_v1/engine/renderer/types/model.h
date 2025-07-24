#pragma once
#include <string>
#include <vector>


#include <glm/glm.hpp>


#include "../renderer_common.h"


struct Model
{
private:
	std::string m_name = "undefined";
	std::vector<uint32_t> m_mesh_indices;
	Bounding_box m_bounding_box;
public:
	glm::vec3 m_aabb_min;
	glm::vec3 m_aabb_max;
	bool m_awaiting_loading_from_disk = true;
	bool m_loaded_from_disk = false;

public:
	Model() = default;
	Model(const std::string name)
	{
		m_name = name;
	}

	void add_mesh_index(uint32_t index)
	{
		m_mesh_indices.push_back(index);
	}

	size_t get_mesh_count() {
		return m_mesh_indices.size();
	}

	std::vector<uint32_t>& get_mesh_indices() {
		return m_mesh_indices;
	}

	void set_name(const std::string& model_name) {
		this->m_name = model_name;
	}

	const std::string get_name() {
		return m_name;
	}

	const Bounding_box& get_bounding_box() {
		return m_bounding_box;
	}

	void SetBoundingBox(Bounding_box& model_bounding_box) {
		this->m_bounding_box = model_bounding_box;
	}
	

};
