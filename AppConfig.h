#pragma once

#include <string>
#include <vector>
#include <SFML/Graphics/Color.hpp>
#include "FrameType.h"
#include "SlotConfig.h"

// Przechowuje wszystkie wczytane z pliku ustawienia aplikacji.
struct AppConfig
{
    std::string backgroundFile;
    FrameType frameType = FrameType::Simple;
    std::string frameFilePath;
    sf::Color frameColor = sf::Color::White;
    float interval = 2.0f;
    std::vector<SlotConfig> slots;
};
