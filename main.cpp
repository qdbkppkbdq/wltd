#include <iostream>
#include <unordered_map>
#include <queue>
#include <functional>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>


#include "event.h"
#include "utils.h"
#include "enum.h"
void test_enum() {
    std::cout << "TraderResult::TRADER_RESULT_SUCCESS: " << TraderResultStr(TraderResult::TRADER_RESULT_SUCCESS) << std::endl;
    std::cout << "TraderResult::TRADER_RESULT_FAILURE: " << TraderResultStr(TraderResult::TRADER_RESULT_FAILURE) << std::endl;
}
void test_singleton() {
    auto& instance = singleton<int>::instance();
    std::cout << "instance: " << instance << std::endl;
}
void test_event_engine() {
    EventEngine engine;

    Role doorLock("locked");
    doorLock.registerHandler("locked", "unlock", [](void* data) {
        std::cout << "Unlocking door with code: " << data << std::endl;
    });

    Role thermostat("off");
    thermostat.registerHandler("off", "turn_on", [](void* data) {
        std::cout << "Turning on thermostat with temperature: " << data << std::endl;
    });

    engine.addRole(&doorLock);
    engine.addRole(&thermostat);

    // 启动事件引擎处理线程
    std::thread engineThread(&EventEngine::run, &engine);

    // 生成事件
    engine.emitEvent(Event{"unlock", (void *)1234});
    engine.emitEvent(Event{"turn_on", (void *)78});

    // 等待引擎线程完成
    engineThread.join();
}
// 示例使用
int main() {
    test_enum();
    test_singleton();
    test_event_engine();
    return 0;
}
