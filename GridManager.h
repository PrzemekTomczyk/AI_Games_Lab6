#pragma once
#include "GridTile.h"


class GridManager
{
public:
	GridManager(sf::Font& t_font, sf::RenderWindow& t_window);
	~GridManager();
	void render(sf::RenderWindow& t_window);
	void handleLeftClick(sf::Vector2i t_mousePos);
	void handleRightClick(sf::Vector2i t_mousePos);


private:
	std::vector<GridTile> m_grid;
	sf::Vector2f m_tileSize;

	sf::Font& m_font;

	const int MAX_TILES = 2500;
	const int TILES_PER_ROW = 50;
	const int TILES_PER_COL = 50;
};

