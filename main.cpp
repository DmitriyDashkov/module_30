#include <chrono>
#include <iostream>
#include "quick_sort.h"

int main()
{
    system("chcp 1251");
    system("cls");
    srand((unsigned int)time(NULL));
    std::shared_ptr<int[]> array1(new int[arrSize]);        //для однопоточного запуска
    std::shared_ptr<int[]> array2(new int[arrSize]);        //для многопоточного запуска
    std::shared_ptr<int[]> array3(new int[arrSize]);        //для многопоточного запуска в пуле потоков
    std::vector<std::thread> threads;
    std::mutex mtx;
    auto start = std::chrono::high_resolution_clock::now();
    for (auto i = 0; i < threadCount; ++i)
        threads.push_back(std::thread([&array1, &array2, &array3, i]()
            {
                for (auto j = arrSize / threadCount * i; j < arrSize /
                    threadCount * (i + 1); ++j)
                    array3[j] = array2[j] = array1[j] = rand() % 500000;
            }));
    for (auto& th : threads)
        if (th.joinable())
            th.join();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedTime = end - start;
    double seconds = elapsedTime.count();
    printf("Время создания массива: %f секунд\n", seconds);

    //однопоточный запуск
    start = std::chrono::high_resolution_clock::now();
    QuickSortSolo(array1, 0, arrSize - 1);
    end = std::chrono::high_resolution_clock::now();
    elapsedTime = end - start;
    seconds = elapsedTime.count();
    printf("Время однопоточной сортировки: %f секунд\n", seconds);

    //многопоточный запуск async
    start = std::chrono::high_resolution_clock::now();
    QuickSortAsync(array2, 0, arrSize - 1);
    end = std::chrono::high_resolution_clock::now();
    elapsedTime = end - start;
    seconds = elapsedTime.count();
    printf("Время многопоточной сортировки: %f секунд\n", seconds);
    for (auto i = 0; i < threads.size(); ++i)
        threads[i] = std::thread([&array2, i, &mtx]()
            {
                for (auto j = arrSize / threadCount * i; j < (arrSize / threadCount *
                    (i + 1) - 1); ++j)
                    if (array2[i] > array2[i + 1])
                    {
                        mtx.lock();
                        std::cout << "Отсортировано с ошибкой!!!" << std::endl;
                        mtx.unlock();
                    }
            });
    for (auto& th : threads)
        if (th.joinable())
            th.join();

    //многопоточный запуск в пуле потоков
    start = std::chrono::high_resolution_clock::now();
    QuickSortPool(array3, 0, arrSize - 1);
    end = std::chrono::high_resolution_clock::now();
    elapsedTime = end - start;
    seconds = elapsedTime.count();;
    printf("Время сортировки в пуле потоков: %f секунд\n", seconds);
    for (auto i = 0; i < threads.size(); ++i)
        threads[i] = std::thread([&array3, i, &mtx]()
            {
                for (auto j = arrSize / threadCount * i; j < (arrSize / threadCount *
                    (i + 1) - 1); ++j)
                    if (array3[i] > array3[i + 1])
                    {
                        mtx.lock();
                        std::cout << "Отсортировано с ошибкой!!!" << std::endl;
                        mtx.unlock();
                    }
            });
    for (auto& th : threads)
        if (th.joinable())
            th.join();
    return 0;
}