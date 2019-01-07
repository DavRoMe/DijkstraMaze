#pragma once

//#include <SFML/System/Vector2.hpp>

class DijkNode {
private:
    int accumDistance;
    bool labeled;
    std::pair<int, int> predecessor;
};
