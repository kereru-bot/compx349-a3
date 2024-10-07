#include "MicroBit.h"
#include "drivers.h"

static MicroBit uBit;

#define STATE_MOVE_FOWARD 0
#define STATE_TURN_LEFT 1
#define STATE_TURN_RIGHT 2
#define STATE_TURN_CLOCKWISE 3
#define STATE_TURN_ANTICLOCKWISE 4

int8_t currentState = 0;
int8_t previousTurningState = 0;

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

        uBit.sleep(10);
    }
}

void run_left_motor() {
    while(1) {
        switch(currentState) {
            case STATE_MOVE_FOWARD:
                start_motor_left(DIRECTION_FORWARD, 0x40);
                break;
            case STATE_TURN_LEFT:
                stop_motor_left();
                break;
            case STATE_TURN_RIGHT:
                start_motor_left(DIRECTION_FORWARD, 0x20);
                break;
            case STATE_TURN_ANTICLOCKWISE:
                break;
            case STATE_TURN_CLOCKWISE:
                break;
        }

        uBit.sleep(1);
    }
}

void run_right_motor() {
    while(1) {
        switch(currentState) {
            case STATE_MOVE_FOWARD:
                start_motor_right(DIRECTION_FORWARD, 0x30);
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
        }
        uBit.sleep(1);
    }
}

//next steps
//if driving on black and then white is seen, rotate until black is seen anywhere,
//then continue as normal

void manage_direction() {
    while(1) {
        //if steep corner, reverse a little bit for more leeway
        //if steep corner turning right, turn off left motor turn on right motor backwards
        //if steep corner turning left, turn off right motor turn on left motor backwards
        int8_t left = read_greyscale_sensor_left();
        int8_t right = read_greyscale_sensor_right();

        
        if(left == 0 && right == 0) {
            currentState = STATE_MOVE_FOWARD;
        } else if(left == 0 && right == 1) {
            //turn left
            currentState = STATE_TURN_LEFT;
            previousTurningState = currentState;
        } else if(right == 0 && left == 1) {
            //turn right
            currentState = STATE_TURN_RIGHT;
            previousTurningState = currentState;
        } else if(right == 1 && left == 1) {
            //looking at white right now
            if(previousTurningState == STATE_TURN_LEFT) {
                //maybe overshot a left corner?
                currentState = STATE_TURN_ANTICLOCKWISE;
            } else if(previousTurningState == STATE_TURN_RIGHT) {
                currentState = STATE_TURN_CLOCKWISE;
            }
        }

        uBit.sleep(1);
    }
}

int main()
{
    on_start(&uBit);
    create_fiber(manage_direction);
    create_fiber(run_left_motor);
    create_fiber(run_right_motor);
    create_fiber(run_led_lights);
    release_fiber();
}
