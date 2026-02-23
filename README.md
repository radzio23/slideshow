# 📸 Slideshow Engine (SFML)
An interactive C++ application designed for dynamic image presentations with custom framing systems and smooth transition animations.

## 🚀 Key Features

* **Advanced Framing System**: Support for multiple frame styles:
    * `None` (Clean look)
    * `Simple` (Classic border)
    * `Polaroid` (Automatic captioning under the photo)
    * `CustomFromFile` (User-defined textures)
* **Fluid Animations**: Features smooth Fade-in/Fade-out transitions and a unique "falling" effect in random mode.
* **Dynamic Configuration**: All settings are loaded from `config.txt` at runtime—no recompilation needed.
* **Asset Management**: Centralized loading for fonts and textures.

## 📂 Project Structure

* `src/` – Core logic and implementation (`.cpp`).
* `include/` – Header files and class definitions (`.h`).
* `assets/` – Images, fonts.
* `docs/` – Technical documentation.

## 🛠️ Requirements & Setup

### Prerequisites
* **C++17** or newer.
* **SFML Library** (Simple and Fast Multimedia Library).

### Quick Start
1.  Place your images in the `assets/images/` folder.
2.  Configure `config.txt`.
3.  Open `slideshow.sln` in Visual Studio and run application.
