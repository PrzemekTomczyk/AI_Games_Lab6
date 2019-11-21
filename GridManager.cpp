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
		m_grid[i].render(m_window, m_showCost, m_showHeatmap);
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

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::R) && !m_rPressed)
	{
		resetGrid();
		m_rPressed = true;
	}
	else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::R))
	{
		m_rPressed = false;
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

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) && !m_numTwoPressed)
	{
		m_numTwoPressed = true;
		m_showHeatmap = !m_showHeatmap;
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
		if (m_rightBtn && m_gridUpdateRequired)
		{
			m_gridUpdateRequired = false;
			resetNonObstacleCosts();
			if (m_goalIndex >= 0)
			{
				doBrushfireCalc(m_goalIndex);
			}
		}
	}

	//handle mmb
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle))
	{
		m_middleBtn = true;
		handleMiddleClick(sf::Mouse::getPosition(m_window));
	}
	else if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle) && m_gridUpdateRequired)
	{
		if (m_middleBtn && m_gridUpdateRequired)
		{
			m_gridUpdateRequired = false;
			resetNonObstacleCosts();
			if (m_goalIndex >= 0)
			{
				doBrushfireCalc(m_goalIndex);
			}
		}
	}
}

void GridManager::handleLeftClick(sf::Vector2i t_mousePos)
{
	int tileIndex = getTileIndex(t_mousePos);
	checkIfStartRemoved(tileIndex);
	m_grid[tileIndex].reset();

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
	int tileIndex = getTileIndex(t_mousePos);

	if (m_grid[tileIndex].getType() != GridTile::TileType::Goal)
	{
		//change 0 to the cost of the start node after calculations
		if (m_grid[tileIndex].getType() == GridTile::TileType::Obstacle && m_goalIndex > -1)
		{
			m_gridUpdateRequired = true;
		}
		else if (m_goalIndex > -1)
		{
			findAndSetPath(tileIndex);
		}
		m_grid[tileIndex].setToStart(m_grid[tileIndex].getCost());
		m_startIndexes.push_back(tileIndex);
	}
}

void GridManager::handleMiddleClick(sf::Vector2i t_mousePos)
{
	int tileIndex = getTileIndex(t_mousePos);

	//change 0 to the cost of the start node after calculations
	if (m_deleteMode && (m_grid[tileIndex].getType() == GridTile::TileType::Obstacle || m_grid[tileIndex].getType() == GridTile::TileType::Start))
	{
		checkIfStartRemoved(tileIndex);

		m_grid[tileIndex].reset();
		m_gridUpdateRequired = true;
	}
	else if (!m_deleteMode && (m_grid[tileIndex].getType() == GridTile::TileType::None || m_grid[tileIndex].getType() == GridTile::TileType::Unreachable || m_grid[tileIndex].getType() == GridTile::TileType::Path))
	{
		m_grid[tileIndex].setToObstacle();

		//if goal is set
		if (m_goalIndex >= 0 && m_grid[tileIndex].getType() != GridTile::TileType::Unreachable)
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
	m_startIndexes.clear();

	for (int i = 0; i < m_grid.size(); i++)
	{
		m_grid[i].reset();
	}
}

void GridManager::resetNonObstacleCosts()
{
	for (int i = 0; i < m_grid.size(); i++)
	{
		if (m_grid[i].getType() == GridTile::TileType::None || m_grid[i].getType() == GridTile::TileType::Unreachable || m_grid[i].getType() == GridTile::TileType::Path)
		{
			m_grid[i].reset();
		}
		else if (m_grid[i].getType() == GridTile::TileType::Start)
		{
			m_grid[i].setCost(-1);
		}
	}
}

/// <summary>
/// recursive function that goes from start to goal, or until it meets an already found path, and changes tiles to be of type Path
/// </summary>
/// <param name="t_startIndex"></param>
void GridManager::findAndSetPath(int t_startIndex)
{
	bool isStart = false;
	bool pathFinished = false;
	for (int i = 0; i < m_startIndexes.size(); i++)
	{
		if (t_startIndex == m_startIndexes[i])
		{
			isStart = true;
		}
	}
	int indexWithLowestCost = -1;
	float smallestCost = INT_MAX;// = m_grid[indexWithLowestCost].getCost() * 300 + m_grid[indexWithLowestCost].getHeuristic();
	float neighbourCost;// = m_grid[getNeighbourIndex(vecFlowIndex, t_neighbours[j])].getCost() * 300 + m_grid[getNeighbourIndex(vecFlowIndex, t_neighbours[j])].getHeuristic();

	for (int neighboursLoopIndex = 0; neighboursLoopIndex < 8; neighboursLoopIndex++)
	{
		int neighbourIndex = getNeighbourIndex(neighboursLoopIndex, t_startIndex);

		if (neighbourIndex >= m_grid.size() || neighbourIndex < 0 || m_grid[neighbourIndex].getCost() == -1 || m_grid[neighbourIndex].getType() == GridTile::TileType::Obstacle)
		{
			continue;
		}
		//make sure its an actual neighbour and not a "neighbour" that wrapped to the other side of the screen
		else if (thor::length(m_grid[t_startIndex].getPos() - m_grid[neighbourIndex].getPos()) > (m_grid[0].getDiagonal() * 1.1f)) // mult by 1.1f to account for floating point numbers
		{
			continue;
		}

		//if (m_grid[neighbourIndex].getType() != GridTile::TileType::Path)
		neighbourCost = m_grid[neighbourIndex].getCost() * 300 + m_grid[neighbourIndex].getHeuristic();
		if (neighbourCost < smallestCost)
		{
			smallestCost = neighbourCost;
			indexWithLowestCost = neighbourIndex;

			//check if that neighbour tile is already a path. This means we have hit an already set path - no further calc needed
			if (m_grid[indexWithLowestCost].getType() == GridTile::TileType::Goal)
			{
				pathFinished = true;
				break;
			}
		}
	}
	//if the neighbour we consider the closest is already a path, we found an already established path to the goal meaning we can stop
	if (indexWithLowestCost > -1)
	{
		if (m_grid[indexWithLowestCost].getType() == GridTile::TileType::Path)
		{
			pathFinished = true;
		}
	}


	//if we didnt come across already marked out path, keep marking tiles
	if (!pathFinished && indexWithLowestCost > -1)
	{
		if (m_grid[indexWithLowestCost].getType() == GridTile::TileType::None)
		{
			m_grid[indexWithLowestCost].setToPath();

		}
		findAndSetPath(indexWithLowestCost);
	}
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

void GridManager::checkIfStartRemoved(int t_tileClicked)
{
	if (m_grid[t_tileClicked].getType() == GridTile::TileType::Start)
	{
		for (int i = 0; i < m_startIndexes.size(); i++)
		{
			if (m_startIndexes[i] == t_tileClicked)
			{
				m_startIndexes.erase(m_startIndexes.begin() + i);
				i--;
			}
		}
	}
}

int GridManager::getTileIndex(sf::Vector2i t_mousePos)
{
	int col = t_mousePos.x / m_tileSize.x;
	int row = t_mousePos.y / m_tileSize.y;

	if (col < 0)
	{
		col = 0;
	}
	else if (col > 49)
	{
		col = 49;
	}
	if (row < 0)
	{
		row = 0;
	}
	else if (row > 49)
	{
		row = 49;
	}
	return  col + (row * TILES_PER_ROW);
}

void GridManager::doBrushfireCalc(int t_currentTileIndex)
{
	std::cout << "Brushfire for GOAL" << std::endl;
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

	for (int i = 0; i < m_startIndexes.size(); i++)
	{
		findAndSetPath(m_startIndexes[i]);
	}
}

void GridManager::doBrushfireForNeighbours(std::vector<int>& t_neighbours)
{
	std::cout << "    Brushfire for NEIGHBOURS" << std::endl;

	while (t_neighbours.size() > 0)
	{
		//where old neighbours end
		int startSize = t_neighbours.size();
		//where new neighbours should start
		int newNeighbours = startSize;

		//for loop that goes through current neighbours and finds their neighbours
		for (int i = 0; i < startSize; i++)
		{
			//for loop to find neighbours of the tile we're iterating through
			int newNeighboursAdded = 0;
			for (int k = 0; k < 8; k++)
			{
				//get index for neighbour
				int neighbourIndex = getNeighbourIndex(k, t_neighbours[i]);

				//checks to see if this is a valid neighbour and if so, add it to neighbours vector
				if (neighbourIndex < m_grid.size() && neighbourIndex >= 0 && m_grid[neighbourIndex].getCost() == -1)
				{
					if (thor::length(m_grid[t_neighbours[i]].getPos() - m_grid[neighbourIndex].getPos()) <= (m_grid[0].getDiagonal() * 1.1f)) // mult by 1.1f to account for floating point numbers
					{
						t_neighbours.push_back(neighbourIndex);
						newNeighboursAdded++;
					}
				}
			}
			//check if any valid neighbours have been added, if none have been added, skip next steps
			if (newNeighboursAdded == 0)
			{
				continue;
			}

			//for loop to set up newly added neighbours
			for (int j = newNeighbours; j < t_neighbours.size(); j++)
			{
				//set up cost
				m_grid[t_neighbours[j]].setCost(m_grid[t_neighbours[i]].getCost() + 1);
				//set up heuristic
				m_grid[t_neighbours[j]].setHeuristic(m_grid[m_goalIndex].getPos());

				//if this newly added neighbour has the highest cost in the grid, update highest cost
				if (m_highestCost < m_grid[t_neighbours[i]].getCost() + 1)
				{
					m_highestCost = m_grid[t_neighbours[i]].getCost() + 1;
				}

				//(assume)set lowest cost node to the neighbour that added new cell to the vector
				int indexWithLowestCost = t_neighbours[i];

				//for loop to check neighbours of newly added cell
				for (int vecFlowIndex = 0; vecFlowIndex < 8; vecFlowIndex++)
				{
					//get the index
					int neighbourIndex = getNeighbourIndex(vecFlowIndex, t_neighbours[j]);
					//make sure the "neighbour cell" is a valid one
					if (neighbourIndex >= m_grid.size() || neighbourIndex < 0 || m_grid[neighbourIndex].getCost() == -1 || m_grid[neighbourIndex].getType() == GridTile::TileType::Obstacle)
					{
						continue;
					}
					//make sure its an actual neighbour and not a "neighbour" that wrapped to the other side of the screen
					else if (thor::length(m_grid[t_neighbours[j]].getPos() - m_grid[neighbourIndex].getPos()) > (m_grid[0].getDiagonal() * 1.1f)) // mult by 1.1f to account for floating point numbers
					{
						continue;
					}

					//Multiply cell cost by 300 as we value cheaper cells more over the heuristic cost
					//tweak scalar for tile cost
					float costForPreviousTile = m_grid[indexWithLowestCost].getCost() * 300 + m_grid[indexWithLowestCost].getHeuristic();
					float costForNeighbour = m_grid[getNeighbourIndex(vecFlowIndex, t_neighbours[j])].getCost() * 300 + m_grid[getNeighbourIndex(vecFlowIndex, t_neighbours[j])].getHeuristic();

					//if the neigbour we just checked is cheaper than current one, update the cheapest index
					if (costForPreviousTile > costForNeighbour)
					{
						indexWithLowestCost = getNeighbourIndex(vecFlowIndex, t_neighbours[j]);
					}
				}
				//set flow field up for new neighbour
				m_grid[t_neighbours[j]].setFlowField(m_grid[indexWithLowestCost].getPos());
			}
			//index point where new neighbours start
			newNeighbours = t_neighbours.size();
		}
		//remove old/already set neigbours from the vector
		t_neighbours.erase(t_neighbours.begin(), t_neighbours.begin() + startSize);
	}
	std::cout << "        Finished calculation Brushfire" << std::endl;

	//Debug cout
	//std::cout << "Highest cost tile: " << m_highestCost << std::endl;
}

void GridManager::update()
{
	handleInput();
}

void GridManager::init()
{
	//use height of the window to make squares as the right side of the screen is used for tooltip info
	m_tileSize.x = m_window.getSize().y / 50.0f;
	m_tileSize.y = m_window.getSize().y / 50.0f;
	m_grid.reserve(2500);

	for (int i = 0; i < TILES_PER_ROW; i++)
	{
		for (int j = 0; j < NO_OF_COLS; j++)
		{
			GridTile tile(sf::Vector2f(j * m_tileSize.x + (m_tileSize.x / 2.0), i * m_tileSize.y + (m_tileSize.y / 2.0)), m_font, m_highestCost, m_tileSize);
			m_grid.push_back(tile);
		}
	}
}
