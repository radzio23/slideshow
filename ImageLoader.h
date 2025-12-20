#pragma once

#include <vector>
#include <filesystem>

// Przestrzeń nazw dla funkcji narzędziowych.
namespace ImageLoader {
    // Skanuje podany katalog w poszukiwaniu plików graficznych.
    std::vector<std::filesystem::path> scanDirectory(const std::string &path);
}
