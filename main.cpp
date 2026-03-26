#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <Windows.h>

int start = 0;
std::string ListName("");
std::string ListConfirmation("");
std::string ListSearch("");
std::vector<std::string> ListSearchVector;

int main(){
    std::cout << "Welcome User!" << std::endl;
    std::cout << "What would you like to do?" << std::endl << "Create New List(1)   " << "Open Existing List(2)   " << "Exit Program(3)" << std::endl;
    std::cin >> start;
    while(true){
        if(start == 1){
            std::cout << "Please input a name for the list, or type 'exit' to return back to the start menu.";
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
                            /*TODO DATABASE OPEN function */

                        }
                        else if(ListConfirmation == "N"){
                            break;
                        }
                        else{
                            std::cout << ListConfirmation << " is not an available command, please input 'Y' or 'N'" << std::endl;
                            break;
                        }
                    }

                }
            }
        }
        else if(start == 2){
            std::cout << "Please type the name of the list(s) you are looking for, Seperate them by hitting enter, press 'crtl + a' once you are done." << std::endl;  
            while(true){
                if(GetAsyncKeyState(VK_CONTROL)<0 && GetAsyncKeyState(0x41)<0){
                    std::cout << "Ctrl + A pressed, Searching Lists now!" << std::endl;
                    Sleep(100);
                }else{
                    std::cin >> ListSearch;
                    ListSearchVector.push_back(ListSearch);}
            }
            /*TODO search the DB for existing lists*/
            break;
        }
        else if(start == 3 ){
            std::cout << "Goodbye!";
            exit(0);
        }
        else{
           std::cout << start << " is not a valid number, please chose 1, 2, or 3." << std::endl;
        }
    }
}