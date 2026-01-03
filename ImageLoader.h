#pragma once

#include <vector>
#include <filesystem>

// Zestaw narzędzi do przeszukiwania dysku w celu znalezienia obrazów.
namespace ImageLoader {
    // Przeszukuje wskazany folder i tworzy listę ścieżek do plików PNG i JPG.
    std::vector<std::filesystem::path> scanDirectory(const std::string& path);
}