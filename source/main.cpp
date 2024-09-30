#include "MicroBit.h"

MicroBit uBit;

#define MOTOR_LEFT 0x00
#define MOTOR_RIGHT 0x02

#define DIRECTION_FORWARD 0x00
#define DIRECTION_BACKWARD 0x01

#define LED_PIN_RIGHT uBit.io.P12
#define LED_PIN_LEFT uBit.io.P8

#define GREYSCALE_PIN_LEFT uBit.io.P13
#define GREYSCALE_PIN_RIGHT uBit.io.P14

int8_t runLeftMotor = 1;
int8_t runRightMotor = 1;

//returns 0 if reading black and 1 if reading white
int8_t read_greyscale_sensor_left() {
    return GREYSCALE_PIN_LEFT.getDigitalValue();
}

int8_t read_greyscale_sensor_right() {
    return GREYSCALE_PIN_RIGHT.getDigitalValue();
}

void turn_left_led_on() {
    LED_PIN_LEFT.setDigitalValue(1);
}

void turn_right_led_on() {
    LED_PIN_RIGHT.setDigitalValue(1);
}

void turn_left_led_off() {
    LED_PIN_LEFT.setDigitalValue(0);
}

void turn_right_led_off() {
    LED_PIN_RIGHT.setDigitalValue(0);
}

void turn_both_leds_on() {
    turn_left_led_on();
    turn_right_led_on();
}

void turn_both_leds_off() {
    turn_left_led_on();
    turn_left_led_off();
}

//flashes the led lights
void run_led_lights() {
    while(1) {
        int8_t left = read_greyscale_sensor_left();
        int8_t right = read_greyscale_sensor_right();

        if(left == 1) {
            turn_left_led_on();
        } else if(left == 0) {
            turn_left_led_off();
        }

        if(right == 1) {
            turn_right_led_on();
        } else if(right == 0) {
            turn_right_led_off();
        }

        if(left == 0 && right == 1) {
            runLeftMotor = 0;
            uBit.sleep(100);
            runLeftMotor = 1;
        }

        if(right == 0 && left == 1) {
            runRightMotor = 0;
            uBit.sleep(100);
            runRightMotor = 1;
        }

        uBit.sleep(10);
    }
}

void stop_motor_left() {
    uint8_t buf[3];
    buf[0] = MOTOR_LEFT;
    buf[1] = 0;
    buf[2] = 0; 
    uBit.i2c.write( 0x20, buf, 3); 
}

void stop_motor_right() {
    uint8_t buf[3];
    buf[0] = MOTOR_RIGHT;
    buf[1] = 0;
    buf[2] = 0; 
    uBit.i2c.write( 0x20, buf, 3);
}

void stop_both_motors() {
    stop_motor_left();
    stop_motor_right();
}

void start_motor_left(uint8_t direction) {
    uint8_t buf[3];
    buf[0] = MOTOR_LEFT;
    buf[1] = direction;
    //speed of the motors
    buf[2] = 0x40; 
    uBit.i2c.write( 0x20, buf, 3); 
}

void start_motor_right(uint8_t direction) {
    uint8_t buf[3];
    buf[0] = MOTOR_RIGHT;
    buf[1] = direction;
    //speed of the motors
    buf[2] = 0x40; 
    uBit.i2c.write( 0x20, buf, 3); 
}

void start_both_motors(uint8_t motor, uint8_t direction) {
    start_motor_left(direction);
    start_motor_right(direction);
}

void run_left_motor() {
    while(1) {
        if(runLeftMotor == 1) {
            start_motor_left(DIRECTION_FORWARD);
        } else {
            stop_motor_left();
        }
        uBit.sleep(10);
    }
}

void run_right_motor() {
    while(1) {
        if(runRightMotor == 1) {
            start_motor_right(DIRECTION_FORWARD);
        } else {
            stop_motor_right();
        }
        uBit.sleep(10);
    }
}

int main()
{
    uBit.init();
    while(1) {
        
        create_fiber(run_left_motor);
        create_fiber(run_right_motor);
        create_fiber(run_led_lights);
        release_fiber();
    }
}
