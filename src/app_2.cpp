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
        std::cerr << "  " BLUE "view_prep_list" RESET << std::endl;
        std::cerr << "  " BLUE "add_to_prep_list" RESET " <recipe_id> <count>" << std::endl;
        std::cerr << "  " BLUE "modify_prep_list_count" RESET " <recipe_id> <new_count>" << std::endl;
        std::cerr << "  " BLUE "delete_from_prep_list" RESET " <recipe_id>" << std::endl;
        std::cerr << "  " BLUE "create_meal_plan" RESET << std::endl;
        return 1;
    }

    std::string command = argv[1];
    std::vector<Recipe> recipes;
    std::map<std::string, int> prepList;
    std::vector<MealPlan> mealPlans;

    loadRecipes("recipes.txt", recipes);
    loadPrepList("prep_list.txt", prepList);
    loadMealPlans("meal_plans.txt", mealPlans, recipes);

    if (command == "view_prep_list") {
        viewPrepList(prepList, recipes);
    } else if (command == "add_to_prep_list") {
        if (argc != 4) {
            std::cerr << RED "Usage" RESET ": " CYAN << argv[0] << RESET " " BLUE "add_to_prep_list" RESET " <recipe_id> <count>" << std::endl;
            return 1;
        }
        std::string recipeId = argv[2];
        int count;
        try {
            count = std::stoi(argv[3]);
        } catch (const std::invalid_argument& e) {
            std::cerr << RED "Error" RESET ": Invalid " YELLOW "count" RESET " format." << std::endl;
            return 1;
        } catch (const std::out_of_range& e) {
            std::cerr << RED "Error" RESET ": " YELLOW "Count" RESET " value out of range." << std::endl;
            return 1;
        }
        addToPrepList(prepList, recipes, recipeId, count);
    } else if (command == "modify_prep_list_count") {
        if (argc != 4) {
            std::cerr << RED "Usage" RESET ": " CYAN << argv[0] << RESET " " BLUE "modify_prep_list_count" RESET " <recipe_id> <new_count>" << std::endl;
            return 1;
        }
        std::string recipeId = argv[2];
        int newCount;
        try {
            newCount = std::stoi(argv[3]);
        } catch (const std::invalid_argument& e) {
            std::cerr << RED "Error" RESET ": Invalid " YELLOW "new count" RESET " format." << std::endl;
            return 1;
        } catch (const std::out_of_range& e) {
            std::cerr << RED "Error" RESET ": " YELLOW "New count" RESET " value out of range." << std::endl;
            return 1;
        }
        modifyPrepListCount(prepList, recipes, recipeId, newCount);
    } else if (command == "delete_from_prep_list") {
        if (argc != 3) {
            std::cerr << RED "Usage" RESET ": " CYAN << argv[0] << RESET " " BLUE "delete_from_prep_list" RESET " <recipe_id>" << std::endl;
            return 1;
        }
        std::string recipeId = argv[2];
        deleteFromPrepList(prepList, recipeId);
    } else if (command == "create_meal_plan") {
        createMealPlan(prepList, mealPlans, recipes); // CORRECT ORDER
    } else {
        std::cerr << RED "Invalid command" RESET ": " CYAN << command << RESET << std::endl;
        return 1;
    }

    savePrepList("prep_list.txt", prepList);
    saveMealPlans("meal_plans.txt", mealPlans);
    // Recipes are only loaded for lookup, no need to save them here unless modified by this app

    return 0;
}
