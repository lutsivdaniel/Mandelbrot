#include "ComplexPlane.h"
#include <cmath>

using namespace sf;
using namespace std;

ComplexPlane::ComplexPlane(int pixelWidth, int pixelHeight)
    : m_vArray(sf::Points)
{
    m_pixel_size = { static_cast<unsigned int>(pixelWidth),
                     static_cast<unsigned int>(pixelHeight) };

    // aspect ratio = height / width
    m_aspectRatio = static_cast<float>(pixelHeight) /
                    static_cast<float>(pixelWidth);

    // Start centered
    m_plane_center = { 0.0f, 0.0f };

    // Base window size
    m_plane_size = { BASE_WIDTH, BASE_HEIGHT * m_aspectRatio };

    m_mouseLocation = { 0.0f, 0.0f };

    m_zoomCount = 0;
    m_state = State::CALCULATING;

    // Vertex array, 1 vertex per pixel
    m_vArray.setPrimitiveType(Points);
    m_vArray.resize(static_cast<std::size_t>(pixelWidth) *
                    static_cast<std::size_t>(pixelHeight));
}

void ComplexPlane::draw(RenderTarget& target, RenderStates states) const
{
    target.draw(m_vArray, states);
}

void ComplexPlane::updateRender()
{
    if (m_state != State::CALCULATING)
        return;

    unsigned int width  = m_pixel_size.x;
    unsigned int height = m_pixel_size.y;

    for (unsigned int i = 0; i < height; ++i)       // y
    {
        for (unsigned int j = 0; j < width; ++j)    // x
        {
            std::size_t index = static_cast<std::size_t>(j)
                               + static_cast<std::size_t>(i) * width;

            // Set pixel position on screen
            m_vArray[index].position = Vector2f(
                static_cast<float>(j),
                static_cast<float>(i)
            );

            // Map pixel
            Vector2f coord = mapPixelToCoords(Vector2i(
                static_cast<int>(j),
                static_cast<int>(i)
            ));

            // Count iterations for this point
            std::size_t count = countIterations(coord);

            // Convert iteration count to color
            Uint8 r = 0, g = 0, b = 0;
            iterationsToRGB(count, r, g, b);

            m_vArray[index].color = Color(r, g, b);
        }
    }

    m_state = State::DISPLAYING;
}

void ComplexPlane::zoomIn()
{
    ++m_zoomCount;

    float zoomFactor = std::pow(BASE_ZOOM, static_cast<float>(m_zoomCount));

    float xSize = BASE_WIDTH  * zoomFactor;
    float ySize = BASE_HEIGHT * m_aspectRatio * zoomFactor;

    m_plane_size = { xSize, ySize };
    m_state = State::CALCULATING;
}

void ComplexPlane::zoomOut()
{
    --m_zoomCount;

    float zoomFactor = std::pow(BASE_ZOOM, static_cast<float>(m_zoomCount));

    float xSize = BASE_WIDTH  * zoomFactor;
    float ySize = BASE_HEIGHT * m_aspectRatio * zoomFactor;

    m_plane_size = { xSize, ySize };
    m_state = State::CALCULATING;
}

void ComplexPlane::setCenter(Vector2i mousePixel)
{
    Vector2f coord = mapPixelToCoords(mousePixel);
    m_plane_center = coord;
    m_state = State::CALCULATING;
}

void ComplexPlane::setMouseLocation(Vector2i mousePixel)
{
    m_mouseLocation = mapPixelToCoords(mousePixel);
}

void ComplexPlane::loadText(Text& text)
{
    stringstream ss;
    ss.setf(ios::fixed);
    ss.precision(6);

    ss << "Center: (" << m_plane_center.x << ", " << m_plane_center.y << ")\n";
    ss << "Cursor: (" << m_mouseLocation.x << ", " << m_mouseLocation.y << ")\n";
    ss << "Zoom level: " << m_zoomCount << "\n";
    ss << "Max Iter: " << MAX_ITER;

    text.setString(ss.str());
}

std::size_t ComplexPlane::countIterations(Vector2f coord)
{
    // Treat coord.x as real, coord.y as imaginary
    complex<double> c(coord.x, coord.y);
    complex<double> z = c;

    std::size_t i = 0;

    while (std::norm(z) < 4.0 && i < MAX_ITER)
    {
        z = z * z + c;
        ++i;
    }

    return i;
}

void ComplexPlane::iterationsToRGB(std::size_t count,
                                   sf::Uint8& r, sf::Uint8& g, sf::Uint8& b)
{
    // Inside points: black
    if (count >= MAX_ITER)
    {
        r = g = b = 0;
        return;
    }

    float t = static_cast<float>(count) / static_cast<float>(MAX_ITER);

    if (t < 0.2f)
    {
        // Purple / blue
        r = static_cast<Uint8>(128 * t / 0.2f);
        g = 0;
        b = 255;
    }
    else if (t < 0.4f)
    {
        // Turquoise
        float local = (t - 0.2f) / 0.2f;
        r = 0;
        g = static_cast<Uint8>(255 * local);
        b = static_cast<Uint8>(255 - 128 * local);
    }
    else if (t < 0.6f)
    {
        // Green
        float local = (t - 0.4f) / 0.2f;
        r = 0;
        g = 255;
        b = static_cast<Uint8>(127 * (1.0f - local));
    }
    else if (t < 0.8f)
    {
        // Yellow
        float local = (t - 0.6f) / 0.2f;
        r = static_cast<Uint8>(255 * local);
        g = 255;
        b = 0;
    }
    else
    {
        // Red
        float local = (t - 0.8f) / 0.2f;
        r = 255;
        g = static_cast<Uint8>(255 * (1.0f - local));
        b = 0;
    }
}

sf::Vector2f ComplexPlane::mapPixelToCoords(sf::Vector2i mousePixel)
{

    float width  = static_cast<float>(m_pixel_size.x);
    float height = static_cast<float>(m_pixel_size.y);

    // Real axis
    float realMin = m_plane_center.x - m_plane_size.x / 2.0f;
    float realMax = m_plane_center.x + m_plane_size.x / 2.0f;

    // Imag axis (positive up, but screen y is positive down)
    float imagMin = m_plane_center.y - m_plane_size.y / 2.0f;
    float imagMax = m_plane_center.y + m_plane_size.y / 2.0f;

    float xPixel = static_cast<float>(mousePixel.x);
    float yPixel = static_cast<float>(mousePixel.y);

    // Normalize pixel to [0,1]
    float xNorm = xPixel / width;         // 0 at left, 1 at right
    float yNorm = yPixel / height;        // 0 at top, 1 at bottom

    // Map x: [0,1], [realMin, realMax]
    float real = realMin + xNorm * (realMax - realMin);

    // For y, invert so top of screen is imagMax, bottom is imagMin
    float imag = imagMax - yNorm * (imagMax - imagMin);

    return Vector2f(real, imag);
}