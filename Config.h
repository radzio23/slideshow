#pragma once

#include "AppConfig.h"
#include <string>

// Klasa odpowiedzialna za parsowanie pliku konfiguracyjnego.
class Config
{
public:
    // W konstruktorze wczytuje i parsuje plik.
    Config(const std::string &filename);

    // Zwraca stałą referencję do wczytanej konfiguracji.
    const AppConfig &get() const;

private:
    AppConfig m_appConfig;
};
