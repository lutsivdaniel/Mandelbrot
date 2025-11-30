#pragma once
#include <SFML/Graphics.hpp>
#include <complex>
#include <sstream>

const unsigned int MAX_ITER   = 64;
const float        BASE_WIDTH  = 4.0f;
const float        BASE_HEIGHT = 4.0f;
const float        BASE_ZOOM   = 0.5f;

class ComplexPlane : public sf::Drawable
{
public:
    enum class State { CALCULATING, DISPLAYING };

    ComplexPlane(int pixelWidth, int pixelHeight);

    void updateRender();

    void zoomIn();
    void zoomOut();

    void setCenter(sf::Vector2i mousePixel);
    void setMouseLocation(sf::Vector2i mousePixel);

    void loadText(sf::Text& text);

private:
    // override from sf::Drawable
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    std::size_t countIterations(sf::Vector2f coord);
    void iterationsToRGB(std::size_t count, sf::Uint8& r, sf::Uint8& g, sf::Uint8& b);
    sf::Vector2f mapPixelToCoords(sf::Vector2i mousePixel);

    sf::VertexArray m_vArray;       // holds one vertex per pixel
    sf::Vector2u    m_pixel_size;   // pixel width/height of the window
    float           m_aspectRatio;  // height / width

    sf::Vector2f    m_plane_center; // center of complex plane (real, imag)
    sf::Vector2f    m_plane_size;   // width/height of complex window in complex units

    sf::Vector2f    m_mouseLocation; // complex coords of current mouse position

    int             m_zoomCount;
    State           m_state;
};