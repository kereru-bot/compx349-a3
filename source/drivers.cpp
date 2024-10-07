#include "drivers.h"

MicroBit* uBit;

//returns 0 if reading black and 1 if reading white
int8_t read_greyscale_sensor_left() {
    return GREYSCALE_PIN_LEFT.getDigitalValue();
}

int8_t read_greyscale_sensor_right() {
    return GREYSCALE_PIN_RIGHT.getDigitalValue();
}

int8_t read_ultrasonic()
{

    TRIG_PIN.setDigitalValue(0);
    TRIG_PIN.setDigitalValue(1);
    uBit->sleep(1);
    TRIG_PIN.setDigitalValue(0);

    unsigned long time = uBit-> systemTime();

    while(ECHO_PIN.getDigitalValue())
    {
        if(uBit->systemTime()-time > 1)
        {
            return 0;
        }
    }

    return  1;
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

void stop_motor_left() {
    uint8_t buf[3];
    buf[0] = MOTOR_LEFT;
    buf[1] = 0;
    buf[2] = 0; 
    (*uBit).i2c.write( 0x20, buf, 3); 
}

void stop_motor_right() {
    uint8_t buf[3];
    buf[0] = MOTOR_RIGHT;
    buf[1] = 0;
    buf[2] = 0; 
    (*uBit).i2c.write( 0x20, buf, 3);
}

void stop_both_motors() {
    stop_motor_left();
    stop_motor_right();
}

void start_motor_left(uint8_t direction, uint8_t speed) {
    uint8_t buf[3];
    buf[0] = MOTOR_LEFT;
    buf[1] = direction;
    //speed of the motors
    buf[2] = speed; 
    (*uBit).i2c.write( 0x20, buf, 3); 
}

void start_motor_right(uint8_t direction, uint8_t speed) {
    uint8_t buf[3];
    buf[0] = MOTOR_RIGHT;
    buf[1] = direction;
    //speed of the motors
    buf[2] = speed; 
    (*uBit).i2c.write( 0x20, buf, 3); 
}

void start_both_motors(uint8_t direction, uint8_t speed) {
    start_motor_left(direction, speed);
    start_motor_right(direction, speed);
}

void on_start(MicroBit* microbit) {
    uBit = microbit;
    (*uBit).init();
}
