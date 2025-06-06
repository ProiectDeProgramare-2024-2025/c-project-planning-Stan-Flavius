#ifndef RECIPE_H
#define RECIPE_H

#include <string>
#include <vector>

class Recipe {
public:
    std::string id;
    std::string name;
    std::vector<std::string> ingredients;
    int prepTime;
};

#endif // RECIPE_H
