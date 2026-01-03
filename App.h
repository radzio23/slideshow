#pragma once

#include <SFML/Graphics.hpp>
#include "Config.h"
#include "PhotoSlot.h"
#include <vector>

// Główna klasa aplikacji zarządzająca oknem, zasobami i logiką wyświetlania.
class App {
public:
    App();
    // Uruchamia główną pętlę programu (zdarzenia, aktualizacja, rysowanie).
    void run();

private:
    // Obsługa wejścia użytkownika i zdarzeń systemowych.
    void processEvents();
    // Aktualizacja logiki, liczników czasu i animacji płynnych przejść.
    void update();
    // Renderowanie wszystkich elementów graficznych na ekranie.
    void render();

    // Wczytuje i ustawia tło aplikacji.
    void setupBackground();
    // Tworzy i rozmieszcza miejsca na zdjęcia zgodnie z konfiguracją.
    void setupSlots();

    sf::RenderWindow m_window; // Okno renderowania SFML.
    Config m_config;           // Przechowuje ustawienia wczytane z pliku.

    std::vector<std::filesystem::path> m_imagePaths; // Lista ścieżek do znalezionych obrazów.
    std::vector<std::unique_ptr<PhotoSlot>> m_slots; // Kontener na obiekty wyświetlające zdjęcia.

    // Zasoby graficzne
    sf::Texture m_bgTexture;         // Tekstura tła.
    sf::Sprite m_bgSprite;           // Sprite tła.
    sf::Texture m_customFrameTexture; // Tekstura dla niestandardowej ramki z pliku.
    bool m_hasBg = false;            // Flaga określająca, czy tło zostało załadowane.

    // Sterowanie czasem
    sf::Clock m_clock;           // Główny zegar do odmierzania przerw między zdjęciami.
    int m_currentImageIndex = 0; // Indeks następnego zdjęcia do wczytania.
    int m_currentSlotIndex = -1;  // Indeks slotu, który ma zostać odświeżony.

    // Zarządzanie animacją zanikania
    bool m_isTransition = false;       // Czy trwa właśnie animacja zmiany zdjęcia.
    float m_transitionTime = 0.0f;     // Aktualny czas trwania przejścia.
    float m_transitionDuration = 1.0f; // Całkowity czas trwania animacji.
    int m_fadingSlotIndex = -1;        // Numer slotu, w którym zachodzi zmiana.

};