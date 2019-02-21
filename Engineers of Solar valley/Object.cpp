//
//  Object.cpp
//  Engineers of Solar valley
//
//  Created by Tobias Fjellström on 2018-11-05.
//  Copyright © 2018 Solvind Studio. All rights reserved.
//

#include "Object.hpp"
#include <iostream> // Ta bort vid första tillfälle

/*
OBJECT
*/

void Object::setHp(int New)
{
    hp = New;
}

void Object::draw(sf::RenderTarget &window)
{
    this->animate(window);
    window.draw(sheet);
}

bool Object::dead()
{
    return (hp == 0);
}

sf::FloatRect Object::getBounds()
{
    return (sheet.getGlobalBounds());
}

int Object::getRad()
{
    // Return radius of object
    return (radius);
}

sf::Vector2f Object::getSpeed()
{
    sf::Vector2f dxy = {dx, dy};
    return (dxy);
}

//----------------------------
//      SHIP
//----------------------------

Ship::Ship(float wx, float wy, int iID)
{
    scale = 1.2;
    points = 0;
    if (txt.loadFromFile(resourcePath() + "Simple_ship_exp.png"))
    {
        sheet.sf::Sprite::setTexture(txt, true);
        sheet.sf::Sprite::setOrigin((sheet.getLocalBounds().width)*0.5, (sheet.getLocalBounds().height)*0.5);
        sheet.sf::Sprite::setScale(scale,scale);
        sheet.sf::Sprite::setPosition(iID*wx,wy/2);
        sheet.sf::Sprite::setRotation(0);
        
        health_sheet.sf::Sprite::setTexture(txt, true);
        health_sheet.sf::Sprite::setOrigin((sheet.getLocalBounds().width)*0.5, (sheet.getLocalBounds().height)*0.5);
        health_sheet.sf::Sprite::setScale(scale,scale);
    }
    //health_sheet = sheet; // to be drawn at top of screen
    ID = iID;
    hp = 3;
    radius = 16*scale;
    if (ID == 1)
    {
        anim_row = 0;
    } else if (ID == 2)
    {
        anim_row = 3;
    }
    sRect = sf::IntRect(0,anim_row*32,32,32);
    health_sheet.setTextureRect(sRect);
    anim_clock.restart();
}

void Ship::move(float wx, float wy, std::vector<std::unique_ptr<Star>>& Stars)
{
        thrust = false; // for animate()
        // Moves ship according to key. Make sure it loops back to lower half of screen when it goes off.
        if (ID == 1) {
            
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                if (!(sqrt(dx*dx + dy*dy) > maxSpeed)) {
                    dx += acc*cos(a*(PI/(180)));
                    dy += acc*sin(a*(PI/(180)));
                }
                thrust = true;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                a -= rotSpeed;
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                a += rotSpeed;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && timer.getElapsedTime().asSeconds() > charge) // If shooting
            {
                auto bullet = std::unique_ptr<Bullet> (new Bullet(x-2*dx,y-2*dy,a));
                sub.push_back(std::move(bullet));
                timer.restart();
            }
            
        } else if (ID == 2) {
            
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                if (!(sqrt(dx*dx + dy*dy) > maxSpeed)) {
                    dx += acc*cos(a*(PI/(180)));
                    dy += acc*sin(a*(PI/(180)));
                }
                thrust = true;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                a -= rotSpeed;
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                a += rotSpeed;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && timer.getElapsedTime().asSeconds() > charge) // If shooting
            {
                auto bullet = std::unique_ptr<Bullet> (new Bullet(x-2*dx,y-2*dy,a));
                sub.push_back(std::move(bullet));
                timer.restart();
            }
            
        }
    
    // Get position and move
    sheet.move(-dx,-dy);
    x = sheet.getPosition().x;
    y = sheet.getPosition().y;
    
    // Move background with ship
    for (int j=0; j<Stars.size();j++)
    {
    Stars[j]->move(dx, dy, wx, wy);
    }
        
        // Bullet move
        for (int i = 0; i < sub.size(); i++)
        {
            if (sub[i]->lifetime.asSeconds() > 0.5) // Remove old shots
            {
                sub.erase(sub.begin()+i);
            }else
            {
            sub[i]->move(wx, wy);
            }
        }
        
        // Gör skärmen kontinuerlig
        if (x < 0-radius) {sheet.setPosition(wx+radius,y);}
        else if (x > wx+radius) {sheet.setPosition(0-radius,y);};
        if (y < 0-radius) {sheet.setPosition(x,wy+radius);}
        else if (y > wy+radius) {sheet.setPosition(x,0-radius);};
        
        sheet.setRotation(a-90);
        // slow down
        dx*=0.99;
        dy*=0.99;
}

    


void Ship::animate(sf::RenderTarget& window)
{
    // Change the viewing rectangle
    if (explode)  // exploding
    {
        sRect = sf::IntRect(32*anim_Count,(anim_row+2)*32,32,32);
        if (anim_clock.getElapsedTime().asSeconds() > exp_timer)
        {
            if (anim_Count < 7) // if not last in sprite sheet
            {
                    anim_Count++;
            } else
            {
                anim_Count = 0;
                explode = false;
                //inv_clock.restart();
                //blink_clock.restart();
                resetPos(window); // DRARS LAST FRAME ON NEW POSITION.
                hit();
            }
            anim_clock.restart();
        }
    } else if (!thrust) //&& (anim_clock.getElapsedTime().asSeconds() >a_timer)) // resting
    {
        sRect = sf::IntRect(32*anim_Count,anim_row*32,32,32);
        if (anim_clock.getElapsedTime().asSeconds() > a_timer)
        {
            if (anim_Count < 7) // if not last in sprite sheet
            {
                anim_Count++;
            } else
            {
                anim_Count = 0;
            }
            anim_clock.restart();
        }
    } else// if (anim_clock.getElapsedTime().asSeconds() >a_timer) // ship thrust
    {
        sRect = sf::IntRect(32*anim_Count,(anim_row+1)*32,32,32);
        if (anim_clock.getElapsedTime().asSeconds() > a_timer)
        {
            if (anim_Count < 7) // if not last in sprite sheet
            {
                anim_Count++;
            } else
            {
                anim_Count = 0;
            }
            anim_clock.restart();
        }
    }
    // Change the view
    sheet.setTextureRect(sRect);
    // if just respawned
    if (blink)
    {
        if (inv_clock.getElapsedTime().asSeconds() < inv_time)
        {
            if (blink_clock.getElapsedTime().asSeconds()>2*blink_time)
            {
                blink_clock.restart();
                sheet.setColor(sf::Color(255,255,255,0));
                
            } else if (blink_clock.getElapsedTime().asSeconds()>blink_time)
            {
                sheet.setColor(sf::Color(255,255,255,255));
            }
        } else
        {
            blink = false; // reset blinking
            sheet.setColor(sf::Color(255,255,255,255));
        }
    }
    sheet.sf::Sprite::setOrigin((sheet.getLocalBounds().width)*0.5, (sheet.getLocalBounds().height)*0.5);
    
    for (int i = 0; i < sub.size(); i++) // Bullet animation
    {
        sub[i]->animate(window);
    }
}

void Ship::draw(sf::RenderTarget& window)
{
    this->animate(window); // Animate the ship movement; also calls animate for bullets and repositions ship after exp
    for (int i = 0; i < sub.size(); i++) // Bullet animation
    {
        sub[i]->draw(window);
    }
    healthBar(window); // draw health bar
    window.draw(sheet);
    sf::CircleShape circle;
    /*circle.setRadius(4);
    circle.setOrigin(4,4);
    circle.setPosition(getAft(1).x, getAft(1).y);
    circle.setOutlineColor(sf::Color::Red);
    window.draw(circle);*/
}

sf::Vector2f Ship::getTip()
{
    // Return coordinate to point of ship.
    sf::Vector2f xy = sheet.getPosition();
    //xy.y -= sheet.getLocalBounds().height;
    xy.x -= 0.5*(sheet.getLocalBounds().height*cos(a*(PI/(180))));
    xy.y -= 0.5*(sheet.getLocalBounds().height*sin(a*(PI/(180))));
    return (xy);
}

sf::Vector2f Ship::getAft(int left_right)
{
    // BROKEN, needs fixing. Should mark corners of ship at all times.
    // Return coordinate to point of ship.
    sf::Vector2f xy = sheet.getPosition();
    if (left_right==1)
    {
        xy.y += 0.5*(0.5*sheet.getLocalBounds().width*cos(a*(PI/(180)))+0.5*sheet.getLocalBounds().height*sin(a*(PI/(180))));
        xy.x += 0.5*(0.5*sheet.getLocalBounds().height*cos(a*(PI/(180)))-0.5*sheet.getLocalBounds().width*sin(a*(PI/(180))));
    } else
    {
        xy.y -= (0.5*sheet.getLocalBounds().width*cos(a*(PI/(180)))-0.5*sheet.getLocalBounds().height*sin(a*(PI/(180))));
        xy.x += (0.5*sheet.getLocalBounds().height*cos(a*(PI/(180)))+0.5*sheet.getLocalBounds().width*sin(a*(PI/(180))));
    }
    
    return (xy);
}

bool Ship::checkColl(sf::Vector2f ast_centre, int ast_rad)
{
    for (int i=0; i<sub.size(); i++)
    {
        if (getDiff(ast_centre, sub[i]->getTip(false)) < ast_rad+sub[i]->getRad() || getDiff(ast_centre, sub[i]->getTip(true)) < ast_rad+sub[i]->getRad())
        {
            sub.erase(sub.begin()+i);
            return (true);
        }
    }
    return (false);
}

void Ship::hit()
{
    hp -=1;
}

void Ship::healthBar(sf::RenderTarget& window)
{
    int px;
    int py = 40; // y position of bar
    if (ID==1) // x position depending on ship number.
    {
        px = window.getSize().x-200;
    } else if (ID ==2)
    {
        px = 0;
    }
    for (int i=1;i<=hp;i++)
    {
        sRect = sf::IntRect(0,anim_row*32,32,32);
        health_sheet.setTextureRect(sRect);
        health_sheet.sf::Sprite::setOrigin((sheet.getLocalBounds().width)*0.5, (sheet.getLocalBounds().height)*0.5);
        health_sheet.setPosition((i*50)+px,py);
        window.draw(health_sheet);
    }
}

void Ship::score()
{
    points += 10;
}

int Ship::getPoints()
{
    return (points);
}

void Ship::exp()
{
    explode = true;
    anim_Count = 0;
}

void Ship::resetPos(sf::RenderTarget& window)
{
    x=ID*window.getSize().x/3;
    y=window.getSize().y/2;
    dx = 0;
    dy = 0;
    sheet.setPosition(x,y);
    inv_clock.restart(); // make invincible for limited time
    blink_clock.restart();
    blink = true;
}

//-----------------------------------
//      STAR
//-----------------------------------

Star::Star(int x, int y, int itype, float size, float ia_timer) // int x, int y
{
    if (txt.loadFromFile(resourcePath() + "Star.png"))
    {
        sheet.sf::Sprite::setTexture(txt, true);
        sheet.sf::Sprite::setOrigin((sheet.getLocalBounds().width)*0.5, (sheet.getLocalBounds().height)*0.5);
        sheet.sf::Sprite::setScale(size,size);
        sheet.sf::Sprite::setPosition(x,y);
    }
    a_timer = ia_timer;
    type = itype;
    sRect = sf::IntRect(0,(type-1)*32,32,32);
    // set radius
    radius = 10;

}

void Star::animate(sf::RenderTarget& window)
{
    sRect = sf::IntRect(32*anim_Count,(type-1)*32,32,32);
    if (clock.getElapsedTime().asSeconds() > a_timer) // if enough time for next frame
    {
        if (anim_Count < 4) // if not last in sprite sheet
        {
            anim_Count++;
        } else
        {
            anim_Count = 0;
        }
        clock.restart();
    }
    // Change the view
    sheet.setTextureRect(sRect);
    // Re-set origin
    sheet.sf::Sprite::setOrigin((sheet.getLocalBounds().width)*0.5, (sheet.getLocalBounds().height)*0.5);
}

void Star::move(float ship_dx, float ship_dy, int wx, int wy)
{
    // Update position
    x=sheet.getPosition().x;
    y=sheet.getPosition().y;
    // Move
    sheet.move(0.1*ship_dx, 0.1*ship_dy);

    // Continuous screen
    if (x < 0-radius) {sheet.setPosition(wx+radius,y);}
    else if (x > wx+radius) {sheet.setPosition(0-radius,y);};
    if (y < 0-radius) {sheet.setPosition(x,wy+radius);}
    else if (y > wy+radius) {sheet.setPosition(x,0-radius);};
}



//--------------------------------------
//      BULLET
//--------------------------------------

Bullet::Bullet(float x, float y, float angle)
{
    if (txt.loadFromFile(resourcePath() + "Bullet.png"))
    {
        sheet.sf::Sprite::setTexture(txt, true);
        sheet.sf::Sprite::setOrigin((sheet.getLocalBounds().width)*0.5,0);//(sheet.getLocalBounds().height)); // set at back so it sinks in when hiting
        sheet.sf::Sprite::setScale(2,2);
        sheet.sf::Sprite::setPosition(x,y);
        sheet.sf::Sprite::setRotation(angle-90);
    }
    //radius = sqrt(pow(sheet.getLocalBounds().width,2) + pow(sheet.getLocalBounds().height, 2)); // set radius of bullet
    radius = 0;
    // Set constant speed vector
    maxSpeed = 45;
    dx = maxSpeed*cos(angle*(PI/(180)));
    dy = maxSpeed*sin(angle*(PI/(180)));
    clock.restart();
    a = angle-90;
}

void Bullet::move(float wx, float wy)
{
    // move bullet
    sheet.move(-dx,-dy);
    // Get position so bullet KNOWS where it is
    x = sheet.getPosition().x;
    y = sheet.getPosition().y;
    // Check for map border
   // if (x < 0-radius) {sheet.setPosition(wx+radius,y);}
   // else if (x > wx+radius) {sheet.setPosition(0-radius,y);};
   // if (y < 0-radius) {sheet.setPosition(x,wy+radius);}
   // else if (y > wy+radius) {sheet.setPosition(x,0-radius);};
    
    lifetime = clock.getElapsedTime(); // count lifetime for move and animate
}

void Bullet::animate(sf::RenderTarget& window)
{
    // Makes shot elongated and weak at end of their lifetime.
    sheet.sf::Sprite::setScale(1-0.7*lifetime.asSeconds(),1+lifetime.asSeconds());
}

sf::Vector2f Bullet::getTip(bool aft)
{
    // BROKEN!!!!
    // Return coordinate to point of bullet.
    sf::Vector2f xy;
    xy = sheet.getPosition();
    if (aft)
    { // shift coordinate to back
    xy.x -= sheet.getLocalBounds().height*sin(a*(PI/(180)));
    xy.y += sheet.getLocalBounds().height*cos(a*(PI/(180)));
    }
    return (xy);
}

int Bullet::getRad()
{
    return (radius);
}
/*
void Bullet::draw(sf::RenderTarget &window)
{
    window.draw(sheet);
    sf::CircleShape circle;
    circle.setRadius(10);
    circle.setOrigin(10,10);
    circle.setPosition(getTip(true).x, getTip(true).y);
    circle.setOutlineColor(sf::Color::Red);
    window.draw(circle);
}*/


//----------------------------------
//      Asteroid
//----------------------------------

Asteroid::Asteroid(int px, int py, int itype, int size, float spx, float spy, int iID)
{
    if (txt.loadFromFile(resourcePath() + "Asteroid_exp.png"))
    {
        sheet.sf::Sprite::setTexture(txt, true);
        sheet.sf::Sprite::setOrigin((sheet.getLocalBounds().width)*0.5, (sheet.getLocalBounds().height)*0.5);
        sheet.sf::Sprite::setScale(size,size);
        sheet.sf::Sprite::setPosition(px,py);
        //sheet.sf::Sprite::setRotation(x+y);
    }
    // set radius
    radius = 6*size; // about 1/4 of viewing square times magnification
    // Set type of asteroid
    type = itype;
    // Set original viewing rect.
    sRect = sf::IntRect(0,(type-1)*32,32,32);
    // Set constant speed vector
    dx = spx;
    dy = spy;
    // Set rotation speed
    rotSpeed = size/(spx+spy);
    a_timer = size/(spx+spy);
    hp = 1;
    ID = iID; // Used to see what generation the asteroid is.
}

void Asteroid::animate(sf::RenderTarget& window)
{
    if (!explode)
    {
        sRect = sf::IntRect(32*(anim_Count),(type-1)*32,32,32); //(type-1)*32
        if (clock.getElapsedTime().asSeconds() > a_timer) // if enough time for next frame
        {
            if (anim_Count < 7) // if not last in sprite sheet
            {
                anim_Count++;
            } else
            {
                anim_Count = 0;
            }
            clock.restart();
        }
    } else // explosion
    {
        sRect = sf::IntRect(32*anim_Count,4*32,32,32);
        if (clock.getElapsedTime().asSeconds() > exp_timer) // if enough time for next frame
        {
            if (anim_Count < 7) // if not last in sprite sheet
            {
                anim_Count++;
            } else
            {
                hp = 0; // after this it should be destroyed
            }
            clock.restart();
        }
    }

    //Rotate
    sheet.setRotation(sheet.getRotation()+1);
    // Change the view
sheet.setTextureRect(sRect);
    // Re-set origin
sheet.sf::Sprite::setOrigin((sheet.getLocalBounds().width)*0.5, (sheet.getLocalBounds().height)*0.5);

}


void Asteroid::move(int wx, int wy)
{
    // Update position
    sheet.move(-dx,-dy);
    x = sheet.getPosition().x;
    y = sheet.getPosition().y;
    
    // Check for map border
    if (x < 0-radius) {sheet.setPosition(wx+radius,y);}
    else if (x > wx+radius) {sheet.setPosition(0-radius,y);};
    if (y < 0-radius) {sheet.setPosition(x,wy+radius);}
    else if (y > wy+radius) {sheet.setPosition(x,0-radius);};
}

void Asteroid::exp()
{
    explode = true;
    anim_Count = 0;
}

sf::Vector2f Asteroid::centre()
{
    return (sheet.getPosition());
}

int Asteroid::getType()
{
    return (type);
}
