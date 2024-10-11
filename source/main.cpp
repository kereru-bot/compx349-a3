#include "MicroBit.h"
#include "drivers.h"
static MicroBit uBit;

#define STATE_MOVE_STOP -1
#define STATE_MOVE_FOWARD 0
#define STATE_TURN_LEFT 1
#define STATE_TURN_RIGHT 2
#define STATE_TURN_CLOCKWISE 3
#define STATE_TURN_ANTICLOCKWISE 4

#define TURNING_BIAS_LEFT 1
#define TURNING_BIAS_RIGHT 2

int8_t currentTurningBias = TURNING_BIAS_RIGHT;

int16_t manageDirectionSleepTime = 1;

int8_t currentState = 0;
int8_t previousTurningState = 0;
int8_t object_detected = 0;

int8_t leftSeenBlack = 0;
int8_t rightSeenBlack = 0;

int8_t rotating = 0;

int16_t turningSleepTime = 1000;
int16_t afterTurnSleepTime = 600;

/**
 * Runs the left and right led lights based
 * on the output of the left and right sensors
 * goes high if the sensor is seeing white, otherwise
 * low
 */
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

        uBit.sleep(10);
    }
}

/**
 * Manages the behaviour of the left motor
 * based on the current state
 */
void run_left_motor() {
    while(1) {
        switch(currentState) {
            case STATE_MOVE_FOWARD:
                start_motor_left(DIRECTION_FORWARD, 0x30);
                break;
            case STATE_TURN_LEFT:
                stop_motor_left();
                break;
            case STATE_TURN_RIGHT:
                start_motor_left(DIRECTION_FORWARD, 0x25);
                break;
            case STATE_TURN_ANTICLOCKWISE:
                start_motor_right(DIRECTION_FORWARD, 0x20);
                start_motor_left(DIRECTION_BACKWARD, 0x20);
                break;
            case STATE_TURN_CLOCKWISE:
                start_motor_left(DIRECTION_FORWARD, 0x20);
                start_motor_right(DIRECTION_BACKWARD, 0x20);
                break;
            case STATE_MOVE_STOP:
                 stop_motor_left();
                 break;
        }

        uBit.sleep(1);
    }
}

/**
 * Manages the behaviour of the right motor
 * based on the current state
 */
void run_right_motor() {
    while(1) {
        switch(currentState) {
            case STATE_MOVE_FOWARD:
                start_motor_right(DIRECTION_FORWARD, 0x25);
                break;
            case STATE_TURN_LEFT:
                start_motor_right(DIRECTION_FORWARD, 0x20);
                break;
            case STATE_TURN_RIGHT:
                stop_motor_right();
                break;
            case STATE_TURN_ANTICLOCKWISE:
                start_motor_right(DIRECTION_FORWARD, 0x20);
                start_motor_left(DIRECTION_BACKWARD, 0x20);
                break;
            case STATE_TURN_CLOCKWISE:
                start_motor_left(DIRECTION_FORWARD, 0x20);
                start_motor_right(DIRECTION_BACKWARD, 0x20);
                break;
            case STATE_MOVE_STOP:
                 stop_motor_right();
                 break;
        }
        uBit.sleep(1);
    }
}

/**
 * Manages the states that dictate which direction the
 * motors will move in
 */
void manage_direction() {
    while(1) {
        int8_t left = read_greyscale_sensor_left();
        int8_t right = read_greyscale_sensor_right();

        if (object_detected==1)
        {
            currentState = STATE_MOVE_STOP;
        }
        else if(right == 1 && left == 1) {
            //white space has been reached, determine which direction
            //the wheels should turn
            if(currentTurningBias == TURNING_BIAS_LEFT) {
                currentState = STATE_TURN_ANTICLOCKWISE;
                currentTurningBias = TURNING_BIAS_RIGHT;
                //sleep for a bit while the wheels rotate
                //to avoid "noise" on the sensors
                uBit.sleep(turningSleepTime);
                currentState = STATE_TURN_RIGHT;
                uBit.sleep(afterTurnSleepTime);
            } else if(currentTurningBias == TURNING_BIAS_RIGHT) {
                currentState = STATE_TURN_CLOCKWISE;
                currentTurningBias = TURNING_BIAS_LEFT;
                //sleep for a bit while the wheels rotate
                //to avoid "noise" on the sensors
                uBit.sleep(turningSleepTime);
                currentState = STATE_TURN_LEFT;
                uBit.sleep(afterTurnSleepTime);
            }
        } else {
            //looking at black, move forward
            if(left == 0 && right == 0) {
                currentState = STATE_MOVE_FOWARD;
            } else if(left == 0 && right == 1) {
                //turn left to try and recenter
                currentState = STATE_TURN_LEFT;
                previousTurningState = currentState;
                
            } else if(right == 0 && left == 1) {
                //turn right to try and recenter
                currentState = STATE_TURN_RIGHT;
                previousTurningState = currentState;
            }
        }
        uBit.sleep(1);
    }
}

void poll_ultrasonic()
{
while(1)
{
    object_detected = read_ultrasonic();

    uBit.sleep(50);
}
}


int main()
{
    on_start(&uBit);

    create_fiber(manage_direction);
    create_fiber(run_left_motor);
    create_fiber(run_right_motor);
    create_fiber(run_led_lights);
    create_fiber(poll_ultrasonic);
    

    release_fiber();
}
