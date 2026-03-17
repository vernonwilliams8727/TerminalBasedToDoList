#include <iostream>
#include <filesystem>

using namespace std;

int start = 0;
string ListName("");
int main(){
    cout << "Welcome User!" << endl;
    cout << "What would you like to do?" << endl 
         << "Create New List(1)   " << "Open Existing List(2)   " << "Exit Program(3)" << endl;
    cin >> start;
    while(true){
        if(start == 1){
            cout << "Please input a name for the list, or type 'exit' to return back to the start menu.";
            cin >> ListName;
            if(ListName == "exit"){
                break;
            }else{
                while(true){

                }
            }
        }
        else if(start == 2){}
        else if(start == 3 ){
            cout << "Goodbye!";
            exit(0);
        }
        else{
            cout << "That is not a valid number, please chose 1, 2, or 3." << endl;
        }
    }
}