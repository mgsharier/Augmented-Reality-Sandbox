#pragma once

#include "Vec2.hpp"
#include "Scene.h"
#include "ViewController.hpp"
#include "Grid.hpp"
#include "Perlin.hpp"
#include "ContourLines.hpp"
#include "ContourLines_Interpolate.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <chrono>
#include <iostream>

class Scene_Grid : public Scene
{   
    sf::Font            m_font;             
    sf::Text            m_text;

    float               m_gridSize = 32;
    bool                m_drawGrid = false;
    bool                m_drawGrey = false;

    int                 m_waterLevel = 80;
    bool                m_drawContours = false;
    bool                m_drawContoursInterpolate = false;
    int                 m_contourSkip = 20;
    float               m_contourLevel = 0.5;

    ContourLines                    m_contour;
    ContourLines_Interpolate        m_contourInterpolate;

    Grid<float>         m_grid;

    sf::Vector2f        m_mouseScreen;
    sf::Vector2f        m_mouseWorld;
    Vec2                m_mouseGrid;
    
    ViewController      m_viewController;
    
    void init();  

    void renderUI();
    void sUserInput();  
    void sRender();
    
public:

    Scene_Grid(GameEngine * game);

    void onFrame();
};
