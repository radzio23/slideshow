#include "App.h"
#include "ImageLoader.h"
#include <iostream>

// Tworzenie okna fullscreen
App::App() : m_config("config.txt"), m_window(sf::VideoMode::getDesktopMode(), "Slideshow", sf::State::Fullscreen), m_bgSprite(m_bgTexture) {
    m_window.setFramerateLimit(60);
    m_window.setMouseCursorVisible(false);

    // Wczytanie listy zdjęć
    m_imagePaths = ImageLoader::scanDirectory("./images/");

    // Ustawienie tła
    setupBackground();

    // Utworzenie slotów
    setupSlots();
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
    // Jeśli trwa animacja fade
    if (m_isTransition) {

        m_transitionTime = m_clock.getElapsedTime().asSeconds();
        float t = m_transitionTime / m_transitionDuration;

        if (t >= 1.0f) {
            m_isTransition = false;
            m_fadingSlotIndex = -1;
        }

        return; // nie zmieniamy zdjęć w trakcie animacji
    }

    // Normalna zmiana zdjęcia
    if (m_clock.getElapsedTime().asSeconds() > m_config.get().interval) {
        // Start animacji
        m_fadingSlotIndex = m_currentSlotIndex;

        // Kopia starego zdjęcia
        m_oldTexture = m_slots[m_fadingSlotIndex].getTexture();
        m_oldSprite.emplace(m_oldTexture);
        m_oldSprite->setScale(m_slots[m_fadingSlotIndex].getSprite().getScale());
        m_oldSprite->setOrigin(m_slots[m_fadingSlotIndex].getSprite().getOrigin());
        m_oldSprite->setPosition(m_slots[m_fadingSlotIndex].getSprite().getPosition());
        m_oldSprite->setRotation(m_slots[m_fadingSlotIndex].getSprite().getRotation());

        // Załaduj nowe zdjęcie
        if (!m_slots.empty() && !m_imagePaths.empty()) {
            m_slots[m_currentSlotIndex].loadImage(m_imagePaths[m_currentImageIndex], m_config.get().frameType);

            // Indeksy
            m_currentImageIndex = (m_currentImageIndex + 1) % m_imagePaths.size();
            m_currentSlotIndex = (m_currentSlotIndex + 1) % m_slots.size();
        }

        // Start animacji
        m_isTransition = true;
        m_transitionTime = 0.0f;
        m_clock.restart();
    }
}

void App::render() {
    m_window.clear(sf::Color::Black);

    // Tło
    if (m_hasBg) {
        m_window.draw(m_bgSprite);
    }

    // Rysowanie slotów
    for (int i = 0; i < m_slots.size(); i++) {

        // Jeśli ten slot jest w trakcie przejścia
        if (m_isTransition && i == m_fadingSlotIndex) {

            float t = m_transitionTime / m_transitionDuration;
            if (t > 1.0f) t = 1.0f;

            // Fade-out starego zdjęcia
            float alphaOld = 255.0f * (1.0f - t);
            sf::Color cOld = m_oldSprite->getColor();
            cOld.a = static_cast<uint8_t>(alphaOld);
            m_oldSprite->setColor(cOld);
            m_window.draw(*m_oldSprite);

            // Fade-in nowego zdjęcia
            float alphaNew = 255.0f * t;
            sf::Color cNew = m_slots[i].getSprite().getColor();
            cNew.a = static_cast<uint8_t>(alphaNew);
            m_slots[i].getSprite().setColor(cNew);

            m_slots[i].draw(m_window, m_config.get().frameType);
        }
        else {
            // Normalne rysowanie slotu
            m_slots[i].draw(m_window, m_config.get().frameType);
        }
    }
    m_window.display();
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
