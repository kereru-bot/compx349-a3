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
#define TURNING_BIAS_NONE 3

int8_t currentTurningBias = 0;
int8_t currentState = 0;
int8_t object_detected = 0;

int8_t buttonPressed = 0;

/**
 * Runs the left and right led lights based
 * on the output of the left and right sensors
 * goes high if the sensor is seeing white, otherwise
 * low
 */
void run_led_lights()
{
    while (1)
    {
        int8_t left = read_greyscale_sensor_left();
        int8_t right = read_greyscale_sensor_right();

        if (left == 1)
        {
            turn_left_led_on();
        }
        else if (left == 0)
        {
            turn_left_led_off();
        }

        if (right == 1)
        {
            turn_right_led_on();
        }
        else if (right == 0)
        {
            turn_right_led_off();
        }

        uBit.sleep(10);
    }
}

/**
 * Manages the behaviour of the left motor
 * based on the current state
 */
void run_left_motor()
{
    while (1)
    {
        switch (currentState)
        {
        case STATE_MOVE_FOWARD:
            start_motor_left(DIRECTION_FORWARD, 0x33);
            break;
        case STATE_TURN_LEFT:
            stop_motor_left();
            break;
        case STATE_TURN_RIGHT:
            start_motor_left(DIRECTION_FORWARD, 0x35);
            break;
        case STATE_TURN_ANTICLOCKWISE:
            start_motor_right(DIRECTION_FORWARD, 0x25);
            start_motor_left(DIRECTION_BACKWARD, 0x20);
            break;
        case STATE_TURN_CLOCKWISE:
            start_motor_left(DIRECTION_FORWARD, 0x25);
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
void run_right_motor()
{
    while (1)
    {
        switch (currentState)
        {
        case STATE_MOVE_FOWARD:
            start_motor_right(DIRECTION_FORWARD, 0x25);
            break;
        case STATE_TURN_LEFT:
            start_motor_right(DIRECTION_FORWARD, 0x25);
            break;
        case STATE_TURN_RIGHT:
            stop_motor_right();
            break;
        case STATE_TURN_ANTICLOCKWISE:
            start_motor_right(DIRECTION_FORWARD, 0x25);
            start_motor_left(DIRECTION_BACKWARD, 0x20);
            break;
        case STATE_TURN_CLOCKWISE:
            start_motor_left(DIRECTION_FORWARD, 0x25);
            start_motor_right(DIRECTION_BACKWARD, 0x20);
            break;
        case STATE_MOVE_STOP:
            stop_motor_right();
            break;
        }
        uBit.sleep(1);
    }
}


void manage_direction_straight() {
    int8_t isRotating = 0;
    int8_t left = 0;
    int8_t right = 0;
    int8_t previousState = -2;
    int8_t testedLeft = 0;
    int8_t prevLeft = 0;
    int8_t prevRight = 0;
    int8_t testedWhite = 0;
    currentTurningBias = TURNING_BIAS_RIGHT;
    while(1) {
        left = read_greyscale_sensor_left();
        right = read_greyscale_sensor_right();

        //both seeing black
        if(left == 0 && right == 0) {
            if(isRotating == 1) {
                if(currentTurningBias == TURNING_BIAS_LEFT) {
                    currentTurningBias = TURNING_BIAS_RIGHT;
                    isRotating = 0;
                    testedWhite = 0;
                } else {
                    currentTurningBias = TURNING_BIAS_LEFT;
                    isRotating = 0;
                    testedWhite = 0;
                }
            }
            currentState = STATE_MOVE_FOWARD;
        }

        if(left == 1 && right == 0) {
            currentState = STATE_TURN_RIGHT;
        }

        if(left == 0 && right == 1) {
            currentState = STATE_TURN_LEFT;
        }

        if(left == 1 && right == 1) {
            if(testedWhite == 1) {
                if(currentTurningBias == TURNING_BIAS_LEFT) {
                    isRotating = 1;
                    currentState = STATE_TURN_LEFT;
                } else {
                    isRotating = 1;
                    currentState = STATE_TURN_RIGHT;
                }
            } else {
                currentState = STATE_MOVE_FOWARD;
                uBit.sleep(500);
                testedWhite = 1;
            }

        }

        previousState = currentState;
        prevLeft = left;
        prevRight = right;
        uBit.sleep(5);
    }
}

/**
 * Manages the states that dictate which direction the
 * motors will move in
 */
void manage_direction()
{
    int8_t isRotating = 0;
    int8_t left = 0;
    int8_t right = 0;
    int8_t previousState = -2;
    int8_t testedLeft = 0;
    int8_t leftTurnCycles = 0;

    while (1) {
        left = read_greyscale_sensor_left();
        right = read_greyscale_sensor_right();

        if (object_detected == 1) {
            currentState = STATE_MOVE_STOP;
        } else if(currentTurningBias == TURNING_BIAS_LEFT) {

            //white on left and back on right
            if(left == 1 && right == 0) {
                if(leftTurnCycles > 30) {
                    uBit.display.image.setPixelValue(3,3,255);
                    //mustve seen black and turned at intersection
                    currentTurningBias = TURNING_BIAS_RIGHT;
                    isRotating = 0;
                } else if(isRotating == 1) {
                    currentTurningBias = TURNING_BIAS_RIGHT;
                    isRotating = 0;
                } else {
                    currentState = STATE_MOVE_FOWARD;
                }
                leftTurnCycles = 0;
            }

            //black on both
            if(left == 0 && right == 0) {
                if(testedLeft == 1) {
                    currentState = STATE_TURN_LEFT;
                    testedLeft = 0;
                } else {
                    testedLeft = 1;
                }
            }

            //black on left and white on right, at an intersection
            if(left == 0 && right == 1) {
                isRotating = 1;
                currentState = STATE_TURN_LEFT;
            }

            //white on both
            if (left == 1 && right == 1) {
                currentState = STATE_TURN_RIGHT;
            }

            if(currentState == STATE_TURN_LEFT) {
                leftTurnCycles++;
            }

        } else if(currentTurningBias == TURNING_BIAS_RIGHT) {
            //white on left and back on right
            if(left == 1 && right == 0) {
                if(isRotating == 1) {
                    currentTurningBias = TURNING_BIAS_LEFT;
                    isRotating = 0;
                } else {
                    currentState = STATE_MOVE_FOWARD;
                }
            }

            //black on both
            if (left == 0 && right == 0) {
                if(testedLeft == 1) {
                    currentState = STATE_TURN_LEFT;
                    testedLeft = 0;
                } else {
                    testedLeft = 1;
                }
            } 

            //black on left and white on right, at an intersection
            if(left == 0 && right == 1) {
                isRotating = 1;
                currentState = STATE_TURN_RIGHT;
                uBit.sleep(1000);
            }
            
            //white on both
            if(left == 1 && right == 1) {
                currentState = STATE_TURN_RIGHT;
            }
    
        }

        uBit.sleep(5);
    }
}

void poll_ultrasonic()
{
    while(1) {
        object_detected = read_ultrasonic();

        uBit.sleep(50);
    }
}

void straight_intersection_program(MicroBitEvent event) {
    if(buttonPressed == 0) {
        currentTurningBias = TURNING_BIAS_NONE;
        buttonPressed = 1;
    }
}

void turn_intersection_program(MicroBitEvent event) {
    if(buttonPressed == 0) {
        currentTurningBias = TURNING_BIAS_LEFT;
        buttonPressed = 1;
    }
}

int main()
{   

    on_start(&uBit);

    //uBit.messageBus.listen(MICROBIT_ID_BUTTON_AB,MICROBIT_BUTTON_EVT_CLICK,);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A,MICROBIT_BUTTON_EVT_CLICK,straight_intersection_program);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B,MICROBIT_BUTTON_EVT_CLICK,turn_intersection_program);

    while(buttonPressed == 0) {
        uBit.display.image.setPixelValue(1,1,255);
        uBit.sleep(50);
        uBit.display.image.clear();
        uBit.sleep(50);
    }

    if(currentTurningBias == TURNING_BIAS_NONE) {
        create_fiber(manage_direction_straight);
    } else {
        create_fiber(manage_direction);
    }

    create_fiber(run_left_motor);
    create_fiber(run_right_motor);
    create_fiber(run_led_lights);
    create_fiber(poll_ultrasonic);

    release_fiber();
}
