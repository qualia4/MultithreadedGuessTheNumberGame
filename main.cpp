#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <ctime>
using namespace std;
using std::chrono::nanoseconds;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
bool lost = false;
bool win = false;
int iteration = 0;
int guessed = 0;
mutex _mutex;
condition_variable number_added;

void addTasks(vector<int>* numbers){
    srand(time(0));
    while(!lost and !win){
        int number = rand() % 10 + 1;
        numbers->emplace_back(number);
        cout << "Number " << iteration << "added" << endl;
        iteration++;
        number_added.notify_all();
        std::this_thread::sleep_for(std::chrono::duration<float>(3));
    }
}

void userHandler(vector<int>* numbers){
    unique_lock<mutex> lock(_mutex);
    if(iteration == 0){
        number_added.wait(lock);
    }
    while(!lost and !win){
        cout << "Game start, guess the number" << endl;
        int guessed_number;
        cin >> guessed_number;
        if(guessed_number < numbers->at(guessed)){
            cout << "The number is higher than " << guessed_number << endl;
        }
        else if(guessed_number > numbers->at(guessed)){
            cout << "The number is less than " << guessed_number << endl;
        }
        else{
            cout << "Correct!";
            guessed++;
            if(guessed == iteration){
                cout << "Congratulations! You won!" << endl;
            }
            else{
                cout << "Lets do the next one!" << endl;
            }
        }
    }

}

void checkIfLost(){
    while(!lost){
        int pastIteration = iteration;
        unique_lock<mutex> lock(_mutex);
        if(pastIteration != iteration){
            number_added.wait(lock);
        }
        if(iteration - guessed >= 15){
            lost = true;
            cout << "You lost!" << endl;
        }
    }
}

int main() {
    vector<int>* random_numbers;
    cout << "Numbers from 1 to 10 will be added to the queue every 3 seconds. Your goal is catch up with numbers. Write anything to start";
    string start;
    cin >> start;
    vector<thread> threads;
    threads.emplace_back(addTasks, random_numbers);
    threads.emplace_back(userHandler, random_numbers);
    threads.emplace_back(checkIfLost);
    for(int i = 0; i < threads.size(); i++){
        threads[i].join();
    }
}


