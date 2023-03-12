#ifndef WORLDCUP2022_FIELD_H
#define WORLDCUP2022_FIELD_H

#include <memory>
#include <string>
#include <vector>
#include <list>
#include <cstdint>
#include <iostream>
#include "player.h"

/* FieldAction is an abstract class representing
   a square on board. Specific fields inherit from
   that class and override methods
*/
class FieldAction
{
public:
	FieldAction(std::string name) : squareName(name){};

	// Some fields do not do anything at stay() or passBy()
	// so stay and passBy are empty functions instead of
	// making them abstract and empty in case of specific
	// implementations
	virtual void stay([[maybe_unused]] Player &p1)
	{
		return;
	}

	virtual void passBy([[maybe_unused]] Player &p1)
	{
		return;
	}

	std::string getSquareName()
	{
		return this->squareName;
	}

private:
	std::string squareName;
};

class SeasonBeginning : public FieldAction
{
public:
	SeasonBeginning() : FieldAction("Początek sezonu")
	{
		this->fee = 50;
	}

	void stay(Player &p1) override
	{
		p1.updateMoney(p1.getMoney() + this->fee);
	}

	void passBy(Player &p1) override
	{
		p1.updateMoney(p1.getMoney() + this->fee);
	}

private:
	uint64_t fee;
};

class FreeDay : public FieldAction
{
public:
	FreeDay() : FieldAction("Dzień wolny od treningu") {}
};

class Goal : public FieldAction
{
public:
	Goal() : FieldAction("Gol")
	{
		this->fee = 120;
	}

	void stay(Player &p1) override
	{
		p1.updateMoney(p1.getMoney() + this->fee);
	}

private:
	uint64_t fee;
};

class Penalty : public FieldAction
{
public:
	Penalty() : FieldAction("Rzut karny")
	{
		this->fee = 180;
	}

	void stay(Player &p1) override
	{
		uint64_t m = p1.getMoney();
		if (m < this->fee)
		{
			p1.bankrupcy();
			p1.updateMoney(0);
		}
		else
			p1.updateMoney(m - this->fee);
	}

private:
	uint64_t fee;
};

class Bookmaker : public FieldAction
{
public:
	Bookmaker() : FieldAction("Bukmacher")
	{
		this->turn = 0;
		this->fee = 100;
	}

	void stay(Player &p1) override
	{
		uint64_t m = p1.getMoney();
		if (!turn)
		{
			p1.updateMoney(m + this->fee);
		}
		else
		{
			if (m < this->fee)
			{
				p1.bankrupcy();
				p1.updateMoney(0);
			}
			else
				p1.updateMoney(m - this->fee);
		}

		turn++;
		if (turn == 3)
			turn = 0;
	}

private:
	short turn;
	uint64_t fee;
};

class YellowCard : public FieldAction
{
public:
	YellowCard() : FieldAction("Żółta kartka") {}

	void stay(Player &p1) override
	{
		p1.makeWait(3);
	}
};

class Match : public FieldAction
{
public:
	Match(std::string name, uint64_t fee, double weight) : FieldAction(name)
	{
		this->weight = weight;
		this->bank = 0;
		this->fee = fee;
	}

	void stay(Player &p1) override
	{
		p1.updateMoney(p1.getMoney() + this->bank);
		this->bank = 0;
	}

	void passBy(Player &p1) override
	{
		uint64_t m = p1.getMoney();
		if (m < this->fee)
		{
			double bonus = (double)m * this->weight;
			this->bank += uint64_t(bonus);
			p1.bankrupcy();
			p1.updateMoney(0);
		}
		else
		{
			p1.updateMoney(m - this->fee);
			double bonus = (double)this->fee * this->weight;
			this->bank += uint64_t(bonus);
		}
	}

private:
	double weight;
	uint64_t bank;
	uint64_t fee;
};

#endif
