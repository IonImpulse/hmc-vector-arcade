#include <unistd.h>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <string>
#include <csignal>
#include <termios.h>
#include <iostream>

#include "../include/rawio.h"

#define BUF_SIZE 256

int pipe_to_simulator_fd;
int child_pid;

struct InputState get_inputs() {
    struct InputState inputs;
    char buf[BUF_SIZE];
    bool w,a,s,d,space;
    while (read(STDIN_FILENO, buf, BUF_SIZE-1) > 0) {
        buf[BUF_SIZE-1] = '\0';
        if (strstr(buf, "\003")) {
            // exit on receiving a ^C input
            kill(child_pid, 9);
            exit(0);
        }
        if (strcasestr(buf, "w")) {
            w = true;
        }
        if (strcasestr(buf, "a")) {
            a = true;
        }
        if (strcasestr(buf, "s")) {
            s = true;
        }
        if (strcasestr(buf, "d")) {
            d = true;
        }
        if (strcasestr(buf, " ")) {
            space = true;
        }
    }
    inputs.xpos = 0;
    inputs.ypos = 0;
    if (w) inputs.ypos += 1.0;
    if (s) inputs.ypos -= 1.0;
    if (a) inputs.xpos += 1.0;
    if (d) inputs.xpos -= 1.0;
    inputs.buttons = 0;
    if (space) inputs.buttons |= 1;
    return inputs;
}

char SIMULATOR_PATH[]{ "../vector-simulator/target/release/vector-simulator" };
// Do some hacky stuff to make g++ not complain about converting string constants to `char*`
char SIMULATOR_ARG_1[]{ "-p" };
char SIMULATOR_ARG_2[]{ "-d" };
char SIMULATOR_ARG_3[]{ "0.001" };
char SIMULATOR_ARG_4[]{ "-f" };
char SIMULATOR_ARG_5[]{ "0.00000001" };
char SIMULATOR_ARG_6[]{ "-s" };
char SIMULATOR_ARG_7[]{ "100000000" };
char* const SIMULATOR_ARGS[9] = {
    SIMULATOR_PATH,
    SIMULATOR_ARG_1,
    SIMULATOR_ARG_2,
    SIMULATOR_ARG_3,
    SIMULATOR_ARG_4,
    SIMULATOR_ARG_5,
    SIMULATOR_ARG_6,
    SIMULATOR_ARG_7,
    NULL
};

struct termios old_options;

void restore_old_options() {
    if (tcsetattr(STDIN_FILENO, TCSANOW, &old_options)) {
        fprintf(stderr, "Error restoring old options: %s\n", strerror(errno));
    }
    fprintf(stdout, "\n");
}

static inline int setup_stdin() {
    // Set up stdin to behave how we want (using termios)
    struct termios new_options;
    if (tcgetattr(STDIN_FILENO, &old_options) == -1) {
        fprintf(stderr, "Unexpected tcgetattr failure: %d\n", errno);
        return -1;
    }
    new_options = old_options;
    atexit(restore_old_options);
    new_options.c_iflag = ISTRIP;
    new_options.c_oflag = 0;
    new_options.c_lflag = 0;
    new_options.c_cc[VMIN] = 0;
    new_options.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_options) == -1) {
        fprintf(stderr, "Unexpected tcsetattr failure while creating terminal state: %d", errno);
        return -1;
    }
    return 0;
}

void initialize_input_output() {
    int pipe_fds[2], ret_val;
    if (pipe(pipe_fds) == -1) {
        goto fail;
    }
    if ((ret_val = fork()) == -1) {
        goto fail;
    }
    if (ret_val == 0) {
        dup2(pipe_fds[0], STDIN_FILENO);
        dup2(pipe_fds[1], STDOUT_FILENO);
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        execv(SIMULATOR_PATH, SIMULATOR_ARGS);
        fprintf(stderr, "Failed to exec simulator: %s\r\n", strerror(errno));
        exit(1);
    }
    pipe_to_simulator_fd = pipe_fds[1];
    child_pid = ret_val;
    close(pipe_fds[0]);
    if (setup_stdin() == -1) {
        goto fail;
    }
    return;
fail:
    fprintf(stderr, "Failed to initialize simulated_io\n");
    exit(1);
}

static inline void send(std::string data) {
    if (write(pipe_to_simulator_fd, data.c_str(), data.length()) < 0) {
        fprintf(stderr, "Failed to write to simulator: %s\r\n", strerror(errno));
        exit(1);
    }
}

static inline std::string number_to_binary(int16_t number, int16_t num_bits) {
    std::string bits;
    for (int i=0; i<num_bits; i++) {
        bits.insert(0, 1, (char)('0' + (number & 1)));
        number >>= 1;
    }
    return bits;
}

void draw_buffer_switch() {
    send("0111\n");
}
static inline void draw_vector(const char* lead, int16_t x_position, int16_t y_position, int16_t brightness) {
    char command[40];
    std::string x_bits = number_to_binary(x_position, 10);
    std::string y_bits = number_to_binary(y_position, 10);
    std::string b_bits = number_to_binary(brightness, 10);
    sprintf(
        command,
        "%s%s%s%s\n",
        lead,
        x_bits.c_str(),
        y_bits.c_str(),
        b_bits.c_str()
    );
    send(command);
}
void draw_absolute_vector(int16_t x_position, int16_t y_position, int16_t brightness) {
    draw_vector("10", x_position, y_position, brightness);
}
void load_abs_pos(int16_t x_position, int16_t y_position) {
    draw_absolute_vector(x_position, y_position, 0);
}

void draw_relative_vector(int16_t delta_x, int16_t delta_y, int16_t brightness) {
    draw_vector("11", delta_x, delta_y, brightness);
}
void draw_end_buffer() {
    send("0\n");
}

bool is_halted() {
    // assume vector simulator is always ready to switch buffers
    return true;
}

typedef std::chrono::high_resolution_clock Clock;
static std::chrono::time_point<Clock> target_time;
void start_timer(uint32_t milliseconds) {
    target_time = Clock::now() + std::chrono::milliseconds(milliseconds);
}
bool timer_done() {
    auto now = Clock::now();
    if (now > target_time) {
        return false;
    }
    std::chrono::nanoseconds sleep_duration = target_time - now;
    usleep(sleep_duration.count() / 1000);
    return true;
}

void printChar(char data) {
    std::cout<<data;
}

void sendString(const char* txStr) {
    std::cout<<txStr;
}
