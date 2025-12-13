#include "Config.h"
#include <fstream>
#include <sstream>
#include <iostream>

Config::Config(const std::string &filename)
{
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open())
    {
        std::cerr << "Nie mozna otworzyc pliku konfiguracyjnego: " << filename << std::endl;
        // Ustawienie domyślne w razie błędu, żeby program działał
        m_appConfig.slots.push_back({500, 400, 400, 300, 0});
        return;
    }

    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue; // Ignoruj puste linie i komentarze
        std::stringstream ss(line);
        std::string key;
        ss >> key;

        if (key == "background")
        {
            ss >> m_appConfig.backgroundFile;
        }
        else if (key == "interval")
        {
            ss >> m_appConfig.interval;
        }
        else if (key == "frame_type")
        {
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
                m_appConfig.frameFilePath = type;
            }
        }
        else if (key == "frame_color")
        {
            int r, g, b;
            ss >> r >> g >> b;
            m_appConfig.frameColor = sf::Color(r, g, b);
        }
        else if (key == "slot")
        {
            SlotConfig sc;
            ss >> sc.x >> sc.y >> sc.width >> sc.height >> sc.rotation;
            m_appConfig.slots.push_back(sc);
        }
    }
}

const AppConfig &Config::get() const
{
    return m_appConfig;
}
