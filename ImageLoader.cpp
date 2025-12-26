#include "ImageLoader.h"
#include <algorithm>
#include <iostream>

std::vector<std::filesystem::path> ImageLoader::scanDirectory(const std::string &path) {
    std::vector<std::filesystem::path> imagePaths;

    try {
        for (const auto &entry : std::filesystem::directory_iterator(path)) {
            if (!entry.is_regular_file()) continue;

            std::string ext = entry.path().extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

            if (ext == ".png" || ext == ".jpg" || ext == ".jpeg") {
                imagePaths.push_back(entry.path());
            }
        }
    }
    catch (const std::filesystem::filesystem_error &e) {
        std::cerr << "Blad odczytu folderu '" << path << "': " << e.what() << std::endl;
    }

    if (!imagePaths.empty()) {
        std::sort(imagePaths.begin(), imagePaths.end());
    }

    return imagePaths;
}
