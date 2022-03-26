#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <string>
#include <vector>

class Actor;
class Peach;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetPath);
	~StudentWorld();

	virtual int init();
	virtual int move();
	virtual void cleanUp();

	void addActor(Level::GridEntry ge, int i, int j);
	void addObject(int object, int x, int y, int dir);
	void bonkObject(double x, double y);
	void bonkPeach();
	void getPeachLocation(double& x, double y);
	bool overlapsPeach(Actor* a) const;
	bool isPeachInvincibleStar();
	bool isPeachTempInvincibleStar();
	void reachedFlag();
	void reachedMario();
	void grantShootPower() const;
	void grantJumpPower() const;
	void grantInvincibility(int ticks) const;
	void setPeachHP(int hp) const;
	void damagePeach();
	void damageObject(Actor* a);
	bool isPeachMovePossible(int x, int y);
	bool isMovePossible(Actor* a, int x, int y);
	bool damageIfPossible(Actor* a, int x, int y);
private:
	void gameStat();
	bool overlapsAt(Actor* a, double x, double y) const;
	Peach* m_peach;
	std::vector<Actor*> m_actor;
	bool m_nextLevel;
	bool m_win;
};

#endif // STUDENTWORLD_H_