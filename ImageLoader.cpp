#include "ImageLoader.h"
#include <algorithm>
#include <iostream>

std::vector<std::filesystem::path> ImageLoader::scanDirectory(const std::string& path) {
    std::vector<std::filesystem::path> imagePaths;

    try {
        // Przegl¹danie folderu plik po pliku
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (!entry.is_regular_file()) continue;

            std::string ext = entry.path().extension().string();
            // Konwersja rozszerzenia na ma³e litery
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

            if (ext == ".png" || ext == ".jpg" || ext == ".jpeg") {
                imagePaths.push_back(entry.path());
            }
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Blad odczytu folderu: " << e.what() << std::endl;
    }

    // Sortowanie alfabetyczne œcie¿ek
    if (!imagePaths.empty()) {
        std::sort(imagePaths.begin(), imagePaths.end());
    }

    return imagePaths;
}