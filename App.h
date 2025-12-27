#pragma once

#include <SFML/Graphics.hpp>
#include "Config.h"
#include "PhotoSlot.h"
#include <vector>

// Główna klasa aplikacji, która zarządza wszystkim.
class App {
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

    // Zanikanie zdjęcia
    bool m_isTransition = false;
    float m_transitionTime = 0.0f;
    float m_transitionDuration = 2.0f; // 1 sekunda = 0.5 fade out + 0.5 fade in
    int m_fadingSlotIndex = -1;

    sf::Texture m_oldTexture;
    std::optional<sf::Sprite> m_oldSprite;
    std::optional<sf::Text> m_oldText;
    std::optional<sf::Sprite> m_oldFrameSprite;          // dla customfromfile
    std::optional<sf::RectangleShape> m_oldFrameShape;   // dla simple/polaroid
    bool m_oldFrameIsCustom = false;
};
