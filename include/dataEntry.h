//
// Created by root on 12/10/24.
//

#ifndef TD_DATAENTRY_H
#define TD_DATAENTRY_H

#include <cstdint>

class DataEntry {
    uint64_t timestamp;
    double ask_high;
    double ask_low;
    double bid_high;
    double bid_low;
    double ask_open;
    double bid_open;
    double ask_close;
    double bid_close;
    double ask_volume;
    double bid_volume;
    double ask_vwap;//卖方成交量加权平均价，计算方式通常是用卖出价格乘以交易量的加权平均。
    double bid_vwap;//买方成交量加权平均价，类似于ask_vwap，但用于买方报价。
    double ask_spread;//卖方报价的价差，可能指在特定时间内的最高和最低卖价的差值。
    double bid_spread;//买方报价的价差，可能指在特定时间内的最高和最低买价的差值。
};

union dataType
{
    DataEntry dataEntry;
};

#endif //TD_DATAENTRY_H

