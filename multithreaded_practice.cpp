#include <iostream>
#include <thread>
#include <functional>

using namespace std;

int main() {

    auto func1 = [](int thread_num){
       for(int i = 0; i < 10; ++i)
          cout << "This is thread " << thread_num << endl;
    };
 
    thread thread1(func1, 1); // create and run thread
    // NOTE: the first accepted argument is lambda expression, then its arguments follow up
    thread1.detach(); // make thread run independantly from main

    thread thread2(func1, 2); // create and run thread

    for(int i = 0; i < 10; ++i)
       cout << "This is main thread" << endl;

    thread2.join(); // wait until t1 returns

    return 0;
}
