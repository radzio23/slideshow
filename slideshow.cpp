#include <filesystem>
#include <iostream>
#include <string>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

int main() {
    //otwieranie okna w trybie pelnoekranowym
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Slideshow", sf::State::Fullscreen);

    std::optional<sf::Event> event;
    const sf::Event::KeyPressed* keyPressed;

    //pobieranie listy obrazów z folderu
    std::vector<std::filesystem::path> images;
    for (const auto& img : std::filesystem::directory_iterator("./images/")) {
        if (!img.is_regular_file()) continue;
        std::string ext = img.path().extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (ext == ".png" || ext == ".jpg" || ext == ".jpeg") images.push_back(img.path());
    }
    std::sort(images.begin(), images.end());

    sf::Texture texture(images.at(0));
    sf::Sprite sprite(texture);
    int i = 0;

    //dzialanie okna
    while (window.isOpen()) {
        while (event = window.pollEvent()) {
            //obs³uga klawiatury
            if (keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                switch (keyPressed->code) {
                    //zamykanie okna klawiszem ESC
                    case sf::Keyboard::Key::Escape: window.close(); break;
                }
            }
        }

        //wyœwietlanie zdjêæ
        if (i < images.size()) {
            if (texture.loadFromFile(images.at(i))) {
                window.clear();
                window.draw(sprite);
                window.display();
                sf::sleep(sf::seconds(1));
                i++;
            }
        }
    }

    return 0;
}