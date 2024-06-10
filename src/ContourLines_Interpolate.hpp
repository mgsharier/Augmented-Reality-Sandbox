#pragma once

#include "Grid.hpp"
#include "Scene.h"
#include <iostream>

#include <SFML/Graphics.hpp>

struct Point {
    float x;
    float y;
};

class Line
{
public:
	std::vector<sf::Vector2f> points;

	void addPoint(float x, float y)
	{
		points.push_back({ x, y });
	}

	bool isEmpty() const
	{
		return points.empty();
	}

	void clear()
	{
		points.clear();
	}
};

class  ContourPair
{
public:
	std::vector<sf::Vector2f> cells;

	void addCell(float x, float y)
	{
		cells.push_back({ x, y });
	}
};

class ContourLines_Interpolate
{
	Grid<char> m_onContour;
	float m_contourLevel = 0.5;
	std::vector <Line> m_lines;
	std::vector<ContourPair> contourPairs;
    bool initialized = false;
	Line line;

public:
	ContourLines_Interpolate()
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
		m_lines.clear();
		int gridSize = 32;
		Point lastPoint;
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
					const sf::Vector2i order[] = { {0,-1}, {1,-1}, {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}, {-1,-1}, {0,-1} };

					bool continuing = false;
					for (auto step : order) {
						int ax = x + step.x;
						int ay = y + step.y;
						if (ax < 0 || ay < 0 || ax >= heightGrid.width() || ay >= heightGrid.height()) { continue; } // Bounds check

						if (heightGrid.get(ax, ay) <= m_contourLevel) 
						{
							float slope = heightGrid.get(x, y) - heightGrid.get(ax, ay);
							float intermediateValue = m_contourLevel - heightGrid.get(ax, ay);
							float position = intermediateValue / slope;
							float xCoordinate = ax * gridSize - step.x * (position * gridSize);
							float yCoordinate = ay * gridSize - step.y * (position * gridSize);
							lastPoint.x = xCoordinate;
							lastPoint.y = yCoordinate;
							line.addPoint(xCoordinate, yCoordinate);
							continuing = true;
						}
						else if (continuing)
						{
							if (line.points.size() >= 2)
							{
								m_lines.push_back(line);
							}
							line.clear();
							continuing = false;
						}
					}

					

					if (line.points.size() >= 2)
					{
						m_lines.push_back(line);
						//line.addPoint(lastPoint.x, lastPoint.y);
					}
					line.clear();
                }

				else if (cellHeight < m_contourLevel)
				{
					const sf::Vector2i order[] = { {0,-1}, {1,-1}, {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}, {-1,-1}, {0,-1} };

					bool continuing = false;
					for (auto step : order) {
						int ax = x + step.x;
						int ay = y + step.y;
						if (ax < 0 || ay < 0 || ax >= heightGrid.width() || ay >= heightGrid.height()) { continue; } // Bounds check

						if (heightGrid.get(ax, ay) >= m_contourLevel)
						{
							float slope = heightGrid.get(ax, ay) - heightGrid.get(x, y);
							float intermediateValue = m_contourLevel - heightGrid.get(x, y);
							float position = intermediateValue / slope;
							float xCoordinate = ax * gridSize - step.x * (position * gridSize);
							float yCoordinate = ay * gridSize - step.y * (position * gridSize);
							lastPoint.x = xCoordinate;
							lastPoint.y = yCoordinate;
							line.addPoint(xCoordinate, yCoordinate);
							continuing = true;
						}
						else if (continuing)
						{
							if (line.points.size() >= 2)
							{
								m_lines.push_back(line);
							}
							line.clear();
							continuing = false;
						}
					}



					if (line.points.size() >= 2)
					{
						m_lines.push_back(line);
						//line.addPoint(lastPoint.x, lastPoint.y);
					}
					line.clear();
				}
            }

        }

		for (size_t i = 0; i < m_lines.size(); i++)
		{
			auto& points = m_lines[i].points;
			for (size_t p = 0; p < points.size(); p++)
			{
				points[p].x += gridSize / 2;
				points[p].y += gridSize / 2;
			}
		}
    }

    void render(std::shared_ptr<Scene> scene, float gridSize, const Grid<float>& heightGrid)
	{
		for (size_t i = 0; i < m_lines.size(); i++)
		{
			auto& points = m_lines[i].points;
			for (size_t p = 0; p < points.size() - 1; p++)
			{
				//std::cout << "working" << "\n";
				scene->drawLine<float>(points[p].x, points[p].y, points[p + 1].x, points[p + 1].y, sf::Color::Red);
			}
		}
    }
};
