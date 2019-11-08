#include "GridManager.h"
#include <iostream>
#include <cmath>

GridManager::GridManager(sf::Font& t_font, sf::RenderWindow& t_window) :
	m_font(t_font)
{
	m_tileSize.x = t_window.getSize().x / 50.0f;
	m_tileSize.y = t_window.getSize().y / 50.0f;

	for (int i = 0; i < TILES_PER_COL; i++)
	{
		for (int j = 0; j < TILES_PER_ROW; j++)
		{
			GridTile tile(sf::Vector2f(j * m_tileSize.x + (m_tileSize.x / 2.0), i * m_tileSize.y + (m_tileSize.y / 2.0)), m_font, m_tileSize);
			m_grid.push_back(tile);
		}
	}
}

GridManager::~GridManager()
{
}

void GridManager::render(sf::RenderWindow& t_window)
{
	for (int i = 0; i < m_grid.size(); i++)
	{
		m_grid[i].render(t_window);
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

	std::cout << "Row: " << row << std::endl;
	std::cout << "Col: " << col << std::endl;


	m_grid[tileIndex].setToObstacle();
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

	std::cout << "Row: " << row << std::endl;
	std::cout << "Col: " << col << std::endl;


	m_grid[tileIndex].setToStart();
}
