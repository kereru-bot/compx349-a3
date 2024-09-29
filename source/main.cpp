#include "MicroBit.h"

MicroBit uBit;

#define MOTOR_LEFT 0x00
#define MOTOR_RIGHT 0x02

#define DIRECTION_FORWARD 0x00
#define DIRECTION_BACKWARD 0x01

#define PIN_LED_RIGHT uBit.io.P12
#define PIN_LED_LEFT uBit.io.P8

#define PIN_PATROL_LEFT uBit.io.P13
#define PIN_PATROL_RIGHT uBit.io.P14

int8_t runLeftMotor = 1;
int8_t runRightMotor = 1;

//returns 0 if reading black and 1 if reading white
int8_t read_patrol_light(NRF52Pin pin) {
    int8_t value = 2;
    value = pin.getDigitalValue();
    return value;
}

void turn_led_on(NRF52Pin pin) {
    pin.setDigitalValue(1);
}

void turn_led_off(NRF52Pin pin) {
    pin.setDigitalValue(0);
}

//flashes the led lights
void run_led_lights() {
    while(1) {
        int8_t left = read_patrol_light(PIN_PATROL_LEFT);
        int8_t right = read_patrol_light(PIN_PATROL_RIGHT);

        if(left == 1) {
            turn_led_on(PIN_LED_LEFT);
        } else if(left == 0) {
            turn_led_off(PIN_LED_LEFT);
        }

        if(right == 1) {
            turn_led_on(PIN_LED_RIGHT);
        } else if(right == 0) {
            turn_led_off(PIN_LED_RIGHT);
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

//0 to move forward 1 to move backward
void run_motors(uint8_t motor, uint8_t direction, int duration) {
    uint8_t buf[3];

    buf[0] = motor;
    buf[1] = direction;
    
    //speed of the motors
    buf[2] = 0x40; 
    uBit.i2c.write( 0x20, buf, 3); 

    //uBit.sleep(duration);
    //buf[2] = 0x00;
    //uBit.i2c.write( 0x20, buf, 3);    // device address is 0x10 but must be left shifted for Micro:bit libraries.
    //buf[0] = 0x02;
    //ret = uBit.i2c.write( 0x20, buf, 3); 
        
    //uBit.display.scroll(ret);
}

void stop_motors(uint8_t motor) {
    uint8_t buf[3];

    buf[0] = motor;
    buf[1] = 0;
    buf[2] = 0; 
    uBit.i2c.write( 0x20, buf, 3); 
}

void run_left_motor() {
    while(1) {
        if(runLeftMotor == 1) {
            run_motors(MOTOR_LEFT,DIRECTION_FORWARD,10);
        } else {
            stop_motors(MOTOR_LEFT);
        }
        uBit.sleep(10);
    }
}

void run_right_motor() {
    while(1) {
        if(runRightMotor == 1) {
            run_motors(MOTOR_RIGHT,DIRECTION_FORWARD,10);
        } else {
            stop_motors(MOTOR_RIGHT);
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
