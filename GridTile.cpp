#include "GridTile.h"

GridTile::GridTile(sf::Vector2f t_pos, sf::Font& t_font, sf::Vector2f t_size) :
	m_font(t_font),
	m_cost(-1),
	m_pos(t_pos),
	m_type(TileType::None)
{
	m_tile.setFillColor(sf::Color(m_rgb[0], m_rgb[1], m_rgb[2]));
	m_tile.setSize(t_size);
	m_tile.setOrigin(t_size.x / 2.0f, t_size.y / 2.0f);
	m_tile.setPosition(m_pos);
	m_tile.setOutlineColor(sf::Color::Black);
	m_tile.setOutlineThickness(-1.0f);


	//setup tooltip text
	m_costText.setFont(m_font);
	m_costText.setFillColor(sf::Color::Black);
	m_costText.setCharacterSize(10);
	m_costText.setString(std::to_string(m_cost));
	m_costText.setOrigin(m_costText.getGlobalBounds().width / 2.0f, m_costText.getGlobalBounds().height / 2.0f);
	m_costText.setPosition(m_pos);
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

void GridTile::render(sf::RenderWindow& t_window, bool t_showCost)
{
	switch (m_type)
	{
	case GridTile::TileType::Start:
		m_costText.setFillColor(sf::Color::Black);
		m_rgb[0] = 255;
		m_rgb[1] = 0;
		m_rgb[2] = 0;
		break;
	case GridTile::TileType::Goal:
		m_costText.setFillColor(sf::Color::Black);
		m_rgb[0] = 0;
		m_rgb[1] = 255;
		m_rgb[2] = 0;
		break;
	case GridTile::TileType::Obstacle:
		m_costText.setFillColor(sf::Color::White);
		m_rgb[0] = 0;
		m_rgb[1] = 0;
		m_rgb[2] = 0;
		break;
	case GridTile::TileType::None:
		m_costText.setFillColor(sf::Color::Black);
		m_rgb[0] = 0;
		m_rgb[1] = 0;
		m_rgb[2] = 255;
		break;
	case GridTile::TileType::Path:
		m_costText.setFillColor(sf::Color::Black);
		m_rgb[0] = 255;
		m_rgb[1] = 255;
		m_rgb[2] = 0;
		break;
	default:
		break;
	}
	m_tile.setFillColor(sf::Color(m_rgb[0], m_rgb[1], m_rgb[2]));
	t_window.draw(m_tile);

	if (t_showCost)
	{
		m_costText.setString(std::to_string(m_cost));
		m_costText.setOrigin(m_costText.getGlobalBounds().width / 2.0f, m_costText.getGlobalBounds().height / 2.0f);
		m_costText.setPosition(m_pos);
		t_window.draw(m_costText);
	}
}

int GridTile::getCost()
{
	return m_cost;
}

void GridTile::setToObstacle()
{
	if (m_type != TileType::Goal && m_type != TileType::Start)
	{
		m_type = TileType::Obstacle;
		m_cost = 999;
	}
}

void GridTile::setToStart(int t_cost)
{
	m_type = TileType::Start;
	m_cost = t_cost;
}

void GridTile::setToGoal()
{
	m_type = TileType::Goal;
	m_cost = 0;
}

void GridTile::reset()
{
	//reset cost
	m_cost = -1;

	//reset colour
	m_rgb[0] = 0;
	m_rgb[1] = 0;
	m_rgb[2] = 255;

	//reset type
	m_type = TileType::None;

}
