//
//  Utility.cpp
//  Engineers of Solar valley
//  Amateur clone of Asteroids(1979) created by Tobias Fjellström

#include "Utility.hpp"

float getDiff(sf::Vector2f p1, sf::Vector2f p2)
{
    // Returns straight-line distance between 2 points
    float diff = sqrt(pow((p2.x-p1.x),2)+pow((p2.y-p1.y),2));
    return (diff);
}
