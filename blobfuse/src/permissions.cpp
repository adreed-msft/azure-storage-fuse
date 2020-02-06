//
// Created by adreed on 2/6/2020.
//

#include <permissions.h>

std::string modeToString(mode_t mode) {
    std::string result;

    for (int i = 0; i < 3; i++)
    {
        // Push back the string with each of the mode segments
        result += mode & 01 ? 'x' : '-';
        result += mode & 02 ? 'w' : '-';
        result += mode & 04 ? 'r' : '-';
        // Push mode forward 3
        mode = mode >> 3;
    }

    return result;
}