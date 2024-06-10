#include "Scene_Grid.h"
#include "Scene_Menu.h"
#include "GameEngine.h"
#include "Assets.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>

#include "imgui.h"
#include "imgui-SFML.h"


Scene_Grid::Scene_Grid(GameEngine* game)
    : Scene(game)
{
    init();
}

void Scene_Grid::init()
{
    ImGui::GetStyle().ScaleAllSizes(2.0f);
    ImGui::GetIO().FontGlobalScale = 2.0f;

    m_viewController.zoomTo(m_game->window(), 20, { 0, 0 });

    m_font = Assets::Instance().getFont("Tech");
    m_text.setFont(m_font);
    m_text.setPosition(10, 5);
    m_text.setCharacterSize(10);

    std::ifstream fin("grid.txt");
    int width, height;
    fin >> width;
    fin >> height;
    m_grid = Grid<float>(width, height, 0.5);
    m_contour.init(width, height);
    m_contourInterpolate.init(width, height);
    

    // read the file values into the grid
    for (size_t y = 0; y < height; y++)
    {
        for (size_t x = 0; x < width; x++)
        {
            float val;
            fin >> val;
            m_grid.set(x, y, val);

        }
    }
}

void Scene_Grid::onFrame()
{
    sUserInput();
    sRender();
    renderUI();
    m_currentFrame++;
}

void Scene_Grid::sUserInput()
{
    sf::Event event;
    while (m_game->window().pollEvent(event))
    {
        ImGui::SFML::ProcessEvent(m_game->window(), event);
        m_viewController.processEvent(m_game->window(), event);

        // this event triggers when the window is closed
        if (event.type == sf::Event::Closed)
        {
            m_game->quit();
        }

        // this event is triggered when a key is pressed
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::Escape:
            {
                m_game->changeScene<Scene_Menu>("Menu");
                break;
            }
            case sf::Keyboard::G: m_drawGrey = !m_drawGrey; break;
            case sf::Keyboard::Num2: { m_waterLevel++; break; }
            case sf::Keyboard::Num1: { m_waterLevel--; break; }
            }
        }

        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::W: break;
            case sf::Keyboard::A: break;
            case sf::Keyboard::S: break;
            case sf::Keyboard::D: break;
            }
        }

        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left) {}
            if (event.mouseButton.button == sf::Mouse::Right) {}
        }

        // happens when the mouse button is released
        if (event.type == sf::Event::MouseButtonReleased)
        {
            if (event.mouseButton.button == sf::Mouse::Left) {}
            if (event.mouseButton.button == sf::Mouse::Right) {}
        }

        // happens whenever the mouse is being moved
        if (event.type == sf::Event::MouseMoved)
        {
            m_mouseScreen = { (float)event.mouseMove.x, (float)event.mouseMove.y };

            // record the grid cell that the mouse position is currently over
            m_mouseGrid = { floor(m_mouseWorld.x / m_gridSize) * m_gridSize, floor(m_mouseWorld.y / m_gridSize) * m_gridSize };
        }
    }
}

// renders the scene
void Scene_Grid::sRender()
{
    const sf::Color gridColor(64, 64, 64);

    m_game->window().clear();
    m_lineStrip.clear();
    m_quadArray.clear();
    float gs = (float)m_gridSize;

    // draw grid cells with the associated colors
    for (size_t x = 0; x < m_grid.width(); x++)
    {
        for (size_t y = 0; y < m_grid.height(); y++)
        {
            if (m_drawGrey)
            {
                // get the value from the grid
                int val = (int)(m_grid.get(x, y) * 255);

                // create a greyscale color to display
                sf::Color c(val, val, val);

                // draw the tile as a greyscale color
                drawRect<float>(x * m_gridSize, y * m_gridSize, m_gridSize, m_gridSize, c);
            }
            else
            {
                // get the value from the grid
                int val = (int)(m_grid.get(x, y) * 255);

                // the color of the tile
                sf::Color c;

                // if the value is less than some intuitive value
                if (val < m_waterLevel)
                {
                    // color the tile like water from blue->black
                    c = sf::Color(0, 0, 255 - m_waterLevel + val);
                }
                // if the value is above that, it's not water
                else
                {
                    // color tile like grass getting brighter
                    c = sf::Color(0, val, 0);
                }

                drawRect<float>(x * m_gridSize, y * m_gridSize, m_gridSize, m_gridSize, c);
            }
        }
    }
    if (m_drawContoursInterpolate)
    {   
        m_contourInterpolate.render(m_game->currentScene(), m_gridSize, m_grid);
    }

    if (m_drawContours)
    {
        m_contour.render(m_game->currentScene(), m_gridSize);
    }

    for (int x = 0; m_drawGrid && x <= (int)m_grid.width(); x++)
    {
        drawLine<float>(x * m_gridSize, 0, x * m_gridSize, m_grid.height() * m_gridSize, gridColor);
    }

    for (int y = 0; m_drawGrid && y <= (int)m_grid.height(); y++)
    {
        drawLine<float>(0, y * m_gridSize, m_grid.width() * m_gridSize, y * m_gridSize, gridColor);
    }

    m_game->window().draw(m_quadArray);
    m_game->window().draw(m_lineStrip);
    m_game->window().draw(m_text);
}

void Scene_Grid::renderUI()
{
    const char vals[7] = { '.', 'G', '@', 'O', 'T', 'S', 'W' };

    ImGui::Begin("Options");

    if (ImGui::BeginTabBar("MyTabBar"))
    {
        if (ImGui::BeginTabItem("Grid"))
        {
           
            ImGui::SliderInt("Water Level", &m_waterLevel, 0, 255);


            // PC Display Options
            //ImGui::Checkbox("ContoursInt", &m_drawContoursInterpolate);
            if (ImGui::Checkbox("ContoursInt", &m_drawContoursInterpolate))
            {
                m_contourInterpolate.calculate(m_grid);
            }
            if (ImGui::Checkbox("Contours", &m_drawContours))
            {
                m_contour.calculate(m_grid);
            }
            if (ImGui::SliderFloat("C Level", &m_contourLevel, 0, 1))
            {
                m_contourInterpolate.setContourLevel(m_contourLevel);
                m_contourInterpolate.calculate(m_grid);
            }

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}
