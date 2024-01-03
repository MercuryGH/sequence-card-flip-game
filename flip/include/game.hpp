#pragma once

#include <memory>

#include <table.hpp>
#include <agent.hpp>
#include <player.hpp>

class Game {
public:
    Game(int n_players, int n_cards):
    n_players(n_players), n_cards(n_cards), table(n_cards) {
        // TODO: set your own strategy here
        // agent_strategy = std::make_unique<RandomStrategy>();
        // agent_strategy = std::make_unique<AlwaysLastStrategy>();
        // agent_strategy = std::make_unique<QSortStrategy>();
        // agent_strategy = std::make_unique<ZStrategy>();
        agent_strategy = std::make_unique<OnePersonStrategy>();

        print_debug_msg = true;
        // print_debug_msg = false;

        reset_game();
    }
    
    bool game_loop() {
        table.new_player_turn();

        players.at(cur_player_index).turn_action(table);
        // when n_players is very large, simulate with this
        // players.at(cur_player_index).turn_action_without_memory(table); 

        n_rounds++;
        // DEBUG msg
        if (print_debug_msg) {
            printf("\n");
            // if (n_rounds % 10 == 0)
            //     printf("n_round = %d\n", n_rounds);
            table.print_table_states();
        }

        if (table.game_terminal == true) {
            if (print_debug_msg) {
                printf("Game over, #rounds = %d\n", n_rounds);
            }
            return true;
        }

        cur_player_index++;
        cur_player_index %= n_players;
        return false;
    }

    int get_n_rounds() const {
        return n_rounds;
    }

    void reset_game() {
        cur_player_index = 0;
        n_rounds = 0;

        table.reset_cards(n_cards);
        table.print_debug_msg = print_debug_msg;
        if (print_debug_msg)
            table.print_table_states();

        players.clear();
        players.reserve(n_players);
        for (int i = 0; i < n_players; i++) {
            players.emplace_back(*agent_strategy);
        }
    }

private:
    std::unique_ptr<IAgentStrategy> agent_strategy;
    std::vector<Player> players;
    Table table;

    int n_players; 
    int n_cards;
    int cur_player_index{ 0 };
    int n_rounds{ 0 };

    bool print_debug_msg{ false };
};
