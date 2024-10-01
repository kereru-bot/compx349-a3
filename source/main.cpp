#include "MicroBit.h"
#include "drivers.h"

static MicroBit uBit;

int8_t runLeftMotor = 1;
int8_t runRightMotor = 1;

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
    on_start(&uBit);

    create_fiber(run_left_motor);
    create_fiber(run_right_motor);
    create_fiber(run_led_lights);
    release_fiber();
    
}
