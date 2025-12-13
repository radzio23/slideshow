#pragma once

#include <SFML/Graphics.hpp>
#include "Config.h"
#include "PhotoSlot.h"
#include <vector>

// Główna klasa aplikacji, która zarządza wszystkim.
class App
{
public:
    App();
    void run();

private:
    void processEvents();
    void update();
    void render();

    void setupBackground();
    void setupSlots();

    sf::RenderWindow m_window;
    Config m_config;

    std::vector<std::filesystem::path> m_imagePaths;
    std::vector<PhotoSlot> m_slots;

    // Zasoby graficzne
    sf::Texture m_bgTexture;
    sf::Sprite m_bgSprite;
    sf::Texture m_customFrameTexture;
    bool m_hasBg = false;

    // Sterowanie czasem
    sf::Clock m_clock;
    int m_currentImageIndex = 0;
    int m_currentSlotIndex = 0;
};
