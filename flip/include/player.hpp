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
        constexpr int k_mem_size = 10000;
        memory_buffer = malloc(k_mem_size * sizeof(int));
        int* int_buffer = reinterpret_cast<int*>(memory_buffer);
        for (int i = 0; i < k_mem_size; i++) { 
            int_buffer[i] = -1;
        }
        int_buffer[k_mem_size - 1] = 0;
    }

    const IAgentStrategy& strategy;
    void* memory_buffer{ nullptr };
};
