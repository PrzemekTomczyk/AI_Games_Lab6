#pragma once
#include "GridTile.h"


class GridManager
{
public:
	GridManager(sf::Font& t_font, sf::RenderWindow& t_window);
	~GridManager();
	void render();
	void update();
	void init();

private:
	void handleInput();
	void handleKeyboard();
	void handleMouse();
	void handleLeftClick(sf::Vector2i t_mousePos);
	void handleRightClick(sf::Vector2i t_mousePos);
	void handleMiddleClick(sf::Vector2i t_mousePos);
	void resetGrid();
	void doBrushfireCalc(int t_currentTileIndex);
	void doBrushfireForNeighbours(std::vector<int>& t_neighbours);
	void resetNonObstacleCosts();
	void calcPath(int t_startIndex);
	void recalcPaths();
	int getNeighbourIndex(int t_index, int t_indexOfTileToGetNeighbours);
	void checkIfStartRemoved(int t_tileClicked);
	int getTileIndex(sf::Vector2i t_mousePos);

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
	const int NO_OF_COLS = 50;

	//bools
	bool m_leftBtn = false;
	bool m_rightBtn = false;
	bool m_middleBtn = false;
	bool m_showTooltips = false;
	bool m_numOnePressed = false;
	bool m_showCost = false;
	bool m_rPressed = false;
	bool m_gridUpdateRequired = false;
	bool m_numThreePressed = false;
	bool m_showVecFields = false;
	bool m_numFourPressed = false;
	bool m_numTwoPressed = false;
	bool m_showHeatmap = true;
	bool m_spacePressed = false;
	bool m_deleteMode = false;

	//ints
	int m_goalIndex = -1;
	std::vector<int> m_startIndexes;
	int m_highestCost = -1;
};

