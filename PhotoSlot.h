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

    // Ustawia przezroczystość zdjęcia, tekstu i ramki
    void setFrameAlpha(uint8_t a);

    // GETTERY
    sf::Texture& getTexture() { return m_photoTexture; }
    const sf::Texture& getTexture() const { return m_photoTexture; }

    sf::Sprite& getSprite() { return m_photoSprite; }
    const sf::Sprite& getSprite() const { return m_photoSprite; }

    sf::Text& getText() { return m_text; }
    const sf::Text& getText() const { return m_text; }

    sf::Font& getFont() { return m_font; }
    const sf::Font& getFont() const { return m_font; }

    sf::RectangleShape& getShapeFrame() { return m_shapeFrame; }
    const std::optional<sf::Sprite>& getCustomFrameSprite() const { return m_customFrameSprite; }

private:
    sf::Texture m_photoTexture;
    sf::Sprite m_photoSprite;
    sf::Text m_text;
    sf::Font m_font;

    sf::RectangleShape m_shapeFrame;
    std::optional<sf::Sprite> m_customFrameSprite;

    SlotConfig m_config;
    bool m_hasImage = false;
    FrameType m_frameType;
};
