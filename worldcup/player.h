#ifndef WORLDCUP2022_PLAYER_H
#define WORLDCUP2022_PLAYER_H

#include <memory>
#include <string>
#include <vector>
#include <list>
#include <iostream>


class Player
{
public:
	Player(std::string name) : name(name)
	{
		this->money = 1'000;
		this->position = 0;
		this->to_wait = 0;
		this->bankrupt = 0;
	}

	void bankrupcy()
	{
		this->bankrupt = true;
	}

	bool isBankrupt() const
	{
		return this->bankrupt;
	}

	void updateMoney(uint64_t new_balance)
	{
		this->money = new_balance;
	}

	uint64_t getMoney() const
	{
		return this->money;
	}

	void makeWait(short turns)
	{
		this->to_wait = turns;
	}

	void waitTurn()
	{
		this->to_wait--;
	}

	short waiting() const
	{
		return this->to_wait;
	}

	void changePosition(uint64_t newPos)
	{
		this->position = newPos;
	}

	uint32_t getPosition() const
	{
		return this->position;
	}

	std::string getName() const
	{
		return this->name;
	}

private:
	std::string name;
	uint64_t money;
	uint32_t position;
	short to_wait;
	bool bankrupt;
};

#endif
