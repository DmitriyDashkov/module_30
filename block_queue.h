#pragma once
#include <mutex>
#include <queue>
#include <condition_variable>

//потокобезопасная очередь
template <class T> 
class BlockedQueue
{
	std::mutex mLocker;					//мьютекс очереди
	std::queue<T> mTaskQueue;			//очередь задач
	std::condition_variable mNotifier;  //уведомитель
public:
	void push(T& item);					//помещает элемент в очередь
	void pop(T& item);					//забирает элемент из очереди
	bool fastPop(T& item);				//Неблокирующий метод получения элемента из очереди. Если задачи нет - возвращает false, иначе - true
	
};

template <typename T>
void BlockedQueue<T>::push(T& item)
{
	std::lock_guard<std::mutex> locker(mLocker);
	mTaskQueue.push(item);
	mNotifier.notify_one();	//элемент можно забрать из очереди
}

template <typename T>
void BlockedQueue<T>::pop(T& item)
{
	std::unique_lock<std::mutex> locker(mLocker);
	//ждем появления задачи в очереди
	if (mTaskQueue.empty())
		mNotifier.wait(locker, [this] {return !mTaskQueue.empty(); });
	item = mTaskQueue.front();
	mTaskQueue.pop();
}

template <typename T>
bool BlockedQueue<T>::fastPop(T& item)
{
	std::lock_guard<std::mutex> locker(mLocker);
	//Если очередь пуста - выходит.
	if (mTaskQueue.empty())
		return false;
	//Забирает элемент.
	item = mTaskQueue.front();
	mTaskQueue.pop();
	return true;
}