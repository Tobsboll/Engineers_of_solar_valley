//
//  Space.hpp
//  Engineers of Solar valley
//  Amateur clone of Asteroids(1979) created by Tobias Fjellström

#ifndef Space_hpp
#define Space_hpp

#include"Object.hpp"
#include<iostream> // REMOVE after testing

class Space{
public:
    Space(int numPlayers);
    void update(); // float wx, float wy
    void draw();//sf::RenderWindow& window);
    void generateAsteroids(int num, float speed, sf::Vector2f old_centre, int old_type, sf::Vector2f odxy, int generation);
    bool clear();
    void levelUp();
    sf::RenderWindow window;
private:
    // "Global" game objects
    std::vector<std::unique_ptr<Ship>> Players; // Players owns public Ship owning vector of Bullet.
    std::vector<std::unique_ptr<Ship>> explodingPlayers;
    std::vector<std::unique_ptr<Asteroid>> Asteroids;
    std::vector<std::unique_ptr<Asteroid>> deadAsteroids;
    std::vector<std::unique_ptr<Star>> Stars;
    int level = 0;
    int wx;
    int wy;
    std::mt19937 gen;
    std::uniform_int_distribution<> dis;
    std::uniform_int_distribution<> min_dis;
    sf::Font font;
    std::vector<sf::Text> text;
    sf::Text text1;
    sf::Text text2;
    
    // Private functions
    void checkColl(); // Checks collision between all objects
                        // make smarter at later point...
};

#include <stdio.h>

#endif /* Space_hpp */
