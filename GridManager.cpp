#include "GridManager.h"
#include <iostream>
#include <cmath>

GridManager::GridManager(sf::Font& t_font, sf::RenderWindow& t_window) :
	m_font(t_font),
	m_window(t_window)
{
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

	int tileIndex = row + (col * NO_OF_COLS);

	if (m_goalIndex > -1)
	{
		m_grid[m_goalIndex].reset();
	}
	if (m_grid[tileIndex].getType() == GridTile::TileType::Obstacle)
	{
		m_obstacleAmnt--;
	}
	m_grid[tileIndex].setToGoal();
	m_goalIndex = tileIndex;

	//DRAW VECTOR FIELD IN A SQUARE IF NO OBSTACLES
	if (m_obstacleAmnt == 0)
	{
		for (int i = 0; i < m_grid.size(); i++)
		{
			if (m_grid[i].getType() == GridTile::TileType::Goal)
			{
				continue;
			}
			else if (m_grid[i].getType() == GridTile::TileType::None || m_grid[i].getType() == GridTile::TileType::Start)
			{
				sf::Vector2f vecToGoal = m_grid[m_goalIndex].getPos() - m_grid[i].getPos();
				float costToGoal;
				if (std::abs(vecToGoal.x) >= std::abs(vecToGoal.y))
				{
					costToGoal = vecToGoal.x / m_tileSize.x;
				}
				else
				{
					costToGoal = vecToGoal.y / m_tileSize.y;
				}
				m_grid[i].setCost((int)(std::abs(costToGoal) + 0.5f));
			}
		}
	}
	else
	{
		//do brushfire()
		resetNonObstacleCosts();
		doBrushfireCalc(m_goalIndex);
	}
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

	int tileIndex = row + (col * NO_OF_COLS);

	if (m_grid[tileIndex].getType() != GridTile::TileType::Goal)
	{
		//change 0 to the cost of the start node after calculations
		if (m_grid[tileIndex].getType() == GridTile::TileType::Obstacle)
		{
			m_obstacleAmnt--;
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

	int tileIndex = row + (col * NO_OF_COLS);

	//change 0 to the cost of the start node after calculations
	if (m_grid[tileIndex].getType() == GridTile::TileType::None)
	{
		m_grid[tileIndex].setToObstacle();
		m_obstacleAmnt++;
	}
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

void GridManager::resetNonObstacleCosts()
{
	for (int i = 0; i < m_grid.size(); i++)
	{
		if (m_grid[i].getType() != GridTile::TileType::Obstacle && i != m_goalIndex)
		{
			m_grid[i].setCost(-1);
		}
	}
}

void GridManager::doBrushfireCalc(int t_currentTileIndex)
{
	std::vector<int> neighbourIndex;

	if (t_currentTileIndex + 1 < m_grid.size() && (t_currentTileIndex + 1) % 50 != 0)
	{
		neighbourIndex.push_back(t_currentTileIndex + 1);
	}
	if (t_currentTileIndex - 1 >= 0 && (t_currentTileIndex - 1) % 50 != 0)
	{
		neighbourIndex.push_back(t_currentTileIndex - 1);
	}

	if (t_currentTileIndex + 49 < m_grid.size() && (t_currentTileIndex + 49) % 50 != 0)
	{
		neighbourIndex.push_back(t_currentTileIndex + 49);
	}
	if (t_currentTileIndex + 50 < m_grid.size())
	{
		neighbourIndex.push_back(t_currentTileIndex + 50);
	}
	if (t_currentTileIndex + 51 < m_grid.size() && (t_currentTileIndex + 51) % 50 != 0)
	{
		neighbourIndex.push_back(t_currentTileIndex + 51);
	}

	if (t_currentTileIndex - 49 >= 0 && (t_currentTileIndex - 49) % 50 != 0)
	{
		neighbourIndex.push_back(t_currentTileIndex - 49);
	}
	if (t_currentTileIndex - 50 >= 0)
	{
		neighbourIndex.push_back(t_currentTileIndex - 50);
	}
	if (t_currentTileIndex - 51 >= 0 && (t_currentTileIndex - 51) % 50 != 0)
	{
		neighbourIndex.push_back(t_currentTileIndex - 51);
	}

	for (int i = 0; i < neighbourIndex.size(); i++)
	{
		if (m_grid[neighbourIndex[i]].getCost() < 0 && m_grid[neighbourIndex[i]].getType() != GridTile::TileType::Obstacle)
		{
			m_grid[neighbourIndex[i]].setCost(m_grid[t_currentTileIndex].getCost() + 1);
		}
		else
		{
			neighbourIndex.erase(neighbourIndex.begin() + i);
			i--;
		}
	}

	if (neighbourIndex.size() > 0)
	{
		doBrushfireCalc(neighbourIndex[0]);
		//doBrushfireCalc(neighbourIndex[1]);
	}

	//doBrushfireForNeighbours();
}

void GridManager::doBrushfireForNeighbours(std::vector<int>& t_neighbours)
{
}

void GridManager::update()
{
	handleInput();
}

void GridManager::init()
{
	m_tileSize.x = m_window.getSize().x / 50.0f;
	m_tileSize.y = m_window.getSize().y / 50.0f;
	m_grid.reserve(2500);

	for (int i = 0; i < NO_OF_COLS; i++)
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



	for (int i = 0; i < NO_OF_COLS; i++)
	{
		std::vector<GridTile> row;
		for (int j = 0; j < TILES_PER_ROW; j++)
		{
			GridTile tile(sf::Vector2f(j * m_tileSize.x + (m_tileSize.x / 2.0), i * m_tileSize.y + (m_tileSize.y / 2.0)), m_font, m_tileSize);
			row.push_back(tile);
		}
		m_gridTwoD.push_back(row);
	}
}
