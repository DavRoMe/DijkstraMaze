#pragma once

#include <cstdio>
#include <limits>
#include <forward_list>
#include <unordered_map>
#include <utility>
#include <SFML/System/Vector2.hpp>
#include "MazeGrid.hpp"

class DijkNode {
private:
  int accumDistance;
  bool permanent;
  bool frstCheck;
  sf::Vector2i preceding;
  std::forward_list<sf::Vector2i> connectedNodes;
  std::forward_list<sf::Vector2i>::iterator iter;

public:
  DijkNode() { restart(); }

  int getDistance() { return accumDistance; }
  bool isPermanent() { return permanent; }
  bool isFirstCheck() { return frstCheck; }     // Para saber si es la primera ves que se etiqueta o no
  sf::Vector2i getPreceding() { return preceding; }
  std::forward_list<sf::Vector2i>& getConnections() { return connectedNodes; }

  void setDistance(int distance) { accumDistance = distance; }
  void makePermanent() { permanent = true; }
  void check() { frstCheck = false; }
  void setPreceding(sf::Vector2i node) { preceding = node; }
  void restart() {
    accumDistance = std::numeric_limits<int>::max();
    permanent = false;
    frstCheck = true;
    preceding = sf::Vector2i(-1, -1);
    connectedNodes.clear();
  }

  void addConnection(int i_nodePos, int j_nodePos) {
    connectedNodes.push_front(sf::Vector2i(i_nodePos, j_nodePos));
  }
};

////////////////////////////////////////////////////////////////////////////////////////////////

class DijkstraMaze {
private:
  MazeGrid& maze;
  int permCount;
  sf::Vector2i origin;
  sf::Vector2i goal;
  std::unordered_map<int, std::unordered_map<int, DijkNode>> nodesMap;
  std::forward_list<sf::Vector2i> potentialPermNodes;
  std::forward_list<sf::Vector2i>::iterator iter;

  DijkNode& node(int i, int j) { return nodesMap.find(i)->second.find(j)->second; }

  void makeNodes() {
    nodesMap.clear();
    for(int i = 0; i < maze.getCols(); i++) {
      for(int j = 0; j < maze.getRows(); j++) {
        nodesMap[i].insert(std::make_pair(j, DijkNode()));
      }
    }
  }

  void checkNodeConnections() {
    for(int i = 0; i < maze.getCols(); i++) {
      for(int j = 0; j < maze.getRows(); j++) {
        int c = 0;
        if(!maze.room(i, j).hasUpperWall()) {
          node(i, j).addConnection(i, j - 1);
          c++;
        }
        if(!maze.room(i, j).hasLowerWall()) {
          node(i, j).addConnection(i, j + 1);
          c++;
        }
        if(!maze.room(i, j).hasLeftWall()) {
          node(i, j).addConnection(i - 1, j);
          c++;
        }
        if(!maze.room(i, j).hasRightWall()) {
          node(i, j).addConnection(i + 1, j);
          c++;
        }
//        printf("\n %d conexiones en %d, %d", c, i, j);
      }
    }
  }

public:
  DijkstraMaze(MazeGrid& g) : maze(g) {
    restart();
  }

  void restart() {
    permCount = 0;
    origin = sf::Vector2i(-1, -1);
    goal = sf::Vector2i(-1, -1);
  }

  bool makeOrigin(int i, int j) {
    if(origin.x == i && origin.y == j) {
      return false;
    } else {
      makeNodes();
      maze.room(i, j).setStatus(RoomStatus::Origin);
      origin = sf::Vector2i(i, j);
      node(i, j).makePermanent();
      node(i, j).setDistance(0);
      permCount++;

      int a = i;
      int b = j;

      checkNodeConnections();

      while(permCount < maze.getCols()*maze.getRows()) {
        sf::Vector2i crntNode(a, b);
        std::forward_list<sf::Vector2i> crntList = node(a, b).getConnections();

        // Para cada nodo conectado al nodo actual
        for(iter = crntList.begin(); iter != crntList.end(); iter++) {

          // Si el nodo no es permanente, revisarlo
          if(!node((*iter).x, (*iter).y).isPermanent()) {

            // Si la distancia guardada en el nodo en revision es mayor que la distancia de la revision
            //  actual, reemplazar los valores anteriores con los nuevos
            int distance = node(crntNode.x, crntNode.y).getDistance() + 1;
            if(distance < node((*iter).x, (*iter).y).getDistance()) {
              node((*iter).x, (*iter).y).setDistance(distance);
              node((*iter).x, (*iter).y).setPreceding(crntNode);

              // Si es su primer etiquetado
              if(node((*iter).x, (*iter).y).isFirstCheck()) {
                // Agregar a la lista de nodos a revisar para el siguiente permanente
                // y marcarlo como ya etiquetado
                potentialPermNodes.push_front((*iter));
                node((*iter).x, (*iter).y).check();
              }
            }
          }
        }
        // Buscar cual sera el siguiente permanente
        int bestDistance = std::numeric_limits<int>::max();
        sf::Vector2i bestPos;
        std::forward_list<sf::Vector2i>::iterator ppnIt;
        // Buscando al mejor de la lista
        for(ppnIt = potentialPermNodes.begin(); ppnIt != potentialPermNodes.end(); ppnIt++) {
          if(bestDistance > node((*ppnIt).x, (*ppnIt).y).getDistance()) {
            bestDistance = node((*ppnIt).x, (*ppnIt).y).getDistance();
            bestPos = (*ppnIt);
          }
        }
        node(bestPos.x, bestPos.y).makePermanent();
        permCount++;
        a = bestPos.x;
        b = bestPos.y;
//        printf("\n Nodo permanente: %d, %d", a, b);
//        printf("\t\t Restantes: %d", maze.getCols()*maze.getRows() - permCount);

        // Borrar de la lista el nodo escogido
        potentialPermNodes.remove(sf::Vector2i(a, b));
      }
      return true;
    }
  }

  bool solve(int i, int j) {
    if(goal.x == i && goal.y == j) {
      return false;
    } else {
      goal = sf::Vector2i(i, j);
      maze.room(i, j).setStatus(RoomStatus::Goal);

      while(i >= 0 && j >= 0) {
        sf::Vector2i prev = node(i, j).getPreceding();
        if(prev.x != -1 || prev.y != -1) {
          if(prev.x != i) {      // Si esta en el eje X
            if(prev.x < i) {   // Si esta a la izquierda
              maze.room(i, j).setLeftPath(true);      // Actual
              maze.room(i - 1, j).setRightPath(true); // Izquierdo
            } else {    // Si esta a la derecha
              maze.room(i, j).setRightPath(true);     // Actual
              maze.room(i + 1, j).setLeftPath(true);  // Derecho
            }
          } else {                // Si esta en el eje Y
            if(prev.y < j) {    // Si esta arriba
              maze.room(i, j).setUpperPath(true);     // Actual
              maze.room(i, j - 1).setLowerPath(true); // Superior
            } else {            // Si esta abajo
              maze.room(i, j).setLowerPath(true);     // Actual
              maze.room(i, j + 1).setUpperPath(true); // Inferior
            }
          }
        }
        // Continuar hasta llegar al origen, actualizando posiciones
        i = prev.x;
        j = prev.y;
      }
    return true;
    }
  }

};























