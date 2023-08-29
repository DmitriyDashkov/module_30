#pragma once
#include "thread_pool.h"

/*Обработчик задач - закидывает задачи на исполнение
в свой внутренний пул потоков*/
class RequestHandler
{
	//пул потоков
	ThreadPool threadPool;
public:
	RequestHandler();
	~RequestHandler();
	//помещает задачу на выполнение
	void PushRequest(func_type f, std::shared_ptr<int[]> arr, long l, long r);
};

RequestHandler::RequestHandler()
{
	threadPool.start();
}

RequestHandler::~RequestHandler()
{
	threadPool.stop();
}

void RequestHandler::PushRequest(func_type f, std::shared_ptr<int[]> arr,
	long l, long r)
{
	threadPool.pushTask(f, arr, l, r);
}