#include "create_folder.h"
#include "yijinjing/journal/JournalReader.h"
#include "yijinjing/journal/JournalWriter.h"
#include "yijinjing/journal/Timer.h"
#include <iostream>


struct TestData {
    double last_price;
    double open_price;
    double highest_price;
    double lowest_price;
    int64_t local_timestamp_us;
};


int main() {
    create_folder("ts");

    yijinjing::JournalWriterPtr writer;
    yijinjing::JournalReaderPtr reader;
    yijinjing::JournalReaderPtr reader2;

    writer = yijinjing::JournalWriter::create("ts", "test_data", "writer");
    reader = yijinjing::JournalReader::create("ts", "test_data", yijinjing::getNanoTime(), "reader");
    reader2 = yijinjing::JournalReader::create("ts", "test_data", yijinjing::getNanoTime(), "reader2");

    //    writer
    TestData tick{};
    timespec ts{};
    clock_gettime(CLOCK_MONOTONIC, &ts);
    tick.local_timestamp_us = ts.tv_nsec + ts.tv_sec * 1000000000UL;
    writer->write_data(tick, 0, 0);
    writer->write_data(tick, 0, 1);

    //    reader
    std::cout << "test reader-1" << std::endl;
    yijinjing::FramePtr frame;
    do {
        frame = reader->getNextFrame();
    } while (!frame);
    auto *tick_reader = reinterpret_cast<TestData *>(frame->getData());
    std::cout << tick_reader->local_timestamp_us << std::endl;

    do {
        frame = reader->getNextFrame();
    } while (!frame);
    tick_reader = reinterpret_cast<TestData *>(frame->getData());
    std::cout << tick_reader->local_timestamp_us << std::endl;

    //    do {
    //        frame = reader->getNextFrame();
    //    } while (!frame);
    //    tick_reader = reinterpret_cast<TestData*>(frame->getData());
    //    std::cout<<tick_reader->local_timestamp_us<<std::endl;

    std::cout << "test reader-2" << std::endl;
    do {
        frame = reader2->getNextFrame();
    } while (!frame);
    tick_reader = reinterpret_cast<TestData *>(frame->getData());
    std::cout << tick_reader->local_timestamp_us << std::endl;

    do {
        frame = reader2->getNextFrame();
    } while (!frame);
    tick_reader = reinterpret_cast<TestData *>(frame->getData());
    std::cout << tick_reader->local_timestamp_us << std::endl;

    //    do {
    //        frame = reader2->getNextFrame();
    //    } while (!frame);
    //    tick_reader = reinterpret_cast<TestData*>(frame->getData());
    //    std::cout<<tick_reader->local_timestamp_us<<std::endl;

    return 0;
}
