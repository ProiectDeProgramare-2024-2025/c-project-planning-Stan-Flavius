#include "RecipePlannerUtils.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <stdexcept> // For std::invalid_argument, std::out_of_range

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << RED "Usage" RESET ": " CYAN << argv[0] << RESET " <command> [arguments]" << std::endl;
        std::cerr << YELLOW "Commands" RESET ":" << std::endl;
        std::cerr << "  " BLUE "view_recipes" RESET << std::endl;
        std::cerr << "  " BLUE "add_recipe" RESET " <id> <name> <ingredient_count> <ingredient1> ... <ingredientN> <prepTime>" << std::endl;
        std::cerr << "  " BLUE "delete_recipe" RESET " <id>" << std::endl;
        std::cerr << "  " BLUE "modify_recipe" RESET " <id> <prepTime|ingredients> <new_value>" << std::endl;
        std::cerr << "  " BLUE "view_meal_plans" RESET << std::endl; // Now part of app_1
        return 1;
    }

    std::string command = argv[1];
    std::vector<Recipe> recipes;
    std::vector<MealPlan> mealPlans; // Needed for view_meal_plans in this app

    loadRecipes("recipes.txt", recipes);
    // Load meal plans if this app is viewing them
    loadMealPlans("meal_plans.txt", mealPlans, recipes);

    if (command == "view_recipes") {
        viewRecipes(recipes);
    } else if (command == "add_recipe") {
        if (argc < 6) {
            std::cerr << RED "Usage" RESET ": " CYAN << argv[0] << RESET " " BLUE "add_recipe" RESET " <id> <name> <ingredient_count> <ingredient1> ... <ingredientN> <prepTime>" << std::endl;
            return 1;
        }
        std::string id = argv[2];
        std::string name = argv[3];
        int ingredientCount;
        try {
            ingredientCount = std::stoi(argv[4]);
        } catch (const std::invalid_argument& e) {
            std::cerr << RED "Error" RESET ": Invalid " YELLOW "ingredient count" RESET " format." << std::endl;
            return 1;
        } catch (const std::out_of_range& e) {
            std::cerr << RED "Error" RESET ": " YELLOW "Ingredient count" RESET " value out of range." << std::endl;
            return 1;
        }

        if (argc < 5 + ingredientCount + 1) { // 2 (app_name, cmd) + 3 (id, name, count) + ingredientCount + 1 (prepTime)
            std::cerr << RED "Error" RESET ": Not enough arguments for " BLUE "add_recipe" RESET ". Expected " CYAN << (5 + ingredientCount + 1) << RESET " but got " CYAN << argc << RESET << std::endl;
            std::cerr << RED "Usage" RESET ": " CYAN << argv[0] << RESET " " BLUE "add_recipe" RESET " <id> <name> <ingredient_count> <ingredient1> ... <ingredientN> <prepTime>" << std::endl;
            return 1;
        }

        std::vector<std::string> ingredients = parseIngredients(ingredientCount, argv, 5);
        int prepTime;
        try {
            prepTime = std::stoi(argv[5 + ingredientCount]);
        } catch (const std::invalid_argument& e) {
            std::cerr << RED "Error" RESET ": Invalid " YELLOW "preparation time" RESET " format." << std::endl;
            return 1;
        } catch (const std::out_of_range& e) {
            std::cerr << RED "Error" RESET ": " YELLOW "Preparation time" RESET " value out of range." << std::endl;
            return 1;
        }
        addRecipe(recipes, id, name, ingredients, prepTime);
    } else if (command == "delete_recipe") {
        if (argc != 3) {
            std::cerr << RED "Usage" RESET ": " CYAN << argv[0] << RESET " " BLUE "delete_recipe" RESET " <id>" << std::endl;
            return 1;
        }
        std::string id = argv[2];
        deleteRecipe(recipes, id);
    } else if (command == "modify_recipe") {
        if (argc != 5) {
            std::cerr << RED "Usage" RESET ": " CYAN << argv[0] << RESET " " BLUE "modify_recipe" RESET " <id> <prepTime|ingredients> <new_value>" << std::endl;
            return 1;
        }
        std::string id = argv[2];
        std::string type = argv[3];
        std::string newValue = argv[4];
        modifyRecipe(recipes, id, type, newValue);
    } else if (command == "view_meal_plans") {
        viewMealPlans(mealPlans);
    } else {
        std::cerr << RED "Invalid command" RESET ": " CYAN << command << RESET << std::endl;
        return 1;
    }

    saveRecipes("recipes.txt", recipes);
    saveMealPlans("meal_plans.txt", mealPlans); // Save meal plans if they were loaded/modified (though only view here)

    return 0;
}
