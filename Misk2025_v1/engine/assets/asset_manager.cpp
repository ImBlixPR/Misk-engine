#include "mkpch.h"
#include "asset_manager.h"

#define STBI_FAILURE_USERMSG


#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include "stb_image.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <chrono>




#include "core/misk_type.h"
#include <filesystem>
#include "core/util.h"
#include "core/backend.h"
#include "renderer/opengl/opengl_renderer.h"
#include "renderer/types/mesh.h"
#include "renderer/types/model.h"
#include "API/opengl/opengl_backend.h"
#include <future>
#include "renderer/renderer_util.h"




namespace asset {

	//vertices and indices data
	std::vector<Vertex> g_vertices;
	std::vector<uint32_t> g_indices;

	//texture data
	std::vector<Texture> g_textures;
	std::unordered_map<std::string, int> g_textureIndexMap;

	std::vector<Mesh> g_meshes;
	std::vector<Model> g_models;
	std::vector<std::future<void>> g_futures;

	//data tracker for model and mesh
	int _nextVertexInsert = 0;
	int _nextIndexInsert = 0;

	int _quadMeshIndex = 0;
	int _cubeMeshIndex = 0;
	int _upFacingPlaneMeshIndex = 0;

	struct Loading_item_complate
	{
		bool hardcoded_model = false;
		bool imported_models = false;
		bool all = false;
	} g_loading_item_complate;

	//log data
	std::vector<std::string> g_load_log;


	Texture_data load_texture_data(std::string filepath)
	{
		stbi_set_flip_vertically_on_load(false);
		Texture_data data;
		data.m_data = stbi_load(filepath.data(), &data.m_width, &data.m_height, &data.m_num_channels, 0);
		return data;
	}


	unsigned int load_texture(std::string filepath)
	{
		Texture_data data = load_texture_data(filepath);
		unsigned int textureID;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Set up the texture with RGBA format
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data.m_width, data.m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.m_data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(data.m_data);
		return textureID;
	}

	std::string read_text_from_file(std::string path)
	{
		std::ifstream file(path);
		if (!file.is_open()) {
			MK_CORE_ERROR("Failed to open file: {}", path);
		}

		std::string str;
		std::string line;
		while (std::getline(file, line)) {
			str += line + "\n";
		}
		return str;
	}

	void upload_vertex_data()
	{
		if (backend::GetAPI() == API::OPENGL) {
			opengl_backend::upload_vertex_data(g_vertices, g_indices);
		}
		else if (backend::GetAPI() == API::VULKAN) {
			//vulkan implementation
		}
		//AddItemToLoadLog("Uploaded vertex data");
	}

	void load_texture(Texture* texture)
	{
		texture->load();
	}

	Texture* get_texture_by_name(const std::string& name)
	{
		for (Texture& texture : g_textures) {
			if (texture.get_filename() == name)
				return &texture;
		}
		MK_CORE_ERROR("asset::get_texture_by_name() failed because {} does not exist", name);
		return nullptr;
	}

	Texture* get_texture_by_index(const int index)
	{
		if (index >= 0 && index < g_textures.size()) {
			return &g_textures[index];
		}
		MK_CORE_ERROR("asset::get_texture_by_index() failed because index {} is out of range. Size is {} ", index, g_textures.size());
		return nullptr;
	}

	int get_texture_count()
	{
		return g_textures.size();
	}

	int get_texture_index_by_name(const std::string& filename, bool ignoreWarning)
	{
		auto it = g_textureIndexMap.find(filename);
		if (it != g_textureIndexMap.end()) {
			return it->second;
		}
		else {
			/*std::cout << "g_textureIndexMap.size(): " << g_textureIndexMap.size() << "\n";
			for (const auto& pair : g_textureIndexMap) {
				std::cout << pair.first << ": " << pair.second << '\n';
			}*/
			if (!ignoreWarning) {
				MK_CORE_ERROR("asset::GetTextureIndexByName() failed because {} does not exist", filename);
			}
			return -1;
		}
	}

	bool texture_exists(const std::string& name)
	{
		for (Texture& texture : g_textures)
			if (texture.get_filename() == name)
				return true;
		return false;
	}

	std::vector<Texture>& get_textures()
	{
		return g_textures;
	}

	void load_font()
	{
		std::filesystem::path fontPath = "res/textures/font/";

		try {
			if (!std::filesystem::exists(fontPath)) {
				std::cerr << "Font directory not found: " << std::filesystem::absolute(fontPath) << '\n';
				return;
			}

			// Collect all valid files first
			std::vector<File_info> validFiles;
			for (const auto& entry : std::filesystem::directory_iterator(fontPath)) {
				if (entry.is_regular_file()) {
					File_info info = util::get_file_info(entry);
					if (info.ext == "png" || info.ext == "jpg" || info.ext == "tga") {
						validFiles.push_back(std::move(info));
					}
				}
			}

			// Reserve space to avoid reallocations
			g_textures.reserve(g_textures.size() + validFiles.size());

			// Load textures
			for (const auto& info : validFiles) {
				try {
					Texture& texture = g_textures.emplace_back(Texture(info.path.c_str(), false));
					load_texture(&texture);
					texture.bake();
				}
				catch (const std::exception& e) {
					std::cerr << "Failed to load texture: " << info.path << " - " << e.what() << '\n';
					// Remove the failed texture
					g_textures.pop_back();
				}
			}

			// API-specific operations
			if (backend::GetAPI() == API::OPENGL) {
				opengl::bind_bindless_textures();
			}
			else if (backend::GetAPI() == API::VULKAN) {
				// vulkan implementation
			}

			// Update index map
			for (size_t i = 0; i < g_textures.size(); i++) {
				g_textureIndexMap[g_textures[i].get_filename()] = i;
			}

		}
		catch (const std::filesystem::filesystem_error& e) {
			std::cerr << "Filesystem error in load_font(): " << e.what() << '\n';
		}
		catch (const std::exception& e) {
			std::cerr << "Error in load_font(): " << e.what() << '\n';
		}
	}

	misk::ivec2 get_texture_size_by_name(const char* textureName)
	{
		static std::unordered_map<const char*, int> textureIndices;
		if (textureIndices.find(textureName) == textureIndices.end()) {
			textureIndices[textureName] = asset::get_texture_index_by_name(textureName);
		}
		Texture* texture = asset::get_texture_by_index(textureIndices[textureName]);
		if (texture) {
			return misk::ivec2(texture->get_width(), texture->get_height());
		}
		else {
			return misk::ivec2(0, 0);
		}
	}

	void find_asset_path()
	{
		//MK_CORE_INFO("Scanning res/models/ directory for model files...");

		//models 
		for (File_info& fileInfo : util::iterate_directory("res/models/")) {
			//MK_CORE_INFO("Found model file: {}", fileInfo.GetFileNameWithExtension());
			g_models.emplace_back(fileInfo.GetFileNameWithExtension());
		}

		//MK_CORE_INFO("Total models found: {}", g_models.size());
	}

	/*
	
███╗   ███╗ ██████╗ ██████╗ ███████╗██╗     ███████╗
████╗ ████║██╔═══██╗██╔══██╗██╔════╝██║     ██╔════╝
██╔████╔██║██║   ██║██║  ██║█████╗  ██║     ███████╗
██║╚██╔╝██║██║   ██║██║  ██║██╔══╝  ██║     ╚════██║
██║ ╚═╝ ██║╚██████╔╝██████╔╝███████╗███████╗███████║
╚═╝     ╚═╝ ╚═════╝ ╚═════╝ ╚══════╝╚══════╝╚══════╝
                                                    
	*/
	void create_hardcoded_model()
	{


		/* Quad */ {
			Vertex vertA, vertB, vertC, vertD;
			vertA.position = { -1.0f, -1.0f, 0.0f };
			vertB.position = { -1.0f, 1.0f, 0.0f };
			vertC.position = { 1.0f,  1.0f, 0.0f };
			vertD.position = { 1.0f,  -1.0f, 0.0f };
			vertA.uv = { 0.0f, 0.0f };
			vertB.uv = { 0.0f, 1.0f };
			vertC.uv = { 1.0f, 1.0f };
			vertD.uv = { 1.0f, 0.0f };
			vertA.normal = glm::vec3(0, 0, 1);
			vertB.normal = glm::vec3(0, 0, 1);
			vertC.normal = glm::vec3(0, 0, 1);
			vertD.normal = glm::vec3(0, 0, 1);
			vertA.tangent = glm::vec3(1, 0, 0);
			vertB.tangent = glm::vec3(1, 0, 0);
			vertC.tangent = glm::vec3(1, 0, 0);
			vertD.tangent = glm::vec3(1, 0, 0);
			std::vector<Vertex> vertices;
			vertices.push_back(vertA);
			vertices.push_back(vertB);
			vertices.push_back(vertC);
			vertices.push_back(vertD);
			std::vector<uint32_t> indices = { 2, 1, 0, 3, 2, 0 };
			std::string name = "Quad";

			glm::vec3 aabbMin;
			glm::vec3 aabbMax;
			renderer_util::calculateAABB(vertices, aabbMin, aabbMax);

			Model& model = g_models.emplace_back();
			model.set_name(name);
			model.add_mesh_index(asset::create_mesh("Fullscreen", vertices, indices, aabbMin, aabbMax));
			//imblix
			
			_quadMeshIndex = model.get_mesh_indices()[0];
			
			model.m_awaiting_loading_from_disk = false;
			model.m_loaded_from_disk = true;
		}

		/* Cube */ {
			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;

			glm::vec3 positions[8] = {
				{-1.0f, -1.0f, -1.0f}, // 0
				{ 1.0f, -1.0f, -1.0f}, // 1
				{ 1.0f,  1.0f, -1.0f}, // 2
				{-1.0f,  1.0f, -1.0f}, // 3
				{-1.0f, -1.0f,  1.0f}, // 4
				{ 1.0f, -1.0f,  1.0f}, // 5
				{ 1.0f,  1.0f,  1.0f}, // 6
				{-1.0f,  1.0f,  1.0f}  // 7
			};

			glm::vec2 uvs[4] = {
				{0.0f, 0.0f},
				{1.0f, 0.0f},
				{1.0f, 1.0f},
				{0.0f, 1.0f}
			};

			struct Face {
				int i0, i1, i2, i3;
				glm::vec3 normal;
				glm::vec3 tangent;
			};

			Face faces[6] = {
				{0, 1, 2, 3, { 0,  0, -1}, {1, 0,  0}}, // Back
				{5, 4, 7, 6, { 0,  0,  1}, {1, 0,  0}}, // Front
				{4, 0, 3, 7, {-1,  0,  0}, {0, 0, -1}}, // Left
				{1, 5, 6, 2, { 1,  0,  0}, {0, 0,  1}}, // Right
				{3, 2, 6, 7, { 0,  1,  0}, {1, 0,  0}}, // Top
				{4, 5, 1, 0, { 0, -1,  0}, {1, 0,  0}}, // Bottom
			};

			for (const Face& face : faces) {
				Vertex v0, v1, v2, v3;
				v0.position = positions[face.i0];
				v1.position = positions[face.i1];
				v2.position = positions[face.i2];
				v3.position = positions[face.i3];

				v0.uv = uvs[0];
				v1.uv = uvs[1];
				v2.uv = uvs[2];
				v3.uv = uvs[3];

				v0.normal = v1.normal = v2.normal = v3.normal = face.normal;
				v0.tangent = v1.tangent = v2.tangent = v3.tangent = face.tangent;

				uint32_t baseIndex = static_cast<uint32_t>(vertices.size());
				vertices.push_back(v0);
				vertices.push_back(v1);
				vertices.push_back(v2);
				vertices.push_back(v3);

				indices.push_back(baseIndex + 0);
				indices.push_back(baseIndex + 1);
				indices.push_back(baseIndex + 2);
				indices.push_back(baseIndex + 2);
				indices.push_back(baseIndex + 3);
				indices.push_back(baseIndex + 0);
			}

			glm::vec3 aabbMin, aabbMax;
			renderer_util::calculateAABB(vertices, aabbMin, aabbMax);

			std::string name = "Cube";

			Model& model = g_models.emplace_back();
			model.set_name(name);
			model.add_mesh_index(asset::create_mesh("Cube", vertices, indices, aabbMin, aabbMax));

			_cubeMeshIndex = model.get_mesh_indices()[0];

			model.m_awaiting_loading_from_disk = false;
			model.m_loaded_from_disk = true;
		}

		/* Upfacing Plane */ {
			Vertex vertA, vertB, vertC, vertD;
			vertA.position = glm::vec3(-0.5, 0, 0.5);
			vertB.position = glm::vec3(0.5, 0, 0.5f);
			vertC.position = glm::vec3(0.5, 0, -0.5);
			vertD.position = glm::vec3(-0.5, 0, -0.5);
			vertA.uv = { 0.0f, 1.0f };
			vertB.uv = { 1.0f, 1.0f };
			vertC.uv = { 1.0f, 0.0f };
			vertD.uv = { 0.0f, 0.0f };
			vertA.normal = glm::vec3(0, 1, 0);
			vertB.normal = glm::vec3(0, 1, 0);
			vertC.normal = glm::vec3(0, 1, 0);
			vertD.normal = glm::vec3(0, 1, 0);
			vertA.tangent = glm::vec3(0, 0, 1);
			vertB.tangent = glm::vec3(0, 0, 1);
			vertC.tangent = glm::vec3(0, 0, 1);
			vertD.tangent = glm::vec3(0, 0, 1);
			std::vector<Vertex> vertices;
			vertices.push_back(vertA);
			vertices.push_back(vertB);
			vertices.push_back(vertC);
			vertices.push_back(vertD);
			std::vector<uint32_t> indices = { 0, 1, 2, 2, 3, 0 };
			std::string name = "UpFacingPLane";

			glm::vec3 aabbMin;
			glm::vec3 aabbMax;
			renderer_util::calculateAABB(vertices, aabbMin, aabbMax);

			Model& model = g_models.emplace_back();
			model.set_name("UpFacingPLane");
			model.add_mesh_index(asset::create_mesh(name, vertices, indices, aabbMin, aabbMax));
			model.m_awaiting_loading_from_disk = false;
			model.m_loaded_from_disk = true;
			_upFacingPlaneMeshIndex = model.get_mesh_indices()[0];
		}




		asset::upload_vertex_data();
	}

	void load_model_from_file(const std::string& filename) {
		std::string filepath = "res/models/" + filename;
		std::filesystem::path filepath_s = filepath;

		// Check if file exists
		if (!std::filesystem::exists(filepath_s)) {
			MK_CORE_ERROR("Model file does not exist: {}", filepath);
			return;
		}

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filepath,
			aiProcess_Triangulate |
			aiProcess_GenNormals |
			aiProcess_CalcTangentSpace |
			aiProcess_JoinIdenticalVertices |
			aiProcess_FlipUVs |  // Often needed for OpenGL
			aiProcess_OptimizeMeshes |
			aiProcess_ValidateDataStructure
		);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			MK_CORE_ERROR("Assimp failed to load {}: {}", filepath, importer.GetErrorString());
			return;
		}

		std::string model_name = filename;

		// Find existing model instead of creating new one
		Model* target_model = nullptr;
		for (Model& model : g_models) {
			if (model.get_name() == model_name) {
				target_model = &model;
				break;
			}
		}

		if (!target_model) {
			MK_CORE_ERROR("Model {} not found in g_models vector", model_name);
			return;
		}

		// Track overall model bounds
		glm::vec3 model_aabb_min(FLT_MAX);
		glm::vec3 model_aabb_max(-FLT_MAX);

		std::function<void(aiNode*, const aiScene*)> process_node;
		process_node = [&](aiNode* node, const aiScene* scene) {
			for (unsigned int i = 0; i < node->mNumMeshes; i++) {
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

				std::vector<Vertex> vertices;
				std::vector<uint32_t> indices;

				// Reserve space for efficiency
				vertices.reserve(mesh->mNumVertices);
				indices.reserve(mesh->mNumFaces * 3);

				// Process vertices
				for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
					Vertex vertex;
					vertex.position = glm::vec3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
					vertex.normal = mesh->HasNormals() ?
						glm::vec3(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z) :
						glm::vec3(0.0f, 0.0f, 1.0f);
					vertex.uv = mesh->HasTextureCoords(0) ?
						glm::vec2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y) :
						glm::vec2(0.0f);
					vertex.tangent = mesh->HasTangentsAndBitangents() ?
						glm::vec3(mesh->mTangents[v].x, mesh->mTangents[v].y, mesh->mTangents[v].z) :
						glm::vec3(1.0f, 0.0f, 0.0f);
					vertices.push_back(vertex);
				}

				// Process indices
				for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
					aiFace face = mesh->mFaces[f];
					if (face.mNumIndices != 3) {
						MK_CORE_WARN("Non-triangular face found in mesh {}, skipping", mesh->mName.C_Str());
						continue;
					}
					for (unsigned int j = 0; j < face.mNumIndices; j++) {
						indices.push_back(face.mIndices[j]);
					}
				}

				// Calculate mesh AABB
				glm::vec3 mesh_aabb_min, mesh_aabb_max;
				renderer_util::calculateAABB(vertices, mesh_aabb_min, mesh_aabb_max);

				// Update model AABB
				model_aabb_min = glm::min(model_aabb_min, mesh_aabb_min);
				model_aabb_max = glm::max(model_aabb_max, mesh_aabb_max);

				// Create mesh name
				std::string mesh_name = mesh->mName.C_Str();
				if (mesh_name.empty()) {
					mesh_name = model_name + "_Mesh_" + std::to_string(target_model->get_mesh_count());
				}

				// Create mesh and add to model
				int mesh_index = asset::create_mesh(mesh_name, vertices, indices, mesh_aabb_min, mesh_aabb_max);
				target_model->add_mesh_index(mesh_index);
			}

			// Process child nodes
			for (unsigned int i = 0; i < node->mNumChildren; i++) {
				process_node(node->mChildren[i], scene);
			}
			};

		process_node(scene->mRootNode, scene);

		// Set model bounding box
		target_model->m_aabb_min = model_aabb_min;
		target_model->m_aabb_max = model_aabb_max;

		// Create bounding box for the model
		Bounding_box bbox;
		// Build the bounding box
		float width = std::abs(model_aabb_max.x - model_aabb_min.x);
		float height = std::abs(model_aabb_max.y - model_aabb_min.y);
		float depth = std::abs(model_aabb_max.z - model_aabb_min.z);
		bbox.size = glm::vec3(width, height, depth);
		bbox.offset_from_model_origin = model_aabb_min;
		target_model->SetBoundingBox(bbox);

		//MK_CORE_INFO("the bounding box size is ({}, {}, {})", bbox.size.x, bbox.size.y, bbox.size.z);

		// Update model state
		target_model->m_awaiting_loading_from_disk = false;
		target_model->m_loaded_from_disk = true;

		

		//MK_TRACE("Model {} loaded successfully with {} meshes",
		//	target_model->get_name(), target_model->get_mesh_count());
	}

	std::vector<Model> get_models_list()
	{
		return g_models;
	}

	int get_model_index_by_name(const std::string& name)
	{
		for (int i = 0; i < g_models.size(); i++) {
			if (g_models[i].get_name() == name) {
				return i;
			}
		}
		//MK_CORE_ERROR("Model with name '{}' not found!", name);
		return -1;
	}



		/*
	█▄ ▄█ █▀▀ █▀▀ █ █
	█ █ █ █▀▀ ▀▀█ █▀█
	▀   ▀ ▀▀▀ ▀▀▀ ▀ ▀ */

	int create_mesh(std::string name, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, glm::vec3 aabbMin, glm::vec3 aabbMax) {
		Mesh& mesh = g_meshes.emplace_back();
		mesh.baseVertex = _nextVertexInsert;
		//imblix
		mesh.baseIndex = _nextIndexInsert;
		mesh.vertexCount = (uint32_t)vertices.size();
		mesh.indexCount = (uint32_t)indices.size();
		mesh.name = name;
		mesh.aabbMin = aabbMin;
		mesh.aabbMax = aabbMax;
		mesh.extents = aabbMax - aabbMin;
		mesh.boundingSphereRadius = std::max(mesh.extents.x, std::max(mesh.extents.y, mesh.extents.z)) * 0.5f;

		g_vertices.reserve(g_vertices.size() + vertices.size());
		g_vertices.insert(std::end(g_vertices), std::begin(vertices), std::end(vertices));
		g_indices.reserve(g_indices.size() + indices.size());
		g_indices.insert(std::end(g_indices), std::begin(indices), std::end(indices));
		_nextVertexInsert += mesh.vertexCount;
		_nextIndexInsert += mesh.indexCount;
		return g_meshes.size() - 1;
	}

	Mesh* get_quad_mesh()
	{
		
		return &g_meshes[_quadMeshIndex];
	}

	Mesh* get_mesh_by_index(int index)
	{
		return &g_meshes[index];
	}

	std::vector<Mesh> get_mesh_list()
	{
		return g_meshes;
	}

	int get_quad_mesh_index()
	{
		return _quadMeshIndex;
	}

	int get_up_facing_plane_mesh_index()
	{
		return _upFacingPlaneMeshIndex;
	}

	int get_cube_mesh_index()
	{
		return _cubeMeshIndex;
	}

	void load_next_items()
	{
		//MK_TRACE("model number {}", g_models.size());

		if (!g_loading_item_complate.hardcoded_model)
		{
			
			add_item_to_load_log("Creating hardcoded models...");
			create_hardcoded_model();
			g_loading_item_complate.hardcoded_model = true;
			return;
		}

		// Check if there are any pending async tasks
		if (!g_futures.empty())
		{
			// Check if all futures are ready
			bool all_ready = true;
			for (auto& future : g_futures)
			{
				if (future.valid() && future.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
				{
					all_ready = false;
					break;
				}
			}

			if (!all_ready)
			{
				// Still waiting for async tasks to complete
				return;
			}
			else
			{
				// All async tasks are complete, clean up futures
				//MK_CORE_INFO("All async model loading tasks completed");
				g_futures.clear();
			}
		}

		// Look for models that still need loading
		bool found_awaiting = false;
		for (Model& model : g_models)
		{
			if (model.m_awaiting_loading_from_disk)
			{
				found_awaiting = true;
				model.m_awaiting_loading_from_disk = false;

				std::string model_name = model.get_name(); // safely capture by value

				//MK_CORE_INFO("Queued model for loading: {}", model_name);
				add_item_to_load_log("res/assets/models/" + model.get_name() + ".model");

				g_futures.push_back(std::async(std::launch::async, [model_name]() {
					//MK_CORE_INFO("Loading model asynchronously: {}", model_name);
					load_model_from_file(model_name);
					}));
				return;
			}
		}

		// If we get here, no models are awaiting loading and all async tasks are done
		if (!found_awaiting && g_futures.empty())
		{
			//MK_CORE_INFO("No models awaiting loading, completing asset loading");
			g_loading_item_complate.imported_models = true;
			g_loading_item_complate.all = true;
		}
	}



	bool is_loading_asset_complate()
	{
		return g_loading_item_complate.all;
	}

	//log

	void add_item_to_load_log(std::string item) {
		g_load_log.push_back(item);
	}

	std::vector<std::string>& get_load_log() {
		return g_load_log;
	}
	  
	void log_data()
	{

		MK_TRACE("--------------------------------asset data--------------------------------------------");
		MK_TRACE("vertices size {}", g_vertices.size());
		MK_TRACE("indices size {}", g_indices.size());
		MK_TRACE("texture size {}", g_textures.size());
		MK_TRACE("texture index map size {}", g_textureIndexMap.size());
		MK_TRACE("mesh size {}", g_meshes.size());
		MK_TRACE("model size {}", g_models.size());
		MK_TRACE("tracker data: _nextVertexInsert = {}, _nextIndexInsert = {}, _quadMeshIndex = {}", _nextVertexInsert, _nextIndexInsert, _quadMeshIndex);
		MK_TRACE("-------------------------------------------------------------------------------------");

	}

}

