#include "MicroBit.h"

#define MOTOR_LEFT 0x00
#define MOTOR_RIGHT 0x02

#define DIRECTION_FORWARD 0x00
#define DIRECTION_BACKWARD 0x01

#define LED_PIN_RIGHT (*uBit).io.P12
#define LED_PIN_LEFT (*uBit).io.P8

#define GREYSCALE_PIN_LEFT (*uBit).io.P13
#define GREYSCALE_PIN_RIGHT (*uBit).io.P14

#define TRIG_PIN (*uBit).io.P1
#define ECHO_PIN (*uBit).io.P2

//returns 0 if reading black and 1 if reading white
int8_t read_greyscale_sensor_left();

int8_t read_greyscale_sensor_right();

int8_t read_ultrasonic();

void turn_left_led_on();

void turn_right_led_on();

void turn_left_led_off();

void turn_right_led_off();

void turn_both_leds_on();

void turn_both_leds_off();

void stop_motor_left();

void stop_motor_right();

void stop_both_motors();

void start_motor_left(uint8_t direction, uint8_t speed);

void start_motor_right(uint8_t direction, uint8_t speed);

void start_both_motors(uint8_t direction, uint8_t speed);

void on_start(MicroBit* uBit);