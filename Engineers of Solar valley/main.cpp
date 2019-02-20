//#include <SFML/Graphics.hpp>
//#include "Object.hpp"
#include "Space.hpp"
#include <iostream>


int main(int, char const**)
{
    // Create the main window
    //sf::RenderWindow window(sf::VideoMode(1280, 720), "Engineers of Solar valley");
    // Create sense of time
    sf::Clock clock;
    sf::Time dt; // time between loops.
    
    // Create players
//Ship ship(window.getSize().x, window.getSize().y, 1);
    //Space space(window.getSize().x, window.getSize().y, 1);
    
    // Create space, with players and background, (adapted to screen size)
   // Space space(window.getSize().x, window.getSize().y, 2); // 2 player
    Space space(2); // 2 player
    
    
    
    // Start the game loop
    while (space.window.isOpen())
    {
        // If no asteroids, generate level of asteroids
        if (space.clear())
        {
            space.levelUp();
        }
        
        // Reset timer
        //dt = clock.restart();
       
        // Framerate check
        //std::cout << "Framerate: ";
        //std::cout << dt.asMilliseconds();
        //std::cout << "\n";
        
        // Process events
        sf::Event event;
        while (space.window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed) {
                space.window.close();
            }

            // Escape pressed: exit
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                space.window.close();
            }
        }

        // Clear screen
        space.window.clear();
        
        // Update
        space.update();
        
        // Draw sprites
        space.draw();
        
        // Update the window
        space.window.display();
        // Take rest for proper framerate
        sf::sleep(sf::milliseconds(35));
    }

    return EXIT_SUCCESS;
}
