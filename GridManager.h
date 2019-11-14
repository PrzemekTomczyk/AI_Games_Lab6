#pragma once
#include "GridTile.h"


class GridManager
{
public:
	GridManager(sf::Font& t_font, sf::RenderWindow& t_window);
	~GridManager();
	void render();
	void update(bool t_resized);

private:
	void handleInput();
	void handleKeyboard();
	void handleMouse();
	void handleLeftClick(sf::Vector2i t_mousePos);
	void handleRightClick(sf::Vector2i t_mousePos);
	void handleMiddleClick(sf::Vector2i t_mousePos);
	void resetGrid();

	//vectors
	std::vector<GridTile> m_grid;
	sf::Vector2f m_tileSize;

	//references
	sf::Font& m_font;
	sf::RenderWindow& m_window;

	//text
	sf::Text m_tooltipText;

	//consts
	const int MAX_TILES = 2500;
	const int TILES_PER_ROW = 50;
	const int TILES_PER_COL = 50;

	//bools
	bool m_leftBtn = false;
	bool m_rightBtn = false;
	bool m_showTooltips = false;
	bool m_numOnePressed = false;
	bool m_showCost = false;
	bool m_numTwoPressed = false;
	bool m_resized = false;
	bool m_gridUpdateRequired = false;

	//ints for indexes
	int m_goalIndex = -1;
	int m_startIndex = -1;
};

