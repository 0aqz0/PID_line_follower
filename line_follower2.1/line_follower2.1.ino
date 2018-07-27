//提高基准速度 以及调整幅度 


#define motorA_PWM    9                              //red red-red
#define motorA        10                             //black black-black
#define motorB_PWM    5                              //red red-red
#define motorB        6                              //black black-black

int line;
int outline;

const float KP = 25;
const float KD = 2;
const float KI = 0.6;
float error = 0, P = 0, I = 0, D = 0, PID_value = 0;
float previous_error = 0, previous_I = 0;
int sensor[4] = { 0,0,0,0 };
int initial_motor_speed = 200;
bool isClimbing = 0;

const int ss1 = 3;  // sensor 1  
const int ss2 = 4;  // sensor 2   
const int ss3 = 7;  // sensor 3   
const int ss4 = 11; // sensor 4   

void read_sensor_values(void);
void calculate_pid(void);
void motor_control(void);

void setup() {
	// put your setup code here, to run once:
	Serial.begin(9600);
	pinMode(motorA_PWM, OUTPUT);
	pinMode(motorA, OUTPUT);
	pinMode(motorB_PWM, OUTPUT);
	pinMode(motorB, OUTPUT);

	pinMode(ss1, INPUT);
	pinMode(ss2, INPUT);
	pinMode(ss3, INPUT);
	pinMode(ss4, INPUT);
	delay(500);
}

void loop() {
	// put your main code here, to run repeatedly:
	read_sensor_values();
	calculate_pid();
	motor_control();
}

void read_sensor_values()
{
	line = 1;
	outline = 0;
	isClimbing = 0;

	sensor[0] = digitalRead(ss1);
	sensor[1] = digitalRead(ss2);
	sensor[2] = digitalRead(ss3);
	sensor[3] = digitalRead(ss4);
	Serial.print(sensor[0]);
	Serial.print("  ");
	Serial.print(sensor[1]);
	Serial.print("  ");
	Serial.print(sensor[2]);
	Serial.print("  ");
	Serial.print(sensor[3]);
	Serial.print("  ");
	Serial.println();

	if ((sensor[0] == outline) && (sensor[1] == outline) && (sensor[2] == outline) && (sensor[3] == line))
		error = 6;

	else if ((sensor[0] == line) && (sensor[1] == outline) && (sensor[2] == outline) && (sensor[4] == outline))
		error = -6;

	if ((sensor[0] == outline) && (sensor[1] == outline) && (sensor[2] == line) && (sensor[3] == line))
		error = 3;
	else if ((sensor[0] == line) && (sensor[1] == line) && (sensor[2] == outline) && (sensor[3] == outline))
		error = -3;

	/*if ((sensor[0] == outline) && (sensor[1] == outline) && (sensor[2] == outline) && (sensor[3] == line) && (sensor[4] == outline))
	error = 2;
	else if ((sensor[0] == outline) && (sensor[1] == line) && (sensor[2] == outline) && (sensor[3] == outline) && (sensor[4] == outline))
	error = -2;*/

	if ((sensor[0] == outline) && (sensor[1] == line) && (sensor[2] == line) && (sensor[3] == line))
		error = 1;
	else if ((sensor[0] == line) && (sensor[1] == line) && (sensor[2] == line) && (sensor[3] == outline))
		error = -1;

	if ((sensor[1] == line) && (sensor[2] == line))
		error = 0;

	if ((sensor[0] == line) && (sensor[1] == line) && (sensor[2] == line) && (sensor[3] == line))
	{
		isClimbing = 1;
		error = 0, P = 0, I = 0, D = 0, PID_value = 0;
		previous_error = 0, previous_I = 0;
	}

	Serial.println(error);
}

void calculate_pid()
{
	P = error;
	I = I + previous_I;
	D = error - previous_error;

	PID_value = (KP*P) + (KI*I) + (KD*D);

	previous_I = I;
	previous_error = error;
}

void motor_control()
{
	int left_motor_speed;
	int right_motor_speed;

	left_motor_speed = initial_motor_speed + PID_value;
	right_motor_speed = initial_motor_speed - PID_value;
	Serial.print("left: ");
	Serial.print(left_motor_speed);
	Serial.print("   ");
	Serial.print("right: ");
	Serial.println(right_motor_speed);

	if (left_motor_speed < 0)
		left_motor_speed = 0;
	if (right_motor_speed < 0)
		right_motor_speed = 0;
	if (left_motor_speed > 255)
		left_motor_speed = 255;
	if (right_motor_speed > 255)
		right_motor_speed = 255;

	if (isClimbing)
	{
		analogWrite(motorA_PWM, 255);
		analogWrite(motorB_PWM, 255);
	}
	else
	{
		analogWrite(motorA_PWM, left_motor_speed);
		analogWrite(motorB_PWM, right_motor_speed);
	}

	digitalWrite(motorA, LOW);
	digitalWrite(motorB, LOW);
}

void stopMotors() {
	analogWrite(motorA_PWM, 0);
	analogWrite(motorB_PWM, 0);
}