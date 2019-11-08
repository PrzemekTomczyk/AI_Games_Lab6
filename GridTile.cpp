#include "GridTile.h"

GridTile::GridTile(sf::Vector2f t_pos, sf::Font& t_font, sf::Vector2f t_size) :
	m_font(t_font),
	m_cost(0),
	m_pos(t_pos)
{
	m_tile.setFillColor(sf::Color(m_rgb[0], m_rgb[1], m_rgb[2]));
	m_tile.setSize(t_size);
	m_tile.setOrigin(t_size.x / 2.0f, t_size.y / 2.0f);
	m_tile.setPosition(m_pos);
	m_tile.setOutlineColor(sf::Color::Black);
	m_tile.setOutlineThickness(-1.0f);
}

GridTile::~GridTile()
{
}

void GridTile::init(int t_cost, int t_rgb[3])
{
	m_cost = t_cost;
	for (int i = 0; i < 3; i++)
	{
		m_rgb[i] = t_rgb[i];
	}
	m_tile.setFillColor(sf::Color(m_rgb[0], m_rgb[1], m_rgb[2]));
}

void GridTile::render(sf::RenderWindow& t_window)
{
	t_window.draw(m_tile);
}

int GridTile::getCost()
{
	return m_cost;
}

void GridTile::setToObstacle()
{
	m_tile.setFillColor(sf::Color::Black);
	m_cost = 999;
}

void GridTile::setToStart()
{
	m_tile.setFillColor(sf::Color::Red);
}
