#pragma once

#include <string>
#include <vector>
#include <SFML/Graphics/Color.hpp>
#include "FrameType.h"
#include "SlotConfig.h"

// Kontener na wszystkie ustawienia aplikacji pobrane z konfiguracji.
struct AppConfig
{
    std::string backgroundFile;              // Œcie¿ka do pliku z t³em.
    FrameType frameType = FrameType::Simple; // Styl ramki.
    std::string frameFilePath;               // Œcie¿ka do pliku tekstury ramki niestandardowej.
    sf::Color frameColor = sf::Color::White; // Kolor obramowania.
    float interval = 2.0f;                   // Czas w sekundach miêdzy zmianami zdjêæ.
    bool randomMode = false;                 // Czy zdjêcia maj¹ byæ losowane.
    std::vector<SlotConfig> slots;           // Lista definicji poszczególnych miejsc na zdjêcia.
};