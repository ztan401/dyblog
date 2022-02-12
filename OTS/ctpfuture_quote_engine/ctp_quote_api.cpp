
#include "logger.h"
#include "ctp_quote_api.h"
#include "encoding.h"
#include "spdlog/spdlog.h"

CtpQuoteApi::CtpQuoteApi() {
    //m_FrontMdAddr = getConfig("config", "FrontMdAddr");
    //m_InstrumentID = getConfig("config", "InstrumentID");

    sem_init(&m_md_Release_signal, 0, 0);
    sem_init(&m_md_Init_signal, 0, 0);
    sem_init(&m_md_Join_signal, 0, 0);
    sem_init(&m_md_ReqUserLogin_signal, 0, 0);
    sem_init(&m_md_ReqUserLogout_signal, 0, 0);
    sem_init(&m_md_SubscribeMarketData_signal, 0, 0);
    sem_init(&m_md_UnSubscribeMarketData_signal, 0, 0);
    sem_init(&m_md_SubscribeForQuoteRsp_signal, 0, 0);
    sem_init(&m_md_UnSubscribeForQuoteRsp_signal, 0, 0);
}


/// <summary>
/// 析构函数 退出登录
/// </summary>
CtpQuoteApi::~CtpQuoteApi() {
    ReqUserLogout();
    Release();
}


void CtpQuoteApi::Init() {
    m_pMdapi = CThostFtdcMdApi::CreateFtdcMdApi(".//ctp_future_md_flow/");
    m_pMdapi->RegisterSpi(this);
    m_pMdapi->RegisterFront(const_cast<char *>(m_FrontMdAddr.c_str()));
    m_pMdapi->Init();
    SPDLOG_INFO("m_FrontMdAddr: {}", m_FrontMdAddr);
    sem_wait(&m_md_Init_signal);
}

void CtpQuoteApi::Release() {
    m_pMdapi->Release();
    SPDLOG_INFO("<Release>");
}


void CtpQuoteApi::Join() {
    m_pMdapi->Join();
}


void CtpQuoteApi::ReqUserLogin() {
    CThostFtdcReqUserLoginField pReqUserLoginField{};
    memset(&pReqUserLoginField, 0, sizeof(CThostFtdcReqUserLoginField));

    SPDLOG_INFO("<ReqUserLogin>");
    SPDLOG_INFO("\tTradingDay {}", pReqUserLoginField.TradingDay);
    SPDLOG_INFO("\tBrokerID {}", pReqUserLoginField.BrokerID);
    SPDLOG_INFO("\tUserID {}", pReqUserLoginField.UserID);
    SPDLOG_INFO("\tPassword {}", pReqUserLoginField.Password);
    SPDLOG_INFO("\tUserProductInfo {}", pReqUserLoginField.UserProductInfo);
    SPDLOG_INFO("\tInterfaceProductInfo {}", pReqUserLoginField.InterfaceProductInfo);
    SPDLOG_INFO("\tProtocolInfo {}", pReqUserLoginField.ProtocolInfo);
    SPDLOG_INFO("\tMacAddress {}", pReqUserLoginField.MacAddress);
    SPDLOG_INFO("\tOneTimePassword {}", pReqUserLoginField.OneTimePassword);
    SPDLOG_INFO("\tClientIPAddress {}", pReqUserLoginField.ClientIPAddress);
    SPDLOG_INFO("\tLoginRemark {}", pReqUserLoginField.LoginRemark);
    SPDLOG_INFO("\tClientIPPort {}", pReqUserLoginField.ClientIPPort);
    SPDLOG_INFO("</ReqUserLogin>");

    int a = m_pMdapi->ReqUserLogin(&pReqUserLoginField, 1);
    SPDLOG_INFO((a == 0) ? "用户登录请求......发送成功{}" : "用户登录请求......发送失败，错误序号={}", a);
    sem_wait(&m_md_ReqUserLogin_signal);
}

void CtpQuoteApi::ReqUserLogout() {
    CThostFtdcUserLogoutField pUserLogout{};
    memset(&pUserLogout, 0, sizeof(CThostFtdcUserLogoutField));

    SPDLOG_INFO("<ReqUserLogout>\n");
    SPDLOG_INFO("\tBrokerID [%s]\n", pUserLogout.BrokerID);
    SPDLOG_INFO("\tUserID [%s]\n", pUserLogout.UserID);
    SPDLOG_INFO("</ReqUserLogout>\n");

    int a = m_pMdapi->ReqUserLogout(&pUserLogout, 1);
    SPDLOG_INFO((a == 0) ? "用户登出请求......发送成功[%d]\n" : "用户登出请求......发送失败，错误序号=[%d]\n", a);
    sem_wait(&m_md_ReqUserLogout_signal);
}


void CtpQuoteApi::SubscribeMarketData() {
    std::vector<char *> char_list;
    for (const auto &p: sub_list_) char_list.emplace_back(const_cast<char *>(p.c_str()));
    int ret = m_pMdapi->SubscribeMarketData(char_list.data(), char_list.size());
    SPDLOG_INFO((ret == 0) ? "请求订阅行情......发送成功{}" : "请求订阅行情......发送失败，错误序号={}", ret);
    sem_wait(&m_md_SubscribeMarketData_signal);
}


void CtpQuoteApi::UnSubscribeMarketData() {
    SPDLOG_ERROR("UnSubscribeMarketData, 此函数不应该被调用！！");
}


void CtpQuoteApi::OnFrontConnected() {
    SPDLOG_DEBUG("[CtpQuoteApi::OnFrontConnectedMD] Connected");
    sem_post(&m_md_Init_signal);
}

void CtpQuoteApi::OnFrontDisconnected(int nReason) {
    SPDLOG_ERROR("[CtpQuoteApi::OnFrontDisconnectedMD] Disconnected, tnReason{}", nReason);
}

void CtpQuoteApi::OnHeartBeatWarning(int nTimeLapse) {
    SPDLOG_WARN("[CtpQuoteApi::OnHeartBeatWarningMD] Warn. No packet received for a period of time, {}", nTimeLapse);
}


void CtpQuoteApi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo,
                                 int nRequestID, bool bIsLast) {
    SPDLOG_INFO("<OnRspUserLogin>");
    if (pRspUserLogin) {
        SPDLOG_INFO("\tTradingDay {}", pRspUserLogin->TradingDay);
        SPDLOG_INFO("\tLoginTime {}", pRspUserLogin->LoginTime);
        SPDLOG_INFO("\tBrokerID {}", pRspUserLogin->BrokerID);
        SPDLOG_INFO("\tUserID {}", pRspUserLogin->UserID);
        SPDLOG_INFO("\tSystemName {}", pRspUserLogin->SystemName);
        SPDLOG_INFO("\tMaxOrderRef {}", pRspUserLogin->MaxOrderRef);
        SPDLOG_INFO("\tSHFETime {}", pRspUserLogin->SHFETime);
        SPDLOG_INFO("\tDCETime {}", pRspUserLogin->DCETime);
        SPDLOG_INFO("\tCZCETime {}", pRspUserLogin->CZCETime);
        SPDLOG_INFO("\tFFEXTime {}", pRspUserLogin->FFEXTime);
        SPDLOG_INFO("\tINETime {}", pRspUserLogin->INETime);
        SPDLOG_INFO("\tFrontID {}", pRspUserLogin->FrontID);
        SPDLOG_INFO("\tSessionID {}", pRspUserLogin->SessionID);
    }
    if (pRspInfo) {
        SPDLOG_ERROR("\tErrorMsg {}", gbk2utf8(pRspInfo->ErrorMsg));
        SPDLOG_ERROR("\tErrorID {}", pRspInfo->ErrorID);
    }
    SPDLOG_INFO("\tnRequestID {}", nRequestID);
    SPDLOG_INFO("\tbIsLast {}", bIsLast);
    SPDLOG_INFO("</OnRspUserLogin>");
    if (bIsLast) {
        sem_post(&m_md_ReqUserLogin_signal);
    }
}

void CtpQuoteApi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                                  bool bIsLast) {
    SPDLOG_INFO("<OnRspUserLogout>\n");
    if (pUserLogout) {
        SPDLOG_INFO("\tBrokerID [%s]\n", pUserLogout->BrokerID);
        SPDLOG_INFO("\tUserID [%s]\n", pUserLogout->UserID);
    }
    if (pRspInfo) {
        SPDLOG_ERROR("\tErrorMsg [%s]\n", pRspInfo->ErrorMsg);
        SPDLOG_ERROR("\tErrorID [%d]\n", pRspInfo->ErrorID);
    }
    SPDLOG_INFO("\tnRequestID [%d]\n", nRequestID);
    SPDLOG_INFO("\tbIsLast [%d]\n", bIsLast);
    SPDLOG_INFO("</OnRspUserLogout>\n");

    if (bIsLast) {
        sem_post(&m_md_ReqUserLogout_signal);
    }
}

void CtpQuoteApi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    SPDLOG_ERROR("<OnRspError>");
    if (pRspInfo) {
        SPDLOG_ERROR("\tErrorMsg{}", pRspInfo->ErrorMsg);
        SPDLOG_ERROR("\tErrorID {}", pRspInfo->ErrorID);
    }
    SPDLOG_ERROR("\tnRequestID {}", nRequestID);
    SPDLOG_ERROR("\tbIsLast {}", bIsLast);
    SPDLOG_ERROR("</OnRspError>");
}

void CtpQuoteApi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
                                     CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    SPDLOG_INFO("<OnRspSubMarketData>");
    if (pSpecificInstrument) {
        SPDLOG_INFO("\tInstrumentID ={}", pSpecificInstrument->InstrumentID);
    }
    if (pRspInfo) {
        SPDLOG_ERROR("\tErrorMsg ={}", pRspInfo->ErrorMsg);
        SPDLOG_ERROR("\tErrorID = {}", pRspInfo->ErrorID);
    }
    SPDLOG_INFO("\tnRequestID = {}", nRequestID);
    SPDLOG_INFO("\tbIsLast = {}", bIsLast);
    SPDLOG_INFO("</OnRspSubMarketData>");

    if (bIsLast) {
        sem_post(&m_md_SubscribeMarketData_signal);
    }
}

void CtpQuoteApi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument,
                                       CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    SPDLOG_INFO("<OnRspSubMarketData>");
    if (pSpecificInstrument) {
        SPDLOG_INFO("\tInstrumentID ={}", pSpecificInstrument->InstrumentID);
    }
    if (pRspInfo) {
        SPDLOG_ERROR("\tErrorMsg ={}", pRspInfo->ErrorMsg);
        SPDLOG_ERROR("\tErrorID = {}", pRspInfo->ErrorID);
    }
    SPDLOG_INFO("\tnRequestID = {}", nRequestID);
    SPDLOG_INFO("\tbIsLast = {}", bIsLast);
    SPDLOG_INFO("</OnRspSubMarketData>");

    if (bIsLast) {
        sem_post(&m_md_UnSubscribeMarketData_signal);
    }
}

void CtpQuoteApi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) {
    counter += 1;
    if (pDepthMarketData) {
        if (counter > 512) {
            SPDLOG_INFO("OnRtnDepthMarketData, symbol:{}", pDepthMarketData->InstrumentID);
            counter = 0;
        }

        TickData tick_data = {};
        tick_data.local_time = kungfu::yijinjing::time::now_in_nano();
        strcpy(tick_data.TradingDay, pDepthMarketData->TradingDay);
        strcpy(tick_data.ExchangeID, pDepthMarketData->ExchangeID);
        tick_data.LastPrice = pDepthMarketData->LastPrice;
        tick_data.PreSettlementPrice = pDepthMarketData->PreSettlementPrice;
        tick_data.PreClosePrice = pDepthMarketData->PreClosePrice;
        tick_data.PreOpenInterest = pDepthMarketData->PreOpenInterest;
        tick_data.OpenPrice = pDepthMarketData->OpenPrice;
        tick_data.HighestPrice = pDepthMarketData->HighestPrice;
        tick_data.LowestPrice = pDepthMarketData->LowestPrice;
        tick_data.Volume = pDepthMarketData->Volume;
        tick_data.Turnover = pDepthMarketData->Turnover;
        tick_data.OpenInterest = pDepthMarketData->OpenInterest;
        tick_data.ClosePrice = pDepthMarketData->ClosePrice;
        tick_data.SettlementPrice = pDepthMarketData->SettlementPrice;
        tick_data.UpperLimitPrice = pDepthMarketData->UpperLimitPrice;
        tick_data.LowerLimitPrice = pDepthMarketData->LowerLimitPrice;
        strcpy(tick_data.UpdateTime, pDepthMarketData->UpdateTime);
        tick_data.UpdateMillisec = pDepthMarketData->UpdateMillisec;
        tick_data.BidPrice1 = pDepthMarketData->BidPrice1;
        tick_data.BidVolume1 = pDepthMarketData->BidVolume1;
        tick_data.AskPrice1 = pDepthMarketData->AskPrice1;
        tick_data.AskVolume1 = pDepthMarketData->AskVolume1;
        tick_data.BidPrice2 = pDepthMarketData->BidPrice2;
        tick_data.BidVolume2 = pDepthMarketData->BidVolume2;
        tick_data.AskPrice2 = pDepthMarketData->AskPrice2;
        tick_data.AskVolume2 = pDepthMarketData->AskVolume2;
        tick_data.BidPrice3 = pDepthMarketData->BidPrice3;
        tick_data.BidVolume3 = pDepthMarketData->BidVolume3;
        tick_data.AskPrice3 = pDepthMarketData->AskPrice3;
        tick_data.AskVolume3 = pDepthMarketData->AskVolume3;
        tick_data.BidPrice4 = pDepthMarketData->BidPrice4;
        tick_data.BidVolume4 = pDepthMarketData->BidVolume4;
        tick_data.AskPrice4 = pDepthMarketData->AskPrice4;
        tick_data.AskVolume4 = pDepthMarketData->AskVolume4;
        tick_data.BidPrice5 = pDepthMarketData->BidPrice5;
        tick_data.BidVolume5 = pDepthMarketData->BidVolume5;
        tick_data.AskPrice5 = pDepthMarketData->AskPrice5;
        tick_data.AskVolume5 = pDepthMarketData->AskVolume5;
        strcpy(tick_data.ActionDay, pDepthMarketData->ActionDay);
        strcpy(tick_data.InstrumentID, pDepthMarketData->InstrumentID);
        strcpy(tick_data.ExchangeInstID, pDepthMarketData->ExchangeInstID);

        ptr_task_queue->enqueue(tick_data);
    }
}


void CtpQuoteApi::set_config(const std::string &front_address) {
    m_FrontMdAddr = front_address;
}

void CtpQuoteApi::set_queue(moodycamel::BlockingReaderWriterQueue<TickData> &task_queue) {
    ptr_task_queue = &task_queue;
}

void CtpQuoteApi::set_universe(const std::vector<std::string> &sub_list) {
    sub_list_ = sub_list;
}
