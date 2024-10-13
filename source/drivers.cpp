#include "drivers.h"

MicroBit* uBit;

/**
 * Reads left greyscale sensor, returns 
 * 0 if reading black and 1 if reading white
 */
int8_t read_greyscale_sensor_left() {
    return GREYSCALE_PIN_LEFT.getDigitalValue();
}

/**
 * Reads right greyscale sensor, returns 
 * 0 if reading black and 1 if reading white
 */
int8_t read_greyscale_sensor_right() {
    return GREYSCALE_PIN_RIGHT.getDigitalValue();
}

/**
 * Reads from the ultrasonic sensor,
 * returns 1 if an object has been seen
 * or 0 if not
 */
int8_t read_ultrasonic() {
    TRIG_PIN.setDigitalValue(0);
    TRIG_PIN.setDigitalValue(1);
    uBit->sleep(1);
    TRIG_PIN.setDigitalValue(0);

    unsigned long time = uBit->systemTime();

    int i = 0;
    int returnValue = 1;
    //waits for the echo pin to go high
    while(!ECHO_PIN.getDigitalValue()){
        //Just in case
        if (uBit->systemTime()-time>300)
        {
            break;
        }
    }
    while(ECHO_PIN.getDigitalValue())
    {
        i ++ ;
        if(i > 600)
        {
            returnValue = 0;
            break;
        }
    }

    return  returnValue;
}

/**
 * Turns the left red led on
 */
void turn_left_led_on() {
    LED_PIN_LEFT.setDigitalValue(1);
}

/**
 * Turns the right red led on
 */
void turn_right_led_on() {
    LED_PIN_RIGHT.setDigitalValue(1);
}

/**
 * Turns the left red led off
 */
void turn_left_led_off() {
    LED_PIN_LEFT.setDigitalValue(0);
}

/**
 * Turns the right red led off
 */
void turn_right_led_off() {
    LED_PIN_RIGHT.setDigitalValue(0);
}

/**
 * Turns both red leds on
 */
void turn_both_leds_on() {
    turn_left_led_on();
    turn_right_led_on();
}

/**
 * Turns both red leds off
 */
void turn_both_leds_off() {
    turn_left_led_on();
    turn_left_led_off();
}

/**
 * Stops the left motor from running
 */
void stop_motor_left() {
    uint8_t buf[3];
    buf[0] = MOTOR_LEFT;
    buf[1] = 0;
    buf[2] = 0; 
    (*uBit).i2c.write( 0x20, buf, 3); 
}

/**
 * Stops the right motor from running
 */
void stop_motor_right() {
    uint8_t buf[3];
    buf[0] = MOTOR_RIGHT;
    buf[1] = 0;
    buf[2] = 0; 
    (*uBit).i2c.write( 0x20, buf, 3);
}

/**
 * Stops both motors from running
 */
void stop_both_motors() {
    stop_motor_left();
    stop_motor_right();
}

/**
 * Starts the left motor using the given
 * direction and speed
 * @param direction The direction to run the motor
 * @param speed The speed of the motor
 */
void start_motor_left(uint8_t direction, uint8_t speed) {
    uint8_t buf[3];
    buf[0] = MOTOR_LEFT;
    buf[1] = direction;
    //speed of the motors
    buf[2] = speed; 
    (*uBit).i2c.write( 0x20, buf, 3); 
}

/**
 * Starts the right motor using the given
 * direction and speed
 * @param direction The direction to run the motor
 * @param speed The speed of the motor
 */
void start_motor_right(uint8_t direction, uint8_t speed) {
    uint8_t buf[3];
    buf[0] = MOTOR_RIGHT;
    buf[1] = direction;
    //speed of the motors
    buf[2] = speed; 
    (*uBit).i2c.write( 0x20, buf, 3); 
}

/**
 * Starts both motors using the given
 * direction and speed
 * @param direction The direction to run the motor
 * @param speed The speed of the motor
 */
void start_both_motors(uint8_t direction, uint8_t speed) {
    start_motor_left(direction, speed);
    start_motor_right(direction, speed);
}

/**
 * Run this on start with the microbit
 * to use, so it will initialise and the
 * drivers will work
 */
void on_start(MicroBit* microbit) {
    uBit = microbit;
    (*uBit).init();
}
