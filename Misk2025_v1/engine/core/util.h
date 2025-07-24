#pragma once
#include <string_view>
#include "core/misk_type.h"
#include <filesystem>

#include "renderer/renderer_common.h"


namespace util
{
    inline bool file_exists(const std::string_view name) {
        struct stat buffer;
        return (stat(name.data(), &buffer) == 0);
    }

    inline const std::string get_filename(const std::string& filepath) {
        std::string result = filepath.substr(filepath.rfind("/") + 1);
        result = result.substr(0, result.length() - 4);
        return result;
    }

    inline File_info get_file_info(std::string filepath) {
        // isolate name
        std::string filename = filepath.substr(filepath.rfind("/") + 1);
        filename = filename.substr(0, filename.length() - 4);
        // isolate filetype
        std::string filetype = filepath.substr(filepath.length() - 3);
        // isolate directory
        std::string directory = filepath.substr(0, filepath.rfind("/") + 1);
        // material name
        std::string materialType = "NONE";
        if (filename.length() > 5) {
            std::string query = filename.substr(filename.length() - 3);
            if (query == "ALB" || query == "RMA" || query == "NRM")
                materialType = query;
        }
        // RETURN IT
        File_info info;
        info.path = filepath;
        info.name = filename;
        info.ext = filetype;
        info.dir = directory;
        info.materialType = materialType;
        return info;
    }

    inline std::string lowercase(std::string& str) {
        std::string result = "";
        for (auto& c : str) {
            result += std::tolower(c);
        }
        return result;
    }

    inline File_info get_file_info(const std::filesystem::directory_entry& filepath)
    {
        const auto& path{ filepath.path() };

        static const auto get_material_type{ [](std::string_view filename) {
            if (filename.size() > 5) {
                filename.remove_prefix(filename.size() - 3);
                if (filename == "ALB" || filename == "RMA" || filename == "NRM") {
                    return std::string{ filename };
                }
            }
            return std::string{ "NONE" };
        } };

        const auto stem{ path.has_stem() ? path.stem().string() : "" };

        std::string filetype = path.has_extension() ? path.extension().string().substr(1) : "";  // remove dot


        return File_info{
            path.string(),
            stem,
            lowercase(filetype),
            path.parent_path().string(),
            get_material_type(stem)
        };
    }

    inline bool Str_cmp(const char* queryA, const char* queryB) {
        if (strcmp(queryA, queryB) == 0)
            return true;
        else
            return false;
    }



    /*
    
███████╗██╗██╗     ███████╗    ██╗      ██████╗  █████╗ ██████╗ 
██╔════╝██║██║     ██╔════╝    ██║     ██╔═══██╗██╔══██╗██╔══██╗
█████╗  ██║██║     █████╗      ██║     ██║   ██║███████║██║  ██║
██╔══╝  ██║██║     ██╔══╝      ██║     ██║   ██║██╔══██║██║  ██║
██║     ██║███████╗███████╗    ███████╗╚██████╔╝██║  ██║██████╔╝
╚═╝     ╚═╝╚══════╝╚══════╝    ╚══════╝ ╚═════╝ ╚═╝  ╚═╝╚═════╝ 
                                                                

    */

    inline std::string get_full_path(const std::filesystem::directory_entry& entry) {
        return entry.path().string();
    }

    inline std::string get_file_name(const std::filesystem::directory_entry& entry) {
        return entry.path().filename().string();
    }

    inline std::string get_file_name_without_extension(const std::filesystem::directory_entry& entry) {
        return entry.path().stem().string(); // stem() removes the extension
    }

    inline std::string get_file_extension(const std::filesystem::directory_entry& entry) {
        return entry.path().extension().string().substr(1);
    }

    inline std::vector<File_info> iterate_directory(const std::string& directory, std::vector<std::string> extensions = std::vector<std::string>()) {
        std::vector<File_info> fileInfos;
        auto entries = std::filesystem::directory_iterator(directory);
        for (const auto& entry : entries) {
            // Skip directories
            if (!std::filesystem::is_regular_file(entry)) {
                continue;
            }
            File_info fileInfo;
            fileInfo.path = util::get_full_path(entry);
            fileInfo.name = util::get_file_name_without_extension(entry);
            fileInfo.ext = util::get_file_extension(entry);
            fileInfo.dir = directory;
            if (extensions.empty()) {
                fileInfos.push_back(fileInfo);
            }
            else {
                for (std::string& ext : extensions) {
                    if (ext == fileInfo.ext) {
                        fileInfos.push_back(fileInfo);
                        break;
                    }
                }
            }
            MK_TRACE("the file name {}", fileInfo.GetFileNameWithExtension());
        }
        
        return fileInfos;
    }
}