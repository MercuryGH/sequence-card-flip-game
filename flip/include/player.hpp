#pragma once

#include <agent.hpp>

class Player {
public:
    Player(const IAgentStrategy& strategy): strategy(strategy) {
        allocate_memory();
    }

    ~Player() {
        delete memory_buffer;
    }

    void turn_action(Table& table) {
        strategy.action(table, memory_buffer);
    }

    void turn_action_without_memory(Table& table) {
        strategy.action(table);
    }

    void reset_memory() {
        delete memory_buffer;
        allocate_memory();
    }

private:
    void allocate_memory() {
        // TODO: allocate memory here...
    }

    const IAgentStrategy& strategy;
    void* memory_buffer{ nullptr };
};
