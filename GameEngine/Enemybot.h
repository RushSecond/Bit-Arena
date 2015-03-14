// Enemybot.h
// Written by Blake Zoeckler

#pragma once

#include "Cyberbot.h"

class Playerbot;

class Enemybot : public Cyberbot
{
public:
	Enemybot(void);
	~Enemybot(void);

private:
	Playerbot* m_Player;
};

