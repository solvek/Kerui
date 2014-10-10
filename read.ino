#define KERUI_BITS 24
#define KERUI_MAX_SIGNAL_LENGTH 255
#define KERUI_UPPER_THRESHOLD 100
#define KERUI_LOWER_THRESHOLD 80

#define KERUI_DURATION_BIT 500
#define KERUI_DURATION_DELIMITER 5000

#define KERUI_RESULT_SUCCESS 0
#define KERUI_RESULT_TIMEOUT_LOW 1
#define KERUI_RESULT_TIMEOUT_HIGH 2
#define KERUI_RESULT_MANY_ERRORS 3
#define KERUI_RESULT_DELIMETER_NOT_FOUND 4

#define KERUI_MAX_ERROR_COUNT 50

#define LED_PIN 13

void setup() {
 Serial.begin(9600);
 pinMode(LED_PIN, OUTPUT);
 
 digitalWrite(LED_PIN, HIGH); 
 delay(2000);
 digitalWrite(LED_PIN, LOW);
}

const int rowSize = 2*(KERUI_BITS+1);
int row[rowSize];
byte bits[KERUI_BITS];

void loop() {
 digitalWrite(LED_PIN, HIGH); 
 Serial.println("Waiting for a result...");
 int result = keruiReadRow(A0, 2, bits, row);
 digitalWrite(LED_PIN, LOW);
 if (result == KERUI_RESULT_SUCCESS){
   Serial.print("Code: ");
   for(int i=0;i<KERUI_BITS; i++){
     Serial.print(bits[i]);
   }
   Serial.println();
   
   Serial.print("Timing: ");
   for(int i=0;i<rowSize; i++){
     if (i>0) Serial.print(", ");
     Serial.print(row[i]);     
   }
   Serial.println();   
 } 
 else {
   Serial.print("Error: ");
   Serial.println(result);
 }
}

/**
Returns true if data was read successfully otherwise false
analogPin - pin number to read from. Usualy it is A0
result - must be size of KERUI_BITS
passes - number of required succesful passes
row - can be null. If provided size should be 2*(KERUI_BITS+1). Each pair is LOW/HIGH time in microsecond, the first pair is the delimiter
**/
int keruiReadRow(int analogPin, int passes, byte result[], int row[]){
  unsigned long time, time2;
  int counter, low, high;
  time = millis();
  while(analogRead(analogPin)<1);
  
  int successes = 0;
  int bitNumber = -1;
  byte bitValue;
  int errorCount = 0;
  int noDelimiterCounter = 0;
  
  while(successes < passes){
    // Read LOW signal
    time = micros();
    counter = 0;
    while(analogRead(analogPin)>KERUI_UPPER_THRESHOLD){
      counter++;
      if (counter > KERUI_MAX_SIGNAL_LENGTH) return KERUI_RESULT_TIMEOUT_LOW;
    }
    time2 = micros();    
    low = time2-time;
    
    // Read HIGH signal
    counter = 0;
    while(analogRead(analogPin)<KERUI_LOWER_THRESHOLD){
      counter++;
      if (counter > KERUI_MAX_SIGNAL_LENGTH) return KERUI_RESULT_TIMEOUT_HIGH;      
    }
    high = micros()-time2;
    
    if (row){
      row[2*(bitNumber+1)] = low;
      row[2*(bitNumber+1)+1] = high;
    }
    
    if (high > KERUI_DURATION_DELIMITER){
      bitNumber = 0;
      noDelimiterCounter = 0;      
    }
    else {
      if (bitNumber < 0){
        if (noDelimiterCounter++ > KERUI_BITS)
          return KERUI_RESULT_DELIMETER_NOT_FOUND;
      }
      else {        
        if (high < KERUI_DURATION_BIT){
          bitValue = 1;
        }
        else {
          bitValue = 0;        
        }
        
        if (successes>0 && bitValue != result[bitNumber]){
          successes = 0;
          if (errorCount++ > KERUI_MAX_ERROR_COUNT) {
            return KERUI_RESULT_MANY_ERRORS;
          }
        }
        
        result[bitNumber] = bitValue;      
        bitNumber++;
        
        if (bitNumber >= KERUI_BITS){
          bitNumber = -1;
          successes++;
        }
      }
    }
  }
  
  return KERUI_RESULT_SUCCESS;
}


