#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <string>
#include "dataEntry.h"
#include "enum.h"
class DataSource
{
    public:
        DataSource()= default;
        virtual ~DataSource()= default;
        virtual TraderResult Init(void* config)=0;
        virtual TraderResult Start()=0;
        virtual TraderResult Stop()=0;
    private:
        virtual TraderResult publishData(dataType data)=0;
        virtual dataType formatData(const std::string& data, const std::string& format)=0;
};

#endif // DATASOURCE_H