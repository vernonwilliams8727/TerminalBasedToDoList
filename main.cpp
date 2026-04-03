#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <limits>
#define NOMINMAX
#include <Windows.h>
#include <sqlite3.h>

int start = 0;
std::string ListName("");
std::string ListConfirmation("");
std::string ListSearch("");
std::vector<std::string> ListSearchVector;

bool openDatabase(const std::string& filename, sqlite3*& db) {
    int rc = sqlite3_open_v2(filename.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << "\n";
        sqlite3_close(db);
        db = nullptr;
        return false;
    }
    return true;
}

bool ensureSchema(sqlite3* db) {
    const char* sql =
        "CREATE TABLE IF NOT EXISTS todo ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "title TEXT NOT NULL,"
        "done INTEGER NOT NULL DEFAULT 0);";
    char* err = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &err);
    if (rc != SQLITE_OK) {
        std::cerr << "Schema creation failed: " << (err ? err : "unknown") << "\n";
        sqlite3_free(err);
        return false;
    }
    return true;
}

bool addTask(sqlite3* db, const std::string& title) {
    const char* sql = "INSERT INTO todo (title, done) VALUES (?, 0);";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Prepare add task failed: " << sqlite3_errmsg(db) << "\n";
        return false;
    }
    sqlite3_bind_text(stmt, 1, title.c_str(), -1, SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Insert task failed: " << sqlite3_errmsg(db) << "\n";
        return false;
    }
    return true;
}

void listTasks(sqlite3* db) {
    const char* sql = "SELECT id, title, done FROM todo ORDER BY id;";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Prepare list tasks failed: " << sqlite3_errmsg(db) << "\n";
        return;
    }
    std::cout << "\nID  | Done | Task\n";
    std::cout << "---------------------\n";
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        int done = sqlite3_column_int(stmt, 2);
        std::cout << id << "  | " << (done ? "x" : " ") << "    | " << (text ? text : "") << "\n";
    }
    sqlite3_finalize(stmt);
}

bool setDone(sqlite3* db, int id, bool done) {
    const char* sql = "UPDATE todo SET done = ? WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Prepare update failed: " << sqlite3_errmsg(db) << "\n";
        return false;
    }
    sqlite3_bind_int(stmt, 1, done ? 1 : 0);
    sqlite3_bind_int(stmt, 2, id);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

bool deleteTask(sqlite3* db, int id) {
    const char* sql = "DELETE FROM todo WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Prepare delete failed: " << sqlite3_errmsg(db) << "\n";
        return false;
    }
    sqlite3_bind_int(stmt, 1, id);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

std::vector<std::string> searchLists(const std::vector<std::string>&query){
    std::vector<std::string> results;
    for(const auto& entry : std::filesystem::directory_iterator(std::filesystem::current_path())){
        if(entry.is_regular_file() && entry.path().extension() == ".db"){
            std::string stem = entry.path().stem().string();
            for(const auto &q : query){
                if(stem.find(q) != std::string::npos){
                    results.push_back(stem);
                    break;
                }
            }
        }
    }
    return results;
}


int main(){
    std::cout << "Welcome User!" << std::endl;
    std::cout << "What would you like to do?" << std::endl << "Create New List(1)   " << "Open Existing List(2)   " << "Exit Program(3)" << std::endl;
    std::cin >> start;
    while(true){
        if(start == 1){
            std::cout << "Please input a name for the list, or type 'exit' to return back to the start menu." << std::endl;
            std::cin >> ListName;
            std::transform(ListName.begin(), ListName.end(), ListName.begin(),[](unsigned char a){ return std::tolower(a);});
            if(ListName == "exit"){
                break;
            }else{
                while(true){
                    std::cout << "Are you sure you want your file to be named " << ListName << "?(Y/N)";
                    std::cin >> ListConfirmation;
                    while(true){
                        std::transform(ListConfirmation.begin(), ListConfirmation.end(), ListConfirmation.begin(),[](unsigned char b){ return std::toupper(b);});
                        if(ListConfirmation == "Y"){
                            std::string dbFile = ListName + ".db";
                            sqlite3* db = nullptr;
                            if(openDatabase(dbFile, db) && ensureSchema(db)){
                                while(true){
                                    std::cout << "\nList: " << ListName << " - options:\n";
                                    std::cout << "1=Add task, 2=List tasks, 3=Mark done, 4=Delete task, 5=Back" << std::endl;
                                    int op;
                                    if(!(std::cin >> op)){
                                        std::cin.clear();
                                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                        continue;
                                    }
                                    if(op == 5) break;
                                    if(op == 1){
                                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                        std::cout << "Enter task: ";
                                        std::string line;
                                        std::getline(std::cin, line);
                                        if(!line.empty()) addTask(db, line);
                                    } else if(op == 2){
                                        listTasks(db);
                                    } else if(op == 3){
                                        std::cout << "Enter task id to mark done: ";
                                        int id; std::cin >> id;
                                        setDone(db, id, true);
                                    } else if(op == 4){
                                        std::cout << "Enter task id to delete: ";
                                        int id; std::cin >> id;
                                        deleteTask(db, id);
                                    } else {
                                        std::cout << "Unknown option" << std::endl;
                                    }
                                }
                            }
                            if(db) sqlite3_close(db);
                            break;
                        }
                        else if(ListConfirmation == "N"){
                            break;
                        }
                        else{
                            std::cout << ListConfirmation << " is not an available command, please input 'Y' or 'N'" << std::endl;
                            break;
                        }
                    }
                    break;
                }
            }
        }
        else if(start == 2){
            std::cout << "Please type the name of the list(s) you are looking for, Seperate them with a space, press 'Enter' once you are done." << std::endl;
            std::string input;
            std::cin.ignore();  
            while(true){
                std::cout << "> ";
                std::getline(std::cin, input);

                if(input.empty()) break;
                ListSearchVector.push_back(input);
            }

            std::vector<std::string> foundLists;
            if(ListSearchVector.empty()){
                for(const auto & entry : std::filesystem::directory_iterator(std::filesystem::current_path())){
                    if(entry.is_regular_file() && entry.path().extension() == ".db"){
                        foundLists.push_back(entry.path().stem().string());
                    }
                }
            }else{
                foundLists = searchLists(ListSearchVector);
            }
            
            if(foundLists.empty()){
                std::cout << "No Lists Found." << std::endl;
            }else{
                std::cout << std::endl << "Found " << foundLists.size() << " Lists!" << std::endl;
                for(size_t i = 0; i < foundLists.size(); i++){
                    std::cout << i + 1 << ". " << foundLists[i] << std::endl;
                }
                std::cout << std::endl << "Select A List To Open (1-" << foundLists.size() << "), or 0 to go back: ";
                int choice;
                std::cin >> choice;
                
                if(choice > 0 && choice <= (int)foundLists.size()){
                    std::string selectedList = foundLists[choice -1];
                    std::string dbFile = selectedList + ".db";

                    sqlite3* db = nullptr;
                    if (openDatabase(dbFile, db) && ensureSchema(db)){
                        while(true){
                            std::cout << std::endl << "List(s): " << selectedList << " - options:\n";
                            std::cout << "please chose one of the following options for what action you'd like to performe" << std::endl << "1(Add task) 2(List Tasks) 3(Mark Task Done) 4(Delete Task) 0(back)" << std::endl;
                            int op;
                            if (!(std::cin >> op)){
                                std::cin.clear();
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                continue;
                            } 
                            if(op == 0) break;
                            if(op == 1){
                                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                                std::cout << "Enter Task: ";
                                std::string line;
                                std::getline(std::cin, line);
                                if(!line.empty()) addTask(db, line);
                            }else if(op == 2){
                                listTasks(db);
                            }else if(op == 3){
                                std::cout << "Enter Task ID TO Mark Done: ";
                                int id;
                                std::cin >> id;
                                setDone(db, id, true);
                            }else if(op == 4){
                                std::cout << "Enter Task ID to delete: ";
                                int id;
                                std::cin >> id;
                                deleteTask(db, id);
                            }else{
                                std::cout << "Unkown Option" << std::endl;
                            }
                        }
                    }
                    if (db) sqlite3_close(db);
                }
            }
            ListSearchVector.clear();
        }else if(start == 3 ){
            std::cout << std::endl << "Goodbye!";
            exit(0);
        }
        else{
           std::cout << start << " is not a valid number, please chose 1, 2, or 3." << std::endl;
        }
    }
}
