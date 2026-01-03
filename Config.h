#pragma once

#include "AppConfig.h"
#include <string>

// Klasa odpowiedzialna za wczytywanie i przetwarzanie pliku tekstowego z ustawieniami.
class Config
{
public:
    // Otwiera plik o podanej nazwie i parsuje jego zawartość.
    Config(const std::string& filename);
        
    // Daje dostęp do struktury przechowującej wszystkie parametry aplikacji.
    const AppConfig& get() const;

private:
    AppConfig m_appConfig; // Obiekt przechowujący aktualną konfigurację.
};