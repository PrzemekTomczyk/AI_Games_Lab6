#pragma once
#include <SFML/Graphics.hpp>
#include <Thor/Vectors.hpp>

class GridTile
{
public:
	enum class TileType
	{
		Start,
		Goal,
		Obstacle,
		None,
		Path
	};

	const int RED[3]{ 255, 0, 0 };
	const int GREEN[3]{ 0, 255, 0 };
	const int BLUE[3]{ 0, 0, 255 };


public:
	GridTile(sf::Vector2f t_pos, sf::Font& t_font, sf::Vector2f t_size);
	~GridTile();
	void init(int t_cost, int t_rgb[3]);
	void render(sf::RenderWindow& t_window, bool t_showCost);
	int getCost();
	void setCost(int t_cost);
	void setToObstacle();
	void setToStart(int t_cost);
	void setToGoal();
	void reset();
	sf::Vector2f getPos();
	float getDiagnal();
	TileType getType();

private:
	TileType m_type;
	sf::Vector2f m_pos;
	int m_rgb[3]{ 255, 255, 255};
	int m_cost;

	sf::RectangleShape m_tile;

	sf::Font& m_font;
	sf::Text m_costText;
};

