#include <Servo.h>  // Include Servo library

// Define servo and servo pin
#define servo_PIN 9
Servo myservo;

#define TRIG_PIN 12
#define ECHO_PIN 13

// PWM control pin
#define PWM1_PIN            5
#define PWM2_PIN            6      
// 74HCT595N chip pin
#define SHCP_PIN            2                               // The displacement of the clock
#define EN_PIN              7                               // Can make control
#define DATA_PIN            8                               // Serial data
#define STCP_PIN            4                               // Memory register clock          

#define LEFT_LINE_TRACKING          A0
#define CENTER_LINE_TRACKING        A1
#define RIGHT_LINE_TRACKING         A2

const int Forward       = 163;                             // forward
const int Backward      = 92;                              // back
const int Turn_Left     = 106;                             // left translation
const int Turn_Right    = 149;                             // Right translation 
const int Top_Left      = 129;                             // Upper left mobile
const int Bottom_Left   = 20;                              // Lower left mobile
const int Top_Right     = 34;                              // Upper right mobile
const int Bottom_Right  = 72;                              // The lower right move
const int Stop          = 0;                               // stop
const int Contrarotate  = 83;                              // Counterclockwise rotation
const int Clockwise     = 172;                             // Rotate clockwise

// Servo positions
const int right = 0;
const int middle = 90;
const int left = 180;

bool hasStopped = false;

int Left_Tra_Value;
int Center_Tra_Value;
int Right_Tra_Value;
int Black_Line = 500;

// Function to check distance using the ultrasonic sensor
float checkDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    unsigned long duration = pulseIn(ECHO_PIN, HIGH);
    float distance = (duration / 2.0) / 29.1;
    return distance;
}

void setup() {
    // Setup for motor control
    pinMode(SHCP_PIN, OUTPUT);
    pinMode(EN_PIN, OUTPUT);
    pinMode(DATA_PIN, OUTPUT);
    pinMode(STCP_PIN, OUTPUT);
    pinMode(PWM1_PIN, OUTPUT);
    pinMode(PWM2_PIN, OUTPUT);

    // Setup for line tracking sensors
    pinMode(LEFT_LINE_TRACKING, INPUT);
    pinMode(CENTER_LINE_TRACKING, INPUT);
    pinMode(RIGHT_LINE_TRACKING, INPUT);

    // Setup for servo
    Serial.begin(9600);
    myservo.attach(servo_PIN);
    myservo.write(middle);  // Set initial position
    delay(100);

    // Setup for ultrasonic sensor
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
}

int photo = 20000;

bool traseu_start_M() {

    Left_Tra_Value = analogRead(LEFT_LINE_TRACKING);
    Center_Tra_Value = analogRead(CENTER_LINE_TRACKING);
    Right_Tra_Value = analogRead(RIGHT_LINE_TRACKING);

    // Continue moving based on line tracking
    if (!hasStopped) {
        if (Left_Tra_Value < Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Forward, 250);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Contrarotate, 230);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Contrarotate, 250);
        } else if (Left_Tra_Value < Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Clockwise, 250);
        } else if (Left_Tra_Value < Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Clockwise, 230);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Stop, 0);
            hasStopped = true;  // Set flag when stopping condition is met
            return true;  // Indicate transition to next function
        }
        return false;  // Continue running this function
    } else {
        return true;  // Ensure function stops executing further movements after stopping
    }
}

bool traseu_M_E() {
    delay(20000);
    // Move servo to left position to scan
    myservo.write(left);
    delay(500);  // Allow time for servo to move and stabilize

    // Translate to the left or right until the distance to the wall is within 8 to 10 cm
    float distance = checkDistance();
    while (distance > 13 || distance < 8) {
        if (distance > 13) {
            Motor(Turn_Left, 255); // Translate left to reduce distance
        } else if (distance < 8) {
            Motor(Turn_Right, 255); // Translate right to increase distance
        }
        delay(5); // Small delay for movement stability
        distance = checkDistance();
    }
    Motor(Stop, 0); // Stop once the distance is within 8 to 10 cm
    delay(500); // Allow time for robot to stabilize

    // Move forward while maintaining the distance between 8 and 10 cm
    while (true) {
        distance = checkDistance();
        if (distance >= 8 && distance <= 13) {
            Motor(Forward, 255); // Move forward
        } else {
            Motor(Stop, 0); // Stop if the distance goes out of range
            break; // Exit the loop if the distance is out of range
        }
        delay(100); // Small delay for movement stability
    }

    // Move forward until a black line is detected
    Motor(Forward, 255);
    while (true) {
        Left_Tra_Value = analogRead(LEFT_LINE_TRACKING);
        Center_Tra_Value = analogRead(CENTER_LINE_TRACKING);
        Right_Tra_Value = analogRead(RIGHT_LINE_TRACKING);

        if (Left_Tra_Value >= Black_Line || Center_Tra_Value >= Black_Line || Right_Tra_Value >= Black_Line) {
            Motor(Stop, 0); // Stop when a black line is detected
            break; // Exit the loop
        }
        delay(10); // Small delay for sensor readings
    }

    Motor(Turn_Right, 255);
    delay(50);

    // Resume line following
    while (true) {
        Left_Tra_Value = analogRead(LEFT_LINE_TRACKING);
        Center_Tra_Value = analogRead(CENTER_LINE_TRACKING);
        Right_Tra_Value = analogRead(RIGHT_LINE_TRACKING);

        if (Left_Tra_Value < Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Forward, 255);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Contrarotate, 255);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Contrarotate, 255);
        } else if (Left_Tra_Value < Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Clockwise, 255);
        } else if (Left_Tra_Value < Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Clockwise, 255);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Stop, 0); // Stop if all sensors see the black line
            return true;
        }
        delay(10); // Small delay for sensor readings
    }
    return false;
}

bool traseu_E_C() {
  myservo.write(middle);
    delay(photo);
    // Move servo to left position to scan
    myservo.write(left);
    delay(500);  // Allow time for servo to move and stabilize

    // Start by scanning with the ultrasonic and translating to maintain a specific range
    float distance = checkDistance();
    while (distance > 15 || distance < 8) {
        if (distance > 15) {
            Motor(Turn_Left, 255); // Translate left to reduce distance
        } else if (distance < 8) {
            Motor(Turn_Right, 255); // Translate right to increase distance
        }
        delay(10);
        distance = checkDistance();
    }
    Motor(Stop, 0);  // Stop once the distance is within the desired range
    delay(500);

    while (true) {
        distance = checkDistance();
        if (distance >= 6 && distance <= 15) {
            Motor(Backward, 255); // Move forward
        } else {
            Motor(Stop, 0); // Stop if the distance goes out of range
            break; // Exit the loop if the distance is out of range
        }
        delay(100); // Small delay for movement stability
    }

    // Translate left until a black line is detected
    Motor(Turn_Left, 255);
    do {
        Left_Tra_Value = analogRead(LEFT_LINE_TRACKING);
        Center_Tra_Value = analogRead(CENTER_LINE_TRACKING);
        Right_Tra_Value = analogRead(RIGHT_LINE_TRACKING);
        delay(10);  // Small delay for sensor readings
    } while (Left_Tra_Value < Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value < Black_Line);

    Motor(Stop, 0);  // Stop when a black line is detected
    delay(500);

    Motor(Forward, 255);
    delay(500);

    Motor(Stop, 0);  // Stop when a black line is detected
    delay(500);

    // Turn around 180 degrees - implementation depends on your robot's capability
    // This is a placeholder for a 180-degree turn
    Motor(Clockwise, 255);
    delay(400);  // Assuming this turns the robot right
    do {
        Left_Tra_Value = analogRead(LEFT_LINE_TRACKING);
        Center_Tra_Value = analogRead(CENTER_LINE_TRACKING);
        Right_Tra_Value = analogRead(RIGHT_LINE_TRACKING);
        delay(10);  // Small delay for sensor readings
    } while (Left_Tra_Value < Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value < Black_Line);


    Motor(Stop, 0);
    delay(500);
    Motor(Forward, 255);
    delay(25);

    // Resume line following until it stops detecting the line again
    while (true) {
        Left_Tra_Value = analogRead(LEFT_LINE_TRACKING);
        Center_Tra_Value = analogRead(CENTER_LINE_TRACKING);
        Right_Tra_Value = analogRead(RIGHT_LINE_TRACKING);

        if (Left_Tra_Value < Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Forward, 255);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Contrarotate, 255);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Contrarotate, 255);
        } else if (Left_Tra_Value < Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Clockwise, 255);
        } else if (Left_Tra_Value < Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Clockwise, 255);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Stop, 0);  // Stop if all sensors see the black line
            return true;
            break;
        }
        delay(10); // Small delay for sensor readings
    }
    return false;
}

bool traseu_C_A() {
  myservo.write(middle);
    delay(photo);  // Assuming this turns the robot right
    Motor(Clockwise, 255);
    delay(100);
    Motor(Turn_Right, 255);
    delay(750);
    do {
        Left_Tra_Value = analogRead(LEFT_LINE_TRACKING);
        Center_Tra_Value = analogRead(CENTER_LINE_TRACKING);
        Right_Tra_Value = analogRead(RIGHT_LINE_TRACKING);
        delay(10);  // Small delay for sensor readings
    } while (Left_Tra_Value < Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value < Black_Line);


    Motor(Stop, 0);

    // Resume line following until it stops detecting the line again
    while (true) {
        Left_Tra_Value = analogRead(LEFT_LINE_TRACKING);
        Center_Tra_Value = analogRead(CENTER_LINE_TRACKING);
        Right_Tra_Value = analogRead(RIGHT_LINE_TRACKING);

        if (Left_Tra_Value < Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Forward, 255);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Contrarotate, 255);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Contrarotate, 255);
        } else if (Left_Tra_Value < Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Clockwise, 255);
        } else if (Left_Tra_Value < Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Clockwise, 255);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Stop, 0);  // Stop if all sensors see the black line
            return true;
            break;
        }
        delay(10); // Small delay for sensor readings
    }
    return false;
}

bool traseu_A_T(){
  myservo.write(middle);
    delay(photo);
    myservo.write(right);

    Motor(Backward, 255);
    delay(1700);
    Motor(Stop, 0);
    delay(500);
    Motor(Contrarotate, 255);
    delay(30);
    Motor(Turn_Right, 255);
    delay(2300);
    
    Motor(Clockwise, 255);
    do {
        Left_Tra_Value = analogRead(LEFT_LINE_TRACKING);
        Center_Tra_Value = analogRead(CENTER_LINE_TRACKING);
        Right_Tra_Value = analogRead(RIGHT_LINE_TRACKING);
        delay(20);  // Small delay for sensor readings
    } while (Left_Tra_Value < Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value < Black_Line);
    Motor(Stop, 0);
    myservo.write(middle);
    delay(500);
    float distance;
    while (true) {
        Left_Tra_Value = analogRead(LEFT_LINE_TRACKING);
        Center_Tra_Value = analogRead(CENTER_LINE_TRACKING);
        Right_Tra_Value = analogRead(RIGHT_LINE_TRACKING);
        distance = checkDistance();

        if (Left_Tra_Value < Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Forward, 255);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Contrarotate, 255);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Contrarotate, 255);
        } else if (Left_Tra_Value < Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Clockwise, 255);
        } else if (Left_Tra_Value < Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Clockwise, 255);
        }
        else if(distance < 5){
          Motor(Stop, 0);
          break;
        }
        delay(10); // Small delay for sensor readings
    }

    Motor(Backward, 255);  // Stop once the distance is within the desired range
    delay(150);

    Motor(Turn_Left, 255);
    delay(1250);
    Motor(Contrarotate, 255);
    delay(500);

    Motor(Stop, 0);  // Stop once the distance is within the desired range
    delay(500);

    Motor(Turn_Right, 255);
    do {
        Left_Tra_Value = analogRead(LEFT_LINE_TRACKING);
        Center_Tra_Value = analogRead(CENTER_LINE_TRACKING);
        Right_Tra_Value = analogRead(RIGHT_LINE_TRACKING);
        delay(10);  // Small delay for sensor readings
    } while (Left_Tra_Value < Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value < Black_Line);

    Motor(Stop, 0);
    delay(400);

    Motor(Forward, 255);
    delay(50);
    Motor(Stop, 0);
    delay(400);
    while (true) {
        Left_Tra_Value = analogRead(LEFT_LINE_TRACKING);
        Center_Tra_Value = analogRead(CENTER_LINE_TRACKING);
        Right_Tra_Value = analogRead(RIGHT_LINE_TRACKING);

        if (Left_Tra_Value < Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Forward, 200);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Contrarotate, 255);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Contrarotate, 255);
        } else if (Left_Tra_Value < Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Clockwise, 255);
        } else if (Left_Tra_Value < Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Clockwise, 255);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Stop, 0);  // Stop if all sensors see the black line
            return true;
            break;
        }
        delay(5); // Small delay for sensor readings
    }

    return false;

}

bool traseu_T_R(){
  myservo.write(middle);
    delay(photo);
    Motor(Contrarotate, 255);
    delay(250);
    Motor(Forward, 255);
    delay(250);
    do {
        Left_Tra_Value = analogRead(LEFT_LINE_TRACKING);
        Center_Tra_Value = analogRead(CENTER_LINE_TRACKING);
        Right_Tra_Value = analogRead(RIGHT_LINE_TRACKING);
        delay(10);  // Small delay for sensor readings
    } while (Left_Tra_Value < Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value < Black_Line);

    Motor(Stop, 0);

    while (true) {
        Left_Tra_Value = analogRead(LEFT_LINE_TRACKING);
        Center_Tra_Value = analogRead(CENTER_LINE_TRACKING);
        Right_Tra_Value = analogRead(RIGHT_LINE_TRACKING);

        if (Left_Tra_Value < Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Forward, 255);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Contrarotate, 255);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Contrarotate, 255);
        } else if (Left_Tra_Value < Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Clockwise, 255);
        } else if (Left_Tra_Value < Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Clockwise, 255);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Stop, 0);  // Stop if all sensors see the black line
            return true;
            break;
        }
        delay(10); // Small delay for sensor readings
    }

    return false;
}

bool traseu_R_O(){
  myservo.write(middle);
    delay(photo);
    Motor(Clockwise, 255);
    delay(200);
    Motor(Top_Left, 255);
    delay(300);
    do {
        Left_Tra_Value = analogRead(LEFT_LINE_TRACKING);
        Center_Tra_Value = analogRead(CENTER_LINE_TRACKING);
        Right_Tra_Value = analogRead(RIGHT_LINE_TRACKING);
        delay(10);  // Small delay for sensor readings
    } while (Left_Tra_Value < Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value < Black_Line);

    Motor(Stop, 0);

    while (true) {
        Left_Tra_Value = analogRead(LEFT_LINE_TRACKING);
        Center_Tra_Value = analogRead(CENTER_LINE_TRACKING);
        Right_Tra_Value = analogRead(RIGHT_LINE_TRACKING);

        if (Left_Tra_Value < Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Forward, 255);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Contrarotate, 255);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Contrarotate, 255);
        } else if (Left_Tra_Value < Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Clockwise, 255);
        } else if (Left_Tra_Value < Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Clockwise, 255);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Stop, 0);  // Stop if all sensors see the black line
            return true;
            break;
        }
        delay(10); // Small delay for sensor readings
    }

    return false;
}

bool traseu_O_N(){
  myservo.write(middle);
    delay(photo);
    myservo.write(left);
    delay(500);

    float distance = checkDistance();
    while (distance < 30) {
        
        Motor(Backward, 255); // Translate right to increase distance
        delay(10);
        distance = checkDistance();
    }
    Motor(Stop, 0);  // Stop once the distance is within the desired range
    delay(500);

    Motor(Turn_Left, 255);
    delay(500);
    do {
        Left_Tra_Value = analogRead(LEFT_LINE_TRACKING);
        Center_Tra_Value = analogRead(CENTER_LINE_TRACKING);
        Right_Tra_Value = analogRead(RIGHT_LINE_TRACKING);
        delay(10);  // Small delay for sensor readings
    } while (Left_Tra_Value < Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value < Black_Line);

    Motor(Stop, 0);
    delay(300);
    Motor(Forward, 255);
    delay(150);

    while (true) {
        Left_Tra_Value = analogRead(LEFT_LINE_TRACKING);
        Center_Tra_Value = analogRead(CENTER_LINE_TRACKING);
        Right_Tra_Value = analogRead(RIGHT_LINE_TRACKING);

        if (Left_Tra_Value < Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Forward, 255);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Contrarotate, 255);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Contrarotate, 255);
        } else if (Left_Tra_Value < Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Clockwise, 255);
        } else if (Left_Tra_Value < Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Clockwise, 255);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Stop, 0);  // Stop if all sensors see the black line
            return true;
            break;
        }
        delay(10); // Small delay for sensor readings
    }

    return false;
}

bool traseu_N_I(){
  myservo.write(middle);
    delay(photo);
    Motor(Turn_Right, 255);
    delay(400);

    do {
        Left_Tra_Value = analogRead(LEFT_LINE_TRACKING);
        Center_Tra_Value = analogRead(CENTER_LINE_TRACKING);
        Right_Tra_Value = analogRead(RIGHT_LINE_TRACKING);
        delay(10);  // Small delay for sensor readings
    } while (Left_Tra_Value < Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value < Black_Line);

    Motor(Stop, 0);
    delay(300);
    Motor(Forward, 255);
    delay(50);

    while (true) {
        Left_Tra_Value = analogRead(LEFT_LINE_TRACKING);
        Center_Tra_Value = analogRead(CENTER_LINE_TRACKING);
        Right_Tra_Value = analogRead(RIGHT_LINE_TRACKING);

        if (Left_Tra_Value < Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Forward, 255);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Contrarotate, 255);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Contrarotate, 255);
        } else if (Left_Tra_Value < Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Clockwise, 255);
        } else if (Left_Tra_Value < Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Clockwise, 255);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Stop, 0);  // Stop if all sensors see the black line
            return true;
            break;
        }
        delay(10); // Small delay for sensor readings
    }

    return false;
}

bool traseu_I_C(){
  myservo.write(middle);
    delay(photo);
    Motor(Clockwise, 255);
    delay(200);
    Motor(Forward, 255);
    delay(500);
    do {
        Left_Tra_Value = analogRead(LEFT_LINE_TRACKING);
        Center_Tra_Value = analogRead(CENTER_LINE_TRACKING);
        Right_Tra_Value = analogRead(RIGHT_LINE_TRACKING);
        delay(10);  // Small delay for sensor readings
    } while (Left_Tra_Value < Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value < Black_Line);

    Motor(Contrarotate, 255);
    delay(350);
    Motor(Forward, 255);
    do {
        Left_Tra_Value = analogRead(LEFT_LINE_TRACKING);
        Center_Tra_Value = analogRead(CENTER_LINE_TRACKING);
        Right_Tra_Value = analogRead(RIGHT_LINE_TRACKING);
        delay(10);  // Small delay for sensor readings
    } while (Left_Tra_Value < Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value < Black_Line);

    Motor(Stop, 0);
    delay(300);

    while (true) {
        Left_Tra_Value = analogRead(LEFT_LINE_TRACKING);
        Center_Tra_Value = analogRead(CENTER_LINE_TRACKING);
        Right_Tra_Value = analogRead(RIGHT_LINE_TRACKING);

        if (Left_Tra_Value < Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Forward, 255);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Contrarotate, 255);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value < Black_Line) {
            Motor(Contrarotate, 255);
        } else if (Left_Tra_Value < Black_Line && Center_Tra_Value < Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Clockwise, 255);
        } else if (Left_Tra_Value < Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Clockwise, 255);
        } else if (Left_Tra_Value >= Black_Line && Center_Tra_Value >= Black_Line && Right_Tra_Value >= Black_Line) {
            Motor(Stop, 0);  // Stop if all sensors see the black line
            return true;
            break;
        }
        delay(10); // Small delay for sensor readings
    }

    return false;
}

bool traseu_C_A2(){
    myservo.write(middle);
    delay(photo);

}

enum RobotState {
    LINE_FOLLOWING,
    EXECUTE_TASK_M,
    EXECUTE_TASK_E,
    EXECUTE_TASK_C,
    EXECUTE_TASK_A,
    EXECUTE_TASK_T,
    EXECUTE_TASK_R,
    EXECUTE_TASK_O,
    EXECUTE_TASK_N,
    EXECUTE_TASK_I,
    EXECUTE_TASK_C2,
    IDLE
};

RobotState currentState = LINE_FOLLOWING;

void loop() {
    switch (currentState) {
        case LINE_FOLLOWING:
            if (traseu_start_M()) { // If traseu_start_M returns true, it's time to switch
                currentState = EXECUTE_TASK_M;
            }
            break;
        case EXECUTE_TASK_M:
            if (traseu_M_E()) {  // Execute and check if it's done
                currentState = EXECUTE_TASK_E;
            }
            break;
        case EXECUTE_TASK_E:
            traseu_E_C();  // Run the next sequence of operations
            currentState = EXECUTE_TASK_C;  // Assume traseu_E_C completes and goes to IDLE, adjust as necessary
            break;
        case EXECUTE_TASK_C:
            traseu_C_A();  // Run the next sequence of operations
            currentState = EXECUTE_TASK_A;  // Assume traseu_E_C completes and goes to IDLE, adjust as necessary
            break;
        case EXECUTE_TASK_A:
            traseu_A_T();  // Run the next sequence of operations
            currentState = EXECUTE_TASK_T;  // Assume traseu_E_C completes and goes to IDLE, adjust as necessary
            break;
        case EXECUTE_TASK_T:
            traseu_T_R();  // Run the next sequence of operations
            currentState = EXECUTE_TASK_R;  // Assume traseu_E_C completes and goes to IDLE, adjust as necessary
            break;
        case EXECUTE_TASK_R:
            traseu_R_O();  // Run the next sequence of operations
            currentState = EXECUTE_TASK_O;  // Assume traseu_E_C completes and goes to IDLE, adjust as necessary
            break;
        case EXECUTE_TASK_O:
            traseu_O_N();  // Run the next sequence of operations
            currentState = EXECUTE_TASK_N;  // Assume traseu_E_C completes and goes to IDLE, adjust as necessary
            break;
        case EXECUTE_TASK_N:
            traseu_N_I();  // Run the next sequence of operations
            currentState = EXECUTE_TASK_I;  // Assume traseu_E_C completes and goes to IDLE, adjust as necessary
            break;
        case EXECUTE_TASK_I:
            traseu_I_C();  // Run the next sequence of operations
            currentState = EXECUTE_TASK_C;  // Assume traseu_E_C completes and goes to IDLE, adjust as necessary
            break;
        case EXECUTE_TASK_C2:
            traseu_C_A2();  // Run the next sequence of operations
            currentState = IDLE;  // Assume traseu_E_C completes and goes to IDLE, adjust as necessary
            break;
        case IDLE:
            // Handle idle state, perhaps waiting for a reset or shutdown
            break;
    }
}


void Motor(int Dir, int Speed) {
    digitalWrite(EN_PIN, LOW);
    analogWrite(PWM1_PIN, Speed);
    analogWrite(PWM2_PIN, Speed);

    digitalWrite(STCP_PIN, LOW);
    shiftOut(DATA_PIN, SHCP_PIN, MSBFIRST, Dir);
    digitalWrite(STCP_PIN, HIGH);
}
