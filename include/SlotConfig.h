#pragma once

// Parametry określające wygląd i położenie konkretnego miejsca na zdjęcie.
struct SlotConfig
{
    float x = 0.0f;        // Pozycja pozioma środka slotu.
    float y = 0.0f;        // Pozycja pionowa środka slotu.
    float width = 400.0f;  // Szerokość obszaru na zdjęcie.
    float height = 300.0f; // Wysokość obszaru na zdjęcie.
    float rotation = 0.0f; // Kąt obrotu zdjęcia wyrażony w stopniach.
    std::string text = ""; // Napis wyświetlany pod zdjęciem.
};