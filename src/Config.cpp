#include "../include/Config.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Konstruktor - otwarcie pliku i wypełnianie struktury AppConfig.
Config::Config(const std::string& filename)
{
    std::ifstream file(filename);
    std::string line;

    // Sprawdzenie, czy plik istnieje
    if (!file.is_open())
    {
        std::cerr << "Blad: Nie mozna otworzyc pliku konfiguracyjnego: " << filename << std::endl;
        // Domyślny slot
        m_appConfig.slots.push_back({ 500.0f, 400.0f, 400.0f, 300.0f, 0.0f, "Blad wczytywania" });
        return;
    }

    // Przetwarzanie pliku
    while (std::getline(file, line))
    {
        // Ignorowanie pustych linii oraz komentarzy
        if (line.empty() || line[0] == '#')
            continue;

        std::stringstream ss(line);
        std::string key;
        ss >> key;

        // USTAWIENIA

        if (key == "background")
        {
            // Ścieżka do pliku tła
            std::string temp;
            ss >> temp;
            m_appConfig.backgroundFile = "./assets/images/backgrounds/" + temp;
        }
        else if (key == "interval")
        {
            // Czas wyświetlania jednego zdjęcia (w sekundach)
            ss >> m_appConfig.interval;
        }
        else if (key == "random_mode")
        {
            // Przełącznik trybu (true = losowe pozycje, false = stałe sloty)
            std::string value;
            ss >> value;
            m_appConfig.randomMode = (value == "true");
        }
        else if (key == "frame_type")
        {
            // Określenie typu ramki (polaroid, simple, none lub custom)
            std::string type;
            ss >> type;
            if (type == "polaroid")
                m_appConfig.frameType = FrameType::Polaroid;
            else if (type == "simple")
                m_appConfig.frameType = FrameType::Simple;
            else if (type == "none")
                m_appConfig.frameType = FrameType::None;
            else
            {
                m_appConfig.frameType = FrameType::CustomFromFile;
                m_appConfig.frameFilePath = "./assets/images/frames/" + type;
            }
        }
        else if (key == "frame_color")
        {
            // Kolor ramki w formacie RGB
            int r, g, b;
            if (ss >> r >> g >> b) {
                m_appConfig.frameColor = sf::Color(r, g, b);
            }
        }

        // definicja slotów (tryb statyczny)

        else if (key == "slot")
        {
            if (m_appConfig.randomMode)
                continue;

            SlotConfig sc;
            // Odczyt parametrów: X Y Szerokość Wysokość Rotacja
            if (ss >> sc.x >> sc.y >> sc.width >> sc.height >> sc.rotation) {
                // Tekst podpisu
                std::getline(ss, sc.text);
                if (!sc.text.empty() && (sc.text[0] == ' ' || sc.text[0] == '\t')) {
                    sc.text.erase(0, 1);
                }

                m_appConfig.slots.push_back(sc);
            }
        }
    }

    file.close();
}

// Pozwala innym klasom (np. App) czytać ustawienia.
const AppConfig& Config::get() const
{
    return m_appConfig;
}