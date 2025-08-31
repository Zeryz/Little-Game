#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>
using namespace std;

class Stats {
public:
    double level = 1;
    double xp = 10;

    // Methods to calculate stats dynamically based on level
    int getStr() const { return 10 + (level - 1); }
    int getAtk() const { return 1 + (level - 1); }
    int getDef() const { return 10; }
    int getHp() const { return 10 + (level - 1); }
    int hp = getHp(); // Current HP, initialized to max HP
};

class BadGuy {
public:
    int str = 1;
    int atk = 1;
    int def = 9;
    int hp = 9;
    double xpv = 10;
};

void fight(Stats& stats, BadGuy& badGuy) {

    // Initialize bad guy stats based on player level
    badGuy.hp = 9 + stats.level; // Scale enemy HP with player level
    badGuy.atk = 1;    // Scale enemy attack
    badGuy.def = 9 + stats.level;    // Scale enemy defense
    badGuy.xpv = 10 + stats.level * 4; // Scale XP reward


    while (badGuy.hp > 0 && stats.hp > 0) {
        badGuy.hp -= stats.getAtk();
        stats.hp -= badGuy.atk;

        if (badGuy.hp <= 0) {
            cout << "You win!" << endl;
            cout << "You gained " << round(badGuy.xpv) << " xp" << endl;
            stats.xp += badGuy.xpv;
            cout << "You now have " << round(stats.xp) << " xp" << endl;

            // Level up check
            if (stats.xp >= stats.level * 100) {
                stats.level += 1;
                stats.hp = stats.getHp(); // Reset HP to new max HP
                cout << "You leveled up! You are now level " << round(stats.level) << endl;
            } else {
                stats.hp = stats.getHp(); // Reset HP to current max HP after win
            }

            // Reset and strengthen bad guy for next fight
            badGuy.hp += 9;
            badGuy.atk += 1;
            badGuy.def += 1;
            badGuy.xpv += 4;
            break; // Exit the fight loop to allow a new fight
        }
        if (stats.hp <= 0) {
            cout << "You lose!" << endl;
            break;
        }

        cout << "Enemy Hp: " << badGuy.hp << endl;
        cout << "Your Hp: " << stats.hp << endl;
        this_thread::sleep_for(chrono::milliseconds(550));
    }
}

int main() {
    Stats stats;
    BadGuy badGuy;

    // Main game loop
    while (true) {
        // Display stats
        cout << "Str: " << stats.getStr() << endl;
        cout << "Atk: " << stats.getAtk() << endl;
        cout << "Def: " << stats.getDef() << endl;
        cout << "Hp: " << stats.hp << endl;
        cout << "Level: " << round(stats.level) << endl;
        cout << "Xp: " << round(stats.xp) << endl;

        // Start a fight
        fight(stats, badGuy);

        // Check if player lost
        if (stats.hp <= 0) {
            break;
        }

        // Optionally, ask if the player wants to continue
        //cout << "Fight again? (y/n): ";
        //char choice;
        //cin >> choice;
        //if (choice != 'y' && choice != 'Y') {
        //    break;
        //}
    }

    return 0;
}