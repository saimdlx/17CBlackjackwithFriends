/*
 * BlackJack with Friends
 * Author: Saim Ahmed
 * Date 4/30/25
 * Description: Play classic casino blackjack with yourself or with friends against an AI powered Dealer!
 *              Place bets to win big (or lose) and place yourself on a communal leaderboard!
 *              Obtain a hand closest to 21 without going over, but don't let the dealer outscore you, or its bust!
*/

#include <algorithm>
#include <deque>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <random>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <iterator>

using namespace std;

//No more than a deck of cards per deck (obviously).
const int MAXCARDS = 52;
//Average table size for blackjack, not including dealer.
const int MAXPLAYERS = 4;


class Card {
private:
    string m_suit;
    string m_rank;
    bool m_faceUp;

public:
    

    Card() : m_suit(""), m_rank("") {}

    Card(string decSuit, string decRank, bool setFace = true) : m_suit(decSuit), m_rank(decRank), m_faceUp(setFace) {}
    
    // Getters
    string getRank() const{

        return m_rank;

    }
    string getSuit() const {

        return m_suit;

    }
    bool isFaceUp() const {

        return m_faceUp;

    }
    // Setters
    void flip() {

        if (m_faceUp) {

            m_faceUp = false;

        }

    }
    bool operator==(const Card& other) const {
        if (m_rank == other.m_rank && m_suit == other.m_suit) {

            return true;

        }

        else {

            return false;

        }

    }
    bool operator<(const Card& other) const {

        if (m_rank < other.m_rank) {
            return true;

        }
        else if (m_rank > other.m_rank){
            return false;
        }

        //If card ranks are the same, compare the suits.
        return m_suit < other.m_suit;


    }
    
};


class Deck {
private:
    list<Card> cards;
    stack<Card> discardPile;
    
public:
    // Constructor
    Deck() {

        //Storing suit and rank values into an array to then populate the list of Cards, with card objects. Forming a deck!
        string suits[] = {"HEARTS", "DIAMONDS", "CLUBS", "SPADES"};
        string ranks[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King", "Ace"};

        //Populate function goes here:
        for (auto& iterSuits : suits) {
            for (auto& iterRanks : ranks) {
                cards.push_back(Card(iterRanks, iterSuits));
            }
        }

    }
    
    // Deck operations
    void shuffleDeck() {
        random_device ranD;
        //Using a traditional randomizer function, Mersenne Twister.
        mt19937 sh(static_cast<unsigned int>(ranD()));
        shuffle(cards.begin(), cards.end(), sh);
    }
    Card deal(){
        Card dealtCard = cards.front();
        cards.pop_front();

        return dealtCard;
    }
    void addToDiscardPile(const Card& card){
        discardPile.push(card);
    }
    void retrieveCardsFromDiscardPile(){
        cards.push_front(discardPile.top());
    }
    bool isEmpty() const;
    int cardsRemaining() const;
    string toString() const;
    
};


class Hand {
protected:
    deque<Card> m_cards;
    
public:
    // Constructor/Destructor
    Hand();
    
    // Hand operations
    void add(const Card& card);
    void clear();
    int getTotal() const;
    //Get bust or blackjack status at the current iteration.
    bool isBusted() const;
    bool isBlackjack() const;
    
    // Iterators
    
    
    // Display
    string toString() const;
};


class Player {
protected:
    string m_name;
    Hand m_hand;
    int m_money;
    int m_bet;
    
public:
    // Constructor/Destructor
    Player(const string& name = "Player", int money = 1000);
    
    // Getters
    string getName() const;
    int getMoney() const;
    int getBet() const;
    const Hand& getHand() const;
    Hand& getHandRef();
    
    // Betting operations
    bool placeBet(int amount);
    void win();
    void lose();
    void push();
    
    // Game actions
     bool isHitting();
     void showHand(bool showFirstCard = true) const;
    
    // Status checks for game flow
    bool isBusted() const;
    bool isBlackjack() const;
};




class Dealer : public Player {
private:
    Deck m_deck;
    
public:
    // Constructor
    Dealer(const string& name = "Dealer");
    
    // Game actions
    bool isHitting() const;
    void showHand(bool showFirstCard = true) const;
    
    // Deck operations
    //shuffleDeck will, in the instance of multiple decks, switch between them.
    void shuffleDeck();
    Card deal();
    bool deckIsEmpty() const;
};


class GameStats {
private:
    unordered_map<string, pair<int, int>> m_playerStats; // <name, <wins, losses>>
    set<pair<int, string>> m_highScores; // <money, name>
    
public:
    // Constructor
    GameStats();
    
    // Stats operations
    void recordWin(const string& playerName);
    void recordLoss(const string& playerName);
    void updateHighScore(const string& playerName, int money);
    
    // Getters
    int getWins(const string& playerName) const;
    int getLosses(const string& playerName) const;
    double getWinRate(const string& playerName) const;
    
    // Display
    void displayStats() const;
    void displayHighScores(int top = 5) const;
};


class Game {
private:
    deque<unique_ptr<Player>> m_players;
    Dealer m_dealer;
    GameStats m_stats;
    queue<string> m_actionLog;
    priority_queue<pair<int, string>> m_events;
    
    // Game state
    enum class GameState { BETTING, DEALING, PLAYER_TURN, DEALER_TURN, PAYOUT, CLEANUP };
    GameState m_currentState;
    
    // Maps for card values and suits
    map<string, int> m_cardValues;
    map<string, string> m_suitNames;
    
public:
    // Constructor/Destructor
    Game();
    
    // Player management
    void addPlayer(const Player& name);
    void removePlayer(const string& name);
    
    // Game flow control
    void play();
    void placeBets();
    void deal();
    void playerTurns();
    void dealerTurn();
    void payouts();
    void cleanup();
    
    // Game state management
    void setState(GameState state);
    GameState getState() const;
    
    // Game utilities
    void displayTable() const;
    void logAction(const string& action);
    void displayActionLog() const;
    void queueEvent(const string& event, int priority);
    void processEvents();
    
    // Menu system
    void displayMenu() const;
    void processMenuChoice(int choice);

    void gameLoop();
    void showWelcomeScreen() const;
    void createPlayerProfile();
    void loadPlayerProfile();
    void displayRules() const;
};


int main(){};
