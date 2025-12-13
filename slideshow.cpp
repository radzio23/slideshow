#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <optional>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>

// --- KONFIGURACJA I STRUKTURY ---

enum class FrameType { None, Simple, Polaroid, CustomFromFile };

struct SlotConfig {
    float x, y;
    float width, height;
    float rotation;
};

struct AppConfig {
    std::string backgroundFile;
    FrameType frameType = FrameType::Simple;
    std::string frameFilePath;
    sf::Color frameColor = sf::Color::White;
    float interval = 2.0f;
    std::vector<SlotConfig> slots;
};

// Klasa reprezentująca jedno miejsce na zdjęcie
class PhotoSlot {
public:
    sf::Texture photoTexture;
    sf::Sprite photoSprite;

    sf::RectangleShape shapeFrame;
    // Sprite ramki jest tworzony tylko wtedy, gdy jest potrzebny, używając std::optional
   std::optional<sf::Sprite> customFrameSprite;

    SlotConfig config;
    bool hasImage = false;

    PhotoSlot() : photoSprite(photoTexture) {}

    PhotoSlot(const PhotoSlot&) = delete;
    PhotoSlot& operator=(const PhotoSlot&) = delete;
    PhotoSlot(PhotoSlot&&) = default;
    PhotoSlot& operator=(PhotoSlot&&) = default;

    void setup(const SlotConfig& cfg, FrameType fType, sf::Color fColor, const sf::Texture* customFrameTex) {
        config = cfg;

        if (fType == FrameType::Simple || fType == FrameType::Polaroid) {
            shapeFrame.setFillColor(fColor);
            shapeFrame.setRotation(sf::degrees(config.rotation));

            float padding = 20.0f;
            float bottomPadding = (fType == FrameType::Polaroid) ? 80.0f : 20.0f;

            shapeFrame.setSize({ config.width + padding, config.height + bottomPadding });
            shapeFrame.setOrigin({ shapeFrame.getSize().x / 2.0f, shapeFrame.getSize().y / 2.0f });
            shapeFrame.setPosition({ config.x, config.y });
        }
        else if (fType == FrameType::CustomFromFile && customFrameTex) {
            // Tworzymy sprite'a w tym miejscu, przekazując mu teksturę
            customFrameSprite.emplace(*customFrameTex);

            sf::Vector2u frameSize = customFrameTex->getSize();
            float targetW = config.width + 40.0f;
            float targetH = config.height + 40.0f;

            // Dostęp do utworzonego sprite'a przez operator -> lub *
            customFrameSprite->setScale({ targetW / frameSize.x, targetH / frameSize.y });
            customFrameSprite->setOrigin({ (float)frameSize.x / 2.0f, (float)frameSize.y / 2.0f });
            customFrameSprite->setPosition({ config.x, config.y });
            customFrameSprite->setRotation(sf::degrees(config.rotation));
        }
    }

    bool loadImage(const std::filesystem::path& path, FrameType fType) {
        if (!photoTexture.loadFromFile(path.string())) return false;

        photoTexture.setSmooth(true);
        photoSprite.setTexture(photoTexture, true);

        sf::Vector2u texSize = photoTexture.getSize();

        float scaleX = config.width / (float)texSize.x;
        float scaleY = config.height / (float)texSize.y;
        float finalScale = std::min(scaleX, scaleY);

        photoSprite.setScale({ finalScale, finalScale });
        photoSprite.setOrigin({ (float)texSize.x / 2.0f, (float)texSize.y / 2.0f });

        float yOffset = (fType == FrameType::Polaroid) ? -30.0f : 0.0f;

        float rad = config.rotation * 3.14159f / 180.0f;
        float finalX = config.x + yOffset * std::sin(rad);
        float finalY = config.y + yOffset * std::cos(rad);

        photoSprite.setPosition({ finalX, finalY });
        photoSprite.setRotation(sf::degrees(config.rotation));

        hasImage = true;
        return true;
    }

    void draw(sf::RenderWindow& window, FrameType fType) {
        if (!hasImage) return;

        if (fType == FrameType::Simple || fType == FrameType::Polaroid) {
            window.draw(shapeFrame);
            window.draw(photoSprite);
        }
        else if (fType == FrameType::CustomFromFile) {
            window.draw(photoSprite);
            // Rysujemy sprite'a ramki tylko jeśli został utworzony
            if (customFrameSprite) {
                window.draw(*customFrameSprite);
            }
        }
        else {
            window.draw(photoSprite);
        }
    }
};

// --- WCZYTYWANIE KONFIGURACJI ---

AppConfig loadConfig(const std::string& filename) {
    AppConfig config;
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Nie mozna otworzyc pliku konfiguracyjnego: " << filename << std::endl;
        config.slots.push_back({ 500, 400, 400, 300, 0 });
        return config;
    }

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string key;
        ss >> key;

        if (key == "background") {
            ss >> config.backgroundFile;
        }
        else if (key == "interval") {
            ss >> config.interval;
        }
        else if (key == "frame_type") {
            std::string type;
            ss >> type;
            if (type == "polaroid") config.frameType = FrameType::Polaroid;
            else if (type == "simple") config.frameType = FrameType::Simple;
            else if (type == "none") config.frameType = FrameType::None;
            else {
                config.frameType = FrameType::CustomFromFile;
                config.frameFilePath = type;
            }
        }
        else if (key == "frame_color") {
            int r, g, b;
            ss >> r >> g >> b;
            config.frameColor = sf::Color(r, g, b);
        }
        else if (key == "slot") {
            SlotConfig sc;
            ss >> sc.x >> sc.y >> sc.width >> sc.height >> sc.rotation;
            config.slots.push_back(sc);
        }
    }
    return config;
}

// --- MAIN ---

int main() {
    AppConfig config = loadConfig("config.txt");

    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Slideshow", sf::State::Fullscreen);
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(false);

    sf::Texture bgTexture;
    sf::Sprite bgSprite(bgTexture);
    bool hasBg = false;
    if (!config.backgroundFile.empty() && bgTexture.loadFromFile(config.backgroundFile)) {
        hasBg = true;
        bgTexture.setSmooth(true);
        bgSprite.setTexture(bgTexture, true);
        sf::Vector2u winSize = window.getSize();
        sf::Vector2u bgSize = bgTexture.getSize();
        bgSprite.setScale({ (float)winSize.x / bgSize.x, (float)winSize.y / bgSize.y });
    }

    sf::Texture customFrameTexture;
    if (config.frameType == FrameType::CustomFromFile) {
        if (customFrameTexture.loadFromFile(config.frameFilePath)) {
            customFrameTexture.setSmooth(true);
        }
        else {
            std::cerr << "Nie udalo sie wczytac ramki z pliku: " << config.frameFilePath << std::endl;
            config.frameType = FrameType::Simple;
        }
    }

    std::vector<std::filesystem::path> imagePaths;
    try {
        for (const auto& img : std::filesystem::directory_iterator("./images/")) {
            if (!img.is_regular_file()) continue;
            std::string ext = img.path().extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            if (ext == ".png" || ext == ".jpg" || ext == ".jpeg") {
                imagePaths.push_back(img.path());
            }
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Blad odczytu folderu images: " << e.what() << std::endl;
    }

    if (!imagePaths.empty()) std::sort(imagePaths.begin(), imagePaths.end());

    std::vector<PhotoSlot> activeSlots;
    activeSlots.reserve(config.slots.size());

    for (const auto& sc : config.slots) {
        activeSlots.emplace_back();
        activeSlots.back().setup(sc, config.frameType, config.frameColor,
            (config.frameType == FrameType::CustomFromFile ? &customFrameTexture : nullptr));
    }

    int currentImageIndex = 0;
    if (!imagePaths.empty()) {
        for (auto& slot : activeSlots) {
            if (currentImageIndex < imagePaths.size()) {
                slot.loadImage(imagePaths[currentImageIndex], config.frameType);
                currentImageIndex = (currentImageIndex + 1) % imagePaths.size();
            }
        }
    }

    sf::Clock clock;
    int currentSlotIndex = 0;

    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape) {
                    window.close();
                }
            }
        }

        if (clock.getElapsedTime().asSeconds() > config.interval) {
            if (!activeSlots.empty() && !imagePaths.empty()) {
                activeSlots[currentSlotIndex].loadImage(imagePaths[currentImageIndex], config.frameType);
                currentImageIndex = (currentImageIndex + 1) % imagePaths.size();
                currentSlotIndex = (currentSlotIndex + 1) % activeSlots.size();
            }
            clock.restart();
        }

        window.clear(sf::Color::Black);

        if (hasBg) window.draw(bgSprite);

        for (auto& slot : activeSlots) {
            slot.draw(window, config.frameType);
        }

        window.display();
    }

    return 0;
}
