#include <cstdio>

#include <game.hpp>

int main(void) {
    int n_players = 1;
    int n_cards = 13;

    int n_monte_carlo_rounds = 1000;
    // int n_monte_carlo_rounds = 1;

    Game game = Game(n_players, n_cards);

    int sum = 0;
    for (int i = 0; i < n_monte_carlo_rounds; i++) {
        while (true) {
            bool terminal = game.game_loop();
            if (terminal) {
                int n_rounds = game.get_n_rounds();
                sum += n_rounds;
                break;
            } 
        }
        game.reset_game();
    }

    printf("Average #rounds = %lf\n", (double)sum / n_monte_carlo_rounds);

    return 0;
}