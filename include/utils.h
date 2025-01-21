template<typename T>
class singleton {
public:
    singleton(const singleton&) = delete;
    singleton& operator=(const singleton&) = delete;
    static T& instance() {
        static T instance;
        return instance;
    }
};


#include "enum.h"

static double get_conversion_factor(TraderTimeType from, TraderTimeType to) {
        static const double factors[9][9] = {
                // NS       US        MS         S          MIN        HOUR       DAY        WEEK       YEAR
                {1.0,       1e-3,     1e-6,      1e-9,      1.6667e-11,2.7778e-13,1.1574e-14,1.6534e-15,3.171e-17}, // NS
                {1000.0,    1.0,      1e-3,      1e-6,      1.6667e-8, 2.7778e-10,1.1574e-11,1.6534e-12,3.171e-14}, // US
                {1e6,       1000.0,   1.0,       1e-3,      1.6667e-5, 2.7778e-7, 1.1574e-8, 1.6534e-9, 3.171e-11}, // MS
                {1e9,       1e6,      1000.0,    1.0,       0.016667,  2.7778e-4, 1.1574e-5, 1.6534e-6, 3.171e-8},  // S
                {6e10,      6e7,      60000.0,   60.0,      1.0,       0.016667,  6.9444e-4, 9.9206e-5, 1.9026e-6}, // MIN
                {3.6e12,    3.6e9,    3.6e6,     3600.0,    60.0,      1.0,       0.041667,  0.005952,  0.000114},  // HOUR
                {8.64e13,   8.64e10,  8.64e7,    86400.0,   1440.0,    24.0,      1.0,       0.142857,  0.002738},  // DAY
                {6.048e14,  6.048e11, 6.048e8,   604800.0,  10080.0,   168.0,     7.0,       1.0,       0.019165},  // WEEK
                {3.156e16,  3.156e13, 3.156e10,  31536000.0,525600.0,  8760.0,    365.0,     52.1429,   1.0}        // YEAR
        };
        return factors[from][to];
}

#include <iconv.h>
#include <string>
#include <vector>

static std::string gbk_to_utf8(const std::string& gbk_str) {
    iconv_t cd = iconv_open("UTF-8", "GBK");
    if (cd == (iconv_t)-1) {
        perror("iconv_open");
        return "";
    }

    const char* in_buf = gbk_str.data();
    size_t in_left = gbk_str.size();

    std::vector<char> out_buf(in_left * 4);  // 为安全起见，分配更多空间
    char* out_ptr = out_buf.data();
    size_t out_left = out_buf.size();

    std::string result;
    while (in_left > 0) {
        size_t ret = iconv(cd, const_cast<char**>(&in_buf), &in_left, &out_ptr, &out_left);
        if (ret == (size_t)-1) {
            if (errno == E2BIG) {
                // 输出缓冲区不足，先保存已转换的部分
                result.append(out_buf.data(), out_buf.size() - out_left);
                out_ptr = out_buf.data();
                out_left = out_buf.size();
            } else {
                // 其他错误，中断转换
                perror("iconv");
                iconv_close(cd);
                return "";
            }
        } else {
            // 转换成功，保存结果
            result.append(out_buf.data(), out_buf.size() - out_left);
            break;
        }
    }

    iconv_close(cd);
    return result;
}

#include <atomic>
template <size_t capacity,size_t read_nums,RingBufferType type>
class RingBuffer{
public:
    RingBuffer():head(0),tail(0),size(0){
    }

    void* alloc_buffer(size_t message_length){
        if(message_length > capacity){
            return nullptr;
        }
        size_t offset;
        //head 超过capacity 则从0开始
        if(head.load() + message_length > capacity){
            offset = 0;
        }else{
            offset = head.load();
        }
        //如果offset+message_length+1 %capacity >=tail.load() 代表当前空间不够，则返回nullptr
        if((offset+message_length+1) %capacity >=tail.load()){
            return nullptr;
        }else{
            {
                std::lock_guard<std::mutex> lock(mutex);
                headers.emplace_back(BufferHeader{message_length, offset, read_nums});
            }
            alloc_head.fetch_add(message_length);
            return buffer + offset;
        }
    }

private:
    struct BufferHeader{
        size_t message_length;
        size_t offset;
        std::atomic<size_t> read_cnt;
    };

    char buffer[capacity]{};  // 缓冲区
    std::vector<BufferHeader> headers;
    std::mutex mutex;
    std::atomic<size_t> head;       // 写指针
    std::atomic<size_t> alloc_head;       // 分配指针
    std::atomic<size_t> tail;       // 读指针
};