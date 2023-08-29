#pragma once
#include "block_queue.h"
#include <future>
#include <vector>

using task_type = std::function<void()>;						//������
using func_type = void(*)(std::shared_ptr<int[]>, long, long);	//��������� �� ������� ������

//����� ���� �������
class ThreadPool
{
	long mThreadCount = 0;								//���������� �������
	//��� ������������ ������������� �����.
	long mIndex = 0;
	std::vector<std::thread> mThreads;					//��������� �������
	//������� ����� ��� �������.
	std::vector<BlockedQueue<task_type>> mThreadQueues;	//������� ����� ��� �������
public:
	ThreadPool();
	~ThreadPool();
	void start();			//����� ����
	void stop();			//���� ����
	void pushTask(func_type f, std::shared_ptr<int[]> arr, long l, long r); //���������� ������ � ������� �����
	void idle(long qIndex);//������� ������� ��������� ������ ���� ��� �����
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
		//������ ������-�������� � ������ ������� ��� ���������� ������
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
	auto idQueueToPush = mIndex++ % mThreadCount;	//������ �������, ���� �������� ������
	task_type task = [=] {f(arr, l, r); };		    //�������	
	mThreadQueues[idQueueToPush].push(task);		//�������� � �������
}

void ThreadPool::idle(long qIndex)
{
	while (true)
	{
		//������������ ��������� ������.
		task_type task_to_do;
		bool isGotTask = false;
		auto i = 0;
		//�������� ������ ������� ������ �� ����� �������, ������� �� �����.
		for (; i < mThreadCount; ++i)
			if (isGotTask = mThreadQueues[(qIndex + i) % mThreadCount].fastPop(task_to_do))
				break;
		//���� ��� �����. ��� ������.
		if (!isGotTask)
			mThreadQueues[qIndex].pop(task_to_do);
		//����� �� ��������� ��������� ������ ������ ������� ������-��������.
		else if (!task_to_do)
			mThreadQueues[(qIndex + i) % mThreadCount].push(task_to_do);
		if (!task_to_do)
			return;
		//��������� ������.
		task_to_do();
	}
}

