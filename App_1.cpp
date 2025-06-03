#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>

using namespace std;

class Recipe {
public:
    string id;
    string name;
    vector<string> ingredients;
    int prepTime;
};

class Date {
public:
    int day;
    int month;
    int year;
};

class MealPlan : public Date {
public:
    vector<Recipe> recipes;
};

const Recipe* findRecipeById(const string& id, const vector<Recipe>& allRecipes) {
    for (const auto& recipe : allRecipes) {
        if (recipe.id == id) {
            return &recipe;
        }
    }
    return nullptr;
}

void loadRecipes(const string& filename, vector<Recipe>& recipes) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "\033[31mWarning\033[0m: Could not open \033[36m" << filename << "\033[0m. Assuming no \033[34mrecipes\033[0m are available." << endl;
        return;
    }

    int numRecipes;
    if (!(file >> numRecipes)) {
        cerr << "\033[31mError\033[0m: Invalid \033[33mrecipe count\033[0m format." << endl;
        file.close();
        return;
    }
    file.ignore();

    recipes.clear();

    for (int i = 0; i < numRecipes; ++i) {
        Recipe recipe;
        getline(file, recipe.id);
        if (recipe.id.empty()) {
            file.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        getline(file, recipe.name);

        int ingredientCount;
        if (!(file >> ingredientCount)) {
            file.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        file.ignore();

        recipe.ingredients.resize(ingredientCount);
        for (int j = 0; j < ingredientCount; ++j) {
            getline(file, recipe.ingredients[j]);
        }

        if (!(file >> recipe.prepTime)) {
            file.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        file.ignore();
        recipes.push_back(recipe);
    }

    file.close();
}

void saveRecipes(const string& filename, const vector<Recipe>& recipes) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "\033[31mError\033[0m: Could not open \033[36m" << filename << "\033[0m for writing." << endl;
        return;
    }

    file << recipes.size() << endl;
    for (const auto& recipe : recipes) {
        file << recipe.id << endl;
        file << recipe.name << endl;
        file << recipe.ingredients.size() << endl;
        for (const auto& ingredient : recipe.ingredients) {
            file << ingredient << endl;
        }
        file << recipe.prepTime << endl;
    }

    file.close();
}

void loadMealPlans(const string& filename, vector<MealPlan>& mealPlans, const vector<Recipe>& allRecipes) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "\033[31mWarning\033[0m: Could not open \033[36m" << filename << "\033[0m. Starting with no saved \033[34mmeal plans\033[0m." << endl;
        return;
    }

    int numMealPlans;
    if (!(file >> numMealPlans)) {
        cerr << "\033[31mError\033[0m: Invalid \033[33mmeal plan count\033[0m format." << endl;
        file.close();
        return;
    }
    file.ignore();

    mealPlans.clear();

    for (int i = 0; i < numMealPlans; ++i) {
        MealPlan mp;
        if (!(file >> mp.day >> mp.month >> mp.year)) {
            file.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        file.ignore();

        int recipeCountInPlan;
        if (!(file >> recipeCountInPlan)) {
            file.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        file.ignore();

        mp.recipes.clear();

        for (int j = 0; j < recipeCountInPlan; ++j) {
            string recipeId;
            int countInPlan;
            if (!(file >> recipeId >> countInPlan)) {
                file.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            file.ignore();

            const Recipe* foundRecipe = findRecipeById(recipeId, allRecipes);
            if (foundRecipe) {
                for (int k = 0; k < countInPlan; ++k) {
                    mp.recipes.push_back(*foundRecipe);
                }
            } else {
                cerr << "\033[31mWarning\033[0m: \033[33mRecipe ID\033[0m '\033[36m" << recipeId << "\033[0m' not found in \033[36mrecipes.txt\033[0m when loading \033[34mmeal plan\033[0m. Skipping." << endl;
            }
        }
        mealPlans.push_back(mp);
    }
    file.close();
}

void saveMealPlans(const string& filename, const vector<MealPlan>& mealPlans) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "\033[31mError\033[0m: Could not open \033[36m" << filename << "\033[0m for writing." << endl;
        return;
    }

    file << mealPlans.size() << endl;
    for (const auto& mp : mealPlans) {
        file << mp.day << " " << mp.month << " " << mp.year << endl;

        map<string, int> recipeCountsInPlan;
        for (const auto& recipe : mp.recipes) {
            recipeCountsInPlan[recipe.id]++;
        }
        file << recipeCountsInPlan.size() << endl;
        for (const auto& pair : recipeCountsInPlan) {
            file << pair.first << " " << pair.second << endl;
        }
    }
    file.close();
}

void loadMealPrepList(const string& filename, map<string, int>& prepList) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "\033[31mWarning\033[0m: Could not open \033[36m" << filename << "\033[0m. Starting with an empty \033[34mmeal prep list\033[0m." << endl;
        return;
    }

    prepList.clear();
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string id;
        int count;
        if (ss >> id >> count) {
            prepList[id] = count;
        }
    }
    file.close();
}

void saveMealPrepList(const string& filename, const map<string, int>& prepList) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "\033[31mError\033[0m: Could not open \033[36m" << filename << "\033[0m for writing." << endl;
        return;
    }

    for (const auto& pair : prepList) {
        file << pair.first << " " << pair.second << endl;
    }
    file.close();
}

void viewRecipes(const vector<Recipe>& recipes) {
    if (recipes.empty()) {
        cout << "No \033[34mrecipes\033[0m available." << endl;
        return;
    }

    cout << "\033[32mAvailable Recipes\033[0m:" << endl;
    for (const auto& recipe : recipes) {
        cout << "  \033[33mID\033[0m: \033[36m" << recipe.id << "\033[0m" << endl;
        cout << "  \033[33mName\033[0m: \033[36m" << recipe.name << "\033[0m" << endl;
        cout << "  \033[33mIngredients\033[0m:" << endl;
        for (const auto& ingredient : recipe.ingredients) {
            cout << "    - \033[35m" << ingredient << "\033[0m" << endl;
        }
        cout << "  \033[33mPreparation Time\033[0m: \033[36m" << recipe.prepTime << "\033[0m minutes" << endl;
        cout << endl;
    }
}

void addRecipe(vector<Recipe>& recipes, const string& id, const string& name, const vector<string>& ingredients, int prepTime) {
    for (const auto& recipe : recipes) {
        if (recipe.id == id) {
            cout << "\033[31mError\033[0m: \033[34mRecipe\033[0m with \033[33mID\033[0m '\033[36m" << id << "\033[0m' already exists. Use \033[33mmodify_recipe\033[0m to update." << endl;
            return;
        }
    }

    Recipe newRecipe;
    newRecipe.id = id;
    newRecipe.name = name;
    newRecipe.ingredients = ingredients;
    newRecipe.prepTime = prepTime;
    recipes.push_back(newRecipe);
    cout << "\033[34mRecipe\033[0m added \033[32msuccessfully\033[0m!" << endl;
}

void deleteRecipe(vector<Recipe>& recipes, const string& id) {
    auto it = remove_if(recipes.begin(), recipes.end(), [&id](const Recipe& recipe) {
        return recipe.id == id;
    });

    if (it != recipes.end()) {
        recipes.erase(it, recipes.end());
        cout << "\033[34mRecipe\033[0m with \033[33mID\033[0m \033[36m" << id << "\033[0m deleted \033[32msuccessfully\033[0m!" << endl;
    } else {
        cout << "\033[34mRecipe\033[0m with \033[33mID\033[0m \033[36m" << id << "\033[0m not found." << endl;
    }
}

void modifyRecipe(vector<Recipe>& recipes, const string& id, const string& type, const string& newValue) {
    for (auto& recipe : recipes) {
        if (recipe.id == id) {
            if (type == "prepTime") {
                try {
                    recipe.prepTime = stoi(newValue);
                    cout << "\033[33mPreparation time\033[0m updated \033[32msuccessfully\033[0m for \033[34mrecipe ID\033[0m: \033[36m" << id << "\033[0m" << endl;
                } catch (const invalid_argument& e) {
                    cerr << "\033[31mError\033[0m: Invalid \033[33mpreparation time\033[0m format for '\033[36m" << newValue << "\033[0m'." << endl;
                } catch (const out_of_range& e) {
                    cerr << "\033[31mError\033[0m: \033[33mPreparation time\033[0m value out of range for '\033[36m" << newValue << "\033[0m'." << endl;
                }
                return;
            } else if (type == "ingredients") {
                vector<string> newIngredients;
                stringstream ss(newValue);
                string ingredient;
                while (getline(ss, ingredient, ',')) {
                    ingredient.erase(0, ingredient.find_first_not_of(" \t\n\r\f\v"));
                    ingredient.erase(ingredient.find_last_not_of(" \t\n\r\f\v") + 1);
                    if (!ingredient.empty()) {
                        newIngredients.push_back(ingredient);
                    }
                }
                recipe.ingredients = newIngredients;
                cout << "\033[33mIngredients\033[0m updated \033[32msuccessfully\033[0m for \033[34mrecipe ID\033[0m: \033[36m" << id << "\033[0m" << endl;
                return;
            } else {
                cerr << "\033[31mError\033[0m: Invalid modification \033[33mtype\033[0m. Use '\033[33mprepTime\033[0m' or '\033[33mingredients\033[0m'." << endl;
                return;
            }
        }
    }
    cout << "\033[34mRecipe\033[0m with \033[33mID\033[0m \033[36m" << id << "\033[0m not found." << endl;
}

void viewMealPlans(const vector<MealPlan>& mealPlans) {
    if (mealPlans.empty()) {
        cout << "No \033[34mmeal plans\033[0m available." << endl;
        return;
    }

    cout << "\033[32m--- Saved Meal Plans ---\033[0m" << endl;
    for (const auto& mealPlan : mealPlans) {
        cout << "\033[33mMeal Plan Date\033[0m: \033[36m" << mealPlan.day << "/" << mealPlan.month << "/" << mealPlan.year << "\033[0m" << endl;
        if (mealPlan.recipes.empty()) {
            cout << "  (No \033[34mrecipes\033[0m in this plan)" << endl;
        } else {
            map<string, int> recipeCounts;
            for (const auto& recipe : mealPlan.recipes) {
                recipeCounts[recipe.name]++;
            }
            cout << "  \033[33mRecipes\033[0m:" << endl;
            for (const auto& pair : recipeCounts) {
                cout << "    - \033[35m" << pair.first << "\033[0m (x\033[36m" << pair.second << "\033[0m)" << endl;
            }
        }
        cout << "\033[32m------------------------\033[0m" << endl << endl;
    }
}

vector<string> parseIngredients(int ingredientCount, char* argv[], int startIndex) {
    vector<string> ingredients;
    for (int i = 0; i < ingredientCount; ++i) {
        ingredients.push_back(argv[startIndex + i]);
    }
    return ingredients;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "\033[31mUsage\033[0m: \033[36m" << argv[0] << "\033[0m <command> [arguments]" << endl;
        cerr << "\033[33mCommands\033[0m:" << endl;
        cerr << "  \033[34mview_recipes\033[0m" << endl;
        cerr << "  \033[34madd_recipe\033[0m <id> <name> <ingredient_count> <ingredient1> ... <ingredientN> <prepTime>" << endl;
        cerr << "  \033[34mdelete_recipe\033[0m <id>" << endl;
        cerr << "  \033[34mmodify_recipe\033[0m <id> <prepTime|ingredients> <new_value>" << endl;
        cerr << "  \033[34mview_meal_plans\033[0m" << endl;
        return 1;
    }

    string command = argv[1];
    vector<Recipe> recipes;
    vector<MealPlan> mealPlans;
    map<string, int> mealPrepList;

    loadRecipes("recipes.txt", recipes);
    loadMealPlans("meal_plans.txt", mealPlans, recipes);
    loadMealPrepList("meal_prep.txt", mealPrepList);

    if (command == "view_recipes") {
        viewRecipes(recipes);
    } else if (command == "add_recipe") {
        if (argc < 6) {
            cerr << "\033[31mUsage\033[0m: \033[36m" << argv[0] << "\033[0m \033[34madd_recipe\033[0m <id> <name> <ingredient_count> <ingredient1> ... <ingredientN> <prepTime>" << endl;
            return 1;
        }
        string id = argv[2];
        string name = argv[3];
        int ingredientCount;
        try {
            ingredientCount = stoi(argv[4]);
        } catch (const invalid_argument& e) {
            cerr << "\033[31mError\033[0m: Invalid \033[33mingredient count\033[0m format." << endl;
            return 1;
        } catch (const out_of_range& e) {
            cerr << "\033[31mError\033[0m: \033[33mIngredient count\033[0m value out of range." << endl;
            return 1;
        }

        if (argc < 5 + ingredientCount + 1) {
            cerr << "\033[31mError\033[0m: Not enough arguments for \033[34madd_recipe\033[0m. Expected \033[36m" << (5 + ingredientCount + 1) << "\033[0m but got \033[36m" << argc << "\033[0m" << endl;
            cerr << "\033[31mUsage\033[0m: \033[36m" << argv[0] << "\033[0m \033[34madd_recipe\033[0m <id> <name> <ingredient_count> <ingredient1> ... <ingredientN> <prepTime>" << endl;
            return 1;
        }

        vector<string> ingredients = parseIngredients(ingredientCount, argv, 5);
        int prepTime;
        try {
            prepTime = stoi(argv[5 + ingredientCount]);
        } catch (const invalid_argument& e) {
            cerr << "\033[31mError\033[0m: Invalid \033[33mpreparation time\033[0m format." << endl;
            return 1;
        } catch (const out_of_range& e) {
            cerr << "\033[31mError\033[0m: \033[33mPreparation time\033[0m value out of range." << endl;
            return 1;
        }
        addRecipe(recipes, id, name, ingredients, prepTime);
    } else if (command == "delete_recipe") {
        if (argc != 3) {
            cerr << "\033[31mUsage\033[0m: \033[36m" << argv[0] << "\033[0m \033[34mdelete_recipe\033[0m <id>" << endl;
            return 1;
        }
        string id = argv[2];
        deleteRecipe(recipes, id);
    } else if (command == "modify_recipe") {
        if (argc != 5) {
            cerr << "\033[31mUsage\033[0m: \033[36m" << argv[0] << "\033[0m \033[34mmodify_recipe\033[0m <id> <prepTime|ingredients> <new_value>" << endl;
            return 1;
        }
        string id = argv[2];
        string type = argv[3];
        string newValue = argv[4];
        modifyRecipe(recipes, id, type, newValue);
    } else if (command == "view_meal_plans") {
        viewMealPlans(mealPlans);
    } else {
        cerr << "\033[31mInvalid command\033[0m: \033[36m" << command << "\033[0m" << endl;
        return 1;
    }

    saveRecipes("recipes.txt", recipes);
    saveMealPlans("meal_plans.txt", mealPlans);
    saveMealPrepList("meal_prep.txt", mealPrepList);

    return 0;
}
