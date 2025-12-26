#include "App.h"
#include "ImageLoader.h"
#include <ctime>
#include <iostream>
#include <random>

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

        m_transitionTime += m_clock.restart().asSeconds();
        float halfDuration = m_transitionDuration / 2.0f;

        if (m_transitionTime >= m_transitionDuration) {
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
        // Alfa starego zdjęcia na 255
        sf::Color cOld = m_oldSprite->getColor();
        cOld.a = 255;
        m_oldSprite->setColor(cOld);
        m_oldSprite->setScale(m_slots[m_fadingSlotIndex].getSprite().getScale());
        m_oldSprite->setOrigin(m_slots[m_fadingSlotIndex].getSprite().getOrigin());
        m_oldSprite->setPosition(m_slots[m_fadingSlotIndex].getSprite().getPosition());
        m_oldSprite->setRotation(m_slots[m_fadingSlotIndex].getSprite().getRotation());

        // Stara ramka
        m_oldFrameIsCustom = m_slots[m_fadingSlotIndex].getCustomFrameSprite().has_value();
        if (m_oldFrameIsCustom)
            m_oldFrameSprite = *m_slots[m_fadingSlotIndex].getCustomFrameSprite();
        else
            m_oldFrameShape = m_slots[m_fadingSlotIndex].getShapeFrame();

        // Załaduj nowe zdjęcie
        if (!m_slots.empty() && !m_imagePaths.empty()) {
            m_slots[m_currentSlotIndex].loadImage(m_imagePaths[m_currentImageIndex], m_config.get().frameType);

            // Alfa nowego zdjęcia na 0 (żeby nie przykryło starego)
            sf::Sprite& newSpr = m_slots[m_currentSlotIndex].getSprite();
            sf::Color c = newSpr.getColor();
            c.a = 0;
            newSpr.setColor(c);


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
            float halfDuration = m_transitionDuration / 2.0f;
            float tOld = 0.0f;
            float tNew = 0.0f;

            if (m_transitionTime < halfDuration) {
                tOld = m_transitionTime / halfDuration;
                tNew = 0.0f;
            }
            else {
                tOld = 1.0f; // stary już zniknął
                tNew = (m_transitionTime - halfDuration) / halfDuration;
                if (tNew > 1.0f) tNew = 1.0f;
            }

            // Fade-out stare zdjęcie + ramka
            if (m_oldSprite.has_value()) {
                sf::Sprite& oldSpr = m_oldSprite.value();
                oldSpr.setColor(sf::Color(255, 255, 255, static_cast<uint8_t>(255 * (1.0f - tOld))));
                m_window.draw(oldSpr);

                // Ramka starego slotu
                if (m_oldFrameIsCustom && m_oldFrameSprite.has_value()) {
                    sf::Sprite& frame = m_oldFrameSprite.value();
                    frame.setColor(sf::Color(255, 255, 255, static_cast<uint8_t>(255 * (1.0f - tOld))));
                    m_window.draw(frame);
                }
                else if (m_oldFrameShape.has_value()) {
                    sf::RectangleShape& frame = m_oldFrameShape.value();
                    sf::Color c = frame.getFillColor();
                    c.a = static_cast<uint8_t>(255 * (1.0f - tOld));
                    frame.setFillColor(c);
                    m_window.draw(frame);
                }
            }

            // Fade-in nowe zdjęcie + ramka
            sf::Sprite& newSpr = m_slots[i].getSprite();
            newSpr.setColor(sf::Color(255, 255, 255, static_cast<uint8_t>(255 * tNew)));
            m_window.draw(newSpr);

            m_slots[i].setFrameAlpha(static_cast<uint8_t>(255 * tNew));

            // rysujemy ramkę na wierzchu z tym samym alpha
            if (m_slots[i].getCustomFrameSprite().has_value()) {
                const sf::Sprite& frame = *m_slots[i].getCustomFrameSprite();
                sf::Color fc = frame.getColor();
                fc.a = static_cast<uint8_t>(255 * tNew);
                sf::Sprite frameCopy = frame;
                frameCopy.setColor(fc);
                m_window.draw(frameCopy);
            }
            else {
                sf::RectangleShape& frame = m_slots[i].getShapeFrame();
                sf::Color fc = frame.getFillColor();
                fc.a = static_cast<uint8_t>(255 * tNew);
                frame.setFillColor(fc);
                m_window.draw(frame);
            }
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

    // Ramka customowa
    if (appCfg.frameType == FrameType::CustomFromFile) {
        if (m_customFrameTexture.loadFromFile(appCfg.frameFilePath)) {
            m_customFrameTexture.setSmooth(true);
        }
        else {
            std::cerr << "Nie udalo sie wczytac ramki z pliku: " << appCfg.frameFilePath << std::endl;
            appCfg.frameType = FrameType::Simple; // Powrót do prostej ramki
        }
    }

    // Tryb losowy
    if (appCfg.randomMode) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));

        int count = 5; // liczba losowych zdjęć na ekranie 
        m_slots.reserve(count); 

        for (int i = 0; i < count; i++) { 
            SlotConfig cfg;

            // losowa pozycja 
            cfg.x = std::rand() % m_window.getSize().x;
            cfg.y = std::rand() % m_window.getSize().y; 
           
            // losowy obrót 
            cfg.rotation = std::rand() % 360;

            m_slots.emplace_back(); 
            m_slots.back().setup(cfg, appCfg.frameType, appCfg.frameColor, 
                (appCfg.frameType == FrameType::CustomFromFile ? &m_customFrameTexture : nullptr)); 
        } 

        // Tasowanie kolejności zdjęć 
        std::shuffle(m_imagePaths.begin(), m_imagePaths.end(), std::mt19937{std::random_device{}()});
    } else {
        // Normalny tryb slotów zdefiniowany w config.txt
        m_slots.reserve(appCfg.slots.size());
        for (const auto& slotCfg : appCfg.slots) {
            m_slots.emplace_back();
            m_slots.back().setup(slotCfg, appCfg.frameType, appCfg.frameColor,
                (appCfg.frameType == FrameType::CustomFromFile ? &m_customFrameTexture : nullptr));
        }
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
