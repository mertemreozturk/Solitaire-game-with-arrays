#include <iostream>
#include <string>
#include "Controller.cpp"
using namespace std;

int main(int argc, char* argv[]){

    Controller controller;
    controller.control(argv[1],argv[2],argv[3]);

    return 0;
}
