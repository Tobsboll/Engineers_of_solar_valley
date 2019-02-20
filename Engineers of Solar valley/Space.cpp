//
//  Space.cpp
//  Engineers of Solar valley
//  Amateur clone of Asteroids(1979) created by Tobias Fjellström

#include "Space.hpp"
#include <random>

//------------ CONSTRUCTOR--------------

Space::Space(int numPlayers): gen((std::random_device())()), dis(1, 9999), min_dis(-100, 100), window(sf::VideoMode(1280, 720), "Engineers of Solar valley") // initialization list, initializes before etnering constructor body, important!
{
    wx = window.getSize().x;
    wy = window.getSize().y;
    
    // Create stars
    // Make random numbers for star init.
    int sx;
    int sy;
    int st;
    float ss;
    float sa;
    
    for (int j = 0; j<100;j++)
    {
        // Get random starting coordinate
        sx = dis(gen) % wx+1; // x
        sy = dis(gen) % wy+1; // y
        st = dis(gen) % 2 +1; // t
        ss = 0.005*(dis(gen) % 100 +1); // size
        sa = dis(gen) % 8 + 1; // animation timer
        auto star = std::unique_ptr<Star> (new Star(sx,sy,st,ss,sa)); // allocate and push back pointer to star
        Stars.push_back(std::move(star));
    }
    
    // Initialize players
   for (int i = 0; i < numPlayers;i++)
   {
       auto ship = std::unique_ptr<Ship> (new Ship((wx/(numPlayers+1)), wy, i+1));
       Players.push_back(std::move(ship));
   }
    
    // Initialize text for point rendering
    if (!font.loadFromFile(resourcePath()+"MonospaceBold.ttf"))
    {
        std::cout << "Font not loaded!";
    }
    text1.setFont(font);
    text2.setFont(font);
    text1.setCharacterSize(24);
    text2.setCharacterSize(24);
    text1.setPosition(wx-160, 60);
    text2.setPosition(40, 60);
    
}

//------------- FUNCTIONS ----------------

void Space::update() //float wx, float wy
{
    // Perform movement of all objects
    for (int i = 0; i<Players.size();i++)
    {
        // Move players, bullets and stars (background)
        Players[i]->move(wx, wy, Stars);
    }
    // Move asteroids
    for (int j = 0; j<Asteroids.size();j++)
    {
        Asteroids[j]->move(wx, wy);
    }
    for (int p = 0; p<deadAsteroids.size();p++)
    {
        deadAsteroids[p]->move(wx, wy);
    }
    
    for (int k=0;k<deadAsteroids.size();k++) // make new asteroids when the ones in dead-vector have animated all frames
    {
        if (deadAsteroids[k]->dead()==true)
        {
            if (deadAsteroids[k]->ID<3) // Generate children of asteroids not already twice respawned
            {
                int num = dis(gen) % 2 +2; // Generate random number of smaller asteroids!
                int newGen = deadAsteroids[k]->ID; // increase the generation number of Asteroid
                newGen++;
                generateAsteroids(num, 0.05, deadAsteroids[k]->centre(), deadAsteroids[k]->getType(), deadAsteroids[k]->getSpeed(),newGen);
            }
            deadAsteroids.erase(deadAsteroids.begin()+k); // remove dead asteroid
        }
    }
    
    // Check for collsions and remove dead objects
    checkColl();
    
}

void Space::draw()
{
    // Draw Stars
    for (int k = 0; k<Stars.size();k++)
    {
        Stars[k]->draw(window);
    }
    
    // Draw asteroids
    for (int j = 0; j<Asteroids.size();j++)
    {
        //if (Asteroids[j]->dead()==false)
        //{
        Asteroids[j]->draw(window);
        //}
        
    }
    for (int l = 0; l<deadAsteroids.size();l++)
    {
        //if (Asteroids[j]->dead()==false)
        //{
        deadAsteroids[l]->draw(window);
    }
    // Draw players (and bullets) and get their points
    for (int i = 0; i<Players.size();i++)
    {
        if (Players[i]->dead()==false)
        {
            Players[i]->draw(window);
        }
    }
    
    // Draw points (generalize to more than 2 players later)
    text1.setString(std::to_string(Players[0]->getPoints())+" p");
    text2.setString(std::to_string(Players[1]->getPoints())+" p");
    window.draw(text1);
    window.draw(text2);
    std::cout << std::to_string(Players[0]->getPoints())+ " " + std::to_string(Players[1]->getPoints())+"\n";
}

void Space::generateAsteroids(int num, float speed, sf::Vector2f old_centre, int old_type, sf::Vector2f odxy, int generation)
{
    // Make random numbers for star init.
    int sx; // spawn x
    int sy; // spawn y
    int st; // spawn type
    int ss; // spawn size
    float spx; // spawn speed x
    float spy; // spawn speed y
    
    for (int i=0; i<num;i++)
    {
        if (generation==1)
        {
            std::cout << "1 \n";
            sx = dis(gen) % wx +1;
            sy = dis(gen) % wy +1;
            spx = speed*(min_dis(gen) % 30 +1);
            spy = speed*(min_dis(gen) % 30 +1);
            st = dis(gen) % 4 +1;
            ss = dis(gen) % 7 +7;
        } else if (generation > 1)
        {
            sx = old_centre.x;
            sy = old_centre.y;
            spx = odxy.x+(generation*speed)*(min_dis(gen) % 30 +1);
            spy = odxy.y+(generation*speed)*(min_dis(gen) % 30 +1);
            st = old_type;
            if (generation == 2)
            {
                ss = dis(gen) % 2 +5;
                std::cout << "2 \n";
            } else // Last generation = 3
            {
                ss = dis(gen) % 2 +3;
                std::cout << "3 \n";
            }
        }
   
    auto ast = std::unique_ptr<Asteroid> (new Asteroid(sx,sy,st,ss,spx,spy, generation));
    Asteroids.push_back(std::move(ast));
    }
    
    // Make player invincible for limited time after asteroids spawn
    for (int j=0;j<Players.size();j++)
    {
        Players[j]->inv_clock.restart();
    }
}

void Space::checkColl()
{
    // 4 cases
    // Asteroids vs player, then asteroids vs bullets
   for (int i=0; i<Asteroids.size(); i++)
   {
        for (int j=0; j<Players.size(); j++)
        {
            if ((getDiff(Asteroids[i]->centre(), Players[j]->getTip())<Asteroids[i]->getRad()) || (getDiff(Asteroids[i]->centre(), Players[j]->getAft(1))<Asteroids[i]->getRad()) || (getDiff(Asteroids[i]->centre(), Players[j]->getAft(2))<Asteroids[i]->getRad()))
            {
                // Kill player if not inv.
                if (Players[j]->inv_clock.getElapsedTime().asSeconds() > Players[j]->inv_time)
                {
                    if (!Players[j]->explode)
                    {
                        Players[j]->exp(); //trigger animation before hp reduce by hit()
                    }
                    
                    if (Players[j]->dead())
                    {
                    Players.erase(Players.begin()+j);
                    }
                }
            } else
            {   // trigger explosion if hit with bullet
                if (Players[j]->checkColl(Asteroids[i]->centre(), Asteroids[i]->getRad()))
                {
                    Players[j]->score();
                    deadAsteroids.push_back(std::move(Asteroids[i])); // Keep in vector for dead asteroids to animate
                    deadAsteroids.back()->exp();
                    Asteroids.erase(Asteroids.begin()+i); // remove from living-vector
                    break;
                }
            }
        }
   }
    // Player vs player, then player vs bullets
    /*
    for (int k=0;k<Players.size()-1;k++)
    {
        for (int l=k+1;l<Players.size();l++)
        {
            if (getDiff(Players[k]->getTip(), Players[l]->getTip()) < Players[k]->getRad() || // den här jämför konstigt, hörn mot hörn osv...
                getDiff(Players[k]->getAft(1), Players[l]->getAft(1)) < Players[k]->getRad() ||
                getDiff(Players[k]->getAft(2), Players[l]->getAft(2)) < Players[k]->getRad())
            {
                if (!Players[l]->explode)
                {
                    Players[l]->exp(); //trigger animation before hp reduce by hit()
                }
                
                if (!Players[k]->explode)
                {
                    Players[k]->exp(); //trigger animation before hp reduce by hit()
                }
                    // Replace by storing them in some tmep vector to put back if revive.
                if (Players[l]->dead() && Players[k]->dead())
                {
                    Players.pop_back();
                    Players.pop_back();
                } else if (Players[l]->dead())
                {
                    Players.pop_back();
                } else if (Players[k]->dead())
                {
                    Players.erase(Players.begin()+k);
                }
                
                
            }
        }
    }*/
}

bool Space::clear()
{
    return (Asteroids.size() == 0 && deadAsteroids.size() == 0); // When all asteroids have animated their death
}

void Space::levelUp()
{
    level++;
    generateAsteroids(floor(level*1.5), log(level)*0.05, sf::Vector2f(), 0,sf::Vector2f(), 1);
}
