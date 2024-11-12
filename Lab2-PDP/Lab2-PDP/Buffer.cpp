#include "Buffer.h"
#include <iostream>

Buffer::Buffer()
{
}

int Buffer::GetValue()
{
	std::unique_lock < std::mutex > lock(mutex);
	while (queue.empty())
	{
		std::cout << "Thread: " << std::this_thread::get_id() << " The buffer is empty"<<std::endl;
		// unlock
		readyToGet.wait(lock);
		// lock
	}
	int val = queue.front();
	queue.pop();
	
	std::cout << "Thread: " << std::this_thread::get_id() << " got " << val << " from the buffer"<<std::endl;
	readyToSend.notify_one();
	return val;
}

void Buffer::PutValue(int val)
{
	std::unique_lock<std::mutex> lock(mutex);
	while(queue.size() == 1)
	{
		std::cout << "Thread: " << std::this_thread::get_id() << " The buffer is not empty" << std::endl;
		readyToSend.wait(lock);
	}
	queue.push(val);
	std::cout << "Thread: " << std::this_thread::get_id() << " sent " << val << " to the buffer" << std::endl;
	readyToGet.notify_one();

}
