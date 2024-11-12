#include "Bank.h"
#include <iostream>

Bank::Bank()
{
	serial_counter = 0;
}

int Bank::getNextSerialNumber()
{
	return serial_counter++;
}

void Bank::transfer(Account& source, Account& destination, int amount)
{
	Account* first = &source;
	Account* second = &destination;


	if (source.id > destination.id) {
		std::swap(first, second);
	}

	std::lock(first->mtx, second->mtx);
	
	if (source.balance >= amount) {
		source.balance -= amount;
		destination.balance += amount;
		
		int serial_num = getNextSerialNumber();
		Operation operation(serial_num, amount, source.id, destination.id);

		source.addLog(operation);
		destination.addLog(operation);
	}
	else {
		throw std::runtime_error("Insufficient funds in source account.");
	}
    first->mtx.unlock();
    second->mtx.unlock();
}

bool Bank::consistencyCheck(std::vector<std::unique_ptr<Account>>& accounts)
{

    for (const auto& accountPtr : accounts) {
        Account& account = *accountPtr;  
        int calculated_balance = account.getBalance();  

       
        for (const Operation& op : account.log) {
            if (op.source == account.id) {
                calculated_balance -= op.amount;  
            }
            else if (op.destination == account.id) {
                calculated_balance += op.amount;  
            }
        }

       
        if (calculated_balance != account.getBalance()) {
            std::cerr << "Inconsistency detected in account " << account.id
                << ": expected balance " << calculated_balance
                << ", actual balance " << account.getBalance() << std::endl;
            return false;
        }

        
        for (const Operation& op : account.log) {
           
            if (op.source >= accounts.size() || op.destination >= accounts.size()) {
                std::cerr << "Invalid operation " << op.serial_num << " in account " << account.id << std::endl;
                return false;
            }

            Account* counterpart = (op.source == account.id) ? accounts[op.destination].get() : accounts[op.source].get();

            
            bool found = false;
            for (const Operation& cp_op : counterpart->log) {
                if (cp_op.serial_num == op.serial_num) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                std::cerr << "Operation " << op.serial_num << " missing in counterpart account " << counterpart->id << std::endl;
                return false;
            }
        }
    }

    std::cout << "All accounts are consistent." << std::endl;
    return true;
}
