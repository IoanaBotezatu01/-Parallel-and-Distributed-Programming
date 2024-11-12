#include <vector>
#include <iostream>
#include "Buffer.h"
std::vector<int> vector1;
std::vector<int> vector2;
int scalarProduct = 0;


void Producer(Buffer* buffer)
{
	for (int i = 0; i < vector1.size(); i++)
	{
		int res = vector1[i] * vector2[i];
		std::cout << "Producer: " << vector1[i] << "*" << vector2[i] << "=" << vector1[i] * vector2[i]<<std::endl;
		buffer->PutValue(res);
	}
}
void Consumer(Buffer* buffer)
{
	for (int i = 0; i < vector1.size(); i++)
	{
		int val = buffer->GetValue();
		scalarProduct = scalarProduct + val;
		std::cout << "Consumer: current sum-> " << scalarProduct<<std::endl;
}
	std::cout << "Final ScalarProduct:" << scalarProduct << std::endl;
}


int main()
{
	for (int i = 0; i < 100; i++)
	{
		vector1.push_back(1);
		vector2.push_back(i + 1);

		
	}
	Buffer* buffer = new Buffer();
	std::thread producer(Producer, buffer);
	std::thread consumer(Consumer, buffer);

	producer.join();
	consumer.join();
	return 0;
}