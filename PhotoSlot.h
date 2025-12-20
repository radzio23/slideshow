#pragma once

#include <SFML/Graphics.hpp>
#include <optional>
#include <filesystem>
#include "SlotConfig.h"
#include "FrameType.h"

// Klasa reprezentująca jedno miejsce na zdjęcie na ekranie.
class PhotoSlot
{
public:
    PhotoSlot();

    // Blokada kopiowania (ze względu na sf::Texture)
    PhotoSlot(const PhotoSlot &) = delete;
    PhotoSlot &operator=(const PhotoSlot &) = delete;
    PhotoSlot(PhotoSlot &&) = default;
    PhotoSlot &operator=(PhotoSlot &&) = default;

    // Ustawia pozycję, rozmiar i typ ramki na podstawie konfiguracji.
    void setup(const SlotConfig &cfg, FrameType fType, sf::Color fColor, const sf::Texture *customFrameTex);

    // Wczytuje nowy obrazek z pliku i dopasowuje go do slotu.
    bool loadImage(const std::filesystem::path &path, FrameType fType);

    // Rysuje zawartość slotu (zdjęcie i ramkę) w podanym oknie.
    void draw(sf::RenderWindow &window, FrameType fType);

    // GETTERY
    sf::Texture& getTexture() { return m_photoTexture; }
    const sf::Texture& getTexture() const { return m_photoTexture; }

    sf::Sprite& getSprite() { return m_photoSprite; }
    const sf::Sprite& getSprite() const { return m_photoSprite; }

private:
    sf::Texture m_photoTexture;
    sf::Sprite m_photoSprite;

    sf::RectangleShape m_shapeFrame;
    std::optional<sf::Sprite> m_customFrameSprite;

    SlotConfig m_config;
    bool m_hasImage = false;
};
