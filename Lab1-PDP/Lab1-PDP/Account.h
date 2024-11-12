#pragma once
#include <vector>
#include <mutex>
#include "Operation.h"
class Account {
public:
    int id;
    int balance;
    std::vector<Operation> log;
    std::mutex mtx;

    Account(int id, int balance);

   
    void addLog(const Operation& op);
    int getBalance();
};

