#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <string>
#include <fstream>
using namespace std;

enum Type { WARRIOR, ARCHER, MAGE }; // Class types

struct Move {
    string name;
    int damage;
    Type type;
};

class Character {
public:
    string name;
    int level = 1;
    double xp = 10;
    int hp;
    Type type;
    Move move;

    int getAtk() const { return 10 + (level - 1); }
    int getDef() const { return 10 + (level - 1); }
    int getMaxHp() const { return 10 + (level - 1); }
    int getXpv() const { return 10 + level * 5; }
};

float getTypeMultiplier(Type attacker, Type defender) {
    if (attacker == WARRIOR && defender == MAGE) return 2.0;
    if (attacker == MAGE && defender == ARCHER) return 2.0;
    if (attacker == ARCHER && defender == WARRIOR) return 2.0;
    return 1.0; // Neutral or disadvantage = normal damage
}

void saveGame(const Character& player) {
    ofstream out("save.txt");
    if (out) {
        out << static_cast<int>(player.type) << " " << player.level << " " << player.xp << endl;
        cout << "Game saved successfully." << endl;
    }
    else {
        cout << "Error: Could not save game to save.txt" << endl;
    }
}

void fight(Character& player, Character& enemy) {
    enemy.level = player.level; // Sync enemy level
    enemy.hp = 8 + enemy.level; // Slightly below player HP
    int playerHp = player.getMaxHp();

    cout << "An enemy " << enemy.name << " (Level " << enemy.level << ") appears!" << endl;
    while (playerHp > 0 && enemy.hp > 0) {
        // Player's turn
        float playerAccuracy = player.getAtk() / (float)(player.getAtk() + enemy.getDef());
        float playerCritChance = min(0.5f, player.getAtk() * 0.01f);
        bool playerHit = ((rand() % (player.getAtk() + 1)) / (float)(player.getAtk() + round(enemy.getDef() * 0.9))) < playerAccuracy;
        if (playerHit) {
            int baseDamage = player.move.damage + (rand() % 3 - 1);
            bool isCrit = (rand() / (float)RAND_MAX) < playerCritChance;
            int damage = max(1, baseDamage - enemy.getDef());
            if (isCrit) {
                damage = damage * 2.5;
                cout << "Critical hit! ";
            }
            damage = damage * getTypeMultiplier(player.move.type, enemy.type);
            enemy.hp -= damage;
            cout << "Your " << player.name << " uses " << player.move.name << "! Deals " << damage << " damage!" << endl;
        }
        else {
            cout << "Your " << player.name << " misses!" << endl;
        }

        // Enemy's turn (if alive)
        if (enemy.hp > 0) {
            float enemyAccuracy = enemy.getAtk() / (float)(enemy.getAtk() + player.getDef());
            float enemyCritChance = min(0.5f, enemy.getAtk() * 0.01f);
            bool enemyHit = ((rand() % (enemy.getAtk() + 1)) / (float)(enemy.getAtk() + round(player.getDef() * 0.9))) < enemyAccuracy;
            if (enemyHit) {
                int baseDamage = enemy.move.damage + (rand() % 3 - 1);
                bool isCrit = (rand() / (float)RAND_MAX) < playerCritChance;
                int damage = max(1, baseDamage - player.getDef());
                if (isCrit) {
                    damage = damage * 2.5;
                    cout << "Critical hit! ";
                }
                damage = damage * getTypeMultiplier(enemy.move.type, player.type);
                playerHp -= damage;
                cout << "Enemy " << enemy.name << " uses " << enemy.move.name << "! Deals " << damage << " damage!" << endl;
            }
            else {
                cout << "Enemy " << enemy.name << " misses!" << endl;
            }
        }

        cout << "Enemy " << enemy.name << " HP: " << enemy.hp << " | Your " << player.name << " HP: " << playerHp << endl;

        // Enemy dies
        if (enemy.hp <= 0) {
            cout << "You defeated Enemy " << enemy.name << "!" << endl;
            cout << "You gained " << enemy.getXpv() << " XP!" << endl;
            player.xp += enemy.getXpv();
            cout << "Your " << player.name << " now has " << round(player.xp) << " XP" << endl;
            playerHp = player.getMaxHp();
            // Level-up logic: check XP threshold for each level
            while (player.xp >= player.level * 100) {
                player.level += 1;
                cout << "Your " << player.name << " leveled up to level " << player.level << "!" << endl;
            }
            saveGame(player); // Save after victory
            break;
        }
        // Player dies
        if (playerHp <= 0) {
            cout << "Your " << player.name << " been defeated!" << endl;
            playerHp = player.getMaxHp(); // Reset HP, no XP loss
            break;
        }

        this_thread::sleep_for(chrono::milliseconds(750));
    }
}

int main() {
    srand(time(0)); // Initialize random seed
    Character player;

    // Load from save file if it exists
    ifstream in("save.txt");
    if (in) {
        int typeInt;
        in >> typeInt >> player.level >> player.xp;
        if (in.fail()) {
            cout << "Error: Corrupted save file. Starting new game." << endl;
            in.close();
            player.level = 1;
            player.xp = 10;
        }
        else {
            player.type = static_cast<Type>(typeInt);
            switch (player.type) {
            case WARRIOR:
                player.name = "Warrior";
                player.move = { "Slash", 10, WARRIOR };
                break;
            case ARCHER:
                player.name = "Archer";
                player.move = { "Arrow Shot", 10, ARCHER };
                break;
            case MAGE:
                player.name = "Mage";
                player.move = { "Fireball", 10, MAGE };
                break;
            default:
                cout << "Error: Invalid character type in save file. Starting new game." << endl;
                player.level = 1;
                player.xp = 10;
                in.close();
                goto new_game;
            }
            cout << "Loaded saved game for " << player.name << " (Level " << player.level << ", XP " << round(player.xp) << ")" << endl;
        }
    }
    else {
    new_game:
        // Player chooses class
        int classChoice;
        do {
            cout << "Choose your class:\n1. Warrior\n2. Archer\n3. Mage\nEnter choice (1-3): ";
            cin >> classChoice;
            if (classChoice < 1 || classChoice > 3) {
                cout << "Invalid choice. Please enter 1, 2, or 3." << endl;
            }
        } while (classChoice < 1 || classChoice > 3);

        switch (classChoice) {
        case 1:
            player.name = "Warrior";
            player.type = WARRIOR;
            player.move = { "Slash", 10, WARRIOR };
            break;
        case 2:
            player.name = "Archer";
            player.type = ARCHER;
            player.move = { "Arrow Shot", 10, ARCHER };
            break;
        case 3:
            player.name = "Mage";
            player.type = MAGE;
            player.move = { "Fireball", 10, MAGE };
            break;
        }
    }
    player.hp = player.getMaxHp();

    while (true) {
        cout << "Your " << player.name << " (Level " << player.level << ")" << endl;
        cout << "HP: " << player.hp << " | Atk: " << player.getAtk() << " | Def: " << player.getDef() << endl;
        cout << "XP: " << round(player.xp) << endl;

        char choice;
        do {
            cout << "1. Fight\n2. Save\nChoose an action: ";
            cin >> choice;
            if (choice != '1' && choice != '2') {
                cout << "Please enter '1' or '2'." << endl;
            }
        } while (choice != '1' && choice != '2');
        if (choice == '2') {
            cout << "Character Saved." << endl;
            saveGame(player); // Save on exit
            break;
        }

        // Random enemy
        Character enemy;
        int enemyType = rand() % 3;
        switch (enemyType) {
        case 0:
            player.name = "Warrior";
            enemy.type = WARRIOR;
            enemy.move = { "Slash", 10, WARRIOR };
            break;
        case 1:
            enemy.name = "Archer";
            enemy.type = ARCHER;
            enemy.move = { "Arrow Shot", 10, ARCHER };
            break;
        case 2:
            enemy.name = "Mage";
            enemy.type = MAGE;
            enemy.move = { "Fireball", 10, MAGE };
            break;
        }

        fight(player, enemy);

        this_thread::sleep_for(chrono::milliseconds(750));
    }

    return 0;
}