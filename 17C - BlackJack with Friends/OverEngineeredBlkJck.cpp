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
    Card operator+(const Card& other) const {
        return Card();
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
                cards.push_back(Card(iterSuits, iterRanks));
            }
        }

    }
    // Deck operations
    
    //One a one card per deal basis, to make things easier later.
    //Obviously, standard dealing will start with two cards, easy loops.
    void shuffleDeck() {
        /*
        a key issue that needs to be addressed is a lists inability to iterate randomly
        hence, a custom shuffle algorithm needs to be used. 
        */
        random_device rd;
        mt19937 gen(rd());
        int n = distance(cards.begin(), cards.end());
        auto it = cards.begin();
        for (int i = n - 1; i > 0; --i) {
            uniform_int_distribution<> distrib(0, i);
            int j = distrib(gen);
            auto it_j = cards.begin();
            std::advance(it_j, j);
            std::iter_swap(it, it_j);
            std::advance(it, 1);
        }
    }
    Card deal(){
        Card dealtCard = cards.front();
        cards.pop_front();
        return dealtCard;
    }
    //After rounds, add card or cards to discardPile stack.
    void addToDiscardPile(const Card& card){
        discardPile.push(card);
    }
    //Resets discardPile deck back into existing card deck.
    void retrieveCardsFromDiscardPile(){
        while(!discardPile.empty()){
            cards.push_back(discardPile.top());
            discardPile.pop();
        }
    }
    //Simple getter to check if deck has been emptied.
    bool isEmpty() const{
        if (cards.empty()){
            return true;
        }
        return false;
    }
    int cardsRemaining() const{
        return static_cast<int>(cards.size());
    }

    //For future bot logic, action logs, game flow, etc.
    void toString() const{
        cout << "Deck size is: " << cards.size() << endl;
        cout << "Discard Pile size is " << discardPile.size() << endl;
    }
};


class Hand {
protected:
    deque<Card> hand_cards;
    
public:
    // Constructor, but hands are dealt empty as per game logic.
    Hand(){}
    // Hand operations
    void add(const Card& card){
        hand_cards.push_back(card);
    }
    void clear() {
        hand_cards.clear();
    }

    /*
        getTotal() collects and returns the total value of the hand (obviously to use in the blackjack
        determinant).

        int aces keeps track of all aces to be processed differently as per game logic. 
        cards are ranked by either ace, face card, or numerical card, and stored as such with a point value

        for loop from lines 212 to 218 treat aces based on if they would cause the hand to be bust,
        which the player obviously did not intend to do. so, if the total plus the ace is less than or equal
        to 21, we just add it as 11. otherwise, it's just a one, yay!
    */

    int getTotal() const{

        int total = 0;
        int aces = 0;

        for (const auto& card : hand_cards){

            string rank = card.getRank();
            if (rank == "Ace") {
                aces++;
            }
            else if (rank == "King" || rank == "Queen" || rank == "Jack") {
                total += 10;
            }
            else {
                total += stoi(rank);
            }
        }

        for (int i = 0 ; i < aces ; i++){
                if (total + 11 <= 21) {
                    total += 11;
                }
                else {
                    total += 1;
                }

        }

        return total;
    }
    /*
        basic getter functions for game logic.
    */
    bool isBusted() const {
        
        if (getTotal() > 21) {
            return true;
        }

        return false;
    }
    bool isBlackjack() const {

        if (hand_cards.size() == 2 && getTotal() == 21) {
            return true;
        }

        return false;
    }

    /*

        random access iterators used for specific object class
        to be extended to all other classes utilizing card structure iteration

    */
    deque<Card>::iterator begin() {
        return hand_cards.begin();
    }
    
    deque<Card>::iterator end() {
        return hand_cards.end();
    }
    deque<Card>::const_iterator begin() const {
        return hand_cards.cbegin();
    }
    
    deque<Card>::const_iterator end() const {
        return hand_cards.cend();
    }


    void toString() const{

        if (hand_cards.empty()){
            cout << "Hand's empty...";
        }

        for (const auto& card : hand_cards){
            if (card.isFaceUp()) {
                cout << card.getRank() << " of " << card.getSuit() << ", ";
            }
            else{
                cout << "[FACED DOWN], ";
            }
        }

        cout << "Total: " << getTotal() << " " << endl;
    }
};


class Player {
protected:
    string m_name;
    Hand m_hand;
    int m_money;
    int m_bet;
    
public:
    // Constructor/Destructor
    Player(const string& name = "Player", int money = 1000): m_name(name), m_money(money), m_bet(0){}
    
    // Getters
    string getName() const{
        return m_name;
    }
    int getMoney() const {
        return m_money;
    }
    int getBet() const{
        return m_bet;
    }
    const Hand& getHand() const{
        return m_hand;
    }
    Hand& getHandRef(){
        return m_hand;
    }
    
    /*
        placeBet() verifies that the bet places is a legitmate amount, while also making sure the player
        isn't spending more money than they currently have.

        if the conditional is true, the bet amount is updated and the amount parsed is removed from the
        players account/wallet/game balance. 

        good for game logic verification in gameloop()
    
    */
    bool placeBet(int amount){
        if (amount <= 0 || amount > m_money){
            return false;
        }

        m_bet = amount;
        m_money -= amount;
        return true;
    }
    void win(){
        m_money += (m_bet * 2);
        m_bet = 0;
    }
    void lose(){
        m_bet = 0;
    }
    void push(){
        m_money += m_bet;
        m_bet = 0;
    }
    
    /*
    
        isHitting() isn't native to player class, as the dealer will have some logic
        noting if it's going to stand or hit. 
        
        for this instance, the player (at their given turn), will be prompted to hit 
        or not, returning a boolean value.  
    
    */
     bool isHitting(){

        char pChoice;
        cout << m_name << ", " << "do you want to hit? (y/n)";
        cin >> pChoice;
        /*
            effectively ignores all inputs until delimiter '\n'
        */
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        return (pChoice == 'y' || pChoice == 'Y');

     }
     void showHand(bool showFirstCard = true) const{

        cout << m_name << "s hand: ";
        m_hand.toString();
        cout << endl;
        
     }
    
    /*
     status checks specific to player variables. better to overwrite same function
     per class (Write Once Use Anywhere). plus, Player class doesn't have direct access to
     deck/hand class functions, so accessors are needed.
    */
    bool isBusted() const{
        return m_hand.isBusted();
    }
    bool isBlackjack() const{
        return m_hand.isBlackjack();
    }
};


class Dealer : public Player {
private:
    Deck m_deck;
    
public:
    // Constructor
    Dealer(const string& name = "The Dealer"){};
    /*
    unlike the player function couterpart, we need to apply some sort of basic logic to
    the dealer. based on casino rules, the dealer MUST hit on a total sum of 16 or less,
    and stand on a hand of total 17 or more. 
    
    this saves us a lot of unneccessary bot logic as now all we need is to determine the card
    total.
    */
    bool isHitting() const{
        if (m_hand.getTotal() < 17){
            return true;
        }
        return false;
    }
    /*
    
    as per game rules, the dealer only shows one card until the end of the sequence, and then
    reveals their hand.

    */
    void showHand(bool showFirstCard = true) const{
        if (showFirstCard){
            cout << m_name << "'s hand: ";
            m_hand.toString();
            cout << endl;
        }else{
            cout << m_name << "'s hand: [FACED DOWN], ";
        }
        /*
            Set an auto variable to the beginning of the hand, and fetch the rank and respective
            suit of the card while it iterates until it does not equal the end value of the hand.
        */
       auto it = m_hand.begin();
       it++;
        for (; it != m_hand.end() ; it++) {
            const Card& card = *it;
            cout << card.getRank() << " of " << card.getSuit() << ", ";
        }
    }
    
    /*
        overwritten deck operations, in the event of multiple decks, or just dealer shuffle,
        functions will be used. 
    */
    void shuffleDeck(){
        m_deck.shuffleDeck();
    }
    Card deal(){
        return m_deck.deal();
    }
    bool deckIsEmpty() const{
        return m_deck.isEmpty();
    }
};


class GameStats {
private:
    unordered_map<string, pair<int, int>> m_playerStats; // <name, <wins, losses>>
    set<pair<int, string>> m_highScores; // <money, name>
    
public:

    GameStats(){};
    
    void recordWin(const string& playerName){
        m_playerStats[playerName].first++;
    }
    void recordLoss(const string& playerName){
        m_playerStats[playerName].second++;
    }

    /*

        Updates high score by iterating through the set of highscores, and if a players score is found
        erase it for updating with a new entry with the new money and new name.
    
    */
    void updateHighScore(const string& playerName, int money){
        for (auto x = m_highScores.begin() ; x != m_highScores.end();){
            if (x->second == playerName){
                x = m_highScores.erase(x);
            }
            else{
                x++;
            }
        }

        m_highScores.insert(make_pair(money, playerName));
    }
    
    // Getters
    int getWins(const string& playerName) const{
        auto gW = m_playerStats.find(playerName);
        if (gW != m_playerStats.end()){
            return gW->second.first;
        }
        return 0;
    }
    int getLosses(const string& playerName) const{
        auto gL = m_playerStats.find(playerName);
        if (gL != m_playerStats.end()){
            return gL->second.second;
        }
        return 0;
    }
    double getWinRate(const string& playerName) const{
        auto gWR = m_playerStats.find(playerName);
        if (gWR != m_playerStats.end()){
            int wins = gWR->second.first;
            int losses = gWR->second.second;
            int total = wins + losses;
            if (total > 0){
            return static_cast<double>(wins) / total;
            }
        }
        return 0.0;
    }
    
    // Display
    void displayStats() const{
        cout << "\n===== PLAYER STATS =====\n";

        if (m_playerStats.empty()){
            cout << "No information available yet...go play! \n";
            return;
        }
        cout << setw(15) << left << "Player"
             << setw(10) << right << "Wins" 
             << setw(10) << "Losses" 
             << setw(10) << "Win Rate" << endl;
        cout << "===========================================" << endl;

        for (const auto& entry : m_playerStats){
            string name = entry.first;
            int wins = entry.second.first;
            int losses = entry.second.second;
            double winRate = 0;

            if (wins + losses > 0) {
                winRate = static_cast<double>(wins) / (wins + losses) * 100.0;
            }
            cout << setw(15) << left << name 
                 << setw(10) << right << wins 
                 << setw(10) << losses 
                 << setw(9) << fixed << setprecision(1) << winRate << "%" << endl;
        }
        
    }

    void displayHighScores(int top = 5) const{

        cout << "\n===== HIGH SCORES =====\n";

        if (m_highScores.empty()){
            cout << "No highscores available yet...go win! (or don't)\n";
            return;
        }
        cout << setw(15) << left << "Rank"
             << setw(10) << right << "Player" 
             << setw(10) << "Money" << endl;
        cout << "===========================================" << endl;

        int r = 1;
        for (auto dS = m_highScores.rbegin() ; dS != m_highScores.rend() ; dS++, r++) {
            cout << setw(5) << right << r << "."
            << setw(15) << left << dS->second
            << "$" << setw(9) << right << dS->first << endl;
        }
    }
};


class Game : public GameStats{
public:
    /*
    used to make a COPY of the player to use during run.
    */
    deque<unique_ptr<Player>> m_players;
    Dealer m_dealer;
    GameStats m_stats;
    queue<string> m_actionLog;
    priority_queue<pair<int, string>> m_events;
    
    /*
    
    flag class for game flow logic, will be of help with flow chart design and game loop.
    
    */
    enum class GameState { BETTING, DEALING, PLAYER_TURN, DEALER_TURN, PAYOUT, CLEANUP };
    GameState m_currentState;
    
    // Maps for card values and suits
    map<string, int> m_cardValues;
    map<string, string> m_suitNames;
    
    Game() : m_currentState(GameState::BETTING){
        m_cardValues["2"] = 2;
        m_cardValues["3"] = 3;
        m_cardValues["4"] = 4;
        m_cardValues["5"] = 5;
        m_cardValues["6"] = 6;
        m_cardValues["7"] = 7;
        m_cardValues["8"] = 8;
        m_cardValues["9"] = 9;
        m_cardValues["10"] = 10;
        m_cardValues["Jack"] = 10;
        m_cardValues["Queen"] = 10;
        m_cardValues["King"] = 10;
        m_cardValues["Ace"] = 11; 

        //suits for game flow
        m_suitNames["HEARTS"] = "HEARTS";
        m_suitNames["DIAMONDS"] = "DIAMONDS";
        m_suitNames["CLUBS"] = "CLUBS";
        m_suitNames["SPADES"] = "SPADES";
    }
    
    // Player management
    void addPlayer(const Player& name){
        m_players.push_back(make_unique<Player>(name));
        logAction(name.getName() + " joined the game.");
    }
    /*
        use find_if function to search through the deque of unique pointers and return the player pointer if
        the getName() matches the targeted name. afterwards, check to see where the returned player is, and remove it
        log against for better game flow.
    */
    void removePlayer(const string& name){
        auto rP = find_if(m_players.begin(), m_players.end(), [&name](const unique_ptr<Player>&p) 
        {return p->getName() == name;}
        );

        if (rP != m_players.end()){
            logAction((*rP)->getName() + "has forfeited.");
            m_players.erase(rP);
        }
    }


    
    // magnum opus
    void play(){
        if (m_players.empty()){
            cout << "No players at table.\n";
            return;
        }

        m_dealer.shuffleDeck();
        logAction("Dealer shuffles the deck.");

        bool contPlay = true;
        while(contPlay){
            setState(GameState::BETTING);
            placeBets();

            setState(GameState::DEALING);
            deal();

            setState(GameState::PLAYER_TURN);
            playerTurns();

            setState(GameState::DEALER_TURN);
            dealerTurn();

            setState(GameState::PAYOUT);
            payouts();

            setState(GameState::CLEANUP);
            cleanup();
            findMinMaxMoney();

            cout << "\nContinue playing? (y/n):" ;
            char gChoice;
            cin >> gChoice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            contPlay = (gChoice == 'y' || gChoice == 'Y');
        }

    }
    void placeBets(){
        cout << "\n===== PLACING BETS =====\n";
        for (auto& p : m_players){
            int money = p->getMoney();
            cout << p->getName() << " , you've got $" << money << ". Place your bet: $";

            int bet;
            cin >> bet;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            while (!p->placeBet(bet)){
                cout << "Invalid bet. You only have $" << money << ". Place your bet: $";
                cin >> bet;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }

            logAction(p->getName() + " bet $" + to_string(bet));
        }
    }

    void deal(){
        cout << "\n===== DEALING CARDS =====\n";
        for (auto& p : m_players){
            p->getHandRef().clear();
        }
        m_dealer.getHandRef().clear();

        /*
            while performance wise unconventional, i want to stick to real game flow standard
            usually the deal is done one and a time iteratively, so that's what i'm going with here
            and probably for any other game related instance.
        */

        for(auto& p : m_players){
            p->getHandRef().add(m_dealer.deal());
        }
        m_dealer.getHandRef().add(m_dealer.deal());

        for(auto& p : m_players){
            p->getHandRef().add(m_dealer.deal());
        }
        m_dealer.getHandRef().add(m_dealer.deal());

        displayTable();

    }
    void playerTurns(){
        cout << "\n===== DEALING CARDS =====\n";

        for (auto& p : m_players){
            cout << "\n" << p->getName() << "'s turn: \n";
            p->showHand();


            if (p->isBlackjack()){
                cout << "Blackjack! " << p->getName() << " stands.\n";
                logAction(p->getName() + " got a blackjack!");
                continue;
            }

            while (!p->isBusted() && p->isHitting()){
                Card nC = m_dealer.deal();
                cout << p->getName() << " receives: " << nC.getRank() << " of " << nC.getSuit() << endl;
                p->getHandRef().add(nC);


                if (p->isBusted()){
                    cout << p->getName() << " busts with " << p->getHand().getTotal() << "!\n";
                    logAction(p->getName() + " busted with " + to_string(p->getHand().getTotal()));
                }
                else{
                    cout << p->getName() << " has " << p->getHand().getTotal() << ".\n";
                }
            }
            if (!p->isBusted()){
                cout << p->getName() << " stands with " << p->getHand().getTotal() << ".\n";
                logAction(p->getName() + " stands with " + to_string(p->getHand().getTotal()));
            }
        }
    }
    void dealerTurn(){
        cout << "\n===== DEALERS TURN =====\n";
        bool cleanSweep = true;
        for (auto& p : m_players){
            if (!p->isBusted()){
                cleanSweep = false;
                break;
            }
        }

        m_dealer.showHand(true);

        if (cleanSweep){
            cout << "Wow! All players have busted, the Dealer wins!\n";
            logAction("All players busted, dealer wins");
            return;
        }

        while (m_dealer.isHitting()){
            Card nC = m_dealer.deal();
            cout << "Dealer recieves: " << nC.getRank() << " of " << nC.getSuit() << endl;
            m_dealer.getHandRef().add(nC);
            cout << "Dealer has " << m_dealer.getHand().getTotal() << ".\n";
        }

        if(m_dealer.isBusted()){
            cout << "Dealer busts with " << m_dealer.getHand().getTotal() << "!\n";
            logAction("Dealer busted with " + to_string(m_dealer.getHand().getTotal()));
        }
        else{
            cout << "Dealer stands with " << m_dealer.getHand().getTotal() << "!\n";
            logAction("Dealer stands with " + to_string(m_dealer.getHand().getTotal()));
        }

    }
    /*
    
        payout function based on hand totals and if player or dealer has achieved a blackjack.
        basic if else ladder logic to determine how much money is either sent out, lost, or pushed
        back to the players and dealer in the event of a tie.
    
    */
    void payouts(){
        cout << "\n===== RESULTS =====\n";

        int dT = m_dealer.getHand().getTotal();
        bool dB = m_dealer.isBusted();
        bool dBJ = m_dealer.isBlackjack();

        for (auto& p : m_players){

            string name = p->getName();
            int pT = p->getHand().getTotal();
            bool pB = p->isBusted();
            bool pBJ = p->isBlackjack();

            cout << name << ": ";

            if (pB){
                p->lose();
                m_stats.recordLoss(name);
                cout << "Busted and lost $" << p->getBet() << ".\n";
                logAction(name + "lost $" + to_string(p->getBet()));
            }
            else if (dB){
                p->win();
                m_stats.recordWin(name);
                cout << "Won $" << p->getBet() << " (dealer busted).\n";
                logAction(name + " won $" + to_string(p->getBet()) + " (dealer busted)");
            }
            else if(pBJ && !dBJ){
                int wins = static_cast<int>(p->getBet() * 1.5);
                m_stats.recordWin(name);
                cout << "Blackjack! Won $" << wins << ".\n";
                logAction(name + "won $" + to_string(wins) + "with blackjack.");
                p->win();
            }
            else if(!pBJ && dBJ){
                p->lose();
                m_stats.recordLoss(name);
                cout << "Lost $" << p->getBet() << " the dealer's blackjack. \n";
                logAction(name + "lost $ " + to_string(p->getBet()) + " to dealer's blackjack");
            }
            else if (pT > dT){
                p->win();
                m_stats.recordWin(name);
                cout << "Won $ " << p->getBet() << " with " << pT << " over dealer's " << dT << ".\n";
                logAction(name + "won $" + to_string(p->getBet()));
            }
            else if (pT < dT){
                p->lose();
                m_stats.recordLoss(name);
                cout << "Lost $" << p->getBet() << " with " << pT << " under dealer's " << dT << ".\n";
                logAction(name + " lost $" + to_string(p->getBet())); 
            }
            else{
                p->push();
                cout << "Push. Bet of $" << p->getBet() << " returned.\n";
                logAction(name + " pushed with dealer at " + to_string(pT));
            }

            m_stats.updateHighScore(name, p->getMoney());

        }
    }
    void cleanup(){
        auto rP = m_players.begin();
        while (rP != m_players.end()){
            if ((*rP)->getMoney()<=0){
                cout << (*rP)->getName() << " is out of money and forfeits the game.\n";
                logAction( (*rP)->getName() + " left the game (out of money)");
                rP = m_players.erase(rP);
            }
            else{
                rP++;
            }
        }
        processEvents();

        m_stats.displayStats();
        m_stats.displayHighScores();
    }

    /*
        this function stays OUT of the playerstats class because it's to be used in between rounds of blackjack.
    */
    void findMinMaxMoney(){
        if (m_players.empty()){
            cout <<"No players at table to value!\n";
            return ;
        }

        int min = numeric_limits<int>::max();
        int max = 0;
        string richest = "";
        string poorest = "";

        for (const auto& p : m_players){
            if(p->getMoney() > max) {
                richest = p->getName();
                max = p->getMoney();
            }

            if (p->getMoney() < min){
                poorest = p->getName();
                min = p->getMoney();
            }
            
        }

        cout << "\n=====PLAYER MONEY STATS=====\n";
        cout << "Current Top Earner: " << richest << " with $" << max << endl;
        cout << "Current Low Earner: " << poorest << " with $" << min << endl;
    }
    
    // Game state management
    void setState(GameState state){
        m_currentState = state;
    }
    GameState getState() const{
        return m_currentState;
    }
    
    // Game utilities
    void displayTable() const{
        cout << "\n===== TABLE STATUS =====\n";
        m_dealer.showHand(false);
        for (const auto& p: m_players){
            cout << p->getName() << " ($" << p->getMoney() << ", bet: $" << p->getBet() << "): ";
            p->getHand().toString();
            cout << endl;
        }
    }
    void logAction(const string& action){
        m_actionLog.push(action);
    }
    void displayActionLog() const{
        cout << "\n===== ACTION LOG =====\n";

        if (m_actionLog.empty()){
            cout << "No actions currently logged.\n";
            return ;
        }
    }
    void queueEvent(const string& event, int priority){
        m_events.push(make_pair(priority, event));
    }
    void processEvents(){
        while (!m_events.empty()){
            cout << "EVENT: " << m_events.top().second << endl;
            m_events.pop();
        }
    }
    
    // Menu system
    void displayMenu() const{
        cout << "\n=====MENU=====\n";
        cout << "\n1. Play Game\n";
        cout << "\n2. Create Player Profile\n";
        cout << "\n3. Load Player Profile\n";
        cout << "\n4. View Stats\n";
        cout << "\n5. View High Scores\n";
        cout << "\n6. Quit Game\n";
    }
    void showWelcomeScreen() const{
        cout << "\n=====WELCOME TO BLACKJACK! WITH FRIENDS=====\n";
        cout << "\n=====Your goal as the player is to try and beat the dealer by getting as close to 21 without going over!\n";
        cout << "\n=====Each player starts with two cards. The dealer's first card is hidden.\n";
        cout << "\n=====Each player also must decide to hit (take another card), or stand (stop taking cards).\n";
        cout << "\n=====The dealer will play against you! With some mental math and a little bit of luck, you could get rich!\n";
        cout << "\n=====To get started, each of you must create a player profile with your name!\n";
        cout << "\n=====Good luck, and have fun!\n" << endl;
    }
    void createPlayerProfile(){
        string newName = "";
        int nMoney = 1000;

        cout << "Enter your name:";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, newName);

        auto alrExist = find_if(m_players.begin(), m_players.end(), [&newName](const unique_ptr<Player>&p){
            return p->getName() == newName;
        });  

        if (alrExist != m_players.end()){
            cout << "Player already exists." << endl;
            return;
        }

        cout << "\nCreating new profile for " << newName << " with $ " << nMoney << " to start with." << endl;

        Player nP(newName, nMoney);
        addPlayer(nP);
        
        cout << "\nPlayer profile created successfully!\n" << endl;
    }
    
    void loadPlayerProfile(){
        string pName;

        cout << "Enter your profile name: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, pName);

        auto alrExists = find_if(m_players.begin(), m_players.end(), [&pName](const unique_ptr<Player>& p) {
             return p->getName() == pName; 
        });

        if (alrExists != m_players.end()) {
        cout << "Welcome back, " << pName << "!\n";
        cout << "Current balance: $" << (*alrExists)->getMoney() << "\n";
        
        // Display player stats
        cout << "Wins: " << m_stats.getWins(pName) << "\n";
        cout << "Losses: " << m_stats.getLosses(pName) << "\n";
        cout << "Win rate: " << fixed << setprecision(1) << (m_stats.getWinRate(pName) * 100) << "%\n";
        } else {
            cout << "Player profile not found. Create a new profile? (y/n): ";
            char choice;
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
            if (choice == 'y' || choice == 'Y') {
            createPlayerProfile();
            }
        }
    }
};


int main() {
    Game gameinst;
    bool exit = false;
    while (!exit){

        int decision;
        gameinst.displayMenu();
        cin >> decision;
        switch(decision){
            case 1: {
            gameinst.play();
            break;
            }
            case 2: {
                gameinst.createPlayerProfile();
                break;
            }
            case 3:{
                gameinst.loadPlayerProfile();
                break;
            }
            case 4:{
                gameinst.displayStats();
                break;
            }
            case 5:{
                gameinst.displayHighScores();
                break;
            }
            case 6:{
                exit = true;
                break;
            }
            default:{
                cout << "Invalid Input." << endl;
                break;
            }
        }
    }
    cout << "Goodbye!" << endl;
    return 0;
}
