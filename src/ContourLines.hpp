#pragma once

#include "Grid.hpp"
#include "Scene.h"

#include <SFML/Graphics.hpp>


class ContourLines
{
    Grid<char> m_onContour;
    float m_contourLevel = 0.5;

public:

    ContourLines()
    {
    }

    void init(size_t width, size_t height)
    {
        m_onContour = Grid<char>(width, height, 0);
    }

    void setContourLevel(float c)
    {
        m_contourLevel = c;
    }

    bool isOnContour(size_t x, size_t y)
    {
        return m_onContour.get(x, y) == 1;
    }

    void calculate(const Grid<float>& heightGrid)
    {
        m_onContour.clear(0);

        // draw grid cells with the associated colors
        for (size_t x = 0; x < heightGrid.width(); x++)
        {
            for (size_t y = 0; y < heightGrid.height(); y++)
            {
                float cellHeight = heightGrid.get(x, y);

                // if the current cell value is greater than the contour level
                //    if one of its neighbours is less than the contour level
                //       then the current cell is considered on the contour

                if (cellHeight > m_contourLevel)
                {
                    if (y > 0 and heightGrid.get(x, y - 1) <= m_contourLevel)
                    {
                        m_onContour.set(x, y, 1);
                    }

                    if (y < heightGrid.height() - 1 and heightGrid.get(x, y + 1) <= m_contourLevel)
                    {
                        m_onContour.set(x, y, 1);
                    }

                    if (x > 0 and heightGrid.get(x - 1, y) <= m_contourLevel)
                    {
                        m_onContour.set(x, y, 1);
                    }

                    if (x < heightGrid.width() - 1 and heightGrid.get(x + 1, y) <= m_contourLevel)
                    {
                        m_onContour.set(x, y, 1);
                    }
                }
            }
        }
    }

    void render(std::shared_ptr<Scene> scene, float gridSize)
    {
        for (size_t x = 0; x < m_onContour.width(); x++)
        {
            for (size_t y = 0; y < m_onContour.height(); y++)
            {
                // if this cell is not on the contour, then skip it
                if (!isOnContour(x, y)) { continue; }

                float xx = x * gridSize + gridSize / 2;
                float yy = y * gridSize + gridSize / 2;

                if (y > 0 && isOnContour(x, y - 1))
                {
                    scene->drawLine<float>(xx, yy, xx, yy - gridSize, sf::Color::White);
                }

                if (y < m_onContour.height() - 1 && isOnContour(x, y + 1))
                {
                    scene->drawLine<float>(xx, yy, xx, yy + gridSize, sf::Color::White);
                }

                if (x > 0 && isOnContour(x - 1, y))
                {
                    scene->drawLine<float>(xx, yy, xx - gridSize, yy, sf::Color::White);
                }

                if (x < m_onContour.width() - 1 && isOnContour(x + 1, y))
                {
                    scene->drawLine<float>(xx, yy, xx + gridSize, yy, sf::Color::White);
                }

                //

                if (x > 0 && y > 0 && isOnContour(x - 1, y - 1))
                {
                    scene->drawLine<float>(xx, yy, xx - gridSize, yy - gridSize, sf::Color::White);
                }

                if (x > 0 && y < m_onContour.height() - 1 && isOnContour(x - 1, y + 1))
                {
                    scene->drawLine<float>(xx, yy, xx - gridSize, yy + gridSize, sf::Color::White);
                }

                if (x < m_onContour.width() - 1 && y > 0 && isOnContour(x + 1, y - 1))
                {
                    scene->drawLine<float>(xx, yy, xx + gridSize, yy - gridSize, sf::Color::White);
                }

                if (x < m_onContour.width() - 1 && y < m_onContour.height() - 1 && isOnContour(x + 1, y + 1))
                {
                    scene->drawLine<float>(xx, yy, xx + gridSize, yy + gridSize, sf::Color::White);
                }
            }
        }
    }
};