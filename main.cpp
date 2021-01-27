#include <iostream>

#include <vector>

#include <thread>

#include <algorithm>

#include <ctime>

#include <chrono>

using std::cout;
using std::cin;
using std::endl;

std::mutex my_mutex;

void initialization ( int *initial, int size ){
    for (int i = 0; i < size; ++i) {
        initial[i] = std::rand() % 20000000001 - 10000000000;

        //initial[i] = std::rand() % 21 - 10;
    }
}

void find_max (int *initial, int start , int finish, int* result){
    int max = 0;

    for (int i = start; i < finish; ++i) {
        if (initial[i] > max) max = initial[i];
    }
    my_mutex.lock();
    cout << "For thread_id = " << std::this_thread::get_id() << "max is: " << max << endl;
    my_mutex.unlock();
    *result = max;
}

int find_max_seq (int *initial, int start , int finish){
    int max = 0;
    for (int i = start; i < finish; ++i) {
        if (initial[i] > max) max = initial[i];
    }
    cout << max << endl;
    return max;
}

int main() {

//    std::cout << std::thread::hardware_concurrency() << std::endl;
    std::srand(std::time(NULL));
    short int threads_quantity = 8;
    int quantity = 268435456 * 2; // интов в гб
    int step = quantity / threads_quantity;
    int* arr_extremums = new int [threads_quantity];
    int *initial = new int  [quantity];
    initialization(initial, quantity);
    std::thread* threads[threads_quantity];
    int start = 0;
    int finish = 0;
    auto begin = std::chrono::steady_clock::now();
    for (int i = 0; i < threads_quantity - 1; ++i) {
        finish += step;
        threads[i] = new std::thread(find_max, initial, start, finish, &arr_extremums[i]);
        start = finish;
    }
    find_max(initial, start, quantity, &arr_extremums[threads_quantity - 1]);
    for (int i = 0; i < threads_quantity - 1; ++i) {
        threads[i]->join();
        delete threads[i];
    }

    int max = 0;
    for (int i = 0; i < threads_quantity; i++){
        if (arr_extremums[i] > max) max = arr_extremums[i];
    }

    auto end = std::chrono::steady_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    cout << "The time parallel: " << elapsed_ms.count() << "ms" << endl;


    auto begin1 = std::chrono::steady_clock::now();
    int max_seq = find_max_seq(initial, 0, quantity);
    auto end1 = std::chrono::steady_clock::now();
    auto elapsed_ms1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - begin1);
    cout << "The time sequances: " << elapsed_ms1.count() << "ms" << endl;

    return 0;
}
