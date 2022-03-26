#include "Actor.h"
#include "StudentWorld.h"

Actor::Actor(int imageID, int x, int y, StudentWorld* world, int dir = 0, int depth = 0, double size = 1.0)
	: m_alive(true), m_health(1), m_world(world), GraphObject(imageID, x, y, dir, depth, size) {}

Obstacle::Obstacle(int imageID, int x, int y, StudentWorld* world)
	: Actor(imageID, x, y, world, 0, 2, 1.0) {}

Goal::Goal(int imageID, int x, int y, StudentWorld* world, bool end)
	: Actor(imageID, x, y, world, 0, 1, 1.0), m_end(end) {}

Goodie::Goodie(int imageID, int x, int y, StudentWorld* world)
	: Actor(imageID, x, y, world, 0, 1, 1.0) {}

Projectile::Projectile(int imageID, int x, int y, StudentWorld* world, int dir)
	: Actor(imageID, x, y, world, dir, 1, 1.0) {}

Enemy::Enemy(int imageID, int x, int y, StudentWorld* world)
	: Actor(imageID, x, y, world, 0, 0, 1.0)
{
	setDirection((rand() % 2) * 180);
}

Block::Block(int x, int y, StudentWorld* world)
	: Obstacle(IID_BLOCK, x, y, world), m_goodieNo(-1), m_goodieBlock(false) {}

Block::Block(int x, int y, StudentWorld* world, int goodieNo)
	: Obstacle(IID_BLOCK, x, y, world), m_goodieNo(goodieNo), m_goodieBlock(true) {}

Pipe::Pipe(int x, int y, StudentWorld* world)
	: Obstacle(IID_PIPE, x, y, world) {}

Flower::Flower(int x, int y, StudentWorld* world)
	: Goodie(IID_FLOWER, x, y, world) {}

Mushroom::Mushroom(int x, int y, StudentWorld* world)
	: Goodie(IID_MUSHROOM, x, y, world) {}

Star::Star (int x, int y, StudentWorld* world)
	: Goodie(IID_STAR, x, y, world) {}

PiranhaFireball::PiranhaFireball(int x, int y, StudentWorld* world, int dir)
	: Projectile(IID_PIRANHA_FIRE, x, y, world, dir) {}

PeachFireball::PeachFireball(int x, int y, StudentWorld* world, int dir)
	: Projectile(IID_PEACH_FIRE, x, y, world, dir) {}

Shell::Shell(int x, int y, StudentWorld* world, int dir)
	: Projectile(IID_SHELL, x, y, world, dir), m_wait(true) {}

Goomba::Goomba(int x, int y, StudentWorld* world)
	: Enemy(IID_GOOMBA, x, y, world) {}

Koopa::Koopa(int x, int y, StudentWorld* world)
	: Enemy(IID_KOOPA, x, y, world) {}

Piranha::Piranha(int x, int y, StudentWorld* world)
	: Enemy(IID_PIRANHA, x, y, world), m_fireDelay(0) {}

void Actor::damage()
{
	if (canBeDamaged())
		m_health--;
}

bool Actor::blocksMovement() const { return false; }
bool Actor::canBeDamaged() const { return false; }
StudentWorld* Actor::getWorld() { return m_world; }
void Actor::setDead() { m_alive = false; }
void Actor::setHealth(int hp) { m_health = hp; }
bool Actor::isAlive() const { return m_alive; }
int Actor::returnHealth() const { return m_health; }

void Goal::doSomething()
{
	if (!isAlive())
		return;
	if (getWorld()->overlapsPeach(this))
	{
		getWorld()->increaseScore(1000);
		setDead();
		if (m_end)
			getWorld()->reachedMario();
		else
			getWorld()->reachedFlag();
	}
}

Peach::Peach(int x, int y, StudentWorld* world)
	: Actor(IID_PEACH, x, y, world, 0, 0, 1.0)
{
	m_invincibleCount = 0;
	m_rechargeTime = 0;
	m_jumpDistance = 0;
	m_starPowerTime = 0;
	m_invincible = false;
	m_tempInvincible = false;
	m_shootP = false;
	m_jumpP = false;
	m_canShoot = false;
}

void Peach::bonk()
{
	if (isInvincible() || isTempInvincible())
		return;

	damage();
	if (returnHealth() == 0)
	{
		setDead();
		return;
	}
	losePower();
	getWorld()->playSound(SOUND_PLAYER_HURT);
}

void Peach::losePower()
{
	m_shootP = false;
	m_jumpP = false;
	setInvincibility(10);
	m_tempInvincible = true;
}

bool Peach::isJumping()
{
	double x = getX();
	double y = getY();
	if (m_jumpDistance > 0)
	{
		y += 4;
		if (getWorld()->isPeachMovePossible(x, y))
		{
			moveTo(x, y);
			m_jumpDistance--;
			return true;
		}
		else
		{
			getWorld()->bonkObject(x, y);
			m_jumpDistance = 0;
		}
	}
	else
	{
		y -= 4;
		if (getWorld()->isPeachMovePossible(x, y))
			moveTo(x, y);	
		return false;
	}
}

void Peach::giveJumpPower()
{
	m_jumpP = true;
}

void Peach::setInvincibility(int ticks) 
{ 
	m_invincibleCount = ticks; 
	m_invincible = true;
}

void Peach::setTempInvincibility(int ticks)
{
	m_invincibleCount = ticks;
	m_tempInvincible = true;
}

void Peach::shoot()
{
	std::cerr << "shoot" << std::endl;
	//getWorld()->
}

void Peach::doSomething()
{
	if (!isAlive())		// Check if Peach is alive
		return;

	// Check if Peach is invincible (Star Power)
	if (isInvincible())
	{
		if (m_invincibleCount == 0)
			m_invincible = false;
		else
			m_invincibleCount--;
	}

	// Check if Peach is temporarily invincible
	if (isTempInvincible())
	{
		if (m_invincibleCount == 0)
			m_tempInvincible = false;
		else
			m_invincibleCount--;
	}

	// Check if Peach is in "recharge" mode
	if (m_shootP)
	{
		if (m_rechargeTime > 0)
			m_rechargeTime--;
		else
			m_canShoot = true;
	}

	getWorld()->bonkObject(getX(), getY());
	isJumping();

	// Peach movement
	int ch;
	if (getWorld()->getKey(ch))
	{
		switch (ch)
		{
		case KEY_PRESS_UP:
		{
			if (!getWorld()->isPeachMovePossible(getX(), getY() - 3))
			{
				getWorld()->playSound(SOUND_PLAYER_JUMP);
				if (hasJumpPower())
					m_jumpDistance = 12;
				else
					m_jumpDistance = 8;
			}
			break;
		}
		case KEY_PRESS_LEFT:
		{
			setDirection(180);
			if (getWorld()->isPeachMovePossible(getX() - 4, getY())) // not correct, need to include immovable
				moveTo(getX() - 4, getY());
			break;
		}
		case KEY_PRESS_RIGHT:
		{
			setDirection(0);
			if (getWorld()->isPeachMovePossible(getX() + 4, getY()))
				moveTo(getX() + 4, getY());
			break;
		}
		case KEY_PRESS_SPACE:
		{
			if (m_shootP && m_canShoot)
			{
				getWorld()->playSound(SOUND_PLAYER_FIRE);
				m_rechargeTime = 8;
				m_canShoot = false;
				getWorld()->addObject(4, getX(), getY(), getDirection());
			}
		}
		default:
			break;
		}
	}
}

void Block::bonk()
{
	if (!m_goodieBlock)
		getWorld()->playSound(SOUND_PLAYER_BONK);
	else
	{
		getWorld()->playSound(SOUND_POWERUP_APPEARS);
		getWorld()->addObject(m_goodieNo, getX(), getY() + 8, 0);
		m_goodieNo = -1;
		m_goodieBlock = false;
	}
}

void Goodie::doSomething()
{
	if (!isAlive())
		return;

	double x = getX();
	double y = getY();

	if (getWorld()->overlapsPeach(this))
	{
		setDead();
		getWorld()->playSound(SOUND_PLAYER_POWERUP);
	}
	else
	{
		if (getDirection() == 180)
		{
			if (getWorld()->isMovePossible(this, x - 2, y))
				moveTo(x - 2.0, y);
			else
				setDirection(0);
		}
		else if (getDirection() == 0)
		{
			if (getWorld()->isMovePossible(this, x + 2, y))
				moveTo(x + 2.0, y);
			else
				setDirection(180);
		}

		if (getWorld()->isMovePossible(this, x, y - 2))
		{
			moveTo(x, y - 2.0);
		}
	}
}

void Flower::doSomething()
{
	if (getWorld()->overlapsPeach(this))
	{
		getWorld()->increaseScore(50);
		getWorld()->grantShootPower();
		getWorld()->setPeachHP(2);
	}
	Goodie::doSomething();
}

void Mushroom::doSomething()
{
	if (getWorld()->overlapsPeach(this))
	{
		getWorld()->increaseScore(75);
		getWorld()->grantJumpPower();
		getWorld()->setPeachHP(2);
	}
	Goodie::doSomething();
}

void Star::doSomething()
{
	if (getWorld()->overlapsPeach(this))
	{
		getWorld()->increaseScore(100);
		getWorld()->grantInvincibility(150);
	}
	Goodie::doSomething();
}

void Projectile::doSomething()
{
	int x = getX();
	int y = getY();
	if (getDirection() == 180)
	{
		if (getWorld()->isMovePossible(this, x - 2, y) && !getWorld()->isMovePossible(this, x - 2, y - 1))
		{
			moveTo(x - 2.0, y);
			if (getWorld()->isMovePossible(this, x - 2, y - 1) || getWorld()->isMovePossible(this, x, y - 1))
				moveTo(x - 2.0, y - 2.0);
		}
		else if (getWorld()->isMovePossible(this, x - 2, y - 1) || getWorld()->isMovePossible(this, x, y - 1))
			moveTo(x - 2.0, y - 2.0);
		else
			setDead();
	}
	else if (getDirection() == 0)
	{
		if (getWorld()->isMovePossible(this, x + 2, y) && !getWorld()->isMovePossible(this, x + 2, y - 1))
		{
			moveTo(x + 2.0, y);
			if (getWorld()->isMovePossible(this, x, y - 1) || getWorld()->isMovePossible(this, x + 2, y - 1))
				moveTo(x + 2.0, y - 2.0);
		}
		else if (getWorld()->isMovePossible(this, x, y - 1) || getWorld()->isMovePossible(this, x + 2, y - 1))
			moveTo(x + 2.0, y - 2.0);
		else
			setDead();
	}
}

void PiranhaFireball::doSomething()
{
	if (getWorld()->overlapsPeach(this))
	{
		getWorld()->bonkPeach();
		setDead();
		return;
	}
	Projectile::doSomething();
}

void PeachFireball::doSomething()
{
	int x = getX();
	int y = getY();

	if (getWorld()->damageIfPossible(this, x, y))
	{
		setDead();
		return;
	}
	Projectile::doSomething();
}

void Shell::doSomething()
{
	if (m_wait)
	{
		m_wait = false;
		return;
	}

	int x = getX();
	int y = getY();
	Projectile::doSomething();
	if (getWorld()->damageIfPossible(this, x, y))
	{
		setDead();
		return;
	}
}

void Enemy::doSomething()
{
	if (!isAlive())
		return;

	if (getWorld()->overlapsPeach(this))
	{
		getWorld()->bonkPeach();
		return;
	}

	int x = getX();
	int y = getY();

	if (getDirection() == 180)
	{
		if (getWorld()->isMovePossible(this, x - 1, y) && !getWorld()->isMovePossible(this, x - 8, y - 1))
			moveTo(x - 1.0, y);
		else
			setDirection(0);
	}
	else if (getDirection() == 0)
	{
		if (getWorld()->isMovePossible(this, x + 1, y) && !getWorld()->isMovePossible(this, x + 8, y - 1))
			moveTo(x + 1.0, y);
		else
			setDirection(180);
	}
}

void Enemy::bonk()
{
	if ((!getWorld()->isPeachInvincibleStar() || getWorld()->isPeachTempInvincibleStar()) && getWorld()->overlapsPeach(this))
		return;

	getWorld()->playSound(SOUND_PLAYER_KICK);
	getWorld()->increaseScore(100);
	setDead();
}

void Koopa::bonk()
{
	Enemy::bonk();
	if (!isAlive())
		getWorld()->addObject(5, getX(), getY(), getDirection());
}

void Piranha::doSomething()
{
	if (!isAlive())
		return;

	if (getWorld()->overlapsPeach(this))
	{
		getWorld()->bonkPeach();
		return;
	}
	increaseAnimationNumber();

	double peachX = 0, peachY = 0;
	getWorld()->getPeachLocation(peachX, peachY);
	double y = peachY - getY();
	double x = peachX - getX();

	if (y < 0)
		y *= -1;

	if (x < 0)
		x *= -1;

	if (y > 1.5 * SPRITE_HEIGHT)
	{
		if (peachX < getX())
			setDirection(180);
		else 
			setDirection(0);
		
		if (m_fireDelay > 0)
		{
			m_fireDelay--;
			return;
		}

		if (x < SPRITE_WIDTH * 8 && y < SPRITE_WIDTH * 8)
		{
			getWorld()->addObject(3, getX(), getY(), getDirection());
			getWorld()->playSound(SOUND_PIRANHA_FIRE);
			m_fireDelay = 40;
		}
	}
}