#define frequency
int hallcounts = 0;
int counter = 0;
bool hotstreak = false;
int reading;
void setup() {
  pinMode(5, INPUT);// put your setup code here, to run once:
  Serial.begin(115200);
  if (!Serial)delay(1);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(counter < 60241){
    reading = digitalRead(5);
    Serial.println("reading taken");
    if(reading == LOW && !hotstreak){
      hallcounts+= 1;
      hotstreak = !hotstreak; 
      Serial.println("Hall pulled low!");
    } else if(reading == HIGH && hotstreak){
      Serial.println("Hall pulled high!");
      hotstreak = !hotstreak;
    }
    counter +=1;
    delayMicroseconds(166);  
  } else {
    Serial.println("We're done");
    Serial.print("Hall counts: ");
    Serial.print(hallcounts);
    delay(100000000);
  }
  
}
