#include "webdatasource.h"
#include "httplib.h"
#include "enum.h"
#include "utils.h"
TraderResult WebDataSource::Init(void *config) {  
    // 深拷贝配置
    if (config != nullptr) {
        this->config = new webdatasourceconfigtype(*static_cast<webdatasourceconfigtype*>(config));
        if(this->config->formatter) {
            this->formatter = this->config->formatter;
        } else {
            this->formatter = [this](const std::string& data, const std::string& type) {
                return this->formatData(data, type);
            };
        }
        return TRADER_RESULT_SUCCESS;
    } else {
        this->config = nullptr;
        return TRADER_RESULT_INVALID_PARAMETER;
    }
}

TraderResult WebDataSource::Start() {
    // 启动线程,根据config里的url,username,password,querylist,获取数据，并调用publishData
    std::thread getdata_thread([this]() {
        httplib::Client cli(this->config->url);
//        if(this->config->username.empty() || this->config->password.empty()){
////            httplib::Headers headers;
////            cli.set_default_headers(headers);
//        }else{
//            cli.set_basic_auth(this->config->username, this->config->password);
//        }
        for (const auto &query : this->config->querylist) {
            
            auto res = cli.Get(query.c_str());
            if (res && res->status == 200) {
                // 检查并转换编码为UTF-8
                std::string utf8_body;
                if (res->get_header_value("Content-Type").find("utf-8") == std::string::npos) {
                    utf8_body = gbk_to_utf8(res->body);
                } else {
                    utf8_body = res->body;
                }
                this->publishData(this->formatData(utf8_body, "vec"));
                std::cout << "get data success, url: " << this->config->url << ", query: " << query << ", status: " << res->status << ", data: " << gbk_to_utf8(res->body)  << std::endl;
            }else{
                std::cout << "get data failed, url: " << this->config->url << ", query: " << query << ", status: " << res->status << std::endl;
            }
        }
    });
    getdata_thread.detach();
    this->getdata_thread = std::move(getdata_thread);
    return TRADER_RESULT_SUCCESS;
}

TraderResult WebDataSource::Stop() {
    if (this->getdata_thread.joinable()) {
        this->getdata_thread.join();
    }
    return TRADER_RESULT_SUCCESS;
}

TraderResult WebDataSource::publishData(dataType data) {
    std::cout << "Publishing data: " << data.dataEntry.ask_close << "\n";
    return TRADER_RESULT_NOT_IMPLEMENTED;
}