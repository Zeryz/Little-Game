#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
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
    Move basicMove;
    Move aoeMove;

    int getAtk() const { return 10 + (level - 1); }
    int getDef() const { return 10 + (level - 1); }
    int getMaxHp() const { return 10 + (level - 1); }
    int getXpv() const { return 10 + level * 5; }
    int getXpForLevel(int level) {
        // Cumulative XP: 100 for level 2, 300 for level 3, 600 for level 4, etc.
        return (level * (level - 1) * 100) / 2;
    }

    void addXp(double amount, int& currentHp) {
        xp += amount;
        cout << "Gained " << amount << " XP! Now at " << round(xp) << " XP." << endl;
        while (xp >= getXpForLevel(level + 1)) {
            level += 1;
            cout << "Your " << name << " leveled up to level " << level << "!" << endl;
        }
        currentHp = getMaxHp(); // Heal to max after potential level up
    }
};

float getTypeMultiplier(Type attacker, Type defender) {
    if (attacker == WARRIOR && defender == MAGE) return 2.0;
    if (attacker == MAGE && defender == ARCHER) return 2.0;
    if (attacker == ARCHER && defender == WARRIOR) return 2.0;
    if (attacker == MAGE && defender == WARRIOR) return 0.5;
    if (attacker == ARCHER && defender == MAGE) return 0.5;
    if (attacker == WARRIOR && defender == ARCHER) return 0.5;
    return 1.0; // Neutral damage
}

void saveGame(const Character& player, int slot) {
    string filename = "save" + to_string(slot) + ".txt";
    ofstream out(filename);
    if (out) {
        out << static_cast<int>(player.type) << " " << player.level << " " << player.xp << endl;
        cout << "Game saved successfully to slot " << slot << "." << endl;
    }
    else {
        cout << "Error: Could not save game to " << filename << endl;
    }
}

void fight(Character& player, vector<Character>& enemies) {
    int playerHp = player.getMaxHp();
    bool isMulti = enemies.size() > 1;
    double xpMultiplier = isMulti ? 0.75 : 1.0;

    cout << "Enemies appear!" << endl;
    for (size_t i = 0; i < enemies.size(); ++i) {
        cout << "Enemy " << (i + 1) << ": " << enemies[i].name << " (Level " << enemies[i].level << ")" << endl;
    }

    while (playerHp > 0 && !enemies.empty()) {
        // Display HPs
        cout << "Enemies: ";
        for (size_t i = 0; i < enemies.size(); ++i) {
            cout << (i + 1) << ". " << enemies[i].name << " HP: " << enemies[i].hp << " | ";
        }
        cout << "Your " << player.name << " HP: " << playerHp << endl;

        // Player's turn
        bool useAoe = false;
        size_t target = 0;
        if (player.level >= 10) {
            cout << "Choose move: 1. " << player.basicMove.name << " (single target) 2. " << player.aoeMove.name << " (hits all)" << endl;
            int moveChoice;
            cin >> moveChoice;
            useAoe = (moveChoice == 2);
        }

        if (useAoe) {
            vector<size_t> toRemove;
            for (size_t i = 0; i < enemies.size(); ++i) {
                Character& enemy = enemies[i];
                float playerAccuracy = player.getAtk() / (float)(player.getAtk() + enemy.getDef());
                float playerCritChance = min(0.5f, player.getAtk() * 0.01f);
                bool playerHit = ((rand() % (player.getAtk() + 1)) / (float)(player.getAtk() + round(enemy.getDef() * 0.9))) < playerAccuracy;
                if (playerHit) {
                    int baseDamage = player.aoeMove.damage + (rand() % 3 - 1);
                    bool isCrit = (rand() / (float)RAND_MAX) < playerCritChance;
                    int damage = max(1, baseDamage - enemy.getDef());
                    if (isCrit) {
                        damage = static_cast<int>(damage * 2.5);
                        cout << "Critical hit on " << enemy.name << "! ";
                    }
                    damage = static_cast<int>(damage * getTypeMultiplier(player.aoeMove.type, enemy.type));
                    enemy.hp -= damage;
                    cout << "Your " << player.name << " uses " << player.aoeMove.name << " on " << enemy.name << "! Deals " << damage << " damage!" << endl;
                }
                else {
                    cout << "Your " << player.name << " misses " << enemy.name << "!" << endl;
                }
                if (enemy.hp <= 0) {
                    cout << "You defeated Enemy " << enemy.name << "!" << endl;
                    player.addXp(enemy.getXpv() * xpMultiplier, playerHp);
                    toRemove.push_back(i);
                }
            }
            // Remove defeated enemies in reverse order
            sort(toRemove.rbegin(), toRemove.rend());
            for (size_t idx : toRemove) {
                enemies.erase(enemies.begin() + idx);
            }
        }
        else {
            if (enemies.size() > 1) {
                cout << "Choose target (1-" << enemies.size() << "): ";
                cin >> target;
                target--; // 0-based
            } // else target = 0
            if (target < enemies.size()) {
                Character& enemy = enemies[target];
                float playerAccuracy = player.getAtk() / (float)(player.getAtk() + enemy.getDef());
                float playerCritChance = min(0.5f, player.getAtk() * 0.01f);
                bool playerHit = ((rand() % (player.getAtk() + 1)) / (float)(player.getAtk() + round(enemy.getDef() * 0.9))) < playerAccuracy;
                if (playerHit) {
                    int baseDamage = player.basicMove.damage + (rand() % 3 - 1);
                    bool isCrit = (rand() / (float)RAND_MAX) < playerCritChance;
                    int damage = max(1, baseDamage - enemy.getDef());
                    if (isCrit) {
                        damage = static_cast<int>(damage * 2.5);
                        cout << "Critical hit! ";
                    }
                    damage = static_cast<int>(damage * getTypeMultiplier(player.basicMove.type, enemy.type));
                    if (player.level >= 10) {
                        damage = static_cast<int>(damage * 3.0); // 3x damage for single-target at level 10+
                        cout << "Enhanced single-target attack! ";
                    }
                    enemy.hp -= damage;
                    cout << "Your " << player.name << " uses " << player.basicMove.name << "! Deals " << damage << " damage!" << endl;
                }
                else {
                    cout << "Your " << player.name << " misses!" << endl;
                }
                if (enemy.hp <= 0) {
                    cout << "You defeated Enemy " << enemy.name << "!" << endl;
                    player.addXp(enemy.getXpv() * xpMultiplier, playerHp);
                    enemies.erase(enemies.begin() + target);
                }
            }
        }

        if (enemies.empty()) {
            cout << "Victory!" << endl;
            break;
        }

        // Enemies' turns
        for (auto& enemy : enemies) {
            float enemyAccuracy = enemy.getAtk() / (float)(enemy.getAtk() + player.getDef());
            float enemyCritChance = min(0.5f, enemy.getAtk() * 0.01f);
            bool enemyHit = ((rand() % (enemy.getAtk() + 1)) / (float)(enemy.getAtk() + round(player.getDef() * 0.9))) < enemyAccuracy;
            if (enemyHit) {
                int baseDamage = enemy.basicMove.damage + (rand() % 3 - 1);
                bool isCrit = (rand() / (float)RAND_MAX) < enemyCritChance;
                int damage = max(1, baseDamage - player.getDef());
                if (isCrit) {
                    damage = static_cast<int>(damage * 2.5);
                    cout << "Critical hit! ";
                }
                damage = static_cast<int>(damage * getTypeMultiplier(enemy.basicMove.type, player.type));
                playerHp -= damage;
                cout << "Enemy " << enemy.name << " uses " << enemy.basicMove.name << "! Deals " << damage << " damage!" << endl;
            }
            else {
                cout << "Enemy " << enemy.name << " misses!" << endl;
            }
            if (playerHp <= 0) {
                cout << "Your " << player.name << " has been defeated!" << endl;
                playerHp = player.getMaxHp(); // Reset HP
                break;
            }
        }

        if (playerHp <= 0) break;

        this_thread::sleep_for(chrono::milliseconds(750));
    }
    if (playerHp > 0) {
        cout << "You won the battle!" << endl;
    }
    else {
        cout << "You lost the battle." << endl;
    }
}

int main() {
    srand(time(0)); // Initialize random seed
    Character player;
    int currentSlot = -1;

    cout << "1. New Game\n2. Load Game\nEnter choice: ";
    int gameChoice;
    cin >> gameChoice;

    if (gameChoice == 2) {
        cout << "Enter slot (1-3): ";
        int slot;
        cin >> slot;
        if (slot < 1 || slot > 3) {
            cout << "Invalid slot. Starting new game." << endl;
            goto new_game;
        }
        string filename = "save" + to_string(slot) + ".txt";
        ifstream in(filename);
        if (in) {
            int typeInt;
            in >> typeInt >> player.level >> player.xp;
            if (in.fail() || player.level < 1 || player.xp < 0) {
                cout << "Error: Corrupted save file. Starting new game." << endl;
                player.level = 1;
                player.xp = 10;
                goto new_game;
            }
            player.type = static_cast<Type>(typeInt);
            switch (player.type) {
                case WARRIOR:
                    player.name = "Warrior";
                    player.basicMove = {"Slash", 10, WARRIOR};
                    player.aoeMove = {"Cleave", 10, WARRIOR};
                    break;
                case ARCHER:
                    player.name = "Archer";
                    player.basicMove = {"Arrow Shot", 10, ARCHER};
                    player.aoeMove = {"Arrow Volley", 10, ARCHER};
                    break;
                case MAGE:
                    player.name = "Mage";
                    player.basicMove = {"Fireball", 10, MAGE};
                    player.aoeMove = {"Arcane Blast", 10, MAGE};
                    break;
                default:
                    cout << "Error: Invalid character type in save file. Starting new game." << endl;
                    player.level = 1;
                    player.xp = 10;
                    goto new_game;
            }
            cout << "Loaded saved game for " << player.name << " (Level " << player.level << ", XP " << round(player.xp) << ", HP " << player.getMaxHp() << ")" << endl;
            currentSlot = slot;
        }
        else {
            cout << "No save in slot " << slot << ". Starting new game." << endl;
            goto new_game;
        }
    }
    else {
new_game:
        // Player chooses class
        int classChoice;
        do {
            cout << "Choose your class:\n1. Warrior\n2. Archer\n3. Mage\nEnter choice (1-3): ";
            while (!(cin >> classChoice)) {
                cout << "Invalid input. Please enter a number (1-3): ";
                cin.clear();
                cin.ignore(10000, '\n');
            }
            if (classChoice < 1 || classChoice > 3) {
                cout << "Invalid choice. Please enter 1, 2, or 3." << endl;
            }
        } while (classChoice < 1 || classChoice > 3);

        switch (classChoice) {
            case 1:
                player.name = "Warrior";
                player.type = WARRIOR;
                player.basicMove = {"Slash", 10, WARRIOR};
                player.aoeMove = {"Cleave", 10, WARRIOR};
                break;
            case 2:
                player.name = "Archer";
                player.type = ARCHER;
                player.basicMove = {"Arrow Shot", 10, ARCHER};
                player.aoeMove = {"Arrow Volley", 10, ARCHER};
                break;
            case 3:
                player.name = "Mage";
                player.type = MAGE;
                player.basicMove = {"Fireball", 10, MAGE};
                player.aoeMove = {"Arcane Blast", 10, MAGE};
                break;
        }
        cout << "Enter slot to save new character (1-3): ";
        int slot;
        cin >> slot;
        if (slot < 1 || slot > 3) {
            cout << "Invalid slot. Using slot 1." << endl;
            slot = 1;
        }
        currentSlot = slot;
        saveGame(player, currentSlot); // Save initial character
    }
    player.hp = player.getMaxHp();

    while (true) {
        cout << "Your " << player.name << " (Level " << player.level << ")" << endl;
        cout << "HP: " << player.hp << " | Atk: " << player.getAtk() << " | Def: " << player.getDef() << endl;
        cout << "XP: " << round(player.xp) << endl;

        char choice;
        do {
            cout << "1. Fight\n2. Save and Exit\n3. Save\nChoose an action: ";
            cin >> choice;
            if (choice != '1' && choice != '2' && choice != '3') {
                cout << "Please enter '1', '2', or '3'." << endl;
            }
        } while (choice != '1' && choice != '2' && choice != '3');
        if (choice == '2') {
            cout << "Character Saved." << endl;
            saveGame(player, currentSlot); // Save on exit
            break;
        }
        if (choice == '3') {
            saveGame(player, currentSlot); // Save without exiting
            continue;
        }

        // Generate enemies
        vector<Character> enemies;
        int numEnemies = (player.level >= 10) ? 2 : 1;
        for (int i = 0; i < numEnemies; ++i) {
            Character enemy;
            int enemyType = rand() % 3;
            switch (enemyType) {
                case 0:
                    enemy.name = "Warrior";
                    enemy.type = WARRIOR;
                    enemy.basicMove = {"Slash", 10, WARRIOR};
                    enemy.aoeMove = {"Cleave", 10, WARRIOR}; // Not used for enemies
                    break;
                case 1:
                    enemy.name = "Archer";
                    enemy.type = ARCHER;
                    enemy.basicMove = {"Arrow Shot", 10, ARCHER};
                    enemy.aoeMove = {"Arrow Volley", 10, ARCHER};
                    break;
                case 2:
                    enemy.name = "Mage";
                    enemy.type = MAGE;
                    enemy.basicMove = {"Fireball", 10, MAGE};
                    enemy.aoeMove = {"Arcane Blast", 10, MAGE};
                    break;
            }
            enemy.level = player.level;
            enemy.hp = 8 + enemy.level;
            enemies.push_back(enemy);
        }

        fight(player, enemies);
        saveGame(player, currentSlot); // Auto-save after fight

        this_thread::sleep_for(chrono::milliseconds(750));
    }

    return 0;
}