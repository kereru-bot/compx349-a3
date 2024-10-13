#include "MicroBit.h"
#include "drivers.h"
static MicroBit uBit;

#define STATE_MOVE_STOP -1
#define STATE_MOVE_FOWARD 0
#define STATE_TURN_LEFT 1
#define STATE_TURN_RIGHT 2
#define STATE_TURN_CLOCKWISE 3
#define STATE_TURN_ANTICLOCKWISE 4
#define STATE_MOVE_BACKWARD 5

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
            start_motor_left(DIRECTION_FORWARD, 0x31);
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
        case STATE_MOVE_BACKWARD:
            start_motor_left(DIRECTION_BACKWARD, 0x31);
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
            start_motor_right(DIRECTION_FORWARD, 0x27);
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
        case STATE_MOVE_BACKWARD:
            start_motor_right(DIRECTION_BACKWARD, 0x25);
            break;
        case STATE_MOVE_STOP:
            stop_motor_right();
            break;
        }
        uBit.sleep(1);
    }
}

/**
 * Code for following a black line, will try to 
 * adjust itself if it overshoots a line
 */
void follow_line() {
    int8_t isRotating = 0;
    int8_t left = 0;
    int8_t right = 0;
    int8_t testedLeft = 0;

    while(1) {
        left = read_greyscale_sensor_left();
        right = read_greyscale_sensor_right();

        //both seeing black
        if(left == 0 && right == 0) {
            if(isRotating == 0) {
                if(testedLeft == 1) {
                    //must just need an adjustment on the line
                    currentState = STATE_TURN_LEFT;
                    uBit.sleep(250);
                    testedLeft = 0;
                } else {
                    //test to see if it's on a black line or if it's at a corner
                    currentState = STATE_MOVE_FOWARD;
                    uBit.sleep(550);
                    testedLeft = 1;
                    
                    //a corner has been reached, readjust to get back onto the line
                    if(read_greyscale_sensor_left() == 1 && read_greyscale_sensor_right() == 1) {
                        currentState = STATE_MOVE_BACKWARD;
                        uBit.sleep(800);
                        currentState = STATE_TURN_LEFT;
                        isRotating = 1;
                        uBit.sleep(150);
                    }
                }
            } 
        }

        //follow the left side of the line
        if(left == 1 && right == 0) {
            currentState = STATE_MOVE_FOWARD;
            isRotating = 0;
        }
        
        //likely at an intersection
        if(left == 0 && right == 1) {
            if(isRotating == 0) {
                currentState = STATE_TURN_LEFT;
                uBit.sleep(300);
            }
        }

        //both looking at white, rotate by default
        if(left == 1 && right == 1) {
            if(isRotating == 0) {
                currentState = STATE_TURN_RIGHT;
            }
        }

        uBit.sleep(5);
    }
}

/**
 * Manages the direction for the microbit for
 * going straight through intersections
 */
void manage_direction_straight() {
    int8_t isRotating = 0;
    int8_t left = 0;
    int8_t right = 0;
    int8_t testedLeft = 0;
    currentTurningBias = TURNING_BIAS_LEFT;

    while(1) {
        left = read_greyscale_sensor_left();
        right = read_greyscale_sensor_right();

        //both seeing black
        if(left == 0 && right == 0) {
            //might need adjustment on the line
            if(testedLeft == 1) {
                currentState = STATE_TURN_LEFT;
                uBit.sleep(200);
                testedLeft = 0;
            } else {
                //testing to see if it's on a black line or at a corner
                currentState = STATE_MOVE_FOWARD;
                uBit.sleep(500);
                testedLeft = 1;
                
                //is at a corner, needs to readjust
                if(read_greyscale_sensor_left() == 1 && read_greyscale_sensor_right() == 1) {
                    currentState = STATE_MOVE_BACKWARD;
                    isRotating = 1;
                    uBit.sleep(400);
                    if(currentTurningBias == TURNING_BIAS_LEFT) {
                        currentState = STATE_TURN_LEFT;
                        currentTurningBias = TURNING_BIAS_RIGHT;
                        uBit.sleep(200);
                    } else {
                        currentState = STATE_TURN_RIGHT;
                        currentTurningBias = TURNING_BIAS_LEFT;
                        uBit.sleep(200);
                    }
                }
            }
        }

        //go forward on the line
        if(left == 1 && right == 0) {
            currentState = STATE_MOVE_FOWARD;
            isRotating = 0;
        }

        //might be at an intersection
        if(left == 0 && right == 1) {
            if(isRotating == 0) {
                currentState = STATE_MOVE_FOWARD;
                uBit.sleep(300);
            }
        }

        //seeing white, turn right by default
        if(left == 1 && right == 1) {
            if(isRotating == 0) {
                currentState = STATE_TURN_RIGHT;
            }
        }

        uBit.sleep(5);
    }
}

/**
 * Manages the states that dictate which direction the
 * motors will move in for turning at intersections
 */
void manage_direction()
{
    int8_t isRotating = 0;
    int8_t left = 0;
    int8_t right = 0;
    int8_t testedLeft = 0;
    int8_t leftTurnCycles = 0;

    while (1) {
        left = read_greyscale_sensor_left();
        right = read_greyscale_sensor_right();

        //something detected, stop the motor
        if (object_detected == 1) {
            currentState = STATE_MOVE_STOP;
        } else if(currentTurningBias == TURNING_BIAS_LEFT) {

            //white on left and back on right
            if(left == 1 && right == 0) {
                //check if it's done a full intersection turn
                //as it must've only seen black at intersection
                if(leftTurnCycles > 30) {
                    uBit.display.image.setPixelValue(3,3,255);
                    currentTurningBias = TURNING_BIAS_RIGHT;
                    isRotating = 0;
                } else if(isRotating == 1) {
                    //else it might've been rotating, in which case it's finished
                    currentTurningBias = TURNING_BIAS_RIGHT;
                    isRotating = 0;
                } else {
                    currentState = STATE_MOVE_FOWARD;
                }
                leftTurnCycles = 0;
            }

            //black on both, test if it's at an intersection
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

            //white on both, turn right by default
            if (left == 1 && right == 1) {
                currentState = STATE_TURN_RIGHT;
            }

            //increment this to find out if an intersection turn has been made
            if(currentState == STATE_TURN_LEFT) {
                leftTurnCycles++;
            }

        } else if(currentTurningBias == TURNING_BIAS_RIGHT) {
            //white on left and back on right
            if(left == 1 && right == 0) {
                if(isRotating == 1) {
                    //if it was rotating, it's finished now
                    currentTurningBias = TURNING_BIAS_LEFT;
                    isRotating = 0;
                } else {
                    currentState = STATE_MOVE_FOWARD;
                }
            }

            //black on both, test if it's an intersection
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

/**
 * Reads from the ultrasonic sensor to see
 * if an object has been detected
 */
void poll_ultrasonic()
{
    while(1) {
        object_detected = read_ultrasonic();

        uBit.sleep(50);
    }
}

/**
 * Used to choose to run the following line program 
 */
void follow_line_program(MicroBitEvent event) {
    if(buttonPressed == 0) {
        buttonPressed = 1;
    }
}

/**
 * Used to choose to run the straight intersection program
 */
void straight_intersection_program(MicroBitEvent event) {
    if(buttonPressed == 0) {
        currentTurningBias = TURNING_BIAS_NONE;
        buttonPressed = 1;
    }
}

/**
 * Used to choose to run the turn at intersection program
 */
void turn_intersection_program(MicroBitEvent event) {
    if(buttonPressed == 0) {
        currentTurningBias = TURNING_BIAS_LEFT;
        buttonPressed = 1;
    }
}

int main()
{   
    //initialise microbit
    on_start(&uBit);

    //set up event handlers to choose program to run
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A,MICROBIT_BUTTON_EVT_CLICK,straight_intersection_program);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B,MICROBIT_BUTTON_EVT_CLICK,turn_intersection_program);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_AB,MICROBIT_BUTTON_EVT_CLICK,follow_line_program);

    //needs something to do while waiting or compiler will get rid of the loop :(
    while(buttonPressed == 0) {
        uBit.display.image.setPixelValue(1,1,255);
        uBit.sleep(50);
        uBit.display.image.clear();
        uBit.sleep(50);
    }

    //determines which program to run
    if(currentTurningBias == TURNING_BIAS_NONE) {
        create_fiber(manage_direction_straight);
        uBit.display.image.setPixelValue(0,4,255);
    } else if(currentTurningBias == TURNING_BIAS_LEFT) {
        create_fiber(manage_direction);
        uBit.display.image.setPixelValue(1,4,255);
    } else {
        create_fiber(follow_line);
        uBit.display.image.setPixelValue(2,4,255);
    }

    //run fibers for different functions
    create_fiber(run_left_motor);
    create_fiber(run_right_motor);
    create_fiber(run_led_lights);
    create_fiber(poll_ultrasonic);

    release_fiber();
}
