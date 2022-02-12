//
// Created by 观鱼 on 2022/2/9.
//

#include "create_folder.h"
#include "ctp_quote_api.h"
#include "logger.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "readerwriterqueue.h"
#include "spdlog/spdlog.h"
#include <clickhouse/client.h>
#include <iostream>
#include <string>
#include <vector>

namespace py = pybind11;


void tick_to_ck(moodycamel::BlockingReaderWriterQueue<TickData> &task_queue) {
    SPDLOG_TRACE("tick_to_ck start!");
    clickhouse::Client client(clickhouse::ClientOptions()
                                      .SetHost("single-clickhouse-server")
                                      .SetPort(9000)
                                      .SetUser("default")
                                      .SetPassword("default")
                                      .SetPingBeforeQuery(true));
    SPDLOG_TRACE("clickhouse::Client");

    TickData tick_data{};
    while (true) {
        if (task_queue.wait_dequeue_timed(tick_data, std::chrono::seconds(60))) {
            try {
                SPDLOG_TRACE("rev data symbol: {}, last_price: {}", tick_data.InstrumentID, tick_data.LastPrice);
                clickhouse::Block block;

                auto TradingDay = std::make_shared<clickhouse::ColumnString>();
                TradingDay->Append(tick_data.TradingDay);
                auto ExchangeID = std::make_shared<clickhouse::ColumnString>();
                ExchangeID->Append(tick_data.ExchangeID);
                auto LastPrice = std::make_shared<clickhouse::ColumnFloat64>();
                LastPrice->Append(tick_data.LastPrice);
                auto PreSettlementPrice = std::make_shared<clickhouse::ColumnFloat64>();
                PreSettlementPrice->Append(tick_data.PreSettlementPrice);
                auto PreClosePrice = std::make_shared<clickhouse::ColumnFloat64>();
                PreClosePrice->Append(tick_data.PreClosePrice);
                auto PreOpenInterest = std::make_shared<clickhouse::ColumnInt64>();
                PreOpenInterest->Append(tick_data.PreOpenInterest);
                auto OpenPrice = std::make_shared<clickhouse::ColumnFloat64>();
                OpenPrice->Append(tick_data.OpenPrice);
                auto HighestPrice = std::make_shared<clickhouse::ColumnFloat64>();
                HighestPrice->Append(tick_data.HighestPrice);
                auto LowestPrice = std::make_shared<clickhouse::ColumnFloat64>();
                LowestPrice->Append(tick_data.LowestPrice);
                auto Volume = std::make_shared<clickhouse::ColumnInt64>();
                Volume->Append(tick_data.Volume);
                auto Turnover = std::make_shared<clickhouse::ColumnFloat64>();
                Turnover->Append(tick_data.Turnover);
                auto OpenInterest = std::make_shared<clickhouse::ColumnInt64>();
                OpenInterest->Append(tick_data.OpenInterest);
                auto ClosePrice = std::make_shared<clickhouse::ColumnFloat64>();
                ClosePrice->Append(tick_data.ClosePrice);
                auto SettlementPrice = std::make_shared<clickhouse::ColumnFloat64>();
                SettlementPrice->Append(tick_data.SettlementPrice);
                auto UpperLimitPrice = std::make_shared<clickhouse::ColumnFloat64>();
                UpperLimitPrice->Append(tick_data.UpperLimitPrice);
                auto LowerLimitPrice = std::make_shared<clickhouse::ColumnFloat64>();
                LowerLimitPrice->Append(tick_data.LowerLimitPrice);
                auto UpdateTime = std::make_shared<clickhouse::ColumnString>();
                UpdateTime->Append(tick_data.UpdateTime);
                auto UpdateMillisec = std::make_shared<clickhouse::ColumnInt64>();
                UpdateMillisec->Append(tick_data.UpdateMillisec);
                auto BidPrice1 = std::make_shared<clickhouse::ColumnFloat64>();
                BidPrice1->Append(tick_data.BidPrice1);
                auto BidVolume1 = std::make_shared<clickhouse::ColumnInt64>();
                BidVolume1->Append(tick_data.BidVolume1);
                auto AskPrice1 = std::make_shared<clickhouse::ColumnFloat64>();
                AskPrice1->Append(tick_data.AskPrice1);
                auto AskVolume1 = std::make_shared<clickhouse::ColumnInt64>();
                AskVolume1->Append(tick_data.AskVolume1);
                auto BidPrice2 = std::make_shared<clickhouse::ColumnFloat64>();
                BidPrice2->Append(tick_data.BidPrice2);
                auto BidVolume2 = std::make_shared<clickhouse::ColumnInt64>();
                BidVolume2->Append(tick_data.BidVolume2);
                auto AskPrice2 = std::make_shared<clickhouse::ColumnFloat64>();
                AskPrice2->Append(tick_data.AskPrice2);
                auto AskVolume2 = std::make_shared<clickhouse::ColumnInt64>();
                AskVolume2->Append(tick_data.AskVolume2);
                auto BidPrice3 = std::make_shared<clickhouse::ColumnFloat64>();
                BidPrice3->Append(tick_data.BidPrice3);
                auto BidVolume3 = std::make_shared<clickhouse::ColumnInt64>();
                BidVolume3->Append(tick_data.BidVolume3);
                auto AskPrice3 = std::make_shared<clickhouse::ColumnFloat64>();
                AskPrice3->Append(tick_data.AskPrice3);
                auto AskVolume3 = std::make_shared<clickhouse::ColumnInt64>();
                AskVolume3->Append(tick_data.AskVolume3);
                auto BidPrice4 = std::make_shared<clickhouse::ColumnFloat64>();
                BidPrice4->Append(tick_data.BidPrice4);
                auto BidVolume4 = std::make_shared<clickhouse::ColumnInt64>();
                BidVolume4->Append(tick_data.BidVolume4);
                auto AskPrice4 = std::make_shared<clickhouse::ColumnFloat64>();
                AskPrice4->Append(tick_data.AskPrice4);
                auto AskVolume4 = std::make_shared<clickhouse::ColumnInt64>();
                AskVolume4->Append(tick_data.AskVolume4);
                auto BidPrice5 = std::make_shared<clickhouse::ColumnFloat64>();
                BidPrice5->Append(tick_data.BidPrice5);
                auto BidVolume5 = std::make_shared<clickhouse::ColumnInt64>();
                BidVolume5->Append(tick_data.BidVolume5);
                auto AskPrice5 = std::make_shared<clickhouse::ColumnFloat64>();
                AskPrice5->Append(tick_data.AskPrice5);
                auto AskVolume5 = std::make_shared<clickhouse::ColumnInt64>();
                AskVolume5->Append(tick_data.AskVolume5);
                auto ActionDay = std::make_shared<clickhouse::ColumnString>();
                ActionDay->Append(tick_data.ActionDay);
                auto InstrumentID = std::make_shared<clickhouse::ColumnString>();
                InstrumentID->Append(tick_data.InstrumentID);
                auto ExchangeInstID = std::make_shared<clickhouse::ColumnString>();
                ExchangeInstID->Append(tick_data.ExchangeInstID);

                block.AppendColumn("TradingDay", TradingDay);
                block.AppendColumn("ExchangeID", ExchangeID);
                block.AppendColumn("LastPrice", LastPrice);
                block.AppendColumn("PreSettlementPrice", PreSettlementPrice);
                block.AppendColumn("PreClosePrice", PreClosePrice);
                block.AppendColumn("PreOpenInterest", PreOpenInterest);
                block.AppendColumn("OpenPrice", OpenPrice);
                block.AppendColumn("HighestPrice", HighestPrice);
                block.AppendColumn("LowestPrice", LowestPrice);
                block.AppendColumn("Volume", Volume);
                block.AppendColumn("Turnover", Turnover);
                block.AppendColumn("OpenInterest", OpenInterest);
                block.AppendColumn("ClosePrice", ClosePrice);
                block.AppendColumn("SettlementPrice", SettlementPrice);
                block.AppendColumn("UpperLimitPrice", UpperLimitPrice);
                block.AppendColumn("LowerLimitPrice", LowerLimitPrice);
                block.AppendColumn("UpdateTime", UpdateTime);
                block.AppendColumn("UpdateMillisec", UpdateMillisec);
                block.AppendColumn("BidPrice1", BidPrice1);
                block.AppendColumn("BidVolume1", BidVolume1);
                block.AppendColumn("AskPrice1", AskPrice1);
                block.AppendColumn("AskVolume1", AskVolume1);
                block.AppendColumn("BidPrice2", BidPrice2);
                block.AppendColumn("BidVolume2", BidVolume2);
                block.AppendColumn("AskPrice2", AskPrice2);
                block.AppendColumn("AskVolume2", AskVolume2);
                block.AppendColumn("BidPrice3", BidPrice3);
                block.AppendColumn("BidVolume3", BidVolume3);
                block.AppendColumn("AskPrice3", AskPrice3);
                block.AppendColumn("AskVolume3", AskVolume3);
                block.AppendColumn("BidPrice4", BidPrice4);
                block.AppendColumn("BidVolume4", BidVolume4);
                block.AppendColumn("AskPrice4", AskPrice4);
                block.AppendColumn("AskVolume4", AskVolume4);
                block.AppendColumn("BidPrice5", BidPrice5);
                block.AppendColumn("BidVolume5", BidVolume5);
                block.AppendColumn("AskPrice5", AskPrice5);
                block.AppendColumn("AskVolume5", AskVolume5);
                block.AppendColumn("ActionDay", ActionDay);
                block.AppendColumn("InstrumentID", InstrumentID);
                block.AppendColumn("ExchangeInstID", ExchangeInstID);

                client.Insert("test.test", block);
            } catch (const std::exception &e) {
                SPDLOG_ERROR("error insert data! {}", e.what());
            }

        } else {
            SPDLOG_ERROR("no rev data");
        }
    }
}


void run_quote_engine(std::string &front_address,
                      const std::vector<std::string> &universe,
                      std::string &level) {
    flow::utils::init_logger(level, "clogs/ctp_future_quote_engine.log", true, false, false);
    std::string dir_path = "ctp_future_md_flow";
    create_folder(dir_path);

    moodycamel::BlockingReaderWriterQueue<TickData> task_queue;
    SPDLOG_INFO("创建单写单读多线程无锁队列");

    SPDLOG_INFO("{}:{}", "当前API版本号", CThostFtdcMdApi::GetApiVersion());
    auto *client = new CtpQuoteApi;
    client->set_queue(task_queue);
    client->set_config(front_address);
    client->set_universe(universe);

    client->Init();
    client->ReqUserLogin();
    client->SubscribeMarketData();
    tick_to_ck(task_queue);

    client->ReqUserLogout();
    client->Release();
}


PYBIND11_MODULE(ctpfuture_quote_engine, m) {
    m.def("run_quote_engine", &run_quote_engine, py::call_guard<py::gil_scoped_release>(),
          py::arg("front_address"),
          py::arg("universe"),
          py::arg("level") = "info");
}
