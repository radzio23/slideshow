#include "PhotoSlot.h"
#include <cmath>

PhotoSlot::PhotoSlot() : m_photoSprite(m_photoTexture) {}

void PhotoSlot::setup(const SlotConfig &cfg, FrameType fType, sf::Color fColor, const sf::Texture *customFrameTex)
{
    m_config = cfg;

    if (fType == FrameType::Simple || fType == FrameType::Polaroid)
    {
        m_shapeFrame.setFillColor(fColor);
        m_shapeFrame.setRotation(sf::degrees(m_config.rotation));

        float padding = 20.0f;
        float bottomPadding = (fType == FrameType::Polaroid) ? 80.0f : 20.0f;

        m_shapeFrame.setSize({m_config.width + padding, m_config.height + bottomPadding});
        m_shapeFrame.setOrigin({m_shapeFrame.getSize().x / 2.0f, m_shapeFrame.getSize().y / 2.0f});
        m_shapeFrame.setPosition({m_config.x, m_config.y});
    }
    else if (fType == FrameType::CustomFromFile && customFrameTex)
    {
        m_customFrameSprite.emplace(*customFrameTex);

        sf::Vector2u frameSize = customFrameTex->getSize();
        float targetW = m_config.width + 40.0f;
        float targetH = m_config.height + 40.0f;

        m_customFrameSprite->setScale({targetW / frameSize.x, targetH / frameSize.y});
        m_customFrameSprite->setOrigin({(float)frameSize.x / 2.0f, (float)frameSize.y / 2.0f});
        m_customFrameSprite->setPosition({m_config.x, m_config.y});
        m_customFrameSprite->setRotation(sf::degrees(m_config.rotation));
    }
}

bool PhotoSlot::loadImage(const std::filesystem::path &path, FrameType fType)
{
    if (!m_photoTexture.loadFromFile(path.string()))
        return false;

    m_photoTexture.setSmooth(true);
    m_photoSprite.setTexture(m_photoTexture, true);

    sf::Vector2u texSize = m_photoTexture.getSize();

    float scaleX = m_config.width / (float)texSize.x;
    float scaleY = m_config.height / (float)texSize.y;
    float finalScale = std::min(scaleX, scaleY);

    m_photoSprite.setScale({finalScale, finalScale});
    m_photoSprite.setOrigin({(float)texSize.x / 2.0f, (float)texSize.y / 2.0f});

    float yOffset = (fType == FrameType::Polaroid) ? 30.0f : 0.0f;

    float rad = m_config.rotation * 3.14159f / 180.0f;
    float finalX = m_config.x + yOffset * std::sin(rad);
    float finalY = m_config.y - yOffset * std::cos(rad);

    m_photoSprite.setPosition({finalX, finalY});
    m_photoSprite.setRotation(sf::degrees(m_config.rotation));

    m_hasImage = true;
    return true;
}

void PhotoSlot::draw(sf::RenderWindow &window, FrameType fType)
{
    if (!m_hasImage)
        return;

    if (fType == FrameType::Simple || fType == FrameType::Polaroid)
    {
        window.draw(m_shapeFrame);
        window.draw(m_photoSprite);
    }
    else if (fType == FrameType::CustomFromFile)
    {
        if (m_customFrameSprite)
        {
            window.draw(*m_customFrameSprite);
        }
        window.draw(m_photoSprite);
    }
    else
    {
        window.draw(m_photoSprite);
    }
}
