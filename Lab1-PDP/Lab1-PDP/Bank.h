#pragma once
#include <atomic>
#include <vector>
#include "Account.h"
#include<memory>

class Bank
{
private:
	std::atomic<int> serial_counter;

public:
	Bank();
	int getNextSerialNumber();
	void transfer(Account& source, Account& destination, int amount);

	bool consistencyCheck(std::vector<std::unique_ptr<Account>>& accounts);
};

