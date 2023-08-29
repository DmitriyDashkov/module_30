#pragma once
#include "thread_pool.h"

/*���������� ����� - ���������� ������ �� ����������
� ���� ���������� ��� �������*/
class RequestHandler
{
	//��� �������
	ThreadPool threadPool;
public:
	RequestHandler();
	~RequestHandler();
	//�������� ������ �� ����������
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