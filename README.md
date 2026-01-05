# Slideshow Engine (SFML)

Interaktywna aplikacja do wyświetlania pokazu slajdów z dynamicznymi animacjami i systemem ramek.

## Główne Funkcje
* **System Ramek**: Obsługa typów `None`, `Simple`, `Polaroid` oraz `CustomFromFile`.
* **Tryb Polaroid**: Automatyczne wyświetlanie tekstu pod zdjęciem.
* **Animacje Przejść**: Płynne przenikanie (Fade-in/Fade-out) oraz efekt "spadania" w trybie losowym.
* **Dynamiczna Konfiguracja**: Wszystkie ustawienia ładowane z pliku `config.txt` bez konieczności rekompilacji.

## Jak używać?
1. Umieść zdjęcia w folderze `/images/`.
2. Skonfiguruj plik `config.txt` (ustaw `random_mode`, `interval`, `frame_type` oraz `sloty`).
3. Uruchom aplikację.

## Wymagania
* Kompilator C++17 lub nowszy.
* Biblioteka SFML.
