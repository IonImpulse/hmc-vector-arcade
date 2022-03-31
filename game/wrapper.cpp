#ifndef WRAPPER 
#define WRAPPER

#include <iostream>
#include <chrono>
#include <unistd.h>
#include <bitset>
#include <math.h>
#include <string.h>

void send_command(char command[]) {
    std::cout << command << std::endl;
}


std::string num_to_bin(int num, int width) {
    // Writes the number with the specified width to a string.
    // The most significant bit is written first.
    std::bitset<32> b(num);
    std::string s = b.to_string();
    return s.substr(s.size() - width, width);
}

void absolute_vec(int x, int y, int brightness) {
    char command[100];

    std::string x_bin = num_to_bin(x, 10);
    std::string y_bin = num_to_bin(y, 10);
    std::string brightness_bin = num_to_bin(brightness, 10);

    sprintf(command, "10%s%s%s", x_bin.c_str(), y_bin.c_str(), brightness_bin.c_str());

    send_command(command);
}

void relative_vec(int x_delta, int y_delta, int brightness) {
    char command[100];

    std::string x_bin = num_to_bin(x_delta, 10);
    std::string y_bin = num_to_bin(y_delta, 10);
    std::string brightness_bin = num_to_bin(brightness, 10);

    sprintf(command, "11%s%s%s", x_bin.c_str(), y_bin.c_str(), brightness_bin.c_str());

    send_command(command);
}

void draw_current_buffer() {
    send_command("0110");
}

void draw_buffer_A() {
    send_command("0100");
}

void draw_buffer_B() {
    send_command("0101");
}

void draw_buffer_switch() {
    send_command("0111");
}

void halt() {
    send_command("0");
}
#endif