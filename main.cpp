#include <SFML/Graphics.hpp>
#include "ComplexPlane.h"
#include <iostream>

using namespace sf;
using namespace std;

int main()
{
    // Get desktop resolution
    VideoMode desktop = VideoMode::getDesktopMode();
    unsigned int width  = desktop.width  / 2;
    unsigned int height = desktop.height / 2;

    RenderWindow window(VideoMode(width, height), "Mandelbrot - ComplexPlane");
    window.setFramerateLimit(60);

    ComplexPlane cplane(static_cast<int>(width),
                        static_cast<int>(height));

    // Load font
    Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
        cerr << "Error: could not load font. Put arial.ttf next to the executable.\n";
        return 1;
    }

    Text infoText;
    infoText.setFont(font);
    infoText.setCharacterSize(16);
    infoText.setFillColor(Color::White);
    infoText.setPosition(10.0f, 10.0f);

    while (window.isOpen())
    {
        // Handle input
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            else if (event.type == Event::MouseButtonPressed)
            {
                Vector2i mousePixel(event.mouseButton.x, event.mouseButton.y);

                if (event.mouseButton.button == Mouse::Left)
                {
                    // Left click: zoom in + move center
                    cplane.setCenter(mousePixel);
                    cplane.zoomIn();
                }
                else if (event.mouseButton.button == Mouse::Right)
                {
                    // Right click: zoom out + move center
                    cplane.setCenter(mousePixel);
                    cplane.zoomOut();
                }
            }
            else if (event.type == Event::MouseMoved)
            {
                Vector2i mousePixel(event.mouseMove.x, event.mouseMove.y);
                cplane.setMouseLocation(mousePixel);
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Escape))
        {
            window.close();
        }

        // Update
        cplane.updateRender();
        cplane.loadText(infoText);

        // Draw
        window.clear(Color::Black);
        window.draw(cplane);
        window.draw(infoText);
        window.display();
    }

    return 0;
}