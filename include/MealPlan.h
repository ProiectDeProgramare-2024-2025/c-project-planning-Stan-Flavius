#ifndef MEALPLAN_H
#define MEALPLAN_H

#include "Date.h"
#include "Recipe.h"
#include <vector>

class MealPlan : public Date {
public:
    std::vector<Recipe> recipes;
};

#endif // MEALPLAN_H
