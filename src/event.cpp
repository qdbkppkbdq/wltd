//
// Created by root on 12/2/24.
//

#include "event.h"
#include <iostream>
void Role::registerHandler(const std::string& state, const std::string& eventType, std::function<void(void*)> handler) {
    Role::eventHandlers[state][eventType] = handler;
}

void Role::handleEvent(const Event& event) {
    auto& handlers = eventHandlers[context.state];
    if (handlers.find(event.type) != handlers.end()) {
        handlers[event.type](event.data);
    } else {
        std::cerr << "No handler for event type: " << event.type << " in state: " << context.state << std::endl;
    }
}
void EventEngine::addRole(Role* role) {
    roles.push_back(role);
}

void EventEngine::emitEvent(const Event& event) {
    std::unique_lock<std::mutex> lock(queueMutex);
    eventQueue.push(event);
    queueCondition.notify_one();
}

void EventEngine::run() {
    while (true) {
        Event event;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCondition.wait(lock, [this]{ return !eventQueue.empty(); });
            event = eventQueue.front();
            eventQueue.pop();
        }
        processEvent(event);
    }
}

void EventEngine::processEvent(const Event& event) {
    for (auto& role : roles) {
        role->handleEvent(event);
    }
}