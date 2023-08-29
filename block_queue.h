#pragma once
#include <mutex>
#include <queue>
#include <condition_variable>

//���������������� �������
template <class T> 
class BlockedQueue
{
	std::mutex mLocker;					//������� �������
	std::queue<T> mTaskQueue;			//������� �����
	std::condition_variable mNotifier;  //�����������
public:
	void push(T& item);					//�������� ������� � �������
	void pop(T& item);					//�������� ������� �� �������
	bool fastPop(T& item);				//������������� ����� ��������� �������� �� �������. ���� ������ ��� - ���������� false, ����� - true
	
};

template <typename T>
void BlockedQueue<T>::push(T& item)
{
	std::lock_guard<std::mutex> locker(mLocker);
	mTaskQueue.push(item);
	mNotifier.notify_one();	//������� ����� ������� �� �������
}

template <typename T>
void BlockedQueue<T>::pop(T& item)
{
	std::unique_lock<std::mutex> locker(mLocker);
	//���� ��������� ������ � �������
	if (mTaskQueue.empty())
		mNotifier.wait(locker, [this] {return !mTaskQueue.empty(); });
	item = mTaskQueue.front();
	mTaskQueue.pop();
}

template <typename T>
bool BlockedQueue<T>::fastPop(T& item)
{
	std::lock_guard<std::mutex> locker(mLocker);
	//���� ������� ����� - �������.
	if (mTaskQueue.empty())
		return false;
	//�������� �������.
	item = mTaskQueue.front();
	mTaskQueue.pop();
	return true;
}