#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <unordered_map>
#include <functional>
#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>

// 角色上下文
struct RoleContext {
    std::string state;
};

// 事件结构
struct Event {
    std::string type;
    void* data;
};

// 角色类
class Role {
public:
    Role(const std::string& initialState)
            : context{initialState} {}

    void registerHandler(const std::string& state, const std::string& eventType, std::function<void(void*)> handler);

    void handleEvent(const Event& event);

    RoleContext context;

private:
    std::unordered_map<std::string, std::unordered_map<std::string, std::function<void(void*)>>> eventHandlers;
};

// 事件引擎类
class EventEngine {
public:
    void addRole(Role* role);
    void emitEvent(const Event& event);
    void run();

private:
    void processEvent(const Event& event);

    std::vector<Role*> roles;
    std::queue<Event> eventQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondition;
};

#endif // EVENT_H