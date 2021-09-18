#include <stdio.h>

#define POSITIONS 16
#define IGNORE_AFTER_POSITION_CHANGE 5
#define IGNORE_IF_VARIATION_MORE_THAN 20
#define CONSIDER_FIXED_AFTER 400

int is_green(unsigned char position, unsigned int power) {
    switch (position) {
        case 0:
            return !power;
        case 1:
            return power > 40 && power < 110;
        case 2:
            return power > 100 && power < 260;
        case 3:
            return power > 190 && power < 340;
        case 4:
            return power > 265 && power < 470;
        case 5:
            return power > 350 && power < 620;
        case 6:
            return power > 425 && power < 800;
        case 7:
            return power > 520 && power < 870;
        case 8:
            return power > 630 && power < 1020;
        case 9:
            return power > 750 && power < 1140;
        case 10:
            return power > 880 && power < 1250;
        case 11:
            return power > 1000 && power < 1370;
        case 12:
            return power > 1160 && power < 1470;
        case 13:
            return power > 1320 && power < 1550;
        case 14:
            return power > 1530 && power < 1670;
        case 15:
            return power > 1750 && power < 1800;
        default:
            return 0;
    }
}

int is_yellow(unsigned char position, unsigned int power) {
    switch (position) {
        case 0:
            return !power;
        case 1:
            return power > 40 && power < 110;
        case 2:
            return power > 100 && power < 260;
        case 3:
            return power > 190 && power < 340;
        case 4:
            return power > 265 && power < 470;
        case 5:
            return power > 350 && power < 620;
        case 6:
            return power > 425 && power < 800;
        case 7:
            return power > 520 && power < 870;
        case 8:
            return power > 630 && power < 1020;
        case 9:
            return power > 750 && power < 1140;
        case 10:
            return power > 880 && power < 1250;
        case 11:
            return power > 1000 && power < 1370;
        case 12:
            return power > 1160 && power < 1470;
        case 13:
            return power > 1320 && power < 1550;
        case 14:
            return power > 1530 && power < 1670;
        case 15:
            return power > 1750 && power < 1800;
        default:
            return 0;
    }
}

// power cache
#define POWER_CACHE_LENGTH 4

unsigned int power_cache[POWER_CACHE_LENGTH];
unsigned char power_cache_index = 0;

void power_cache_write(unsigned int power) {
    power_cache[power_cache_index] = power;
    power_cache_index++;
    if (power_cache_index >= POWER_CACHE_LENGTH) {
        power_cache_index = 0;
    }
}

unsigned int power_cache_max_diff() {
    unsigned int min = -1;
    unsigned int max = 0;
    for (unsigned char i = 0; i < POWER_CACHE_LENGTH; ++i) {
        if (power_cache[i] < min) {
            min = power_cache[i];
        }
        if (power_cache[i] > max) {
            max = power_cache[i];
        }
    }
    return max - min;
}


// additional state
enum state {
    WHITE,
    GREEN,
    YELLOW,
    RED,
};

unsigned int step_in_position = 0;
unsigned char last_position = 0;

struct position_info {
    enum state state;
    unsigned int green_streak;
    unsigned int yellow_streak;
};

struct position_info info[POSITIONS];

void process(
        unsigned char position,
        unsigned int current,
        unsigned int voltage) {
    current /= 50;
    voltage /= 20;

    unsigned int power = current * voltage;
    power_cache_write(power);

    if (position != last_position) {
        last_position = position;
        step_in_position = 0;
        return;
    }
    step_in_position++;
    if (step_in_position < 5) {
        return;
    }
    if (power_cache_max_diff() > IGNORE_IF_VARIATION_MORE_THAN) {
        return;
    }

    enum state cur_state = GREEN;
    if (!is_green(position, power)) {
        cur_state = YELLOW;
    }
    if (!is_yellow(position, power)) {
        cur_state = RED;
    }

    info[position].green_streak++;
    info[position].yellow_streak++;
    if (cur_state > info[position].state) {
        if (cur_state == RED) {
            info[position].green_streak = 0;
            info[position].yellow_streak = 0;
        }
        if (cur_state == YELLOW) {
            info[position].green_streak = 0;
        }
        info[position].state = cur_state;
    }

    if (info[position].yellow_streak > CONSIDER_FIXED_AFTER) {
        info[position].state = YELLOW;
    }
    if (info[position].green_streak > CONSIDER_FIXED_AFTER) {
        info[position].state = GREEN;
    }
}
