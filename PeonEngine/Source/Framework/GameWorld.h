#pragma once

#include "TimerManager/LocalTimer.h"

class Actor;

class GameWorld
{
public:
	GameWorld();
	~GameWorld();

	
private:
	LocalTimer mTimer;

	vector<shared_ptr<Actor>> mActors;
};

