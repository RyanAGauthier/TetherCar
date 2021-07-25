const int xInput = A1;
const int yInput = A2;
const int zInput = A3;
const int sampleSize = 10;

void setup() 
{
  Serial.begin(9600);
  while (!Serial) delay(1);
  Serial.println("Serial connected!"); 
}

void loop() 
{
  int xRaw = ReadAxis(xInput);
  int yRaw = ReadAxis(yInput);
  int zRaw = ReadAxis(zInput);
  Serial.print("Raw output X: ");
  Serial.println(xRaw);
  Serial.print("Raw output Y: ");
  Serial.println(yRaw);
  Serial.print("Raw output Z: ");
  Serial.println(zRaw);
  delay(500);
}

//
// Read "sampleSize" samples and report the average
//
int ReadAxis(int axisPin)
{
  long reading = 0;
  analogRead(axisPin);
  delay(1);
  for (int i = 0; i < sampleSize; i++)
  {
    reading += analogRead(axisPin);
  }
  return reading/sampleSize;
}

//
// Find the extreme raw readings from each axis
//
