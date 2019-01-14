#pragma once

#include <cstdio>
#include <forward_list>
#include <stack>
#include <utility>
#include <SFML/System/Vector2.hpp>
#include "MazeGrid.hpp"

class BranchingMaze {
private:
  static const int UP = 0, DOWN = 1, LEFT = 2, RIGHT = 3;
  static const int MINCHANCE = 5;
  static const int MAXCHANCE = 30;
  int branchance;
  int maxNewBrchs;
  int crntBranches;
  int totalRooms;
  // int visitedRooms;
  bool completed;
  std::forward_list<std::stack<sf::Vector2i>*> indxLst;
  std::forward_list<std::stack<sf::Vector2i>*>::iterator it;

  int countNeighbours(int i, int j) {
    bool up = grid.canGoUp(i, j);
    bool down = grid.canGoDown(i, j);
    bool left = grid.canGoLeft(i, j);
    bool right = grid.canGoRight(i, j);

    int sum = 0;

    if(up)      sum++;
    if(down)    sum++;
    if(left)    sum++;
    if(right)   sum++;

    return sum;
  }

  int getChances(bool up, bool down, bool left, bool right, int arr[4]) {
    int chance = 0;

    if(up)    { arr[0] = 1; chance += arr[0];
    } else    { arr[0] = 0; }

    if(down)  { arr[1] = 1; chance += arr[1];
    } else    { arr[1] = 0; }

    if(left)  { arr[2] = 1; chance += arr[2];
    } else    { arr[2] = 0; }

    if(right) { arr[3] = 1; chance += arr[3];
    } else    { arr[3] = 0; }

    return chance;
  }

  int chooseNghbr(int i, int j) {
    bool up = grid.canGoUp(i, j);
    bool down = grid.canGoDown(i, j);
    bool left = grid.canGoLeft(i, j);
    bool right = grid.canGoRight(i, j);

    int chances[4];
    int summedChance = getChances(up, down, left, right, chances);
    int randChoose = intRandom(1, summedChance);
    int chosen = -1;      // Variable que definira la eleccion
    while(randChoose > 0 && chosen < 3) {
      chosen++;
      randChoose -= chances[chosen];
    }
    return chosen;
  }

  void eraseWall(int i, int j, int wall) {
    switch(wall) {
    case UP:
      grid.room(i, j).setUpperWall(false);
      break;
    case DOWN:
      grid.room(i, j).setLowerWall(false);
      break;
    case LEFT:
      grid.room(i, j).setLeftWall(false);
      break;
    case RIGHT:
      grid.room(i, j).setRightWall(false);
      break;
    default:
      printf("\n Error: Valor de muro incorrecto");
      break;
    }
  }

  int howManyBranches(int maxBranches) {
    if(intRandom(0, 100) > 100 - branchance)
      return intRandom(1, maxBranches);
    return 0;
  }

  MazeGrid& grid;

public:
  BranchingMaze(MazeGrid& g) : grid(g) {
    totalRooms = grid.getCols() * grid.getRows();

    restart();
  }

  void restart() {
    completed = false;
    branchance = intRandom(MINCHANCE, MAXCHANCE);
    printf("\n\n Prob.Ramif: %d %%", branchance);

    crntBranches = 1;
    maxNewBrchs = intRandom(5, totalRooms / 30);
    printf("\n Max.Ramif: %d", maxNewBrchs);

    // Limpiar la lista para comenzar nuevo laberinto
    indxLst.clear();

    // Creando primer habitacion
    indxLst.push_front(new std::stack<sf::Vector2i>);
    (*indxLst.begin())->push(sf::Vector2i(intRandom(0, grid.getCols() - 1), intRandom(0, grid.getRows() - 1)));

    // Haciendo la nueva habitacion como la actual
    int iIndex = (*indxLst.begin())->top().x;
    int jIndex = (*indxLst.begin())->top().y;
    grid.room(iIndex, jIndex).setStatus(RoomStatus::Current);
  }

  bool isCompleted() { return completed; }

  class empty_stack {
    // Evalua si la pila esta vacia
  public :
    bool operator()(const std::stack<sf::Vector2i>* stck) { return stck->empty(); };
  };

  void nextStep() {
    if(!indxLst.empty()) {
      for(it = indxLst.begin(); it != indxLst.end(); it++) {
        sf::Vector2i crrntIndex = (*it)->top();
        if(grid.hasNeighbours(crrntIndex.x, crrntIndex.y)) {
          int nCount = countNeighbours(crrntIndex.x, crrntIndex.y);
          if(nCount > 1) {
            // Si hay mas de un vecino crear entre 0 y Vecinos-1 ramificaciones
            int branches = howManyBranches(nCount - 1);
//            printf("\n B: %d",branches);      // debug
            while(branches > 0 && crntBranches < maxNewBrchs + 1) {
              // Crear nueva ramificacion
              indxLst.push_front(new std::stack<sf::Vector2i>);
              crntBranches++;

              // Elegir un vecino al azar y agregarlo a la lista
              switch(chooseNghbr(crrntIndex.x, crrntIndex.y)) {
              case UP:
                indxLst.front()->push(sf::Vector2i(crrntIndex.x, crrntIndex.y - 1));
                eraseWall(crrntIndex.x, crrntIndex.y, UP);
                eraseWall(crrntIndex.x, crrntIndex.y - 1, DOWN);
//                  printf("\n Arriba");    // debug
                break;
              case DOWN:
                indxLst.front()->push(sf::Vector2i(crrntIndex.x, crrntIndex.y + 1));
                eraseWall(crrntIndex.x, crrntIndex.y, DOWN);
                eraseWall(crrntIndex.x, crrntIndex.y + 1, UP);
//                  printf("\n Abajo");    // debug
                break;
              case LEFT:
                indxLst.front()->push(sf::Vector2i(crrntIndex.x - 1, crrntIndex.y));
                eraseWall(crrntIndex.x, crrntIndex.y, LEFT);
                eraseWall(crrntIndex.x - 1, crrntIndex.y, RIGHT);
//                  printf("\n Izquierda");    // debug
                break;
              case RIGHT:
                indxLst.front()->push(sf::Vector2i(crrntIndex.x + 1, crrntIndex.y));
                eraseWall(crrntIndex.x, crrntIndex.y, RIGHT);
                eraseWall(crrntIndex.x + 1, crrntIndex.y, LEFT);
//                  printf("\n Derecha");    // debug
                break;
              }
              // Actualizar estado del vecino escogido y cuenta de visitados
              grid.room(indxLst.front()->top().x, indxLst.front()->top().y).setStatus(RoomStatus::Current);
              // visitedRooms++;

              branches--;
            }
          }
          // Moverse hacia la siguiente habitacion en la pila actual
          grid.room(crrntIndex.x, crrntIndex.y).setStatus(RoomStatus::Visited);
          switch(chooseNghbr(crrntIndex.x, crrntIndex.y)) {
          case UP:
            (*it)->push(sf::Vector2i(crrntIndex.x, crrntIndex.y - 1));
            eraseWall(crrntIndex.x, crrntIndex.y, UP);
            eraseWall(crrntIndex.x, crrntIndex.y - 1, DOWN);
//                  printf("\n Arriba");    // debug
            break;
          case DOWN:
            (*it)->push(sf::Vector2i(crrntIndex.x, crrntIndex.y + 1));
            eraseWall(crrntIndex.x, crrntIndex.y, DOWN);
            eraseWall(crrntIndex.x, crrntIndex.y + 1, UP);
//                  printf("\n Abajo");    // debug
            break;
          case LEFT:
            (*it)->push(sf::Vector2i(crrntIndex.x - 1, crrntIndex.y));
            eraseWall(crrntIndex.x, crrntIndex.y, LEFT);
            eraseWall(crrntIndex.x - 1, crrntIndex.y, RIGHT);
//                  printf("\n Izquierda");    // debug
            break;
          case RIGHT:
            (*it)->push(sf::Vector2i(crrntIndex.x + 1, crrntIndex.y));
            eraseWall(crrntIndex.x, crrntIndex.y, RIGHT);
            eraseWall(crrntIndex.x + 1, crrntIndex.y, LEFT);
//                  printf("\n Derecha");    // debug
            break;
          }
          grid.room((*it)->top().x, (*it)->top().y).setStatus(RoomStatus::Current);
          // visitedRooms++;
        } else {
          // Retroceder una vez
          grid.room(crrntIndex.x, crrntIndex.y).setStatus(RoomStatus::Revisited);
          (*it)->pop();
          if(!(*it)->empty()) {
            // Si la pila no esta vacia, tomar la ultima habitacion y hacerla la actual
            crrntIndex = (*it)->top();
            grid.room(crrntIndex.x, crrntIndex.y).setStatus(RoomStatus::Current);
          } else {
            crntBranches--;  // Si la pila esta vacia sera eliminada, hay que actializar la cuenta
          }
        }
      }
      // Eliminar pilas vacias
      empty_stack evaluate;
      indxLst.remove_if(evaluate);
    } else {
      completed = true;
      printf("\n Completado!");
    }
  }

  void drawMaze(sf::RenderWindow& window) {
    for(int i = 0; i < grid.getCols(); i++) {
      for(int j = 0; j < grid.getRows(); j++) {
        grid.drawRoom(i, j, window);
      }
    }
  }

  int intRandom(int minor, int major) {
    if(minor < 0) return 0;
    if(major > minor) {
      return rand() % (major + 1 - minor) + minor;
    } else {
      return minor;
    }
  }
};
