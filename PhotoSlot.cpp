#include "PhotoSlot.h"
#include <cmath>

#define M_PI 3.14159265

PhotoSlot::PhotoSlot() : m_photoSprite(m_photoTexture), m_text(m_font) {}

void PhotoSlot::setup(const SlotConfig& cfg, FrameType fType, sf::Color fColor, const sf::Texture* customFrameTex)
{
    m_config = cfg; // Konfiguracja

    // Konfiguracja ramki Simple/Polaroid
    if (fType == FrameType::Simple || fType == FrameType::Polaroid)
    {
        m_shapeFrame.setFillColor(fColor);
        m_shapeFrame.setRotation(sf::degrees(m_config.rotation));
        float padding = 20.0f;
        float bottomPadding = (fType == FrameType::Polaroid) ? 80.0f : 20.0f;
        m_shapeFrame.setSize({ m_config.width + padding, m_config.height + bottomPadding });
        m_shapeFrame.setOrigin(m_shapeFrame.getGeometricCenter());
        m_shapeFrame.setPosition({ m_config.x, m_config.y });
    }
    // Konfiguracja ramki z pliku graficznego
    else if (fType == FrameType::CustomFromFile && customFrameTex)
    {
        m_customFrameSprite.emplace(*customFrameTex);
        sf::Vector2u frameSize = customFrameTex->getSize();
        float targetW = m_config.width + 40.0f;
        float targetH = m_config.height + 40.0f;
        m_customFrameSprite->setScale({ targetW / frameSize.x, targetH / frameSize.y });
        m_customFrameSprite->setOrigin({ frameSize.x / 2.0f, frameSize.y / 2.0f });
        m_customFrameSprite->setPosition({ m_config.x, m_config.y });
        m_customFrameSprite->setRotation(sf::degrees(m_config.rotation));
    }

    // Ustawienia tekstu pod zdjêciem
    if (m_font.openFromFile("arial.ttf")) {
        m_text.setFont(m_font);
        m_text.setString(m_config.text);
        m_text.setCharacterSize(18);
        m_text.setFillColor(sf::Color::Black);
        m_text.setOrigin(m_text.getLocalBounds().getCenter());
        m_text.setRotation(sf::degrees(m_config.rotation));
        float rad = m_config.rotation * M_PI / 180.0f;
        m_text.setPosition(sf::Vector2f(
            m_config.x - sin(rad) * m_config.height / 2.0,
            m_config.y + cos(rad) * m_config.height / 2.0
        ));
    }
}

bool PhotoSlot::loadImage(const std::filesystem::path& path, FrameType fType)
{
    if (!m_photoTexture.loadFromFile(path.string()))
        return false;

    m_photoTexture.setSmooth(true);
    m_photoSprite.setTexture(m_photoTexture, true);

    sf::Vector2u texSize = m_photoTexture.getSize();

    // Obliczanie skali
    float scaleX = m_config.width / (float)texSize.x;
    float scaleY = m_config.height / (float)texSize.y;
    float finalScale = std::min(scaleX, scaleY);

    m_photoSprite.setScale({ finalScale, finalScale });
    m_photoSprite.setOrigin({ (float)texSize.x / 2.0f, (float)texSize.y / 2.0f });

    // Przesuniêcie zdjêcia
    float yOffset = (fType == FrameType::Polaroid) ? 30.0f : 0.0f;
    float rad = m_config.rotation * 3.14159f / 180.0f;
    float finalX = m_config.x + yOffset * std::sin(rad);
    float finalY = m_config.y - yOffset * std::cos(rad);

    m_photoSprite.setPosition({ finalX, finalY });
    m_photoSprite.setRotation(sf::degrees(m_config.rotation));

    m_hasImage = true;
    return true;
}

void PhotoSlot::draw(sf::RenderWindow& window, FrameType fType)
{
    if (!m_hasImage) return;

    // Rysowanie kolejno: ramka, zdjêcie, tekst
    if (fType == FrameType::Simple || fType == FrameType::Polaroid)
        window.draw(m_shapeFrame);
    else if (fType == FrameType::CustomFromFile && m_customFrameSprite)
        window.draw(*m_customFrameSprite);

    window.draw(m_photoSprite);

    // Wyœwietlanie tekstu TYLKO w trybie Polaroid
    if (!m_config.text.empty() && fType == FrameType::Polaroid) {
        window.draw(m_text);
    }
}