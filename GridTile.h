#pragma once
#include <SFML/Graphics.hpp>
#include <Thor/Vectors.hpp>

class GridTile
{
public:
	GridTile(sf::Vector2f t_pos, sf::Font& t_font, sf::Vector2f t_size);
	~GridTile();
	void init(int t_cost, int t_rgb[3]);
	void render(sf::RenderWindow& t_window);
	int getCost();
	void setToObstacle();
	void setToStart();

private:
	sf::Vector2f m_pos;
	int m_rgb[3]{ 255, 255, 255};
	int m_cost;

	sf::RectangleShape m_tile;

	sf::Font& m_font;
	sf::Text m_text;
};

