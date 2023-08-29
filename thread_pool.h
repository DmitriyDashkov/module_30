#pragma once
#include "block_queue.h"
#include <future>
#include <vector>

using task_type = std::function<void()>;						//задача
using func_type = void(*)(std::shared_ptr<int[]>, long, long);	//указатель на функцию задачи

//класс пула потоков
class ThreadPool
{
	long mThreadCount = 0;								//количество потоков
	//Для равномерного распределения задач.
	long mIndex = 0;
	std::vector<std::thread> mThreads;					//контейнер потоков
	//Очереди задач для потоков.
	std::vector<BlockedQueue<task_type>> mThreadQueues;	//очереди задач для потоков
public:
	ThreadPool();
	~ThreadPool();
	void start();			//старт пула
	void stop();			//стоп пула
	void pushTask(func_type f, std::shared_ptr<int[]> arr, long l, long r); //добавление задачи в очередь задач
	void idle(long qIndex);//функция которую выполняют потоки если нет задач
};




ThreadPool::ThreadPool() : mThreadCount(std::thread::hardware_concurrency() != 0 ?
	std::thread::hardware_concurrency() : 4),
	mThreadQueues(mThreadCount) {}

ThreadPool::~ThreadPool() {}

void ThreadPool::start()
{
	for (auto i = 0; i < mThreadCount; ++i)
		mThreads.emplace_back(&ThreadPool::idle, this, i);
}

void ThreadPool::stop()
{
	for (auto i = 0; i < mThreadCount; ++i)
	{
		//Кладет задачу-пустышку в каждую очередь для завершения потока
		task_type emptyTask;
		mThreadQueues[i].push(emptyTask);
	}
	for (auto& th : mThreads)
		if (th.joinable())
			th.join();
}

void ThreadPool::pushTask(func_type f, std::shared_ptr<int[]> arr, long l,
	long r)
{
	auto idQueueToPush = mIndex++ % mThreadCount;	//индекс очереди, куда положить задачу
	task_type task = [=] {f(arr, l, r); };		    //функтор	
	mThreadQueues[idQueueToPush].push(task);		//помещаем в очередь
}

void ThreadPool::idle(long qIndex)
{
	while (true)
	{
		//Обрабатывает очередную задачу.
		task_type task_to_do;
		bool isGotTask = false;
		auto i = 0;
		//Пытается быстро забрать задачу из любой очереди, начиная со своей.
		for (; i < mThreadCount; ++i)
			if (isGotTask = mThreadQueues[(qIndex + i) % mThreadCount].fastPop(task_to_do))
				break;
		//Если нет задач. Ждёт задачу.
		if (!isGotTask)
			mThreadQueues[qIndex].pop(task_to_do);
		//Чтобы не допустить зависания потока кладет обратно задачу-пустышку.
		else if (!task_to_do)
			mThreadQueues[(qIndex + i) % mThreadCount].push(task_to_do);
		if (!task_to_do)
			return;
		//Выполняет задачу.
		task_to_do();
	}
}

