//
//  Object.hpp
//  Engineers of Solar valley
//
//  Created by Tobias Fjellström on 2018-11-05.
//  Copyright © 2018 Solvind Studio. All rights reserved.
//

#ifndef Object_hpp
#define Object_hpp
#include <SFML/Graphics.hpp>
//#include <SFML/System.hpp>
#include "ResourcePath.hpp"
//#include <math.h>
#include<vector> // Was intended only in slot_map...
#include<memory>
#include<random>
#include "Utility.hpp"

#include <stdio.h>

#define PI 3.14159265

class Object
{
protected:
    int hp;
    int x;
    int y;
    float dx;
    float dy;
    float maxSpeed = 70;
    float rotSpeed = 10;
    float a; // angle
    int anim_Count = 0;
    bool thrust = 0;
    float radius;
    sf::Texture txt;
    sf::Sprite sheet; // sprite sheet
    sf::IntRect sRect; // rect for area selection on s-sheet
    //sf::Drawable hitbox; // En osynlig form som kan användas för att checka kollision osv. Koda som synlig först, så vi kan kolla vad den gör.
    int gridNum; // Number to reduce # of coll. checks.
    // Slot_map <Object>, Bullets för ships och aLiens, kanske mindre stenar för asteroids?
    float scale;
public:
    bool dead(); // returns if hp == 0
    sf::Sprite& getSheet(); // return a reference to the sheet for collision checking
    int ID;  // ID to identify object in list.
    void setHp(int New); // Ändra Hp till New
    void draw(sf::RenderTarget& window);
    virtual void animate(sf::RenderTarget& window) = 0; // Sätter sRect passande runt sheet beroende på tid
    sf::FloatRect getBounds();
    int getRad(); // get radius of asteroid
    sf::Vector2f getSpeed(); // Get speed of object
};

//-----------------------
//      Star
//-----------------------

class Star: public Object
{
    // Class for painting background, should twinkle when animated.
public:
    Star(int x, int y, int type, float size, float a_timer); // int wx, int wy,
    void animate(sf::RenderTarget& window);
    void move(float dx, float dy, int wx, int wy);
private:
    int type = 0; // Sets colour and size
    sf::Clock clock;
    float a_timer = 2; 
};

//-----------------------
//      Bullet
//-----------------------

class Bullet: public Object
{
public:
    Bullet(float x, float y, float angle);
    void animate(sf::RenderTarget& window);
    void move(float wx, float wy);
    sf::Time lifetime;
    sf::Vector2f getTip(bool aft); // coordinate to tip of bullet, or if aft=true to aft of bullet
    int getRad();
   // void draw(sf::RenderTarget &window);
private:
    sf::Clock clock;
};

//-----------------------
//      Asteroid
//-----------------------

class Asteroid: public Object {
public:
    Asteroid(int x, int y, int type, int size, float dx, float dy, int iID);
    void animate(sf::RenderTarget& window);
    void exp(); // Sets explosion animation and then sets hp = 0 to be removed from vector
    void move(int wx, int wy);
    sf::Vector2f centre(); // coordinate to centre of asteroid
    int getType();
private:
    sf::Clock clock;
    float a_timer = 0.1; // Timer for animation
    float exp_timer = 0.02; // Timer for explosion animation
    int type; // sets colour of Asteroid
    bool explode = false;
};

//------------------------
//      SHIP
//------------------------

class Ship: public Object {
    // För att inte bli virtuell måste jag overrida de virtuella funktionerna, samt tillhandahålla en konstruktor.
public:
    Ship(float wx, float wy, int ID);
    void animate(sf::RenderTarget& window);
    void move(float wx, float wy, std::vector<std::unique_ptr<Star>>& Stars);
    void draw(sf::RenderTarget& window);
    sf::Vector2f getTip();      // coordinate to tip of ship
    sf::Vector2f getAft(int left_right); // coordinate of back corner
    bool checkColl(sf::Vector2f ast_centre, int ast_rad);
    sf::Clock inv_clock;        // Clock to track invincibility upon spawning
    const float inv_time = 3;   // Timer for spawn invincibility
    void hit(); // decrement hp
    void score(); // give 10 p
    int getPoints(); // get number of points
    void exp(); // set explosion animation in motion
    bool explode = false;
    void resetPos(sf::RenderTarget& window); // reset coordinates
private:
    void healthBar(sf::RenderTarget& window);
    std::vector<std::unique_ptr<Bullet>> sub;// vector stores shared pointers to Bullets
    sf::Clock timer;
    sf::Clock anim_clock;
    sf::Clock exp_clock;
    sf::Clock blink_clock;
    float exp_timer = 0.15;
    float a_timer = 0.05;
    float cooldown = 0.2;
    const float charge = 0.2; // recharge time for bullets
    sf::Vector2f tip;
    int anim_row;
    sf::Sprite health_sheet;
    int points;
    float acc = 0.75;
    float blink_time = 0.35;
    bool blink = false;
};

#endif /* Object_hpp */
