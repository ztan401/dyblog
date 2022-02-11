// ref:https://github.com/cameron314/readerwriterqueue
#include <iostream>
#include <string>
#include "readerwriterqueue.h"

namespace lq = moodycamel;

// item 数据存储，是数据库的索引
typedef struct test_data {
    size_t th{};
    std::string value;
} test_data;


lq::BlockingReaderWriterQueue<test_data> q;


int main() {
    std::cout << "Hello, World!" << std::endl;

    test_data a;
    a.th = 0;
    a.value = "hello world";
    test_data b;
    b.th = 1;
    b.value = "foo";
    q.enqueue(a);
    q.enqueue(b);

    test_data c;
    q.wait_dequeue(c);
    std::cout << "get struct: " << c.th << "\tvalue: " << c.value << std::endl;
    q.wait_dequeue(c);
    std::cout << "get struct: " << c.th << "\tvalue: " << c.value << std::endl;


    q.enqueue(a);
    q.enqueue(b);
    for (int i = 0; i < 5; i++) {
        if (q.wait_dequeue_timed(c, std::chrono::milliseconds(5))) {
            std::cout << "get struct: " << c.th << "\tvalue: " << c.value << std::endl;
        } else {
            std::cout << "no data, th:" << i << std::endl;
        }
    }


    return 0;
}
