#pragma once

#include <vector>
#include <cassert>

#include <card.hpp>
#include <rng.hpp>

class Table {
    friend class Game;

private:
    Table(int n_cards) {
        reset_cards(n_cards);
    }

public:
    bool query_flipped(int index) const {
        return cards.at(index).flipped;
    }

    int query_val(int index) {
        const auto& card = cards.at(index);
        if (card.flipped == true) {
            return card.val;
        }

        // query a non-flipped card

        check_query_lock();

        rec.card_can_flip = (card.val == get_maximum_flipped_val() + 1); 
        rec.query_lock = true;
        rec.query_index = index;

        if (print_debug_msg) {
            printf("cards[%d] = %d, ", index, card.val);
        }

        return card.val;
    }

    void flip_queried_card() {
        check_queried();
        check_card_can_flip();

        cards.at(rec.query_index).flipped = true;

        if (print_debug_msg) {
            printf("flip, ");
        }

        // check if game is terminal
        if (cards.at(rec.query_index).val == cards.size()) {
            game_terminal = true;
        }
    }

    /**
     * insert_index range = [0, n - 1]
    */
    void replace_queried_card(int insert_index) {
        check_queried();

        Card copied_card = cards.at(rec.query_index);

        cards.erase(cards.begin() + rec.query_index);
        cards.insert(cards.begin() + insert_index, copied_card);

        rec.card_can_flip = false;

        if (print_debug_msg) {
            printf("insert to pos %d.", insert_index);
        }
    }

    int get_maximum_flipped_val() const {
        int ret = 0;
        for (const auto card : cards) {
            if (card.flipped == true) {
                ret = std::max(ret, card.val);
            }
        }
        return ret;
    }

    int get_n_cards() const {
        return cards.size();
    }

private:
    std::vector<Card> cards;
    bool game_terminal{ false };

    void reset_cards(int n_cards) {
        cards = std::vector<Card>(n_cards);
        for (int i = 0; i < n_cards; i++) {
            cards.at(i) = Card{ i + 1 };
        }

        // shuffle the cards
        std::shuffle(std::begin(cards), std::end(cards), get_rng());
        game_terminal = false;
    }

    struct TurnRecord {
        bool query_lock{ false }; // one player can only query once
        bool card_can_flip{ false }; // current card can be flipeed
        int query_index{ -1 }; // the index that player queried
    } rec;
    void new_player_turn() {
        rec.card_can_flip = false;
        rec.query_lock = false;
        rec.query_index = -1;
    }

    void check_query_lock() {
        if (rec.query_lock == true) {
            printf("You cannot call query_val() in a non-flipped card twice in a turn!\n");
            assert(false);
            exit(0);
        }
    }

    void check_card_can_flip() {
        if (rec.card_can_flip == false) {
            printf("You cannot flip a card with val != k_i + 1.\n");
            assert(false);
            exit(0);
        }
    }

    void check_queried() {
        if (rec.query_lock == false) {
            printf("You did not query a card's val!\n");
            assert(false);
            exit(0);
        }
    }

    bool print_debug_msg{ false };
    
    void print_table_states() {
        for (const auto& card : cards) {
            if (card.flipped) {
                printf("%d ", card.val);
            } else {
                printf("X ");
            }
        }
        printf("\n");
    }
};
