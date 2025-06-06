#ifndef RECIPE_PLANNER_UTILS_H
#define RECIPE_PLANNER_UTILS_H

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <limits> // Required for numeric_limits
#include <sstream> // Required for stringstream

#include "Recipe.h"
#include "MealPlan.h"
#include "Date.h"

// ANSI escape codes for colors
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

const Recipe* findRecipeById(const std::string& id, const std::vector<Recipe>& allRecipes);
void loadRecipes(const std::string& filename, std::vector<Recipe>& recipes);
void saveRecipes(const std::string& filename, const std::vector<Recipe>& recipes);
void loadMealPlans(const std::string& filename, std::vector<MealPlan>& mealPlans, const std::vector<Recipe>& allRecipes);
void saveMealPlans(const std::string& filename, const std::vector<MealPlan>& mealPlans);
void loadMealPrepList(const std::string& filename, std::map<std::string, int>& prepList);
void saveMealPrepList(const std::string& filename, const std::map<std::string, int>& prepList);
void viewRecipes(const std::vector<Recipe>& recipes);
void addRecipe(std::vector<Recipe>& recipes, const std::string& id, const std::string& name, const std::vector<std::string>& ingredients, int prepTime);
void deleteRecipe(std::vector<Recipe>& recipes, const std::string& id);
void modifyRecipe(std::vector<Recipe>& recipes, const std::string& id, const std::string& type, const std::string& newValue);
void viewMealPlans(const std::vector<MealPlan>& mealPlans);
std::vector<std::string> parseIngredients(int ingredientCount, char* argv[], int startIndex);
Date getCurrentDate();
void viewPrepList(const std::map<std::string, int>& prepList, const std::vector<Recipe>& allRecipes);
void addRecipeToPrepList(std::map<std::string, int>& prepList, const std::string& id, int count, const std::vector<Recipe>& allRecipes);
void modifyRecipeCountInPrepList(std::map<std::string, int>& prepList, const std::string& id, int newCount);
void deleteRecipeFromPrepList(std::map<std::string, int>& prepList, const std::string& id);
void createMealPlan(std::map<std::string, int>& prepList, std::vector<MealPlan>& mealPlans, const std::vector<Recipe>& allRecipes);

// Inside include/RecipePlannerUtils.h, with other function declarations

// ... other declarations ...

// Prep List management functions
void loadPrepList(const std::string& filename, std::map<std::string, int>& prepList);
void savePrepList(const std::string& filename, const std::map<std::string, int>& prepList);
void viewPrepList(const std::map<std::string, int>& prepList, const std::vector<Recipe>& allRecipes);
void addToPrepList(std::map<std::string, int>& prepList, const std::vector<Recipe>& allRecipes, const std::string& recipeId, int count);
void modifyPrepListCount(std::map<std::string, int>& prepList, const std::vector<Recipe>& allRecipes, const std::string& recipeId, int newCount);
void deleteFromPrepList(std::map<std::string, int>& prepList, const std::string& recipeId);

// Meal Plan functions
void createMealPlan(std::map<std::string, int>& prepList, std::vector<MealPlan>& mealPlans, const std::vector<Recipe>& allRecipes);
void viewMealPlans(const std::vector<MealPlan>& mealPlans);
void saveMealPlans(const std::string& filename, const std::vector<MealPlan>& mealPlans);
void loadMealPlans(const std::string& filename, std::vector<MealPlan>& mealPlans, const std::vector<Recipe>& allRecipes);

// ... other declarations ...

#endif // RECIPE_PLANNER_UTILS_H
