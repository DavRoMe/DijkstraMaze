#pragma once

#include <screenGrid.h>
#include <unordered_map>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

enum class RoomStatus {
  Unvisited, Visited, Revisited, Current, Origin, Goal
};

////////////////////////////////////////////////////////////////////////////////

class MazeRoom {
private:
  int x_pos, y_pos;
  RoomStatus status;
  bool upperWall, lowerWall, leftWall, rightWall;
  bool upperPath, lowerPath, leftPath, rightPath;

public:
  MazeRoom(int x_pos, int y_pos) {
    this->x_pos = x_pos;
    this->y_pos = y_pos;
    status = RoomStatus::Unvisited;
    upperWall = lowerWall = leftWall = rightWall = true;
    upperPath = lowerPath = leftPath = rightPath = false;
  }

  bool hasUpperWall()   { return upperWall; }
  bool hasLowerWall()   { return lowerWall; }
  bool hasLeftWall()    { return leftWall; }
  bool hasRightWall()   { return rightWall; }

  bool hasUpperPath()   { return upperPath; }
  bool hasLowerPath()   { return lowerPath; }
  bool hasLeftPath()    { return leftPath; }
  bool hasRightPath()   { return rightPath; }

  int getXpos() { return x_pos; }
  int getYpos() { return y_pos; }

  RoomStatus getStatus() { return status; }

  void setUpperWall(bool bValue)    { upperWall = bValue; }
  void setLowerWall(bool bValue)    { lowerWall = bValue; }
  void setLeftWall(bool bValue)     { leftWall = bValue; }
  void setRightWall(bool bValue)    { rightWall = bValue; }

  void setUpperPath(bool bValue)    { upperPath = bValue; }
  void setLowerPath(bool bValue)    { lowerPath = bValue; }
  void setLeftPath(bool bValue)     { leftPath = bValue; }
  void setRightPath(bool bValue)    { rightPath = bValue; }

  void setStatus(RoomStatus status) { this->status = status; }
};

////////////////////////////////////////////////////////////////////////////////

class MazeGrid : public ScrnGrid {
private:
  sf::Color visitedColor = sf::Color(232, 92, 0);
  sf::Color revisitedColor = sf::Color(20, 74, 219);
  sf::Color currentColor = sf::Color(255, 255, 255);
  sf::Color startColor = sf::Color(220, 0, 0);
  sf::Color goalColor = sf::Color(230, 24, 240);
  sf::Color pathColor = sf::Color(0, 250, 0);
  sf::Color wallColor = sf::Color(0, 0, 0);

  static const int texAreaWidth = 16;
  static const int texAreaHeight = 16;
  sf::Vector2i texAreaSize  = sf::Vector2i(texAreaWidth, texAreaHeight);
  sf::Vector2i noColorPos   = sf::Vector2i(0, 0);
  sf::Vector2i upperWallPos = sf::Vector2i(texAreaWidth, 0);
  sf::Vector2i lowerWallPos = sf::Vector2i(texAreaWidth * 2, 0);
  sf::Vector2i leftWallPos  = sf::Vector2i(texAreaWidth * 3, 0);
  sf::Vector2i rightWallPos = sf::Vector2i(texAreaWidth * 4, 0);
  sf::Vector2i solidColorPos = sf::Vector2i(0, texAreaHeight);
  sf::Vector2i upperPathPos = sf::Vector2i(texAreaWidth, texAreaHeight);
  sf::Vector2i lowerPathPos = sf::Vector2i(texAreaWidth * 2, texAreaHeight);
  sf::Vector2i leftPathPos  = sf::Vector2i(texAreaWidth * 3, texAreaHeight);
  sf::Vector2i rightPathPos = sf::Vector2i(texAreaWidth * 4, texAreaHeight);

  sf::Sprite sprite;

  std::unordered_map <int, std::unordered_map<int, MazeRoom>> mazeRooms;

  void fillRooms() {
    for(int i = 0; i < getCols(); i++) {
      for(int j = 0; j < getRows(); j++) {
        sf::Vector2i v = getIndexPos(i, j);
        mazeRooms[i].insert(std::make_pair(j, MazeRoom(v.x, v.y)));
      }
    }
  }

public:
  MazeGrid(int width, int height, int columns, int rows, sf::Texture& texture) :
    ScrnGrid(width, height, columns, rows) {
    sprite.setTexture(texture);
    sprite.setTextureRect(sf::IntRect(noColorPos, texAreaSize));
    sprite.setScale((float)(getCellW() / texAreaSize.x), (float)(getCellH() / texAreaSize.y));
    sprite.setColor(currentColor);
    fillRooms();
  }

  MazeRoom& room(int i, int j) { return mazeRooms.find(i)->second.find(j)->second; }

  bool canGoUp(int i, int j) {
    if(j == 0)          // Posicion actual en el borde superior
      return false;
    if(room(i, j - 1).getStatus() == RoomStatus::Unvisited)
      return true;
    return false;
  }

  bool canGoDown(int i, int j) {
    if(j == getRows() - 1) // Posicion actual en el borde inferior
      return false;
    if(room(i, j + 1).getStatus() == RoomStatus::Unvisited)
      return true;
    return false;
  }

  bool canGoLeft(int i, int j) {
    if(i == 0)  // Posicion actual en el borde izquierdo
      return false;
    if(room(i - 1, j).getStatus() == RoomStatus::Unvisited)
      return true;
    return false;
  }

  bool canGoRight(int i, int j) {
    if(i == getCols() - 1) // Posicion actual en el borde derecho
      return false;
    if(room(i + 1, j).getStatus() == RoomStatus::Unvisited)
      return true;
    return false;
  }

  bool hasNeighbours(int i, int j) {
    if(canGoDown(i, j))
      return true;
    if(canGoLeft(i, j))
      return true;
    if(canGoRight(i, j))
      return true;
    if(canGoUp(i, j))
      return true;

    return false;
  }

  void drawRoom(int i, int j, sf::RenderWindow& window) {
    sprite.setTextureRect(sf::IntRect(solidColorPos, texAreaSize));
    switch(room(i, j).getStatus()) {
    case RoomStatus::Unvisited:
      sprite.setTextureRect(sf::IntRect(noColorPos, texAreaSize));
      break;
    case RoomStatus::Visited:
      sprite.setColor(visitedColor);
      break;
    case RoomStatus::Revisited:
      sprite.setColor(revisitedColor);
      break;
    case RoomStatus::Current:
      sprite.setColor(currentColor);
      break;
    case RoomStatus::Origin:
      sprite.setColor(startColor);
      break;
    case RoomStatus::Goal:
      sprite.setColor(goalColor);
      break;
    default:
      break;
    }
    sprite.setPosition(room(i, j).getXpos(), room(i, j).getYpos());
    window.draw(sprite);

    sprite.setColor(wallColor);
    if(room(i, j).hasUpperWall()) {
      sprite.setTextureRect(sf::IntRect(upperWallPos, texAreaSize));
      window.draw(sprite);
    }
    if(room(i, j).hasLowerWall()) {
      sprite.setTextureRect(sf::IntRect(lowerWallPos, texAreaSize));
      window.draw(sprite);
    }
    if(room(i, j).hasLeftWall()) {
      sprite.setTextureRect(sf::IntRect(leftWallPos, texAreaSize));
      window.draw(sprite);
    }
    if(room(i, j).hasRightWall()) {
      sprite.setTextureRect(sf::IntRect(rightWallPos, texAreaSize));
      window.draw(sprite);
    }

    sprite.setColor(wallColor);
    sprite.setTextureRect(sf::IntRect(noColorPos, texAreaSize));
    window.draw(sprite);

    sprite.setColor(pathColor);
    if(room(i, j).hasUpperPath()) {
      sprite.setTextureRect(sf::IntRect(upperPathPos, texAreaSize));
      window.draw(sprite);
    }
    if(room(i, j).hasLowerPath()) {
      sprite.setTextureRect(sf::IntRect(lowerPathPos, texAreaSize));
      window.draw(sprite);
    }
    if(room(i, j).hasLeftPath()) {
      sprite.setTextureRect(sf::IntRect(leftPathPos, texAreaSize));
      window.draw(sprite);
    }
    if(room(i, j).hasRightPath()) {
      sprite.setTextureRect(sf::IntRect(rightPathPos, texAreaSize));
      window.draw(sprite);
    }
  }
};


