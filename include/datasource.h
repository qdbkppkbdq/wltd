#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <string>
#include "dataEntry.h"

class DataSource
{
    public:
        DataSource();
        virtual ~DataSource();
        virtual void Init(void* config);
        virtual void Start();
        virtual void Stop();
    private:
        virtual void publishData(dataType data);
        virtual dataType formatData(const std::string& data, const std::string& format);
};

#endif // DATASOURCE_H