#pragma once

// Struktura przechowująca konfigurację pojedynczego "miejsca" na zdjęcie.
struct SlotConfig
{
    float x = 0.0f;
    float y = 0.0f;
    float width = 400.0f;
    float height = 300.0f;
    float rotation = 0.0f;
    std::string text = "";
};
