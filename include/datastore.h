//
// Created by root on 12/30/24.
//

#ifndef TD_DATASTORE_H
#define TD_DATASTORE_H
#include "dataEntry.h"
#include "string"
#include "enum.h"
#include <vector>
class DataStore
{
public:
    DataStore()= default;
    virtual ~DataStore()= default;
    virtual TraderResult Init(void* config)=0;
    virtual TraderResult Start()=0;
    virtual TraderResult Stop()=0;
    virtual std::vector<dataType> getData(void* fmt)=0;
    virtual TraderResult storeData(dataType data)=0;
    virtual TraderResult updateData(dataType data)=0;
    virtual TraderResult loadDataToMemory(void* fmt)=0;
private:
    virtual dataType checkData(dataType datatype, const std::string& format)=0;
    virtual dataType checkStore(void *config, const std::string& format)=0;
};
#endif //TD_DATASTORE_H
