#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <ctime>

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

Date getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    Date current;
    current.year = 1900 + ltm->tm_year;
    current.month = 1 + ltm->tm_mon;
    current.day = ltm->tm_mday;
    return current;
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

void viewPrepList(const map<string, int>& prepList, const vector<Recipe>& allRecipes) {
    if (prepList.empty()) {
        cout << "\033[34mMeal prep list\033[0m is empty." << endl;
        return;
    }

    cout << "\033[32mCurrent Meal Prep List\033[0m:" << endl;
    for (const auto& pair : prepList) {
        const string& recipeId = pair.first;
        int count = pair.second;
        const Recipe* recipe = findRecipeById(recipeId, allRecipes);

        if (recipe) {
            cout << "  - \033[35m" << recipe->name << "\033[0m (\033[33mID\033[0m: \033[36m" << recipe->id << "\033[0m), \033[33mCount\033[0m: \033[36m" << count << "\033[0m" << endl;
        } else {
            cout << "  - \033[31mUnknown Recipe\033[0m (\033[33mID\033[0m: \033[36m" << recipeId << "\033[0m), \033[33mCount\033[0m: \033[36m" << count << "\033[0m (\033[34mRecipe details\033[0m not found)" << endl;
        }
    }
}

void addRecipeToPrepList(map<string, int>& prepList, const string& id, int count, const vector<Recipe>& allRecipes) {
    if (count <= 0) {
        cerr << "\033[31mError\033[0m: \033[33mCount\033[0m must be a positive integer." << endl;
        return;
    }
    if (!findRecipeById(id, allRecipes)) {
        cerr << "\033[31mError\033[0m: \033[34mRecipe\033[0m with \033[33mID\033[0m '\033[36m" << id << "\033[0m' does not exist in your \033[34mkitchen\033[0m (\033[36mrecipes.txt\033[0m)." << endl;
        return;
    }

    prepList[id] += count;
    cout << "\033[34mRecipe ID\033[0m '\033[36m" << id << "\033[0m' added to \033[34mprep list\033[0m. Current \033[33mcount\033[0m: \033[36m" << prepList[id] << "\033[0m" << endl;
}

void modifyRecipeCountInPrepList(map<string, int>& prepList, const string& id, int newCount) {
    auto it = prepList.find(id);
    if (it != prepList.end()) {
        if (newCount > 0) {
            it->second = newCount;
            cout << "\033[33mCount\033[0m for \033[34mRecipe ID\033[0m '\033[36m" << id << "\033[0m' updated to \033[36m" << newCount << "\033[0m" << endl;
        } else {
            prepList.erase(it);
            cout << "\033[34mRecipe ID\033[0m '\033[36m" << id << "\033[0m' removed from \033[34mprep list\033[0m (\033[33mcount\033[0m set to 0 or less)." << endl;
        }
    } else {
        cout << "\033[34mRecipe ID\033[0m '\033[36m" << id << "\033[0m' not found in \033[34mprep list\033[0m." << endl;
    }
}

void deleteRecipeFromPrepList(map<string, int>& prepList, const string& id) {
    auto it = prepList.find(id);
    if (it != prepList.end()) {
        prepList.erase(it);
        cout << "\033[34mRecipe ID\033[0m '\033[36m" << id << "\033[0m' removed from \033[34mprep list\033[0m." << endl;
    } else {
        cout << "\033[34mRecipe ID\033[0m '\033[36m" << id << "\033[0m' not found in \033[34mprep list\033[0m." << endl;
    }
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
                cerr << "\033[31mWarning\033[0m: \033[34mRecipe ID\033[0m '\033[36m" << recipeId << "\033[0m' not found in \033[36mrecipes.txt\033[0m when loading \033[34mmeal plan\033[0m. Skipping." << endl;
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

void createMealPlan(map<string, int>& prepList, vector<MealPlan>& mealPlans, const vector<Recipe>& allRecipes) {
    if (prepList.empty()) {
        cout << "\033[34mMeal prep list\033[0m is empty. Nothing to create a \033[34mmeal plan\033[0m from." << endl;
        return;
    }

    MealPlan newMealPlan;
    Date currentDate = getCurrentDate();
    newMealPlan.day = currentDate.day;
    newMealPlan.month = currentDate.month;
    newMealPlan.year = currentDate.year;

    cout << "\033[32mCreating meal plan\033[0m for \033[36m" << newMealPlan.day << "/" << newMealPlan.month << "/" << newMealPlan.year << "\033[0m..." << endl;

    for (const auto& pair : prepList) {
        const string& recipeId = pair.first;
        int count = pair.second;
        const Recipe* recipe = findRecipeById(recipeId, allRecipes);

        if (recipe) {
            for (int i = 0; i < count; ++i) {
                newMealPlan.recipes.push_back(*recipe);
            }
        } else {
            cerr << "\033[31mWarning\033[0m: \033[34mRecipe ID\033[0m '\033[36m" << recipeId << "\033[0m' not found in \033[34mkitchen\033[0m. Skipping for \033[34mmeal plan\033[0m." << endl;
        }
    }

    mealPlans.push_back(newMealPlan);
    prepList.clear();
    cout << "\033[34mMeal plan\033[0m created \033[32msuccessfully\033[0m and \033[34mprep list\033[0m cleared." << endl;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "\033[31mUsage\033[0m: \033[36m" << argv[0] << "\033[0m <command> [arguments]" << endl;
        cerr << "\033[33mCommands\033[0m:" << endl;
        cerr << "  \033[34mview_prep_list\033[0m" << endl;
        cerr << "  \033[34madd_recipe\033[0m <id> <count>" << endl;
        cerr << "  \033[34mmodify_recipe\033[0m <id> <newCount>" << endl;
        cerr << "  \033[34mdelete_recipe\033[0m <id>" << endl;
        cerr << "  \033[34mcreate_meal_plan\033[0m" << endl;
        return 1;
    }

    string command = argv[1];
    vector<Recipe> allRecipes;
    map<string, int> mealPrepList;
    vector<MealPlan> mealPlans;

    loadRecipes("recipes.txt", allRecipes);
    loadMealPrepList("meal_prep.txt", mealPrepList);
    loadMealPlans("meal_plans.txt", mealPlans, allRecipes);

    if (command == "view_prep_list") {
        viewPrepList(mealPrepList, allRecipes);
    } else if (command == "add_recipe") {
        if (argc != 4) {
            cerr << "\033[31mUsage\033[0m: \033[36m" << argv[0] << "\033[0m \033[34madd_recipe\033[0m <id> <count>" << endl;
            return 1;
        }
        string id = argv[2];
        int count;
        try {
            count = stoi(argv[3]);
        } catch (const invalid_argument& e) {
            cerr << "\033[31mError\033[0m: Invalid \033[33mcount\033[0m format." << endl;
            return 1;
        } catch (const out_of_range& e) {
            cerr << "\033[31mError\033[0m: \033[33mCount\033[0m value out of range." << endl;
            return 1;
        }
        addRecipeToPrepList(mealPrepList, id, count, allRecipes);
    } else if (command == "modify_recipe") {
        if (argc != 4) {
            cerr << "\033[31mUsage\033[0m: \033[36m" << argv[0] << "\033[0m \033[34mmodify_recipe\033[0m <id> <newCount>" << endl;
            return 1;
        }
        string id = argv[2];
        int newCount;
        try {
            newCount = stoi(argv[3]);
        } catch (const invalid_argument& e) {
            cerr << "\033[31mError\033[0m: Invalid \033[33mnewCount\033[0m format." << endl;
            return 1;
        } catch (const out_of_range& e) {
            cerr << "\033[31mError\033[0m: \033[33mnewCount\033[0m value out of range." << endl;
            return 1;
        }
        modifyRecipeCountInPrepList(mealPrepList, id, newCount);
    } else if (command == "delete_recipe") {
        if (argc != 3) {
            cerr << "\033[31mUsage\033[0m: \033[36m" << argv[0] << "\033[0m \033[34mdelete_recipe\033[0m <id>" << endl;
            return 1;
        }
        string id = argv[2];
        deleteRecipeFromPrepList(mealPrepList, id);
    } else if (command == "create_meal_plan") {
        if (argc != 2) {
            cerr << "\033[31mUsage\033[0m: \033[36m" << argv[0] << "\033[0m \033[34mcreate_meal_plan\033[0m" << endl;
            return 1;
        }
        createMealPlan(mealPrepList, mealPlans, allRecipes);
    } else {
        cerr << "\033[31mInvalid command\033[0m: \033[36m" << command << "\033[0m" << endl;
        return 1;
    }

    saveMealPrepList("meal_prep.txt", mealPrepList);
    saveMealPlans("meal_plans.txt", mealPlans);

    return 0;
}
