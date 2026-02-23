#include "../include/App.h"
#include "../include/ImageLoader.h"
#include <ctime>
#include <iostream>
#include <random>
#include <algorithm>

// Inicjalizuje okno w trybie pełnoekranowym, wczytuje ścieżki do zdjęć oraz ustawia tło i sloty.
App::App() : m_config("config.txt"), m_window(sf::VideoMode::getDesktopMode(), "Slideshow", sf::State::Fullscreen), m_bgSprite(m_bgTexture) {
    m_window.setFramerateLimit(60);
    m_window.setMouseCursorVisible(false);

    m_imagePaths = ImageLoader::scanDirectory("./assets/images/");
    setupBackground();
    setupSlots();
}

// Główna pętla programu.
void App::run() {
    while (m_window.isOpen()) {
        processEvents(); // Obsługa zdarzeń
        update();        // Logika i animacje
        render();        // Rysowanie klatek
    }
}

//Obsługa zdarzeń systemowych.
void App::processEvents() {
    while (const std::optional<sf::Event> event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
        }
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            // Zamknięcie aplikacji po naciśnięciu ESC
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                m_window.close();
            }
        }
    }
}

// Wczytuje teksturę tła z konfiguracji i skaluje ją do rozmiaru ekranu.
void App::setupBackground() {
    const auto& appCfg = m_config.get();
    if (!appCfg.backgroundFile.empty() && m_bgTexture.loadFromFile(appCfg.backgroundFile)) {
        m_hasBg = true;
        m_bgTexture.setSmooth(true);
        m_bgSprite.setTexture(m_bgTexture, true);

        sf::Vector2u winSize = m_window.getSize();
        sf::Vector2u bgSize = m_bgTexture.getSize();
        m_bgSprite.setScale({ (float)winSize.x / bgSize.x, (float)winSize.y / bgSize.y });
    }
}

// Inicjalizuje sloty na zdjęcia na podstawie pliku config.txt lub przygotowuje tryb losowy.
void App::setupSlots() {
    const auto& appCfg = m_config.get();

    if (appCfg.frameType == FrameType::CustomFromFile) {
        if (m_customFrameTexture.loadFromFile(appCfg.frameFilePath)) {
            m_customFrameTexture.setSmooth(true);
        }
    }

    m_slots.clear();

    if (appCfg.randomMode) {
        // W trybie losowym zdjęcia będą tworzone dynamicznie w funkcji update()
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        std::shuffle(m_imagePaths.begin(), m_imagePaths.end(), std::mt19937{ std::random_device{}() });
    }
    else {
        // W trybie statycznym tworzymy sloty zgodnie z definicją w config.txt
        for (const auto& slotCfg : appCfg.slots) {
            auto newSlot = std::make_unique<PhotoSlot>();
            newSlot->setup(slotCfg, appCfg.frameType, appCfg.frameColor,
                (appCfg.frameType == FrameType::CustomFromFile ? &m_customFrameTexture : nullptr));
            if (!m_imagePaths.empty()) {
                newSlot->loadImage(m_imagePaths[m_currentImageIndex], appCfg.frameType);
                m_currentImageIndex = (m_currentImageIndex + 1) % m_imagePaths.size();
            }
            m_slots.push_back(std::move(newSlot));
        }
    }
}

// Aktualizacja logiki aplikacji.
void App::update() {
    const auto& appCfg = m_config.get();

    if (m_isTransition) {
        float dt = m_clock.restart().asSeconds();
        float oldTime = m_transitionTime;
        m_transitionTime += dt;

        // Podmiana obrazka w połowie czasu animacji (tylko w trybie statycznym)
        if (!appCfg.randomMode) {
            if (oldTime < m_transitionDuration / 2.0f && m_transitionTime >= m_transitionDuration / 2.0f) {
                if (!m_slots.empty()) {
                    m_slots[m_currentSlotIndex]->loadImage(m_imagePaths[m_currentImageIndex], appCfg.frameType);
                    m_currentImageIndex = (m_currentImageIndex + 1) % m_imagePaths.size();
                }
            }
        }

        // Zakończenie animacji po upływie czasu trwania
        if (m_transitionTime >= m_transitionDuration) {
            m_isTransition = false;
        }
        return;
    }

    // Sprawdzenie czy minął interwał czasu wyświetlania zdjęcia
    if (m_clock.getElapsedTime().asSeconds() > appCfg.interval) {
        if (m_imagePaths.empty()) return;

        if (appCfg.randomMode) {
            // Tworzenie nowego zdjęcia w losowym punkcie ekranu
            auto newSlot = std::make_unique<PhotoSlot>();
            SlotConfig cfg;
            float margin = 250.0f;
            cfg.x = static_cast<float>(std::rand() % (int)(m_window.getSize().x - margin * 2) + margin);
            cfg.y = static_cast<float>(std::rand() % (int)(m_window.getSize().y - margin * 2) + margin);
            cfg.rotation = static_cast<float>(std::rand() % 60 - 30);

            newSlot->setup(cfg, appCfg.frameType, appCfg.frameColor,
                (appCfg.frameType == FrameType::CustomFromFile ? &m_customFrameTexture : nullptr));

            newSlot->loadImage(m_imagePaths[m_currentImageIndex], appCfg.frameType);
            m_slots.push_back(std::move(newSlot));

            // Maksymalnie tyle zdjęć na ekranie ile w folderze ze zdjęciami
            if (m_slots.size() > m_imagePaths.size()) m_slots.erase(m_slots.begin());
            m_currentImageIndex = (m_currentImageIndex + 1) % m_imagePaths.size();
        }
        else {
            // Przejście do następnego slotu w trybie statycznym
            if (!m_slots.empty()) {
                m_currentSlotIndex = (m_currentSlotIndex + 1) % m_slots.size();
            }
        }

        // Rozpoczęcie nowej animacji przejścia
        m_isTransition = true;
        m_transitionTime = 0.0f;
        m_clock.restart();
    }
}

// Renderowanie wszystkich elementów graficznych.
void App::render() {
    m_window.clear(sf::Color::Black);
    if (m_hasBg) m_window.draw(m_bgSprite);

    const auto& appCfg = m_config.get();

    for (size_t i = 0; i < m_slots.size(); i++) {
        bool isCurrentFading = m_isTransition && (i == (appCfg.randomMode ? m_slots.size() - 1 : m_currentSlotIndex));

        if (isCurrentFading) {
            float t_total = std::min(1.0f, m_transitionTime / m_transitionDuration);
            float half = m_transitionDuration / 2.0f;
            uint8_t alpha = 0;
            float currentDropScale = 1.0f;

            if (appCfg.randomMode) {
                // Animacja spadania zdjęcia
                alpha = static_cast<uint8_t>(255 * t_total);
                currentDropScale = 2.0f - t_total;
            }
            else {
                // fade-out/fade-in
                if (m_transitionTime < half) {
                    alpha = static_cast<uint8_t>(255 * (1.0f - (m_transitionTime / half)));
                }
                else {
                    alpha = static_cast<uint8_t>(255 * ((m_transitionTime - half) / half));
                }
            }

            auto& slot = *m_slots[i];

            // Rysowanie ramki
            if (appCfg.frameType == FrameType::CustomFromFile) {
                auto frameOpt = slot.getCustomFrameSprite();
                if (frameOpt.has_value()) {
                    sf::Sprite frame = *frameOpt;
                    frame.setColor(sf::Color(255, 255, 255, alpha));
                    frame.setScale(frame.getScale() * currentDropScale);
                    m_window.draw(frame);
                }
            }
            else if (appCfg.frameType != FrameType::None) {
                sf::RectangleShape frame = slot.getShapeFrame();
                sf::Color fc = frame.getFillColor();
                fc.a = alpha;
                frame.setFillColor(fc);
                frame.setScale(frame.getScale() * currentDropScale);
                m_window.draw(frame);
            }

            // Rysowanie zdjęcia
            sf::Sprite& spr = slot.getSprite();
            sf::Vector2f sprScale = spr.getScale();
            spr.setColor(sf::Color(255, 255, 255, alpha));
            spr.setScale(sprScale * currentDropScale);
            m_window.draw(spr);
            spr.setScale(sprScale);

            // Rysowanie tekstu (ramka polaroid)
            if (appCfg.frameType == FrameType::Polaroid) {
                sf::Text& txt = slot.getText();
                sf::Color tc = txt.getFillColor();
                tc.a = alpha;
                txt.setFillColor(tc);
                m_window.draw(txt);
            }
        }
        else {
            // Standardowe rysowanie
            m_slots[i]->draw(m_window, appCfg.frameType);
        }
    }
    m_window.display();
}