#include "GridTile.h"

GridTile::GridTile(sf::Vector2f t_pos, sf::Font& t_font, int& t_highestCost, sf::Vector2f t_size) :
	m_font(t_font),
	m_highestCost(t_highestCost),
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
	m_costText.setCharacterSize(t_size.y * 0.75);
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
	{
		m_costText.setFillColor(sf::Color::Black);
		m_rgb[0] = 0;
		m_rgb[1] = 0;
		m_rgb[2] = 255 - 255 * m_cost / m_highestCost;

		if (255.0f / m_rgb[2] >= 10.f)
		{
			m_costText.setFillColor(sf::Color::White);
		}

		if (m_cost == -1)
		{
			m_rgb[2] = 255;
		}
		break;
	}
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
		if (m_type == GridTile::TileType::Obstacle)
		{
			m_costText.setString("-1");
		}		
		else
		{
			m_costText.setString(std::to_string(m_cost));
		}
		m_costText.setOrigin(m_costText.getGlobalBounds().width / 2.0f, m_costText.getGlobalBounds().height / 2.0f);
		m_costText.setPosition(m_pos);
		t_window.draw(m_costText);
	}
}

int GridTile::getCost()
{
	return m_cost;
}

void GridTile::setCost(int t_cost)
{
	m_cost = t_cost;
}

void GridTile::setToObstacle()
{
	m_type = TileType::Obstacle;
	m_cost = INT_MAX;
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
	for (int i = 0; i < 3; i++)
	{
		m_rgb[i] = BLUE[i];
	}

	//reset type
	m_type = TileType::None;
}

void GridTile::setFlowField(sf::Vector2f t_direction)
{

}

sf::Vector2f GridTile::getPos()
{
	return m_pos;
}

float GridTile::getDiagonal()
{
	return thor::length(m_tile.getSize());
}

GridTile::TileType GridTile::getType()
{
	return m_type;
}
