/*

Factory debug controller.

*/
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "socket.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>


#define PLC_IP "192.168.0.10"

#define PUMP_FLAG_ADDRESS 0X0001
#define VALVE_CTRL_FLAG_ADDRESS 0X0003
#define PUMP_RUN_ADDRESS 0X0000
#define VALVE_CTRL_ENABLE_ADDRESS 0X0002
#define VALVE_OPEN_ADDRESS 0X2021

unsigned short random_byte();
void set_pump_flag(int clientfd, unsigned short flag);
void set_valve_ctrl_flag(int clientfd, unsigned short flag);
void enable_valve_control(int clientfd, unsigned short flag);
void run_pump(int clientfd, unsigned short flag);
void stop_pump(int clientfd);
void open_valve(int clientfd);
void close_valve(int clientfd);
void write_holding_register(int clientfd, unsigned short address, unsigned short value);
void write_coil(int clientfd, unsigned short address, bool value);
void reset_terminal_mode();
void set_conio_terminal_mode();
int getch();
int kbhit();

struct termios orig_termios;

int main()
{
    int clientfd;
    TCPClientInit(&clientfd);
    TCPClientConnect(clientfd, PLC_IP, 502);

    // Bootstrap.
    srand(time(NULL));
    int pump_flag = random_byte();
    int valve_ctrl_flag = random_byte();
    set_pump_flag(clientfd, pump_flag);
    set_valve_ctrl_flag(clientfd, valve_ctrl_flag);

    // Control.
    printf("W = up, S = down, Q = quit\n");
    set_conio_terminal_mode();
    char c;

    bool pump = false;
    bool drain = false;
    while (1) {

        int keyupcycles = 0;
        while (!kbhit()) {
            keyupcycles++;
            // No key for 100ms means all key up.
            if (keyupcycles == 10) {
                break;
            }
            usleep(10);
        }
        
        if (kbhit()) {
            c = getch();
            if (c == 'q') {
                return 0;
            }
            else if (c == 'w') {
                pump = true;
                drain = false;
            }
            else if (c == 's') {
                pump = false;
                drain = true;
            }
        }
        else {
            pump = false;
            drain = false;
        }

        enable_valve_control(clientfd, valve_ctrl_flag);
        if (pump) {
            run_pump(clientfd, pump_flag);
        }
        else {
            stop_pump(clientfd);
        }
        if (drain) {
            open_valve(clientfd);
        }
        else {
            close_valve(clientfd);
        }
    }

    return 0;
}

unsigned short random_byte()
{
    return rand() % 255;
}

void set_pump_flag(int clientfd, unsigned short flag)
{
    write_holding_register(clientfd, PUMP_FLAG_ADDRESS, flag);
}

void set_valve_ctrl_flag(int clientfd, unsigned short flag)
{
    write_holding_register(clientfd, VALVE_CTRL_FLAG_ADDRESS, flag);
}

void enable_valve_control(int clientfd, unsigned short flag)
{
    write_holding_register(clientfd, VALVE_CTRL_ENABLE_ADDRESS, flag);
}

void run_pump(int clientfd, unsigned short flag)
{
    write_holding_register(clientfd, PUMP_RUN_ADDRESS, flag);
}

void stop_pump(int clientfd)
{
    write_holding_register(clientfd, PUMP_RUN_ADDRESS, 0);
}

void open_valve(int clientfd)
{
    write_coil(clientfd, VALVE_OPEN_ADDRESS, true);
}

void close_valve(int clientfd)
{
    write_coil(clientfd, VALVE_OPEN_ADDRESS, false);
}

void write_holding_register(int clientfd, unsigned short address, unsigned short value)
{
    char buffer[255];
    unsigned char payload[] =
    {
        // Header.
        0X00,                   // Transaction identifier MSB
        0X01,                   // Transaction identifier LSB
        0X00,                   // Protocol identifier MSB
        0X00,                   // Protocol identifier LSB
        0X00,                   // Message (PDU) length MSB
        0X06,                   // Message (PDU) length LSB

        // Protocol Data Unit.
        0X00,                   // Remote unit identifier
        0X06,                   // Function Code: Write Single Holding Register
        address >> 8,           // Address MSB
        address & 0X00FF,       // Address LSB
        value >> 8,             // Value MSB
        value & 0X00FF,         // Value LSB
    };
    
    TCPWrite(clientfd, payload,  12);
    TCPBlockRead(clientfd, buffer, 255);
}

void write_coil(int clientfd, unsigned short address, bool value)
{
    char buffer[255];
    unsigned char payload[] =
    {
        // Header.
        0X00,                   // Transaction identifier MSB
        0X01,                   // Transaction identifier LSB
        0X00,                   // Protocol identifier MSB
        0X00,                   // Protocol identifier LSB
        0X00,                   // Message (PDU) length MSB
        0X06,                   // Message (PDU) length LSB

        // Protocol Data Unit.
        0X00,                   // Remote unit identifier
        0X05,                   // Function Code: Force Single Coil
        address >> 8,           // Address MSB
        address & 0X00FF,       // Address LSB
        value ? 0XFF : 0X00,    // Value MSB
        0,                      // Value LSB
    };
    
    TCPWrite(clientfd, payload,  12);
    TCPBlockRead(clientfd, buffer, 255);
}


void reset_terminal_mode()
{
    tcsetattr(0, TCSANOW, &orig_termios);
}

void set_conio_terminal_mode()
{
    struct termios new_termios;

    /* take two copies - one for now, one for later */
    tcgetattr(0, &orig_termios);
    memcpy(&new_termios, &orig_termios, sizeof(new_termios));

    /* register cleanup handler, and set the new terminal mode */
    atexit(reset_terminal_mode);
    cfmakeraw(&new_termios);
    tcsetattr(0, TCSANOW, &new_termios);
}

int kbhit()
{
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv) > 0;
}

int getch()
{
    int r;
    unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0) {
        return r;
    } else {
        return c;
    }
}
