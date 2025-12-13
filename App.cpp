#include "App.h"
#include "ImageLoader.h"
#include <iostream>

// --- NAPRAWA BŁĘDU C2512 ---
// Dodajemy m_bgSprite(m_bgTexture) do listy inicjalizacyjnej konstruktora.
App::App()
    : m_config("config.txt"),
    m_window(sf::VideoMode::getDesktopMode(), "Slideshow", sf::State::Fullscreen),
    m_bgSprite(m_bgTexture) // <--- TA LINIA NAPRAWIA BŁĄD
{
    m_window.setFramerateLimit(60);
    m_window.setMouseCursorVisible(false);

    m_imagePaths = ImageLoader::scanDirectory("./images/");

    setupBackground();
    setupSlots();
}

void App::setupBackground() {
    const auto& appCfg = m_config.get();
    if (!appCfg.backgroundFile.empty() && m_bgTexture.loadFromFile(appCfg.backgroundFile)) {
        m_hasBg = true;
        m_bgTexture.setSmooth(true);
        // Upewniamy się, że sprite korzysta z załadowanej tekstury
        m_bgSprite.setTexture(m_bgTexture, true);

        sf::Vector2u winSize = m_window.getSize();
        sf::Vector2u bgSize = m_bgTexture.getSize();
        m_bgSprite.setScale({ (float)winSize.x / bgSize.x, (float)winSize.y / bgSize.y });
    }
}

void App::setupSlots() {
    auto appCfg = m_config.get(); // Kopia, by móc modyfikować
    if (appCfg.frameType == FrameType::CustomFromFile) {
        if (m_customFrameTexture.loadFromFile(appCfg.frameFilePath)) {
            m_customFrameTexture.setSmooth(true);
        }
        else {
            std::cerr << "Nie udalo sie wczytac ramki z pliku: " << appCfg.frameFilePath << std::endl;
            appCfg.frameType = FrameType::Simple; // Wróć do prostej ramki
        }
    }

    m_slots.reserve(appCfg.slots.size());
    for (const auto& slotCfg : appCfg.slots) {
        m_slots.emplace_back();
        m_slots.back().setup(slotCfg, appCfg.frameType, appCfg.frameColor,
            (appCfg.frameType == FrameType::CustomFromFile ? &m_customFrameTexture : nullptr));
    }

    // Wstępne wypełnienie
    if (!m_imagePaths.empty()) {
        for (auto& slot : m_slots) {
            slot.loadImage(m_imagePaths[m_currentImageIndex], appCfg.frameType);
            m_currentImageIndex = (m_currentImageIndex + 1) % m_imagePaths.size();
        }
    }
    else {
        std::cerr << "Brak zdjec w folderze ./images/!" << std::endl;
    }
}

void App::run() {
    while (m_window.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void App::processEvents() {
    while (const std::optional<sf::Event> event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
        }
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                m_window.close();
            }
        }
    }
}

void App::update() {
    if (m_clock.getElapsedTime().asSeconds() > m_config.get().interval) {
        if (!m_slots.empty() && !m_imagePaths.empty()) {
            m_slots[m_currentSlotIndex].loadImage(m_imagePaths[m_currentImageIndex], m_config.get().frameType);

            m_currentImageIndex = (m_currentImageIndex + 1) % m_imagePaths.size();
            m_currentSlotIndex = (m_currentSlotIndex + 1) % m_slots.size();
        }
        m_clock.restart();
    }
}

void App::render() {
    m_window.clear(sf::Color::Black);
    if (m_hasBg) {
        m_window.draw(m_bgSprite);
    }
    for (auto& slot : m_slots) {
        slot.draw(m_window, m_config.get().frameType);
    }
    m_window.display();
}
