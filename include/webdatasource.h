#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <sstream>
#include "datasource.h"
#include "event.h"
#include "enum.h"
// http://qt.gtimg.cn/q=sh600000
struct webdatasourceconfigtype {
    std::string url;
    std::string username;
    std::string password;
    std::vector<std::string> querylist;
    // func to format data
    std::function<dataType(const std::string&, const std::string&)> formatter;

};

class WebDataSource : public DataSource { // 继承 DataSource
public:
    WebDataSource() { std::cout << "MyDataSource created\n"; }

    ~WebDataSource() override { std::cout << "MyDataSource destroyed\n"; }

    TraderResult Init(void *config) override =0;

    TraderResult Start() override;

    TraderResult Stop() override;

protected:
    TraderResult publishData(dataType data) override; 

    dataType formatData(const std::string &data, const std::string &format) override {
        std::cout << "Formatting data with format: " << format << "\n";
        //split data string with '~'
        std::vector<std::string> data_list;
        std::stringstream ss(data);
        std::string item;
        while (std::getline(ss, item, '~')) {
            data_list.push_back(item);
        }
        std::cout << "data_list size: " << data_list.size() << "\n";
        for (int i = 0; i < data_list.size(); i++) {
            std::cout << "item " << i << ": " << data_list[i] << "\n";
        }
        return *new dataType();
    }

private:
    webdatasourceconfigtype *config;
    std::thread getdata_thread;
    std::function<dataType(const std::string&, const std::string&)> formatter;
};
