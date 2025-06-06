#include "RecipePlannerUtils.h"
#include <fstream>
#include <algorithm>
#include <ctime>
#include <limits> // Required for numeric_limits
#include <sstream> // Required for stringstream

const Recipe* findRecipeById(const std::string& id, const std::vector<Recipe>& allRecipes) {
    for (const auto& recipe : allRecipes) {
        if (recipe.id == id) {
            return &recipe;
        }
    }
    return nullptr;
}

void loadRecipes(const std::string& filename, std::vector<Recipe>& recipes) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << RED "Warning" RESET ": Could not open " CYAN << filename << RESET ". Assuming no " BLUE "recipes" RESET " are available." << std::endl;
        return;
    }

    int numRecipes;
    if (!(file >> numRecipes)) {
        std::cerr << RED "Error" RESET ": Invalid " YELLOW "recipe count" RESET " format." << std::endl;
        file.close();
        return;
    }
    file.ignore();

    recipes.clear();

    for (int i = 0; i < numRecipes; ++i) {
        Recipe recipe;
        std::getline(file, recipe.id);
        if (recipe.id.empty()) {
            file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::getline(file, recipe.name);

        int ingredientCount;
        if (!(file >> ingredientCount)) {
            file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        file.ignore();

        recipe.ingredients.resize(ingredientCount);
        for (int j = 0; j < ingredientCount; ++j) {
            std::getline(file, recipe.ingredients[j]);
        }

        if (!(file >> recipe.prepTime)) {
            file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        file.ignore();
        recipes.push_back(recipe);
    }

    file.close();
}

void saveRecipes(const std::string& filename, const std::vector<Recipe>& recipes) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << RED "Error" RESET ": Could not open " CYAN << filename << RESET " for writing." << std::endl;
        return;
    }

    file << recipes.size() << std::endl;
    for (const auto& recipe : recipes) {
        file << recipe.id << std::endl;
        file << recipe.name << std::endl;
        file << recipe.ingredients.size() << std::endl;
        for (const auto& ingredient : recipe.ingredients) {
            file << ingredient << std::endl;
        }
        file << recipe.prepTime << std::endl;
    }

    file.close();
}

void loadMealPlans(const std::string& filename, std::vector<MealPlan>& mealPlans, const std::vector<Recipe>& allRecipes) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << RED "Warning" RESET ": Could not open " CYAN << filename << RESET ". Starting with no saved " BLUE "meal plans" RESET "." << std::endl;
        return;
    }

    int numMealPlans;
    if (!(file >> numMealPlans)) {
        std::cerr << RED "Error" RESET ": Invalid " YELLOW "meal plan count" RESET " format." << std::endl;
        file.close();
        return;
    }
    file.ignore();

    mealPlans.clear();

    for (int i = 0; i < numMealPlans; ++i) {
        MealPlan mp;
        if (!(file >> mp.day >> mp.month >> mp.year)) {
            file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        file.ignore();

        int recipeCountInPlan;
        if (!(file >> recipeCountInPlan)) {
            file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        file.ignore();

        mp.recipes.clear();

        for (int j = 0; j < recipeCountInPlan; ++j) {
            std::string recipeId;
            int countInPlan;
            if (!(file >> recipeId >> countInPlan)) {
                file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }
            file.ignore();

            const Recipe* foundRecipe = findRecipeById(recipeId, allRecipes);
            if (foundRecipe) {
                for (int k = 0; k < countInPlan; ++k) {
                    mp.recipes.push_back(*foundRecipe);
                }
            } else {
                std::cerr << RED "Warning" RESET ": " YELLOW "Recipe ID" RESET " '" CYAN << recipeId << RESET "' not found in " CYAN "recipes.txt" RESET " when loading " BLUE "meal plan" RESET ". Skipping." << std::endl;
            }
        }
        mealPlans.push_back(mp);
    }
    file.close();
}

void saveMealPlans(const std::string& filename, const std::vector<MealPlan>& mealPlans) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << RED "Error" RESET ": Could not open " CYAN << filename << RESET " for writing." << std::endl;
        return;
    }

    file << mealPlans.size() << std::endl;
    for (const auto& mp : mealPlans) {
        file << mp.day << " " << mp.month << " " << mp.year << std::endl;

        std::map<std::string, int> recipeCountsInPlan;
        for (const auto& recipe : mp.recipes) {
            recipeCountsInPlan[recipe.id]++;
        }
        file << recipeCountsInPlan.size() << std::endl;
        for (const auto& pair : recipeCountsInPlan) {
            file << pair.first << " " << pair.second << std::endl;
        }
    }
    file.close();
}

// Implementations for Prep List functions
void loadPrepList(const std::string& filename, std::map<std::string, int>& prepList) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << RED "Warning" RESET ": Could not open " CYAN << filename << RESET ". Starting with an empty " BLUE "meal prep list" RESET "." << std::endl;
        return;
    }

    prepList.clear();
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string id;
        int count;
        if (ss >> id >> count) {
            prepList[id] = count;
        }
    }
    file.close();
}

void savePrepList(const std::string& filename, const std::map<std::string, int>& prepList) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << RED "Error" RESET ": Could not open " CYAN << filename << RESET " for writing." << std::endl;
        return;
    }

    for (const auto& pair : prepList) {
        file << pair.first << " " << pair.second << std::endl;
    }
    file.close();
}

void viewRecipes(const std::vector<Recipe>& recipes) {
    if (recipes.empty()) {
        std::cout << "No " BLUE "recipes" RESET " available." << std::endl;
        return;
    }

    std::cout << GREEN "Available Recipes" RESET ":" << std::endl;
    for (const auto& recipe : recipes) {
        std::cout << "  " YELLOW "ID" RESET ": " CYAN << recipe.id << RESET << std::endl;
        std::cout << "  " YELLOW "Name" RESET ": " CYAN << recipe.name << RESET << std::endl;
        std::cout << "  " YELLOW "Ingredients" RESET ":" << std::endl;
        for (const auto& ingredient : recipe.ingredients) {
            std::cout << "    - " MAGENTA << ingredient << RESET << std::endl;
        }
        std::cout << "  " YELLOW "Preparation Time" RESET ": " CYAN << recipe.prepTime << RESET " minutes" << std::endl;
        std::cout << std::endl;
    }
}

void addRecipe(std::vector<Recipe>& recipes, const std::string& id, const std::string& name, const std::vector<std::string>& ingredients, int prepTime) {
    for (const auto& recipe : recipes) {
        if (recipe.id == id) {
            std::cout << RED "Error" RESET ": " BLUE "Recipe" RESET " with " YELLOW "ID" RESET " '" CYAN << id << RESET "' already exists. Use " YELLOW "modify_recipe" RESET " to update." << std::endl;
            return;
        }
    }

    Recipe newRecipe;
    newRecipe.id = id;
    newRecipe.name = name;
    newRecipe.ingredients = ingredients;
    newRecipe.prepTime = prepTime;
    recipes.push_back(newRecipe);
    std::cout << BLUE "Recipe" RESET " added " GREEN "successfully" RESET "!" << std::endl;
}

void deleteRecipe(std::vector<Recipe>& recipes, const std::string& id) {
    auto it = std::remove_if(recipes.begin(), recipes.end(), [&id](const Recipe& recipe) {
        return recipe.id == id;
    });

    if (it != recipes.end()) {
        recipes.erase(it, recipes.end());
        std::cout << BLUE "Recipe" RESET " with " YELLOW "ID" RESET " " CYAN << id << RESET " deleted " GREEN "successfully" RESET "!" << std::endl;
    } else {
        std::cout << BLUE "Recipe" RESET " with " YELLOW "ID" RESET " " CYAN << id << RESET " not found." << std::endl;
    }
}

void modifyRecipe(std::vector<Recipe>& recipes, const std::string& id, const std::string& type, const std::string& newValue) {
    for (auto& recipe : recipes) {
        if (recipe.id == id) {
            if (type == "prepTime") {
                try {
                    recipe.prepTime = std::stoi(newValue);
                    std::cout << YELLOW "Preparation time" RESET " updated " GREEN "successfully" RESET " for " BLUE "recipe ID" RESET ": " CYAN << id << RESET << std::endl;
                } catch (const std::invalid_argument& e) {
                    std::cerr << RED "Error" RESET ": Invalid " YELLOW "preparation time" RESET " format for '" CYAN << newValue << RESET "'." << std::endl;
                } catch (const std::out_of_range& e) {
                    std::cerr << RED "Error" RESET ": " YELLOW "Preparation time" RESET " value out of range for '" CYAN << newValue << RESET "'." << std::endl;
                }
                return;
            } else if (type == "ingredients") {
                std::vector<std::string> newIngredients;
                std::stringstream ss(newValue);
                std::string ingredient;
                while (std::getline(ss, ingredient, ',')) {
                    ingredient.erase(0, ingredient.find_first_not_of(" \t\n\r\f\v"));
                    ingredient.erase(ingredient.find_last_not_of(" \t\n\r\f\v") + 1);
                    if (!ingredient.empty()) {
                        newIngredients.push_back(ingredient);
                    }
                }
                recipe.ingredients = newIngredients;
                std::cout << YELLOW "Ingredients" RESET " updated " GREEN "successfully" RESET " for " BLUE "recipe ID" RESET ": " CYAN << id << RESET << std::endl;
                return;
            } else {
                std::cerr << RED "Error" RESET ": Invalid modification " YELLOW "type" RESET ". Use '" YELLOW "prepTime" RESET "' or '" YELLOW "ingredients" RESET "'." << std::endl;
                return;
            }
        }
    }
    std::cout << BLUE "Recipe" RESET " with " YELLOW "ID" RESET " " CYAN << id << RESET " not found." << std::endl;
}

void viewMealPlans(const std::vector<MealPlan>& mealPlans) {
    if (mealPlans.empty()) {
        std::cout << "No " BLUE "meal plans" RESET " available." << std::endl;
        return;
    }

    std::cout << GREEN "--- Saved Meal Plans ---" RESET << std::endl;
    for (const auto& mealPlan : mealPlans) {
        std::cout << YELLOW "Meal Plan Date" RESET ": " CYAN << mealPlan.day << "/" << mealPlan.month << "/" << mealPlan.year << RESET << std::endl;
        if (mealPlan.recipes.empty()) {
            std::cout << "  (No " BLUE "recipes" RESET " in this plan)" << std::endl;
        } else {
            std::map<std::string, int> recipeCounts;
            for (const auto& recipe : mealPlan.recipes) {
                recipeCounts[recipe.name]++;
            }
            std::cout << "  " YELLOW "Recipes" RESET ":" << std::endl;
            for (const auto& pair : recipeCounts) {
                std::cout << "    - " MAGENTA << pair.first << RESET " (x" CYAN << pair.second << RESET ")" << std::endl;
            }
        }
        std::cout << GREEN "------------------------" RESET << std::endl << std::endl;
    }
}

std::vector<std::string> parseIngredients(int ingredientCount, char* argv[], int startIndex) {
    std::vector<std::string> ingredients;
    for (int i = 0; i < ingredientCount; ++i) {
        ingredients.push_back(argv[startIndex + i]);
    }
    return ingredients;
}

Date getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    Date current;
    current.year = 1900 + ltm->tm_year;
    current.month = 1 + ltm->tm_mon;
    current.day = ltm->tm_mday;
    return current;
}

void viewPrepList(const std::map<std::string, int>& prepList, const std::vector<Recipe>& allRecipes) {
    if (prepList.empty()) {
        std::cout << BLUE "Meal prep list" RESET " is empty." << std::endl;
        return;
    }

    std::cout << GREEN "Current Meal Prep List" RESET ":" << std::endl;
    for (const auto& pair : prepList) {
        const std::string& recipeId = pair.first;
        int count = pair.second;
        const Recipe* recipe = findRecipeById(recipeId, allRecipes);

        if (recipe) {
            std::cout << "  - " MAGENTA << recipe->name << RESET " (" YELLOW "ID" RESET ": " CYAN << recipe->id << RESET "), " YELLOW "Count" RESET ": " CYAN << count << RESET << std::endl;
        } else {
            std::cout << "  - " RED "Unknown Recipe" RESET " (" YELLOW "ID" RESET ": " CYAN << recipeId << RESET "), " YELLOW "Count" RESET ": " CYAN << count << RESET " (" BLUE "Recipe details" RESET " not found)" << std::endl;
        }
    }
}

void addToPrepList(std::map<std::string, int>& prepList, const std::vector<Recipe>& allRecipes, const std::string& recipeId, int count) {
    if (count <= 0) {
        std::cerr << RED "Error" RESET ": " YELLOW "Count" RESET " must be a positive integer." << std::endl;
        return;
    }
    if (!findRecipeById(recipeId, allRecipes)) {
        std::cerr << RED "Error" RESET ": " BLUE "Recipe" RESET " with " YELLOW "ID" RESET " '" CYAN << recipeId << RESET "' does not exist in your " BLUE "kitchen" RESET " (" CYAN "recipes.txt" RESET ")." << std::endl;
        return;
    }

    prepList[recipeId] += count;
    std::cout << BLUE "Recipe ID" RESET " '" CYAN << recipeId << RESET "' added to " BLUE "prep list" RESET ". Current " YELLOW "count" RESET ": " CYAN << prepList[recipeId] << RESET << std::endl;
}

void modifyPrepListCount(std::map<std::string, int>& prepList, const std::vector<Recipe>& allRecipes, const std::string& recipeId, int newCount) {
    auto it = prepList.find(recipeId);
    if (it != prepList.end()) {
        if (newCount > 0) {
            it->second = newCount;
            std::cout << YELLOW "Count" RESET " for " BLUE "Recipe ID" RESET " '" CYAN << recipeId << RESET "' updated to " CYAN << newCount << RESET << std::endl;
        } else {
            prepList.erase(it);
            std::cout << BLUE "Recipe ID" RESET " '" CYAN << recipeId << RESET "' removed from " BLUE "prep list" RESET " (" YELLOW "count" RESET " set to 0 or less)." << std::endl;
        }
    } else {
        std::cout << BLUE "Recipe ID" RESET " '" CYAN << recipeId << RESET "' not found in " BLUE "prep list" RESET "." << std::endl;
    }
}

void deleteFromPrepList(std::map<std::string, int>& prepList, const std::string& recipeId) {
    auto it = prepList.find(recipeId);
    if (it != prepList.end()) {
        prepList.erase(it);
        std::cout << BLUE "Recipe ID" RESET " '" CYAN << recipeId << RESET "' removed from " BLUE "prep list" RESET "." << std::endl;
    } else {
        std::cout << BLUE "Recipe ID" RESET " '" CYAN << recipeId << RESET "' not found in " BLUE "prep list" RESET "." << std::endl;
    }
}

void createMealPlan(std::map<std::string, int>& prepList, std::vector<MealPlan>& mealPlans, const std::vector<Recipe>& allRecipes) {
    if (prepList.empty()) {
        std::cout << BLUE "Meal prep list" RESET " is empty. Nothing to create a " BLUE "meal plan" RESET " from." << std::endl;
        return;
    }

    MealPlan newMealPlan;
    Date currentDate = getCurrentDate();
    newMealPlan.day = currentDate.day;
    newMealPlan.month = currentDate.month;
    newMealPlan.year = currentDate.year;

    std::cout << GREEN "Creating meal plan" RESET " for " CYAN << newMealPlan.day << "/" << newMealPlan.month << "/" << newMealPlan.year << RESET "..." << std::endl;

    for (const auto& pair : prepList) {
        const std::string& recipeId = pair.first;
        int count = pair.second;
        const Recipe* recipe = findRecipeById(recipeId, allRecipes);

        if (recipe) {
            for (int i = 0; i < count; ++i) {
                newMealPlan.recipes.push_back(*recipe);
            }
        } else {
            std::cerr << RED "Warning" RESET ": " BLUE "Recipe ID" RESET " '" CYAN << recipeId << RESET "' not found in " BLUE "kitchen" RESET ". Skipping for " BLUE "meal plan" RESET "." << std::endl;
        }
    }

    mealPlans.push_back(newMealPlan);
    prepList.clear();
    std::cout << BLUE "Meal plan" RESET " created " GREEN "successfully" RESET " and " BLUE "prep list" RESET " cleared." << std::endl;
}
