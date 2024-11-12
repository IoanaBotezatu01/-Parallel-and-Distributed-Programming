#pragma once
#include <mutex>
#include <queue>
class Buffer
{
private:
	std::mutex mutex;
	std::condition_variable readyToSend;
	std::condition_variable readyToGet;
	std::queue<int> queue;
public:
	Buffer();
	int GetValue();
	void PutValue(int val);
};

