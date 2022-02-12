//
// Created by 观鱼 on 2022/2/8.
//

#include "ThostFtdcTraderApi.h"
#include "create_folder.h"
#include "encoding.h"
#include "logger.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include <iostream>
#include <unordered_map>

namespace py = pybind11;


void timing_ms(int ms) {
    auto us = ms * 1000;
    auto start = std::chrono::system_clock::now();
    while (true) {
        auto end = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        if (duration.count() > us) break;
    }
}

class TraderClient : public CThostFtdcTraderSpi {
private:
    CThostFtdcTraderApi *m_ptraderapi{};

public:
    // 连接
    void connect(char frontAddress[]);

    // 客户端认证
    int authenticate(char brokerID[], char userID[], char appID[], char authCode[]);

    // 释放
    void release();

    //登陆
    void login(char brokerID[], char userID[], char password[]);

    //结算单确认
    void settlementinfoConfirm(char brokerID[], char userID[]);

public:
    //前置连接响应
    //当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
    void OnFrontConnected() override;

    //当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
    //@param nReason 错误原因
    //        0x1001 网络读失败
    //        0x1002 网络写失败
    //        0x2001 接收心跳超时
    //        0x2002 发送心跳失败
    //        0x2003 收到错误报文
    void OnFrontDisconnected(int nReason) override;

    //客户端认证响应
    void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo,
                           int nRequestID, bool bIsLast) override;

    //登录请求响应
    void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                        bool bIsLast) override;

    //结算单确认响应
    void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
                                    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

    //错误应答
    void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

    ///请求查询合约响应
    void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                            bool bIsLast) override;


private:
    int request_id = 0;
    bool is_last = true;
    std::unordered_map<std::string, CThostFtdcInstrumentField> contracts;

public:
    // 查询持仓
    void qry_instrument();

    bool qry_instrument_is_finished() const;

    std::unordered_map<std::string, CThostFtdcInstrumentField> get_contracts();

    int get_nRequestID() {
        request_id++;
        return request_id;
    }
};


// 连接
void TraderClient::connect(char frontAddress[]) {
    m_ptraderapi = CThostFtdcTraderApi::CreateFtdcTraderApi(".//ctp_future_td_flow/");//必须提前创建好flow目录
    m_ptraderapi->RegisterSpi(this);
    m_ptraderapi->SubscribePublicTopic(THOST_TERT_QUICK);
    m_ptraderapi->SubscribePrivateTopic(THOST_TERT_QUICK);//设置私有流订阅模式
    m_ptraderapi->RegisterFront(frontAddress);
    SPDLOG_INFO("注册交易前置机: {} ", frontAddress);
    m_ptraderapi->Init();
    //输出API版本信息
    SPDLOG_INFO("CTPMini-TdApi版本信息: {}", m_ptraderapi->GetApiVersion());
}

// 客户端认证请求
int TraderClient::authenticate(char brokerID[], char userID[], char appID[], char authCode[]) {
    CThostFtdcReqAuthenticateField pReqAuthenticateField = {0};
    strcpy(pReqAuthenticateField.BrokerID, brokerID);
    strcpy(pReqAuthenticateField.UserID, userID);
    strcpy(pReqAuthenticateField.AppID, appID);
    strcpy(pReqAuthenticateField.AuthCode, authCode);

    return m_ptraderapi->ReqAuthenticate(&pReqAuthenticateField, 1);
}

//释放
void TraderClient::release() {
    m_ptraderapi->Release();
}


//登陆
void TraderClient::login(char brokerID[], char userID[], char password[]) {
    CThostFtdcReqUserLoginField t = {0};
    strcpy(t.BrokerID, brokerID);
    strcpy(t.UserID, userID);
    strcpy(t.Password, password);
    while (m_ptraderapi->ReqUserLogin(&t, 1) != 0) timing_ms(100);
}


//结算单确认
void TraderClient::settlementinfoConfirm(char brokerID[], char userID[]) {
    CThostFtdcSettlementInfoConfirmField t = {0};
    strcpy(t.BrokerID, brokerID);
    strcpy(t.InvestorID, userID);
    while (m_ptraderapi->ReqSettlementInfoConfirm(&t, 2) != 0) timing_ms(100);
    SPDLOG_WARN("默认自动确认结算单");
}


void TraderClient::OnFrontConnected() {
    SPDLOG_INFO("交易服务器连接成功");
}

void TraderClient::OnFrontDisconnected(int nReason) {
    SPDLOG_ERROR("交易服务器连接失败: {}", nReason);
}

//客户端认证响应
void TraderClient::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo,
                                     int nRequestID, bool bIsLast) {
    SPDLOG_INFO("客户端认证响应");
    if (pRspInfo != NULL && pRspInfo->ErrorID == 0) {
        // printf("认证成功,ErrorID=0x%04x, ErrMsg=%s\n\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
        SPDLOG_INFO("认证成功,ErrorID={}, ErrMsg={}", pRspInfo->ErrorID, gbk2utf8(pRspInfo->ErrorMsg));
    } else {
        // printf("认证失败,ErrorID=0x%04x, ErrMsg=%s\n\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
        SPDLOG_ERROR("认证失败,ErrorID={}, ErrMsg={}", pRspInfo->ErrorID, gbk2utf8(pRspInfo->ErrorMsg));
    }
}


void TraderClient::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo,
                                  int nRequestID, bool bIsLast) {
    SPDLOG_INFO("交易账户登录成功");
    SPDLOG_INFO("交易日:{}", pRspUserLogin->TradingDay);
    SPDLOG_INFO("登录时间: {}", pRspUserLogin->LoginTime);
    SPDLOG_INFO("交易帐户: {}", pRspUserLogin->UserID);
}

//结算单确认响应
void TraderClient::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
                                              CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    SPDLOG_INFO("程序自动确认结算单，请通过交易终端核对结算单");
    SPDLOG_INFO("确认日期：{}", pSettlementInfoConfirm->ConfirmDate);
    SPDLOG_INFO("确认时间：{}", pSettlementInfoConfirm->ConfirmTime);
}

//错误应答
void TraderClient::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    printf("OnRspError\n");
    SPDLOG_ERROR("OnRspError");
}

// 请求查询合约响应，当执行ReqQryInstrument后，该方法被调用。
void TraderClient::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo,
                                      int nRequestID, bool bIsLast) {
    //if (pInstrument->ProductClass == THOST_FTDC_PC_ETFOption) {
    //	contracts.insert(std::pair<std::string, CThostFtdcInstrumentField>(pInstrument->InstrumentID, *pInstrument));
    //}
    contracts.insert(std::pair<std::string, CThostFtdcInstrumentField>(pInstrument->InstrumentID, *pInstrument));
    is_last = bIsLast;
}

// 查询持仓
void TraderClient::qry_instrument() {
    SPDLOG_INFO("请求获取股指期权和商品期权静态数据！");
    CThostFtdcQryInstrumentField t = {0};
    is_last = false;
    while (m_ptraderapi->ReqQryInstrument(&t, get_nRequestID()) != 0) timing_ms(100);
}

bool TraderClient::qry_instrument_is_finished() const {
    return is_last;
}

std::unordered_map<std::string, CThostFtdcInstrumentField> TraderClient::get_contracts() {
    return contracts;
}


py::dict get_static_data() {
    bool init_log = flow::utils::init_logger("info", "clogs/ctpspot_static_data.log", true, false, false);
    SPDLOG_INFO("CTP-Future获取股指和商品期权静态数据！");

    std::string dir_path = "ctp_future_td_flow";
    create_folder(dir_path);

    /// 参数设置区
    char configFrontAddress[] = "";
    char configBrokerID[] = "";
    char configUserID[] = "";
    char configPassword[] = "";
    char configAppID[] = "";
    char configAuthCode[] = "";
    SPDLOG_TRACE("AppID: {}", configAppID);
    SPDLOG_TRACE("AuthCode: {}", configAuthCode);

    auto *client = new TraderClient();
    SPDLOG_INFO("连接交易前置机");
    client->connect(configFrontAddress);
    timing_ms(1000);
    // 终端认证 容易失败！
    while (true) {
        int intAuthRet = client->authenticate(configBrokerID, configUserID, configAppID, configAuthCode);
        SPDLOG_INFO("发送终端认证请求：" + std::to_string(intAuthRet));
        timing_ms(500);
        if (intAuthRet == 0) {
            break;
        }
    }

    // 账户登录
    client->login(configBrokerID, configUserID, configPassword);
    timing_ms(500);
    // SPDLOG_INFO("登陆交易账户");

    client->settlementinfoConfirm(configBrokerID, configUserID);
    timing_ms(500);
    // SPDLOG_WARN("持仓查询程序默认确认结算单！");

    client->qry_instrument();
    while (!client->qry_instrument_is_finished()) timing_ms(100);

    std::unordered_map<std::string, CThostFtdcInstrumentField> contracts = client->get_contracts();
    // std::cout << "contracts size = " << contracts.size() << std::endl;
    SPDLOG_INFO("contracts size = {}", contracts.size());

    py::dict static_data;
    for (auto &contract: contracts) {
        py::dict data;
        data["InstrumentID"] = gbk2utf8(contract.second.InstrumentID);
        data["ExchangeID"] = gbk2utf8(contract.second.ExchangeID);
        data["InstrumentName"] = gbk2utf8(contract.second.InstrumentName);
        data["ExchangeInstID"] = gbk2utf8(contract.second.ExchangeInstID);
        data["ProductID"] = gbk2utf8(contract.second.ProductID);
        data["ProductClass"] = contract.second.ProductClass;
        data["DeliveryYear"] = contract.second.DeliveryYear;
        data["DeliveryMonth"] = contract.second.DeliveryMonth;
        data["MaxMarketOrderVolume"] = contract.second.MaxMarketOrderVolume;
        data["MinMarketOrderVolume"] = contract.second.MinMarketOrderVolume;
        data["MaxLimitOrderVolume"] = contract.second.MaxLimitOrderVolume;
        data["MinLimitOrderVolume"] = contract.second.MinLimitOrderVolume;
        data["VolumeMultiple"] = contract.second.VolumeMultiple;
        data["PriceTick"] = contract.second.PriceTick;
        data["CreateDate"] = gbk2utf8(contract.second.CreateDate);
        data["OpenDate"] = gbk2utf8(contract.second.OpenDate);
        data["ExpireDate"] = gbk2utf8(contract.second.ExpireDate);
        data["StartDelivDate"] = gbk2utf8(contract.second.StartDelivDate);
        data["EndDelivDate"] = gbk2utf8(contract.second.EndDelivDate);
        data["InstLifePhase"] = contract.second.InstLifePhase;
        data["IsTrading"] = contract.second.IsTrading;
        data["PositionType"] = contract.second.PositionType;
        data["PositionDateType"] = contract.second.PositionDateType;
        data["LongMarginRatio"] = contract.second.LongMarginRatio;
        data["ShortMarginRatio"] = contract.second.ShortMarginRatio;
        data["MaxMarginSideAlgorithm"] = contract.second.MaxMarginSideAlgorithm;
        data["StrikePrice"] = contract.second.StrikePrice;
        data["OptionsType"] = contract.second.OptionsType;
        data["UnderlyingMultiple"] = contract.second.UnderlyingMultiple;
        data["CombinationType"] = contract.second.CombinationType;
        data["UnderlyingInstrID"] = gbk2utf8(contract.second.UnderlyingInstrID);

        static_data[contract.second.InstrumentID] = data;
    }

    client->release();
    return static_data;
}


PYBIND11_MODULE(ctpfuture_client, m) {
    m.def("get_static_data", &get_static_data, "get_static_data");
}
