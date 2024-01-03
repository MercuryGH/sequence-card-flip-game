#pragma once

#include <unordered_map>
#include <cassert>

#include <table.hpp>
#include <rng.hpp>

// interface
class IAgentStrategy {
public:
    virtual void action(Table& table, void* memory_buffer) const = 0;
    virtual void action(Table& table) const = 0;
};

// Very naive O(m^2) strategy
class RandomStrategy: public IAgentStrategy {
public:
    void action(Table& table, void* memory_buffer) const override {
        action(table);
    }

    void action(Table& table) const override {
        int n_cards = table.get_n_cards();
        int maximum_flipped_val = table.get_maximum_flipped_val();
        int n_non_flipped_cards = n_cards - maximum_flipped_val;

        // index mapping
        std::unordered_map<int, int> non_flipped_cards_index_map;
        int cnt = 0;
        for (int i = 0; i < n_cards; i++) {
            if (table.query_flipped(i) == false) {
                non_flipped_cards_index_map[cnt] = i;
                cnt++;
            }
        }
        assert(cnt == n_non_flipped_cards);

        std::uniform_int_distribution<std::mt19937::result_type> dist_n(0, n_non_flipped_cards - 1); 

        auto random_index = dist_n(get_rng());

        int val = table.query_val(non_flipped_cards_index_map.at(random_index));
        if (val == maximum_flipped_val + 1) {
            table.flip_queried_card();
        }

        // table.replace_queried_card(0);
    }
};

// still a O(m^2) strategy, but have a lower constant compared with RandomStrategy. This strategy ended in a finite step.
class AlwaysLastStrategy: public IAgentStrategy {
public:
    void action(Table& table, void* memory_buffer) const override {
        action(table);
    }

    void action(Table& table) const override {
        int n_cards = table.get_n_cards();
        int maximum_flipped_val = table.get_maximum_flipped_val();
        int n_non_flipped_cards = n_cards - maximum_flipped_val;

        int first_non_flipped_index = 0;
        for (int i = 0; i < n_cards; i++) {
            if (table.query_flipped(i) == false) {
                first_non_flipped_index = i;
                break;
            }
        }

        int val = table.query_val(first_non_flipped_index);
        if (val == maximum_flipped_val + 1) {
            table.flip_queried_card();
        }

        table.replace_queried_card(n_cards - 1);
    } 
};

// Divide and Conquer O(m log m)
class QSortStrategy: public IAgentStrategy {
public:
    void action(Table& table, void* memory_buffer) const override {
        action(table);
    }

    void action(Table& table) const override {
        int n_cards = table.get_n_cards();
        std::vector<Card> cards(n_cards);
        for (int i = 0; i < n_cards; i++) {
            bool flipped = table.query_flipped(i);
            if (flipped == true) {
                cards.at(i) = { table.query_val(i), true };
            } else {
                cards.at(i) = { -1, false };
            }
        }

        bool finish_action = false;
        recurse_entry(table, cards, 0, 0, finish_action);
    }

private:
    std::pair<int, int> get_maximum_flipped_card_val_index(const std::vector<Card>& cards) const {
        int val = 0;
        int index = -1;
        for (int i = 0; i < cards.size(); i++) {
            if (cards.at(i).flipped) {
                if (cards.at(i).val > val) {
                    val = cards.at(i).val;
                    index = i;
                }
            }
        }
        return { val, index };
    }

    int get_minimum_flipped_card_index(const std::vector<Card>& cards, int val_offset) const {
        int index = -1;
        for (int i = 0; i < cards.size(); i++) {
            if (cards.at(i).flipped) {
                if (cards.at(i).val == val_offset + 1) {
                    index = i;
                }
            }
        }
        return index;
    }

    void linear_search_val(Table& table, int next_val, int index_offset, int n_cards) const {
        int val = table.query_val(index_offset);
        if (val == next_val) {
            table.flip_queried_card();
        }
        // insert to last
        table.replace_queried_card(index_offset + n_cards - 1);
    }

    void recurse_entry(Table& table, const std::vector<Card>& cards, int index_offset, int val_offset, bool& finish_action) const {
        if (finish_action) {
            return;
        }

        const int n_cards = cards.size();

        // early return, no operation needed
        if (n_cards == 0) {
            return;
        }
        bool all_cards_flipped = true;
        for (int i = 0; i < n_cards; i++) {
            if (cards.at(i).flipped == false) {
                all_cards_flipped = false;
            }
        }
        if (all_cards_flipped == true) {
            return;
        }

        if (n_cards == 1) {
            // flip and leave
            int val = table.query_val(index_offset);
            assert(val == val_offset + 1);
            table.flip_queried_card();
            finish_action = true;
            return;
        }

        const auto [ max_flipped_val, max_flipped_index ] = get_maximum_flipped_card_val_index(cards);
        if (max_flipped_val == 0) {
            // if no card is flipped, linear search.
            linear_search_val(table, val_offset + 1, index_offset, n_cards);
            finish_action = true;
            return;
        }

        // the problem division is complete. handle the left and right subproblem
        const int min_flipped_index = get_minimum_flipped_card_index(cards, val_offset);
        const int target_r_interval_n_cards = n_cards / 2;
        const int cur_r_interval_n_cards = n_cards - 1 - min_flipped_index;

        // put the left wrong number to right
        if (cur_r_interval_n_cards < target_r_interval_n_cards) {
            // flip the leftest card and place it
            int val = table.query_val(index_offset);

            if (val - val_offset > n_cards - target_r_interval_n_cards) {
                // insert to right side
                table.replace_queried_card(index_offset + min_flipped_index);
            } else {
                // insert to left side
                table.replace_queried_card(index_offset + min_flipped_index - 1);
            }

            finish_action = true;
            return;
        }

        std::vector<Card> l_interval_cards(min_flipped_index);
        for (int i = 0; i < min_flipped_index; i++) {
            if (table.query_flipped(i + index_offset) == false) {
                l_interval_cards.at(i) = { -1, false };
            } else {
                l_interval_cards.at(i) = { table.query_val(i + index_offset), true };
            }
        }

        std::vector<Card> r_interval_cards(target_r_interval_n_cards);
        for (int i = min_flipped_index + 1, j = 0; i < n_cards; i++, j++) {
            if (table.query_flipped(i + index_offset) == false) {
                r_interval_cards.at(j) = { -1, false };
            } else {
                r_interval_cards.at(j) = { table.query_val(i + index_offset), true };
            }
        }

        recurse_entry(table, l_interval_cards, 
            index_offset, 
            val_offset + 1,
            finish_action
        );
        recurse_entry(table, r_interval_cards, 
            index_offset + min_flipped_index + 1, 
            val_offset + min_flipped_index + 1,
            finish_action
        );
    }
};

// Linear divide and conqueor strategy, O(m log m)
class ZStrategy: public IAgentStrategy {
public:
    void action(Table& table, void* memory_buffer) const override {
        action(table);
    }

    void action(Table& table) const override {
        int n_cards = table.get_n_cards();
        int maximum_flipped_val = table.get_maximum_flipped_val();
        int interval_start;    // the location where interval we're dividing start
        int divider_loc;       // the location where the card divides the interval at
        int interval_end;      // the location where interval we're dividing end
        for (interval_start = 0; interval_start < n_cards; interval_start++) {
            if (table.query_flipped(interval_start) == false) {
                break;
            }
        }
        for (divider_loc = interval_start + 1; divider_loc < n_cards; divider_loc++) {
            if (table.query_flipped(divider_loc) == true) {
                break;
            }
        }
        for (interval_end = divider_loc + 1; interval_end < n_cards; interval_end++) {
            if (table.query_flipped(interval_end) == true) {
                break;
            }
        }
        interval_end--;
        if (interval_end > n_cards - 1) {
            interval_end = n_cards - 1;
        }
        // whether the interval's divider is flipped
        bool divider_is_in_interval = true;
        if (divider_loc > n_cards - 1) {
            divider_is_in_interval = false;
        } else {
            int divider_num = table.query_val(divider_loc);
            // some number in the interval has been flipped and replaced to front
            int ahead_num_cnt = maximum_flipped_val - divider_num;
            int half_num = (divider_num + (interval_end + 1)) / 2;
            int correct_loc = interval_start + (half_num - divider_num) - ahead_num_cnt;
            // if the interval has been divided, correct the interval we're dividing
            if (divider_loc == correct_loc) {
                divider_is_in_interval = false;
                interval_end = divider_loc - 1;
            }
        }
        if (divider_is_in_interval) {
            int divider_num = table.query_val(divider_loc);
            // printf("%d %d %d %d %d\n", divider_num, divider_loc, interval_start, interval_end, maximum_flipped_val);
            int half_num = (divider_num + (interval_end + 1)) / 2;
            int flip_num = table.query_val(interval_start);
            if (flip_num == maximum_flipped_val + 1) {
                table.flip_queried_card();
                table.replace_queried_card(0);
            } else {
                if (flip_num > half_num) {
                    table.replace_queried_card(divider_loc);
                } else {
                    table.replace_queried_card(divider_loc - 1);
                }
            }
        } else {
            int flip_num = table.query_val(interval_start);
            if (flip_num == maximum_flipped_val + 1) {
                table.flip_queried_card();
            }
            table.replace_queried_card(interval_end);
        }
    }
};

