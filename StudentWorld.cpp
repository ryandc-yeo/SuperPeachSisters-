#include "StudentWorld.h"
#include "Actor.h"
#include "GameConstants.h"
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
	m_peach = nullptr;
	m_nextLevel = false;
	m_win = false;
}

StudentWorld::~StudentWorld() { cleanUp(); }

int StudentWorld::init()
{
	Level lev(assetPath());
	ostringstream oss;
	oss << "level";
	oss.fill('0');
	oss << setw(2) << getLevel() << ".txt";
	string level_file = oss.str();
	Level::LoadResult result = lev.loadLevel(level_file);
	if (result == Level::load_fail_file_not_found)
		cerr << "Could not find level01.txt data file" << endl;
	else if (result == Level::load_fail_bad_format)
		cerr << "level01.txt is improperly formatted" << endl;
	else if (result == Level::load_success)
	{
		cerr << "Successfully loaded level" << endl;
		for (int i = 0; i < GRID_WIDTH; i++)
		{
			for (int j = 0; j < GRID_HEIGHT; j++)
			{
				Level::GridEntry ge;
				ge = lev.getContentsOf(i, j);
				addActor(ge, i, j);
			}
		}
	}
	gameStat();
	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{

	if (m_peach->isAlive())
	{
		m_peach->doSomething();
		
		for (int i = 0; i < m_actor.size(); i++)
		{
			if (m_actor[i]->isAlive())
				m_actor[i]->doSomething();
			else
			{
				delete m_actor[i];
				m_actor.erase(m_actor.begin() + i);
			}
		}
		

		if (!m_peach->isAlive())
		{
			decLives();
			playSound(SOUND_PLAYER_DIE);
			return GWSTATUS_PLAYER_DIED;
		}

		if (m_nextLevel)
		{
			m_nextLevel = false;
			playSound(SOUND_FINISHED_LEVEL);
			return GWSTATUS_FINISHED_LEVEL;
		}

		if (m_win)
		{
			playSound(SOUND_GAME_OVER);
			return GWSTATUS_PLAYER_WON;
		}
	}
	gameStat();
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	delete m_peach;

	for (int i = 0; i < m_actor.size(); i++)
		delete m_actor[i];

	m_actor.clear();
}

void StudentWorld::gameStat()
{
	ostringstream stringStream;
	stringStream.setf(ios::fixed);
	stringStream.precision(2);
	string text = "";

	if (m_peach->hasShootPower())
		text += "ShootPower! ";
	if (m_peach->hasJumpPower())
		text += "JumpPower! ";
	if (m_peach->isInvincible())
		text += "StarPower! ";

	stringStream << "Lives: " << getLives() << "  Level: " << getLevel() << "  Points: " << getScore() << "  " << text;
	string gameStatOutput = stringStream.str();

	setGameStatText(gameStatOutput);
}

void StudentWorld::addActor(Level::GridEntry ge, int i, int j) 
{
	switch (ge)
	{
	case Level::empty:
		break;
	case Level::peach:
	{
		m_peach = new Peach(i * SPRITE_WIDTH, j * SPRITE_HEIGHT, this);
		break;
	}
	case Level::block:
	{
		m_actor.push_back(new Block(i * SPRITE_WIDTH, j * SPRITE_HEIGHT, this));
		break;
	}
	case Level::mushroom_goodie_block:
	{
		m_actor.push_back(new Block(i * SPRITE_WIDTH, j * SPRITE_HEIGHT, this, 0));
		break;
	}
	case Level::flower_goodie_block:
	{
		m_actor.push_back(new Block(i * SPRITE_WIDTH, j * SPRITE_HEIGHT, this, 1));
		break;
	}
	case Level::star_goodie_block:
	{
		m_actor.push_back(new Block(i * SPRITE_WIDTH, j * SPRITE_HEIGHT, this, 2));
		break;
	}
	case Level::pipe:
	{
		m_actor.push_back(new Pipe(i * SPRITE_WIDTH, j * SPRITE_HEIGHT, this));
		break;
	}
	case Level::flag:
	{
		m_actor.push_back(new Goal(IID_FLAG, i * SPRITE_WIDTH, j * SPRITE_HEIGHT, this, false));
		break;
	}
	case Level::mario:
	{
		m_actor.push_back(new Goal(IID_MARIO, i * SPRITE_WIDTH, j * SPRITE_HEIGHT, this, true));
		break;
	}
	case Level::goomba:
	{
		m_actor.push_back(new Goomba(i * SPRITE_WIDTH, j * SPRITE_HEIGHT, this));
		break;
	}
	case Level::koopa:
	{
		m_actor.push_back(new Koopa(i * SPRITE_WIDTH, j * SPRITE_HEIGHT, this));
		break;
	}
	case Level::piranha:
	{
		m_actor.push_back(new Piranha(i * SPRITE_WIDTH, j * SPRITE_HEIGHT, this));
		break;
	}
	}
}

void StudentWorld::addObject(int object, int x, int y, int dir)
{
	switch (object)
	{
	case 0:
	{
		m_actor.push_back(new Mushroom(x, y, this));
		break;
	}
	case 1:
	{
		m_actor.push_back(new Flower(x, y, this));
		break;
	}
	case 2:
	{
		m_actor.push_back(new Star(x, y, this));
		break;
	}
	case 3:
	{
		m_actor.push_back(new PiranhaFireball(x, y, this, dir));
		break;
	}
	case 4:
	{
		m_actor.push_back(new PeachFireball(x, y, this, dir));
		break;
	}
	case 5:
	{
		m_actor.push_back(new Shell(x, y, this, dir));
		break;
	}
	default:
		break;
	}
}

// Return true if a overlaps Peach; otherwise, return false
bool StudentWorld::overlapsPeach(Actor* a) const
{
	double distanceX = a->getX() - m_peach->getX();
	double distanceY = a->getY() - m_peach->getY();

	if (-(SPRITE_WIDTH - 1) <= distanceX && distanceX <= (SPRITE_WIDTH - 1) && -(SPRITE_HEIGHT - 1) <= distanceY && distanceY <= (SPRITE_HEIGHT - 1))
	{
		return true;
	}

	return false;
}

bool StudentWorld::overlapsAt(Actor* a, double x, double y) const
{
	double distanceX = a->getX() - x;
	double distanceY = a->getY() - y;

	if (-(SPRITE_WIDTH - 1) <= distanceX && distanceX <= (SPRITE_WIDTH - 1) && -(SPRITE_HEIGHT - 1) <= distanceY && distanceY <= (SPRITE_HEIGHT - 1))
	{
		return true;
	}

	return false;
}

// If a can move to (destx,destx), return true (but don't move it); otherwise (it would be blocked), return false
bool StudentWorld::isMovePossible(Actor* a, int x, int y)
{
	for (int i = 0; i < m_actor.size(); i++)
	{
		double distanceX = m_actor[i]->getX() - x;
		double distanceY = m_actor[i]->getY() - y;

		if (-(SPRITE_WIDTH - 1) <= distanceX && distanceX <= (SPRITE_WIDTH - 1) && -(SPRITE_HEIGHT - 1) <= distanceY && distanceY <= (SPRITE_HEIGHT - 1))
		{
			if (m_actor[i]->blocksMovement())	// Checks if actor is a passable obstacle
				return false;
		}
	}

	return true;
}

bool StudentWorld::damageIfPossible(Actor* a, int x, int y)
{
	for (int i = 0; i < m_actor.size(); i++)
	{
		double distanceX = m_actor[i]->getX() - x;
		double distanceY = m_actor[i]->getY() - y;

		if (-(SPRITE_WIDTH - 1) <= distanceX && distanceX <= (SPRITE_WIDTH - 1) && -(SPRITE_HEIGHT - 1) <= distanceY && distanceY <= (SPRITE_HEIGHT - 1))
		{
			if (m_actor[i]->canBeDamaged())	// Checks if actor is a damageable object
			{
				damageObject(m_actor[i]);
				m_actor[i]->bonk();
				return true;
			}
		}
	}

	return false;
}

// Return false if Peach's intended position is occupied 
bool StudentWorld::isPeachMovePossible(int x, int y)
{
	for (int i = 0; i < m_actor.size(); i++)
	{
		double distanceX = m_actor[i]->getX() - x;
		double distanceY = m_actor[i]->getY() - y;

		if (-(SPRITE_WIDTH - 1) <= distanceX && distanceX <= (SPRITE_WIDTH - 1) && -(SPRITE_HEIGHT - 1) <= distanceY && distanceY <= (SPRITE_HEIGHT - 1))
		{
			if (m_actor[i]->blocksMovement())	// Checks if actor is an obstacle
				return false;
		}
	}

	return true;
}

void StudentWorld::bonkObject(double x, double y)
{
	for (int i = 0; i < m_actor.size(); i++)
	{
		if (overlapsAt(m_actor[i], x, y))
			m_actor[i]->bonk();
	}
}

void StudentWorld::bonkPeach()
{
	m_peach->bonk();
}

void StudentWorld::grantShootPower() const
{
	m_peach->giveShootPower();
}

void StudentWorld::grantJumpPower() const
{
	m_peach->giveJumpPower();
}

void StudentWorld::grantInvincibility(int ticks) const 
{
	m_peach->setInvincibility(ticks);
}

void StudentWorld::setPeachHP(int hp) const
{
	m_peach->setHealth(hp);
}

void StudentWorld::damagePeach()
{
	m_peach->damage();
}

void StudentWorld::damageObject(Actor* a)
{
	a->damage();
}

bool StudentWorld::isPeachInvincibleStar()
{
	return m_peach->isInvincible();
}

bool StudentWorld::isPeachTempInvincibleStar()
{
	return m_peach->isTempInvincible();
}

void StudentWorld::getPeachLocation(double& x, double y)
{
	x = m_peach->getX();
	y = m_peach->getY();
}

void StudentWorld::reachedFlag() { m_nextLevel = true; }
void StudentWorld::reachedMario() { m_win = true; }