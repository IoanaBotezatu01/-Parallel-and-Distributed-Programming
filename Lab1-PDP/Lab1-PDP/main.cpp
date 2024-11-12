#include "Account.h"
#include "Bank.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <thread>
#include <tuple>
#include <vector>
#include "main.h"

Bank bank;
std::mutex global_mtx;
bool running = true; 

void loadAccountsFromFile(const std::string& filename, std::vector<std::unique_ptr<Account>>& accounts) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) throw std::runtime_error("Unable to open file");

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            std::istringstream iss(line);
            int id, balance;
            iss >> id >> balance;
            accounts.emplace_back(std::make_unique<Account>(id, balance));
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading accounts: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown error loading accounts." << std::endl;
    }
}

void loadTransfersFromFile(const std::string& filename, std::vector<std::tuple<int, int, int>>& transfers) {
    std::ifstream file(filename);
    if (!file.is_open()) throw std::runtime_error("Unable to open file");

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        int source, destination, amount;
        iss >> source >> destination >> amount;
        transfers.emplace_back(source, destination, amount);
    }
}


void transferMoney(Account* source, Account* destination, int amount) {
    std::lock(source->mtx, destination->mtx);

    
    if (source->balance >= amount) {
        source->balance -= amount;
        destination->balance += amount;

    }
    else {
        std::cerr << "Transfer error: Insufficient funds from account " << source->id << std::endl;
    }
    source->mtx.unlock();       
    destination->mtx.unlock();

}


void periodicCheck(std::vector<std::unique_ptr<Account>>& accounts) {
    while (running) { 
        std::this_thread::sleep_for(std::chrono::seconds(1));
        {
            std::lock_guard<std::mutex> lock(global_mtx);
            bank.consistencyCheck(accounts); 
        }
    }
}

int main() {
    std::vector<std::unique_ptr<Account>> accounts;
    std::vector<std::tuple<int, int, int>> transfers;

    try {
        loadAccountsFromFile("Accounts.txt", accounts);
        loadTransfersFromFile("Transfers.txt", transfers);
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading files: " << e.what() << std::endl;
        return 1; 
    }

    std::vector<std::thread> transferThreads;
    std::thread checkerThread(periodicCheck, std::ref(accounts)); 

    
    for (const auto& transfer : transfers) {
        int source_id, dest_id, amount;
        std::tie(source_id, dest_id, amount) = transfer;

       
        if (source_id >= accounts.size() || dest_id >= accounts.size()) {
            std::cerr << "Invalid account index in transfer: " << source_id << ", " << dest_id << std::endl;
            continue;  
        }

        
        transferThreads.emplace_back(transferMoney, accounts[source_id].get(), accounts[dest_id].get(), amount);
    }

    
    for (auto& thread : transferThreads) {
        thread.join();
    }

    
    {
        std::lock_guard<std::mutex> lock(global_mtx);
        bank.consistencyCheck(accounts);
    }

    
    running = false; 
    checkerThread.join(); 

    std::cout << "All transfers completed successfully." << std::endl;
    std::cout << "Account Balances After Transfers:" << std::endl;
    for (const auto& accountPtr : accounts) {
        Account& account = *accountPtr; 
        std::cout << "Account ID: " << account.id << ", Balance: " << account.getBalance() << std::endl;
    }
    return 0;
}
