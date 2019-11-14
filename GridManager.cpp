#include "GridManager.h"
#include <iostream>
#include <cmath>

GridManager::GridManager(sf::Font& t_font, sf::RenderWindow& t_window) :
	m_font(t_font),
	m_window(t_window)
{
	m_tileSize.x = m_window.getSize().x / 50.0f;
	m_tileSize.y = m_window.getSize().y / 50.0f;
	m_grid.reserve(2500);

	for (int i = 0; i < TILES_PER_COL; i++)
	{
		for (int j = 0; j < TILES_PER_ROW; j++)
		{
			GridTile tile(sf::Vector2f(j * m_tileSize.x + (m_tileSize.x / 2.0), i * m_tileSize.y + (m_tileSize.y / 2.0)), m_font, m_tileSize);
			m_grid.push_back(tile);
		}
	}

	//setup tooltip text
	m_tooltipText.setFont(m_font);
	m_tooltipText.setFillColor(sf::Color::Black);
	m_tooltipText.setCharacterSize(15);
	m_tooltipText.setString("Press 1 to display cost values\nPress 2 reset the grid");
	m_tooltipText.setPosition(sf::Vector2f(0, 0));
}

GridManager::~GridManager()
{
}

void GridManager::render()
{
	for (int i = 0; i < m_grid.size(); i++)
	{
		m_grid[i].render(m_window, m_showCost);
	}

	if (m_showTooltips)
	{
		sf::RectangleShape background;
		background.setSize(sf::Vector2f(m_tooltipText.getGlobalBounds().width + 10, m_tooltipText.getGlobalBounds().height + 10));
		background.setFillColor(sf::Color::White);
		background.setPosition(sf::Vector2f(0, 0));
		m_window.draw(background);
		m_window.draw(m_tooltipText);
	}
}

void GridManager::handleInput()
{
	if (m_window.hasFocus())
	{
		handleKeyboard();
		handleMouse();
	}
}

void GridManager::handleKeyboard()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tab))
	{
		m_showTooltips = true;
	}
	else
	{
		m_showTooltips = false;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) && !m_numOnePressed)
	{
		m_showCost = !m_showCost;
		m_numOnePressed = true;
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
	{
		m_numOnePressed = false;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) && !m_numTwoPressed)
	{
		resetGrid();
		m_numTwoPressed = true;
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
	{
		m_numTwoPressed = false;
	}
}

void GridManager::handleMouse()
{
	//handle lmb
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !m_leftBtn)
	{
		m_leftBtn = true;
		handleLeftClick(sf::Mouse::getPosition(m_window));
	}
	else if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
	{
		m_leftBtn = false;
	}

	//handle rmb
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) && !m_rightBtn)
	{
		m_rightBtn = true;
		handleRightClick(sf::Mouse::getPosition(m_window));
	}
	else if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
	{
		m_rightBtn = false;
	}

	//handle mmb
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle))
	{
		m_gridUpdateRequired = true;
		handleMiddleClick(sf::Mouse::getPosition(m_window));
	}
	else if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle) && m_gridUpdateRequired)
	{
		m_gridUpdateRequired = false;
	}
}

void GridManager::handleLeftClick(sf::Vector2i t_mousePos)
{
	int row = t_mousePos.x / m_tileSize.x;
	int col = t_mousePos.y / m_tileSize.y;

	if (row < 0)
	{
		row = 0;
	}
	else if (row > 49)
	{
		row = 49;
	}
	if (col < 0)
	{
		col = 0;
	}
	else if (col > 49)
	{
		col = 49;
	}

	int tileIndex = row + (col * TILES_PER_COL);

	if (m_goalIndex > -1)
	{
		m_grid[m_goalIndex].reset();
	}	
	m_grid[tileIndex].setToGoal();
	m_goalIndex = tileIndex;


	//DRAW VECTOR FIELD IN CIRCLES
	for (int i = 0; i < m_grid.size(); i++)
	{
		if (m_grid[i].getType() == GridTile::TileType::Goal)
		{
			continue;
		}
		else if (m_grid[i].getType() == GridTile::TileType::None || m_grid[i].getType() == GridTile::TileType::Start)
		{
			sf::Vector2f vecToGoal = m_grid[m_goalIndex].getPos() - m_grid[i].getPos();
			int costToGoal;
			if (std::abs(vecToGoal.x) >= std::abs(vecToGoal.y))
			{
				costToGoal = vecToGoal.x / m_tileSize.x;
			}
			else
			{
				costToGoal = vecToGoal.y / m_tileSize.y;
			}
			m_grid[i].setCost(std::abs(costToGoal));
		}
	}



	//DRAW VECTOR FIELD IN CIRCLES
	//for (int i = 0; i < m_grid.size(); i++)
	//{
	//	if (m_grid[i].getType() == GridTile::TileType::Goal)
	//	{
	//		continue;
	//	}
	//	else if (m_grid[i].getType() == GridTile::TileType::None || m_grid[i].getType() == GridTile::TileType::Start)
	//	{
	//		sf::Vector2f vecToGoal = m_grid[m_goalIndex].getPos() - m_grid[i].getPos();
	//		float distToGoal = thor::length(vecToGoal);
	//		int cost = distToGoal / m_grid[i].getDiagnal() + 1;
	//		m_grid[i].setCost(cost);
	//	}
	//}
}

void GridManager::handleRightClick(sf::Vector2i t_mousePos)
{
	int row = t_mousePos.x / m_tileSize.x;
	int col = t_mousePos.y / m_tileSize.y;

	if (row < 0)
	{
		row = 0;
	}
	else if (row > 49)
	{
		row = 49;
	}
	if (col < 0)
	{
		col = 0;
	}
	else if (col > 49)
	{
		col = 49;
	}

	int tileIndex = row + (col * TILES_PER_COL);

	if (m_grid[tileIndex].getType() != GridTile::TileType::Goal)
	{
		//change 0 to the cost of the start node after calculations
		if (m_grid[tileIndex].getType() == GridTile::TileType::Obstacle)
		{
			sf::Vector2f vecToGoal = m_grid[m_goalIndex].getPos() - m_grid[tileIndex].getPos();
			int costToGoal;
			if (std::abs(vecToGoal.x) >= std::abs(vecToGoal.y))
			{
				costToGoal = vecToGoal.x / m_tileSize.x;
			}
			else
			{
				costToGoal = vecToGoal.y / m_tileSize.y;
			}
			m_grid[tileIndex].setCost(std::abs(costToGoal));
		}
		m_grid[tileIndex].setToStart(m_grid[tileIndex].getCost());
		m_startIndex = tileIndex;
	}
}

void GridManager::handleMiddleClick(sf::Vector2i t_mousePos)
{
	int row = t_mousePos.x / m_tileSize.x;
	int col = t_mousePos.y / m_tileSize.y;

	if (row < 0)
	{
		row = 0;
	}
	else if (row > 49)
	{
		row = 49;
	}
	if (col < 0)
	{
		col = 0;
	}
	else if (col > 49)
	{
		col = 49;
	}

	int tileIndex = row + (col * TILES_PER_COL);

	//change 0 to the cost of the start node after calculations
	m_grid[tileIndex].setToObstacle();
}

void GridManager::resetGrid()
{
	m_goalIndex = -1;
	m_startIndex = -1;

	for (int i = 0; i < m_grid.size(); i++)
	{
		m_grid[i].reset();
	}
}

void GridManager::update(bool t_resized)
{
	if (t_resized)
	{
		m_tileSize.x = m_window.getSize().x / 50.0f;
		m_tileSize.y = m_window.getSize().y / 50.0f;

		int rows = 0;
		int cols = 0;
		for (int i = 0; i < m_grid.size(); i++)
		{
			sf::Vector2f newPos(rows * m_tileSize.x + (m_tileSize.x / 2.0), cols * m_tileSize.y + (m_tileSize.y / 2.0));


			m_grid[i].resize(m_tileSize, newPos);
			
			rows++;
			if (rows % 10 == 0)
			{
				cols++;
			}
		}
	}
	handleInput();
}
