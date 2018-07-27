#define motorA_1    9                              //red red-red
#define motorA_2    10                             //black black-black
#define motorB_1    5                              //red red-red
#define motorB_2    6                              //black black-black
#define N_SENS      4
#define SPEED       1000
#define CAL_TIME    2000
const int sensor[N_SENS] = { 14, 15, 16, 17 };     // left-most left right right-most
int sens_max[N_SENS];                              //Maximum value each sensor measures (calibrated)
int sens_min[N_SENS];                          //Minimum value each sensor measures (calibrated)
const float KP = 0.4;
const float KD = 12;
const float KI = 0;
float line_pos = 0;
float last_line_pos = 0;

void calibrate(int cal_time);
void motorSpeed(int m1, int m2);
float get_PID_correction(float line, float last_line, float kp, float kd, float ki);

void Initialize()
{
	pinMode(motorA_1, OUTPUT);
	pinMode(motorA_2, OUTPUT);
	pinMode(motorB_1, OUTPUT);
	pinMode(motorB_2, OUTPUT);

	for(int i=0;i<N_SENS;i++)
	{
		pinMode(sensor[i], INPUT);
		//sens_max[i] = 0;
		//sens_min[i] = 1023;
	}
	//calibrate(CAL_TIME);
	pinMode(14, INPUT);
}

void calibrate(int cal_time)
{
	int ms = millis();
	while (ms + cal_time>millis())
	{
		int sens_value[N_SENS];
		for (int x = 0; x < N_SENS; x++) {
			sens_value[x] = analogRead(sensor[x]);
			sens_min[x] = (sens_value[x] < sens_min[x]) ? sens_value[x] : sens_min[x];
			sens_max[x] = (sens_value[x] > sens_max[x]) ? sens_value[x] : sens_max[x];
		}
	}
	for(int i=0;i<N_SENS;i++)
	{
		Serial.print("sensor");
		Serial.print(i+1);
		Serial.print("_max: ");
		Serial.print(sens_max[i]);
		Serial.print("    ");
		Serial.print("sensor: ");
		Serial.print(i+1);
		Serial.print("_min: ");
		Serial.print(sens_min[i]);
		Serial.print("    ");
	}
	Serial.println();
}

void motorSpeed(int m1, int m2)
{
	int pwm1 = map(m1, 0, 1000, 0, 255);
	int pwm2 = map(m2, 0, 1000, 0, 255);
	analogWrite(motorA_1, pwm1);
	analogWrite(motorB_1, pwm2);
	digitalWrite(motorA_2, LOW);
	digitalWrite(motorB_2, LOW);
}

float get_line_pos()
{
	float line = 0;
	float sens_scaled[N_SENS];
	float avg_num = 0;
	float avg_den = 0;
	for(int i=0;i<N_SENS;i++)
	{
		sens_scaled[i] = (analogRead(sensor[i]) - sens_min[i]) / (sens_max - sens_min) * 1000;
		avg_num += sens_scaled[i] * i * 1000;
		avg_den += sens_scaled[i];
	}
	line = avg_num / avg_den;
	line = line - 1000 * (N_SENS - 1) / 2;
	line = (line > 1500) ? 1500 : line;
	line = (line < -1500) ? -1500 : line;
	Serial.print("line_pos: ");
	Serial.println(line);

	return line;
}

float get_PID_correction(float line, float last_line, float kp, float kd, float ki) {
	float proportional = line;
	float derivative = line - last_line;
	float integral = line + last_line;
	float correction = (kp * proportional + kd * derivative + ki * integral);

	return correction;
}

void PID_line_follow()
{
	last_line_pos = line_pos;
	line_pos = get_line_pos();
	
	float PID_correction = get_PID_correction(line_pos, last_line_pos, KP, KD, KI);
	float max_correction = SPEED;
	if (PID_correction > 0) {
		PID_correction = (PID_correction > max_correction) ? max_correction : PID_correction;
		motorSpeed(SPEED, SPEED - PID_correction);
	}
	else {
		PID_correction = (PID_correction < -max_correction) ? -max_correction : PID_correction;
		motorSpeed(SPEED + PID_correction, SPEED);
	}
}

void line_follow()
{
	int sens_val[N_SENS];
	for(int i=0;i<N_SENS;i++)
	{
		sens_val[i] = digitalRead(sensor[i]);
		Serial.print("sensor");
		Serial.print(i + 1);
		Serial.print(": ");
		Serial.print(sens_val[i]);
		Serial.print("    ");
	}
	Serial.println();

	if (sens_val[0]==0&&sens_val[1]==0&&sens_val[2]==0&&sens_val[3]==0)
	{
		motorSpeed(0, 0);
	}
	// one black
	else if (sens_val[0] == 1 && sens_val[1] == 0 && sens_val[2] == 0 && sens_val[3] == 0)
	{
		motorSpeed(0, SPEED);
	}
	else if (sens_val[0] == 0 && sens_val[1] == 1 && sens_val[2] == 0 && sens_val[3] == 0)
	{
		motorSpeed(SPEED / 2, SPEED);
	}
	else if (sens_val[0] == 0 && sens_val[1] == 0 && sens_val[2] == 1 && sens_val[3] == 0)
	{
		motorSpeed(SPEED, SPEED / 2);
	}
	else if (sens_val[0] == 0 && sens_val[1] == 0 && sens_val[2] == 0 && sens_val[3] == 1)
	{
		motorSpeed(SPEED, 0);
	}
	//two black
	else if (sens_val[0] == 1 && sens_val[1] == 1 && sens_val[2] == 0 && sens_val[3] == 0)
	{
		motorSpeed(SPEED / 2, SPEED);
	}
	else if (sens_val[0] == 0 && sens_val[1] == 1 && sens_val[2] == 1 && sens_val[3] == 0)
	{
		motorSpeed(SPEED, SPEED);
	}
	else if (sens_val[0] == 0 && sens_val[1] == 0 && sens_val[2] == 1 && sens_val[3] == 1)
	{
		motorSpeed(SPEED, SPEED / 2);
	}
	//three black
	else if (sens_val[0] == 1 && sens_val[1] == 1 && sens_val[2] == 1 && sens_val[3] == 0)
	{
		motorSpeed(SPEED/3*4, SPEED);
	}
	else if (sens_val[0] == 0 && sens_val[1] == 1 && sens_val[2] == 1 && sens_val[3] == 1)
	{
		motorSpeed(SPEED, SPEED/3*4);
	}
	else
	{
		motorSpeed(SPEED, SPEED);
	}
}

void setup() {
	// put your setup code here, to run once:
	Serial.begin(9600);
	Initialize();
}

void loop() {
	// put your main code here, to run repeatedly:
	//lineFollow();
	//int line = get_line_pos();
	int sensor_value = analogRead(14);
	Serial.println(sensor_value);
	//line_follow();
	//motorSpeed(SPEED, SPEED);
}

