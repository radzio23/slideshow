#pragma once

// Definicja dostêpnych stylów ramek.
enum class FrameType
{
    None,           // Brak ramki, wyœwietlane jest samo zdjêcie.
    Simple,         // Prosta ramka wokó³ zdjêcia.
    Polaroid,       // Ramka z szerszym do³em na tekst.
    CustomFromFile  // Ramka nak³adana z zewnêtrznego pliku graficznego.
};