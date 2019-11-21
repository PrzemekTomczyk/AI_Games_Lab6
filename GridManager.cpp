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
		m_grid[i].render(m_window, m_showCost, m_showVecFields, m_showHeatmap);
	}
	if (m_showVecFields)
	{
		for (int i = 0; i < m_grid.size(); i++)
		{
			if (i != m_goalIndex && m_grid[i].getType() != GridTile::TileType::Obstacle)
			{
				m_window.draw(m_grid[i].getVectorLine());
			}
		}
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

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !m_spacePressed)
	{
		m_spacePressed = true;
		m_deleteMode = !m_deleteMode;
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
	{
		m_spacePressed = false;
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

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3) && !m_numThreePressed)
	{
		m_numThreePressed = true;
		m_showVecFields = !m_showVecFields;
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
	{
		m_numThreePressed = false;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4) && !m_numFourPressed)
	{
		m_numFourPressed = true;
		if (m_goalIndex != -1)
		{
			m_grid[m_goalIndex].reset();
			resetNonObstacleCosts();
			m_goalIndex = -1;
		}
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Num4))
	{
		m_numFourPressed = false;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5) && !m_numFivePressed)
	{
		m_numFivePressed = true;
		m_showHeatmap = !m_showHeatmap;
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Num5))
	{
		m_numFivePressed = false;
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
		handleMiddleClick(sf::Mouse::getPosition(m_window));
	}
	else if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle) && m_gridUpdateRequired)
	{
		m_gridUpdateRequired = false;
		resetNonObstacleCosts();
		if (m_goalIndex >= 0)
		{
			doBrushfireCalc(m_goalIndex);
		}
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

	m_grid[tileIndex].setToGoal();
	m_goalIndex = tileIndex;

	resetNonObstacleCosts();
	doBrushfireCalc(m_goalIndex);
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
	if (m_deleteMode && (m_grid[tileIndex].getType() == GridTile::TileType::Obstacle || m_grid[tileIndex].getType() == GridTile::TileType::Start))
	{
		m_grid[tileIndex].reset();
		m_gridUpdateRequired = true;
	}
	else if (!m_deleteMode && (m_grid[tileIndex].getType() == GridTile::TileType::None || m_grid[tileIndex].getType() == GridTile::TileType::Unreachable))
	{
		m_grid[tileIndex].setToObstacle();

		//if goal is set
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
		if (m_grid[i].getType() == GridTile::TileType::None || m_grid[i].getType() == GridTile::TileType::Unreachable)
		{
			m_grid[i].reset();
		}
		else if (m_grid[i].getType() == GridTile::TileType::Start)
		{
			m_grid[i].setCost(-1);
		}
	}
}

void GridManager::calcPath(int t_startIndex)
{

}

/// <summary>
/// Returns an index of a neighbouring tile <para>0 = left, 1 = right, 2 = top left, 3 = above, 4 = top right, 5 = bottom left, 6 = below, 7 = bottom right</para>
/// </summary>
/// <param name="t_whichNeighbour">int to pick a neighbour</param>
/// <param name="t_indexOfTileToGetNeighbours">index of the tile to get neighbour of</param>
/// <returns>index of neighbouring tile</returns>
int GridManager::getNeighbourIndex(int t_whichNeighbour, int t_indexOfTileToGetNeighbours)
{
	switch (t_whichNeighbour)
	{
	case 0: //index for tile to the left
		return t_indexOfTileToGetNeighbours - 1;
		break;
	case 1: //index for tile to the right
		return t_indexOfTileToGetNeighbours + 1;
		break;
	case 2: //index for tile to the top left
		return t_indexOfTileToGetNeighbours - 51;
		break;
	case 3: //index for tile above
		return t_indexOfTileToGetNeighbours - 50;
		break;
	case 4: //index for tile to the top right
		return t_indexOfTileToGetNeighbours - 49;
		break;
	case 5: //index for tile to the bottom left
		return t_indexOfTileToGetNeighbours + 49;
		break;
	case 6: //index for tile below
		return t_indexOfTileToGetNeighbours + 50;
		break;
	case 7: //index for tile to the bottom right
		return t_indexOfTileToGetNeighbours + 51;
		break;
	default:
		return t_indexOfTileToGetNeighbours;
		break;
	}
}

void GridManager::doBrushfireCalc(int t_currentTileIndex)
{
	std::vector<int> neighbours;
	m_highestCost = 0;

	for (int k = 0; k < 8; k++)
	{
		int neighbourIndex = getNeighbourIndex(k, t_currentTileIndex);

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
		m_grid[neighbours[i]].setHeuristic(m_grid[m_goalIndex].getPos());

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

	for (int i = 0; i < m_grid.size(); i++)
	{
		if (m_grid[i].getCost() == -1 && m_grid[i].getType() == GridTile::TileType::None)
		{
			m_grid[i].setToUnreachable();
		}
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
				int neighbourIndex = getNeighbourIndex(k, t_neighbours[i]);

				if (neighbourIndex < m_grid.size() && neighbourIndex >= 0 && m_grid[neighbourIndex].getCost() == -1)
				{
					if (thor::length(m_grid[t_neighbours[i]].getPos() - m_grid[neighbourIndex].getPos()) <= (m_grid[0].getDiagonal() * 1.1f)) // mult by 1.1f to account for floating point numbers
					{
						t_neighbours.push_back(neighbourIndex);
					}
				}
			}

			for (int j = newNeighbours; j < t_neighbours.size(); j++)
			{
				m_grid[t_neighbours[j]].setCost(m_grid[t_neighbours[i]].getCost() + 1);
				m_grid[t_neighbours[j]].setHeuristic(m_grid[m_goalIndex].getPos());

				if (m_highestCost < m_grid[t_neighbours[i]].getCost() + 1)
				{
					m_highestCost = m_grid[t_neighbours[i]].getCost() + 1;
				}

				int indexWithLowestCost = t_neighbours[i];

				for (int vecFlowIndex = 0; vecFlowIndex < 8; vecFlowIndex++)
				{
					int neighbourIndex = getNeighbourIndex(vecFlowIndex, t_neighbours[j]);
					if (neighbourIndex >= m_grid.size() || neighbourIndex < 0 || m_grid[neighbourIndex].getCost() == -1 || m_grid[neighbourIndex].getType() == GridTile::TileType::Obstacle)
					{
						continue;
					}
					else if (thor::length(m_grid[t_neighbours[j]].getPos() - m_grid[neighbourIndex].getPos()) > (m_grid[0].getDiagonal() * 1.1f)) // mult by 1.1f to account for floating point numbers
					{
						continue;
					}

					//tweak scalar for tile cost
					float costForPreviousTile = m_grid[indexWithLowestCost].getCost() * 100 + m_grid[indexWithLowestCost].getHeuristic();
					float costForNeighbour = m_grid[getNeighbourIndex(vecFlowIndex, t_neighbours[j])].getCost() * 100 + m_grid[getNeighbourIndex(vecFlowIndex, t_neighbours[j])].getHeuristic();

					if (costForPreviousTile > costForNeighbour)
					{
						indexWithLowestCost = getNeighbourIndex(vecFlowIndex, t_neighbours[j]);
					}
				}
				m_grid[t_neighbours[j]].setFlowField(m_grid[indexWithLowestCost].getPos());
			}
			newNeighbours = t_neighbours.size();
		}

		t_neighbours.erase(t_neighbours.begin(), t_neighbours.begin() + startSize);
	}
	//Debug cout
	//std::cout << "Highest cost tile: " << m_highestCost << std::endl;
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
	m_tooltipText.setString("Press LMB to place Goal\nPress RMB to place Start Tiles\nPress/Hold MMB to place Obstacles\nPress Space to toggle between place/remove using MMB\nPress 1 to display cost values\nPress 2 reset the grid\nPress 3 to show flow fields\nPress 4 to remove Goal tile\nPress 5 to disable heatmap");
	m_tooltipText.setPosition(sf::Vector2f(0, 0));
}
