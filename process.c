#include <stdio.h>

#define POSITIONS 16
#define IGNORE_POWER_LOWER_THAN 50
#define IGNORE_AFTER_POSITION_CHANGE 5
#define IGNORE_IF_VARIATION_MORE_THAN 20
#define YELLOW_IF_VARIATION_MORE_THAN 15

#define FROM_WHITE_TO_GREEN 5
#define FROM_WHITE_TO_YELLOW 5
#define FROM_WHITE_TO_RED 5
#define FROM_GREEN_TO_YELLOW 10
#define FROM_GREEN_TO_RED 10
#define FROM_YELLOW_TO_GREEN 400
#define FROM_YELLOW_TO_RED 10
#define FROM_RED_TO_GREEN 400
#define FROM_RED_TO_YELLOW 400


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
            return power > 0 && power < 260;
        case 2:
            return power > 40 && power < 340;
        case 3:
            return power > 100 && power < 470;
        case 4:
            return power > 190 && power < 620;
        case 5:
            return power > 265 && power < 800;
        case 6:
            return power > 350 && power < 870;
        case 7:
            return power > 425 && power < 1020;
        case 8:
            return power > 520 && power < 1140;
        case 9:
            return power > 630 && power < 1250;
        case 10:
            return power > 750 && power < 1370;
        case 11:
            return power > 880 && power < 1470;
        case 12:
            return power > 1000 && power < 1550;
        case 13:
            return power > 1160 && power < 1670;
        case 14:
            return power > 1320 && power < 1800;
        case 15:
            return power > 1530 && power < 1900;
        default:
            return 0;
    }
}

// power cache
#define POWER_CACHE_LENGTH 8

static unsigned int power_cache[POWER_CACHE_LENGTH];
static unsigned char power_cache_index = 0;

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

static unsigned int step_in_position = 0;
static unsigned char last_position = 0;

static unsigned int last_current = 0;
static unsigned int since_current_last_fell = -20;

struct position_info {
    enum state state;
    unsigned int streak;
};

struct position_info info[POSITIONS];

static unsigned char info_changed = 0;

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
        last_current = 0;
        since_current_last_fell = -20;
        return;
    }
    step_in_position++;
    if (step_in_position < 5) {
        return;
    }
    since_current_last_fell++;
    if (last_current > current + 5) {
        since_current_last_fell = 0;
    }
    last_current = current;
    if (power_cache_max_diff() > IGNORE_IF_VARIATION_MORE_THAN) {
        return;
    }
    if (power < IGNORE_POWER_LOWER_THAN) {
        return;
    }

    enum state cur_state = RED;
    if (is_yellow(position, power)) {
        cur_state = YELLOW;
    }
    if (is_green(position, power)) {
        cur_state = GREEN;
        //if (power_cache_max_diff() > YELLOW_IF_VARIATION_MORE_THAN) {
        //    cur_state = YELLOW;
        //}
    }


    if (info[position].streak == -1) {
        info[position].streak = -2;
    }
    if (power_cache_max_diff() < 10) {
        info[position].streak++;
    } else {
        info[position].streak = 0;
    }

    unsigned char prev_state = info[position].state;

    if (info[position].state == WHITE) {
        switch (cur_state) {
            case GREEN:
                if (info[position].streak > FROM_WHITE_TO_GREEN) info[position].state = cur_state;
                break;
            case YELLOW:
                if (info[position].streak > FROM_WHITE_TO_YELLOW) info[position].state = cur_state;
                break;
            case RED:
                if (info[position].streak > FROM_WHITE_TO_RED) info[position].state = cur_state;
                break;
        }
    }
    if (info[position].state == GREEN) {
        switch (cur_state) {
            case YELLOW:
                if (info[position].streak > FROM_GREEN_TO_YELLOW) info[position].state = cur_state;
                break;
            case RED:
                if (info[position].streak > FROM_GREEN_TO_RED) info[position].state = cur_state;
                break;
        }
    }
    if (info[position].state == YELLOW) {
        switch (cur_state) {
            case GREEN:
                if (info[position].streak > FROM_YELLOW_TO_GREEN) info[position].state = cur_state;
                break;
            case RED:
                if (info[position].streak > FROM_YELLOW_TO_RED) info[position].state = cur_state;
                break;
        }
    }
    if (info[position].state == RED) {
        switch (cur_state) {
            case GREEN:
                if (info[position].streak > FROM_RED_TO_GREEN) info[position].state = cur_state;
                break;
            case YELLOW:
                if (info[position].streak > FROM_RED_TO_YELLOW) info[position].state = cur_state;
                break;
        }
    }

    if (
            since_current_last_fell < 5
            && ((info[position].state == GREEN && (cur_state == YELLOW || cur_state == RED))
                || (info[position].state == YELLOW && cur_state == RED))
        ) {
        info[position].state = cur_state;
    }

    if (info[position].state != prev_state) {
      info_changed = 1;
    }
}
