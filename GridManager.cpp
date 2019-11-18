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
		//m_gridUpdateRequired = true;
		handleMiddleClick(sf::Mouse::getPosition(m_window));
	}
	else if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle) && m_gridUpdateRequired)
	{
		m_gridUpdateRequired = false;
		resetNonObstacleCosts();
		doBrushfireCalc(m_goalIndex);
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
		if (m_grid[tileIndex].getType() == GridTile::TileType::Obstacle && m_goalIndex > -1)
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

		if (m_goalIndex >= 0)
		{
			m_gridUpdateRequired = true;
		}
		else
		{
			m_gridUpdateRequired = false;
		}
	}
}

void GridManager::resetGrid()
{
	m_goalIndex = -1;

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
	std::vector<int> neighbours;
	m_highestCost = 0;

	for (int k = 0; k < 8; k++)
	{
		int neighbourIndex = 0;
		switch (k)
		{
		case 0:
			neighbourIndex = t_currentTileIndex - 1;
			break;
		case 1:
			neighbourIndex = t_currentTileIndex + 1;
			break;
		case 2:
			neighbourIndex = t_currentTileIndex - 51;
			break;
		case 3:
			neighbourIndex = t_currentTileIndex - 50;
			break;
		case 4:
			neighbourIndex = t_currentTileIndex - 49;
			break;
		case 5:
			neighbourIndex = t_currentTileIndex + 49;
			break;
		case 6:
			neighbourIndex = t_currentTileIndex + 50;
			break;
		case 7:
			neighbourIndex = t_currentTileIndex + 51;
			break;
		default:
			break;
		}

		//check if neighbbourIndex is within vector bounds and that the tile has unset cost
		if (neighbourIndex < m_grid.size() && neighbourIndex >= 0 && m_grid[neighbourIndex].getCost() == -1)
		{
			//check that the tile[neighbourIndex] is a neighbouring tile and not on the other side of the screen
			if (thor::length(m_grid[t_currentTileIndex].getPos() - m_grid[neighbourIndex].getPos()) <= (m_grid[0].getDiagonal() * 1.1f))
			{
				neighbours.push_back(neighbourIndex);
			}
		}
	}

	for (int i = 0; i < neighbours.size(); i++)
	{
		m_grid[neighbours[i]].setCost(m_grid[t_currentTileIndex].getCost() + 1);

		if (m_highestCost < m_grid[t_currentTileIndex].getCost() + 1)
		{
			m_highestCost = m_grid[t_currentTileIndex].getCost() + 1;
		}

		m_grid[neighbours[i]].setFlowField(m_grid[t_currentTileIndex].getPos());
	}

	if (neighbours.size() > 0)
	{
		doBrushfireForNeighbours(neighbours);
	}
}

void GridManager::doBrushfireForNeighbours(std::vector<int>& t_neighbours)
{
	while (t_neighbours.size() > 0)
	{
		int startSize = t_neighbours.size();
		int newNeighbours = startSize;

		for (int i = 0; i < startSize; i++)
		{
			for (int k = 0; k < 8; k++)
			{
				int neighbourIndex = 0;
				switch (k)
				{
				case 0: //index for tile to the left
					neighbourIndex = t_neighbours[i] - 1;
					break;
				case 1: //index for tile to the right
					neighbourIndex = t_neighbours[i] + 1;
					break;
				case 2: //index for tile to the top left
					neighbourIndex = t_neighbours[i] - 51;
					break;
				case 3: //index for tile above
					neighbourIndex = t_neighbours[i] - 50;
					break;
				case 4: //index for tile to the top right
					neighbourIndex = t_neighbours[i] - 49;
					break;
				case 5: //index for tile to the bottom left
					neighbourIndex = t_neighbours[i] + 49;
					break;
				case 6: //index for tile below
					neighbourIndex = t_neighbours[i] + 50;
					break;
				case 7: //index for tile to the bottom right
					neighbourIndex = t_neighbours[i] + 51;
					break;
				default:
					break;
				}

				if (neighbourIndex < m_grid.size() && neighbourIndex >= 0 && m_grid[neighbourIndex].getCost() == -1)
				{
					if (thor::length(m_grid[t_neighbours[i]].getPos() - m_grid[neighbourIndex].getPos()) <= (m_grid[0].getDiagonal() * 1.1f))
					{
						t_neighbours.push_back(neighbourIndex);
					}
				}
			}

			for (int j = newNeighbours; j < t_neighbours.size(); j++)
			{
				m_grid[t_neighbours[j]].setCost(m_grid[t_neighbours[i]].getCost() + 1);
				if (m_highestCost < m_grid[t_neighbours[i]].getCost() + 1)
				{
					m_highestCost = m_grid[t_neighbours[i]].getCost() + 1;
				}
			}
			newNeighbours = t_neighbours.size();
		}

		t_neighbours.erase(t_neighbours.begin(), t_neighbours.begin() + startSize);
	}
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
			GridTile tile(sf::Vector2f(j * m_tileSize.x + (m_tileSize.x / 2.0), i * m_tileSize.y + (m_tileSize.y / 2.0)), m_font, m_highestCost, m_tileSize);
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
