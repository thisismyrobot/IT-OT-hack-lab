/*

Factory controller.

*/
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "socket.h"


#define PLC_IP "192.168.0.10"

#define PUMP_FLAG_ADDRESS 0X0001
#define VALVE_CTRL_FLAG_ADDRESS 0X0003
#define PUMP_RUN_ADDRESS 0X0000
#define VALVE_CTRL_ENABLE_ADDRESS 0X0002
#define VALVE_OPEN_ADDRESS 0X2021
#define PRESSURE_HIGH_ADDRESS 0X0000

#define DRAIN_CYCLES 3


unsigned short random_byte();
void set_pump_flag(int clientfd, unsigned short flag);
void set_valve_ctrl_flag(int clientfd, unsigned short flag);
void enable_valve_control(int clientfd, unsigned short flag);
void run_pump(int clientfd, unsigned short flag);
void stop_pump(int clientfd);
void open_valve(int clientfd);
bool pressure_high(int clientfd);
void write_holding_register(int clientfd, unsigned short address, unsigned short value);
void write_coil(int clientfd, unsigned short address, bool value);
bool read_input(int clientfd, unsigned short address);

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
    while (1)
    {
        if (pressure_high(clientfd))
        {
            stop_pump(clientfd);

            int i = 0;
            for (i = 0; i < DRAIN_CYCLES; i++)
            {
                enable_valve_control(clientfd, valve_ctrl_flag);
                open_valve(clientfd);
                sleep(2);
            }
        }
        else
        {
            enable_valve_control(clientfd, valve_ctrl_flag);
            run_pump(clientfd, pump_flag);
            usleep(250);
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

bool pressure_high(int clientfd)
{
    return read_input(clientfd, PRESSURE_HIGH_ADDRESS);
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

bool read_input(int clientfd, unsigned short address)
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
        0X02,                   // Function Code: Read Discrete Input
        address >> 8,           // Address MSB
        address & 0X00FF,       // Address LSB
        0X00,                   // Count of inputs requested MSB
        0X01,                   // Count of inputs requested LSB
    };

    TCPWrite(clientfd, payload,  12);

    int bytes = 0;
    bytes = TCPBlockRead(clientfd, buffer, 255);

    // Last value is the read bit.
    return buffer[bytes - 1] == 0X01;
}
