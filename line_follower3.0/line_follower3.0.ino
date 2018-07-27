const int SI = 12;
const int CLK = 13;
const int exp_time = 1;

int pixel[128];
int pixel_max = 0;
int pixel_min = 255;

void updatePixel();
int findMax();
int findMin();
int findAverage();

void setup()
{
 
  /* add setup code here */
	Serial.begin(9600);
	pinMode(SI, OUTPUT);
	pinMode(CLK, OUTPUT);

	for (int i = 0; i < 128; i++)
		pixel[i] = 0;

	digitalWrite(SI, HIGH);
	digitalWrite(CLK, HIGH);
	digitalWrite(SI, LOW);   
	digitalWrite(CLK, LOW);     
						
    // This clocks out indeterminate pixel data from power up.
	for (int i = 0; i < 128; i++) {             
		digitalWrite(CLK, HIGH);                                                 
		digitalWrite(CLK, LOW);     
	}
}

void loop()
{

  /* add main program code here */
	updatePixel();
}

void updatePixel()
{
	// Starts pixel count.
	digitalWrite(SI, HIGH);
	digitalWrite(CLK, HIGH);
	digitalWrite(SI, LOW);
	digitalWrite(CLK, LOW);

	/*for(int i=0;i<128;i++)
	{
		digitalWrite(CLK, HIGH);
		digitalWrite(CLK, LOW);
	}

	delayMicroseconds(exp_time);
	digitalWrite(SI, HIGH);
	digitalWrite(CLK, HIGH);
	digitalWrite(SI, LOW);
	pixel[0] = analogRead(A0);
	if (pixel[0] == 255) pixel[0] = 254;
	digitalWrite(CLK, LOW);*/
	for (int i = 0; i<128; i++)
	{
		delayMicroseconds(exp_time);

		pixel[i] = analogRead(A0);

		digitalWrite(CLK, HIGH);
		digitalWrite(CLK, LOW);
	}
	//digitalWrite(CLK, HIGH);
	//digitalWrite(CLK, LOW);

	// Send data to computer and process.
	for(int i=0;i<128;i++)
	{
		Serial.print(pixel[i]);
		Serial.print(",");
	}
	Serial.println();
}

int findMax()
{
	for(int i=0;i<128;i++)
	{
		if (pixel_max <= pixel[i])
			pixel_max = pixel[i];
	}
	return pixel_max;
}

int findMin()
{
	for (int i = 0; i<128; i++)
	{
		if (pixel_min >= pixel[i])
			pixel_min = pixel[i];
	}
	return pixel_min;
}

int findAverage()
{
	return (pixel_max + pixel_min) / 2;
}
