#ifndef WORLDCUP2022_H
#define WORLDCUP2022_H

#include <memory>
#include <string>
#include <vector>
#include <exception>
#include <list>
#include <iostream>
#include <sstream>

#include "worldcup.h"
#include "player.h"
#include "fieldaction.h"

// Global constants

namespace
{
	const unsigned int FIELDS = 12;
	const unsigned int PLAYERS_MAX = 11;
	const unsigned int PLAYERS_MIN = 2;
	const unsigned int DIE_NUMBER = 2;
};

// Exceptions

class TooManyDiceException : public std::exception
{
public:
	virtual const char *what() const throw()
	{
		return "too many dice exception";
	}
};

class TooFewDiceException : public std::exception
{
public:
	virtual const char *what() const throw()
	{
		return "too few dice exception";
	}
};

class TooManyPlayersException : public std::exception
{
public:
	virtual const char *what() const throw()
	{
		return "too many players exception";
	}
};

class TooFewPlayersException : public std::exception
{
public:
	virtual const char *what() const throw()
	{
		return "too few players exception";
	}
};

// sample text score board, in case if
// scoreboard will not be set

class SampleScoreBoard : public ScoreBoard {
    std::stringstream info;
public:
    void onRound(unsigned int roundNo) override {
        info << "=== Runda: " << roundNo << "\n";
    }

    void onTurn(std::string const &playerName, std::string const &status,
                std::string const &currentSquareName, unsigned int money) override {
        info << playerName << " [" << status << "] [" << money << "] - " << currentSquareName << "\n";
    }

    void onWin(const std::string &playerName) override {
        info << "=== Zwycięzca: " << playerName << "\n";
    }

    std::string str() {
        return info.str();
    }
};

// WorldCup2022 implementation
/* 
	WorldCup2022 is an implementation of the WorldCup interface,
	it contains shared pointer to the board which is a vector of fields 
	with shared pointers, list of players which take part in the game
	and a shared pointer to the scoreboard and dices which should be provided
	by a user
*/

class WorldCup2022 : public WorldCup
{
public:
	WorldCup2022()
	{
		this->fields.push_back(std::make_shared<SeasonBeginning>());
		this->fields.push_back(std::make_shared<Match>("Mecz z San Marino", 160, 1.0));
		this->fields.push_back(std::make_shared<FreeDay>());
		this->fields.push_back(std::make_shared<Match>("Mecz z Lichtensteinem", 220, 1.0));
		this->fields.push_back(std::make_shared<YellowCard>());
		this->fields.push_back(std::make_shared<Match>("Mecz z Meksykiem", 300, 2.5));
		this->fields.push_back(std::make_shared<Match>("Mecz z Arabią Saudyjską", 280, 2.5));
		this->fields.push_back(std::make_shared<Bookmaker>());
		this->fields.push_back(std::make_shared<Match>("Mecz z Argentyną", 250, 2.5));
		this->fields.push_back(std::make_shared<Goal>());
		this->fields.push_back(std::make_shared<Match>("Mecz z Francją", 400, 4.0));
		this->fields.push_back(std::make_shared<Penalty>());
		this->how_many_die = 0;
		// in case if scoreboard will not be provied
		std::shared_ptr<SampleScoreBoard> scoreboard_sample = std::make_shared<SampleScoreBoard>();
		this->setScoreBoard(scoreboard_sample);
	}

	void addPlayer(std::string const &name)
	{
		this->players.push_back(Player(name));
	}

	void addDie(std::shared_ptr<Die> die)
	{
		if (die == nullptr)
		{
			return;
		}
		how_many_die++;
		if (how_many_die == 1)
		{
			this->die[0] = die;
		}
		else if (how_many_die == 2)
		{
			this->die[1] = die;
		}
	}
	void setScoreBoard(std::shared_ptr<ScoreBoard> scoreboard)
	{
		this->scoreboard = scoreboard;
	}

	void play(unsigned int rounds)
	{
		if (how_many_die < DIE_NUMBER)
			throw TooFewDiceException();
		if (how_many_die > DIE_NUMBER)
			throw TooManyDiceException();
		if (this->players.size() < PLAYERS_MIN)
			throw TooFewPlayersException();
		if (this->players.size() > PLAYERS_MAX)
			throw TooManyPlayersException();

		bool is_winner = false;
		for (size_t round = 0; round < rounds; round++)
		{
			scoreboard->onRound(round);

			for (std::list<Player>::iterator it = players.begin(); it != players.end();)
			{
				if (players.size() == 1)
				{
					is_winner = true;
					break;
				}
				if (it->waiting() == 0)
				{
					uint32_t die1_roll = die[0]->roll();
					uint32_t die2_roll = die[1]->roll();
					uint32_t moves = die1_roll + die2_roll;
					uint32_t position = it->getPosition();
					uint32_t final_position = (position + moves) % FIELDS;
					for (unsigned short m = 1; m < moves; m++)
					{
						position = (position + 1) % FIELDS;
						fields[position]->passBy(*it);
						it->changePosition(position);
						if (it->isBankrupt()) {
							it->changePosition(final_position);
							break;
						}
					}
					if(!(it->isBankrupt())) {
						it->changePosition(final_position);
						fields[final_position]->stay(*it);
					}
				}
				setTurnScoreboard(*it);
				if (it->waiting() > 0)
					it->waitTurn();

				if (!it->isBankrupt())
					++it;
				else
					it = players.erase(it);
			}
			if (is_winner || players.size() == 1)
			{
				is_winner = 1;
				scoreboard->onWin(players.front().getName());
				break;
			}
		}
		
		uint64_t maxcash = 0;
		std::string best;
		if(!is_winner) {
			for(std::list<Player>::iterator it = players.begin(); it != players.end(); it++) {
				if(!(it->isBankrupt()) && it->getMoney() > maxcash) {
					maxcash = it->getMoney();
					best = it->getName();
				}
			}
			scoreboard->onWin(best);
		}
	}

private:
	std::vector< std::shared_ptr<FieldAction> > fields;
	std::list<Player> players;
	std::shared_ptr<ScoreBoard> scoreboard;
	std::shared_ptr<Die> die[DIE_NUMBER];
	unsigned int how_many_die;
	void setTurnScoreboard(Player &p1)
	{
		std::string status = "w grze";
		unsigned int wait_time = p1.waiting();
		unsigned int money = p1.getMoney();
		if (p1.isBankrupt())
		{
			status = "*** bankrut ***";
			money = 0;
		}
		else if (wait_time > 0)
		{
			status = "*** czekanie: ";
			status += std::to_string(wait_time);
			status += " ***";
		}
		std::string position_name = fields[p1.getPosition()]->getSquareName();
		scoreboard->onTurn(p1.getName(), status, position_name, money);
	}
};

#endif
