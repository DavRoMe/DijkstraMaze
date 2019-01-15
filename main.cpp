#include <cstdlib>
#include <iostream>
#include <ctime>
#include <SFML/Graphics.hpp>
#include "BranchingMaze.hpp"
#include "DijkstraMaze.hpp"

#ifdef DEBUG_MODE
const bool DEBUG = true;
const int COLUMNS = 80;
const int ROWS = 40;
#else
const bool DEBUG = false;
const int COLUMNS = 80;
const int ROWS = 45;
#endif // DEBUG_MODE


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

  MazeGrid grid(WIDTH, HEIGHT, COLUMNS, ROWS, texture);
  if(DEBUG) {
    std::cout << " Columnas: " << COLUMNS << std::endl;
    std::cout << " Filas: " << ROWS << std::endl;
    std::cout << " Resolucion ventana: " << WIDTH << " x " << HEIGHT << std::endl;
    std::cout << " Ancho habitacion: " << grid.getCellW() << std::endl;
    std::cout << " Altura habitacion: " << grid.getCellH() << "\n\n";
  }

  BranchingMaze maze(grid);

  DijkstraMaze solver(grid);

  bool runMaze = false;
  bool solved = false;

  int start = maze.intRandom(0, grid.getRows() - 1);
  int goal = maze.intRandom(0, grid.getRows() - 1);

  sf::RenderWindow window;
  if(DEBUG) {
    window.create(sf::VideoMode(WIDTH, HEIGHT), "Laberinto Dijkstra");
  } else {
    window.create(sf::VideoMode(WIDTH, HEIGHT), "Laberinto Dijkstra", sf::Style::Fullscreen);
  }
  window.setFramerateLimit(60);

  // Ciclo while //////////
  while(window.isOpen()) {
    sf::Event event;
    while(window.pollEvent(event)) {
      if(event.type == sf::Event::Closed) {
        window.close();
      }

      if(event.type == sf::Event::KeyReleased) {
        switch(event.key.code) {
        case sf::Keyboard::Delete:
          grid.restart();
          maze.restart();
          solver.restart();
          solved = false;
          break;
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

    if(solved) {
      // Mouse click
      sf::Vector2f worldPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
      if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        sf::Vector2i indice(grid.getAreaIndex(worldPos.x, worldPos.y));
        solver.solve(indice.x, indice.y);
      }
      if(sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        sf::Vector2i indice(grid.getAreaIndex(worldPos.x, worldPos.y));
        solver.makeOrigin(indice.x, indice.y);
      }
    } else {
      if(runMaze) {
        if(!maze.isCompleted()) {
          maze.nextStep();
        } else {
          runMaze = !solver.makeOrigin(0, maze.intRandom(0, grid.getRows() - 1));
          solved = solver.solve(grid.getCols() - 1, maze.intRandom(0, grid.getRows() - 1));
        }
      }
    }
    sf::sleep(sf::seconds(0.05));

    window.clear(sf::Color(5, 5, 5));
    maze.drawMaze(window);
    window.display();
  }
  return 0;
}
