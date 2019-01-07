#include <cstdlib>
#include <iostream>
#include <ctime>
#include <SFML/Graphics.hpp>
#include "BranchingMaze.hpp"

const int COLUMNS = 80;
const int ROWS = 45;
const int WIDTH = 16 * COLUMNS;
const int HEIGHT = 16 * ROWS;

int main(int argc, char* argv[]) {

  srand(time(NULL));

  sf::Texture texture;
  if(!texture.loadFromFile("maze_texture.png")) {
    std::cout << "Cerrando aplicacion" << std::endl;
    sf::sleep(sf::seconds(2.0));
    return 0;
  }
//  texture.setSmooth(true);

  MazeGrid grid(WIDTH, HEIGHT, COLUMNS, ROWS, texture);
  BranchingMaze maze(grid);

  bool runMaze = false;

  sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Laberinto Dijkstra");
  window.setFramerateLimit(60);

  while(window.isOpen()) {
    sf::Event event;
    while(window.pollEvent(event)) {
      if(event.type == sf::Event::Closed) {
        window.close();
      }
      if(sf::Event::KeyPressed) {
        switch(event.key.code) {
        case sf::Keyboard::Escape:
            window.close();
            break;
        case sf::Keyboard::F:
          maze.nextStep();
          sf::sleep(sf::seconds(0.20));
          break;
        case sf::Keyboard::R:
            runMaze = true;
            break;
        case sf::Keyboard::Space:
            runMaze = false;
            break;
        default:
          break;
        }
      }
    }

    if(runMaze) {
      maze.nextStep();
      sf::sleep(sf::seconds(0.1));
    }

    window.clear(sf::Color(0,0,0));
    maze.drawMaze(window);
    window.display();
  }
  return 0;
}
