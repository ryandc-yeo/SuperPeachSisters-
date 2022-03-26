#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

class Actor : public GraphObject
{
public:
	Actor(int imageID, int x, int y, StudentWorld* world, int dir, int depth, double size);
	~Actor() {}
	virtual void doSomething() {};
	virtual void bonk() {};
	void damage();
	virtual bool blocksMovement() const;
	virtual bool canBeDamaged() const;

	StudentWorld* getWorld();
	void setDead();
	void setHealth(int hp);
	bool isAlive() const;
	int returnHealth() const;
private:
	bool m_alive;
	int m_health;
	StudentWorld* m_world;
};

// Separate Actor classes into categories except Peach
class Obstacle : public Actor
{
public:
	Obstacle(int imageID, int x, int y, StudentWorld* world);
	~Obstacle() {}
	virtual bool blocksMovement() const { return true; }
};

class Goal : public Actor	// includes Flag and Mario
{
public:
	Goal(int imageID, int x, int y, StudentWorld* world, bool end);
	~Goal() {}
	virtual void doSomething();
private:
	bool m_end;
};

class Goodie : public Actor
{
public:
	Goodie(int imageID, int x, int y, StudentWorld* world);
	~Goodie() {}
	virtual void doSomething();
};

class Projectile : public Actor
{
public:
	Projectile(int imageID, int x, int y, StudentWorld* world, int dir);
	~Projectile() {}
	virtual void doSomething();
};

class Enemy : public Actor
{
public:
	Enemy(int imageID, int x, int y, StudentWorld* world);
	~Enemy() {}
	virtual void doSomething();
	virtual void bonk();
	virtual bool canBeDamaged() const { return true; }
};


// Actor classes
class Peach : public Actor
{
public:
	Peach(int x, int y, StudentWorld* world);
	~Peach() {}
	virtual void doSomething();
	virtual bool canBeDamaged() const { return true; }
	virtual void bonk();

	void setInvincibility(int ticks);
	void setTempInvincibility(int ticks);
	void setHP(int hp) {}
	void giveShootPower() { m_shootP = true; }
	void giveJumpPower();
	void losePower();
	
	void shoot();

	bool isJumping();
	bool isInvincible() const { return m_invincible; }
	bool isTempInvincible() const { return m_tempInvincible; }
	bool hasShootPower() const { return m_shootP; }
	bool hasJumpPower() const { return m_jumpP; }

private:
	int m_invincibleCount;
	int m_rechargeTime;
	int m_jumpDistance;
	int m_starPowerTime;
	bool m_invincible;
	bool m_tempInvincible;
	bool m_shootP;
	bool m_jumpP;
	bool m_canShoot;
};

class Block : public Obstacle
{
public:
	Block(int x, int y, StudentWorld* world);
	Block(int x, int y, StudentWorld* world, int goodieNo);
	~Block() {}
	virtual void bonk();
private:
	int m_goodieNo;
	bool m_goodieBlock;
};

class Pipe : public Obstacle
{
public:
	Pipe(int x, int y, StudentWorld* world);
	~Pipe() {}
};

class Flower : public Goodie
{
public:
	Flower(int x, int y, StudentWorld* world);
	~Flower() {}
	virtual void doSomething();
};

class Mushroom : public Goodie
{
public:
	Mushroom(int x, int y, StudentWorld* world);
	~Mushroom() {}
	virtual void doSomething();
};

class Star : public Goodie
{
public:
	Star(int x, int y, StudentWorld* world);
	~Star() {}
	virtual void doSomething();
};

class PiranhaFireball : public Projectile
{
public:
	PiranhaFireball(int x, int y, StudentWorld* world, int dir);
	~PiranhaFireball() {}
	virtual void doSomething();
};

class PeachFireball : public Projectile
{
public:
	PeachFireball(int x, int y, StudentWorld* world, int dir);
	~PeachFireball() {}
	virtual void doSomething();
};

class Shell : public Projectile
{
public:
	Shell(int x, int y, StudentWorld* world, int dir);
	~Shell() {}
	virtual void doSomething();
private:
	bool m_wait;
};

class Goomba : public Enemy
{
public:
	Goomba(int x, int y, StudentWorld* world);
	~Goomba() {}
};

class Koopa : public Enemy
{
public:
	Koopa(int x, int y, StudentWorld* world);
	~Koopa() {}
	virtual void bonk();
};

class Piranha : public Enemy
{
public:
	Piranha(int x, int y, StudentWorld* world);
	~Piranha() {}
	virtual void doSomething();
private:
	int m_fireDelay;
};

#endif // ACTOR_H_
