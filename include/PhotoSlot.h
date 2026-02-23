#pragma once

#include <SFML/Graphics.hpp>
#include <optional>
#include <filesystem>
#include "SlotConfig.h"
#include "FrameType.h"

// Klasa reprezentująca pojedynczy obszar na ekranie, w którym wyświetlane jest zdjęcie.
class PhotoSlot
{
public:
    PhotoSlot();

    // Blokada kopiowania ze względu na zarządzanie teksturą (zasób w pamięci GPU).
    PhotoSlot(const PhotoSlot &) = delete;
    PhotoSlot &operator=(const PhotoSlot &) = delete;
    PhotoSlot(PhotoSlot &&) = default;
    PhotoSlot &operator=(PhotoSlot &&) = default;

    // Przygotowuje geometrię slotu, ramkę i parametry wyświetlania.
    void setup(const SlotConfig &cfg, FrameType fType, sf::Color fColor, const sf::Texture *customFrameTex);

    // Wczytuje nową grafikę z dysku i skaluje ją do wymiarów slotu.
    bool loadImage(const std::filesystem::path &path, FrameType fType);

    // Wyświetla zdjęcie wraz z obramowaniem i tekstem w oknie.
    void draw(sf::RenderWindow &window, FrameType fType);

    // Metody dostępowe (gettery) do składowych graficznych
    sf::Texture& getTexture() { return m_photoTexture; }
    sf::Sprite& getSprite() { return m_photoSprite; }
    sf::Text& getText() { return m_text; }
    sf::Font& getFont() { return m_font; }
    sf::RectangleShape& getShapeFrame() { return m_shapeFrame; }
    const std::optional<sf::Sprite>& getCustomFrameSprite() const { return m_customFrameSprite; }

private:
    sf::Texture m_photoTexture;   // Tekstura aktualnego obrazu.
    sf::Sprite m_photoSprite;     // Obiekt graficzny wyświetlający teksturę.
    sf::Text m_text;              // Tekst podpisu pod zdjęciem.
    sf::Font m_font;              // Czcionka używana do podpisu.

    sf::RectangleShape m_shapeFrame;            // Ramka.
    std::optional<sf::Sprite> m_customFrameSprite; // Opcjonalna ramka graficzna z pliku.

    SlotConfig m_config;          // Parametry konfiguracyjne tego slotu.
    bool m_hasImage = false;      // Czy w slocie znajduje się załadowany obraz.
    FrameType m_frameType = FrameType::Simple; // Aktualny typ ramki.
};