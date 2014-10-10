#define PIN 4 // ATAD port of FS1000A

int timing[] = {224, 9192, 896, 336, 896, 224, 896, 336, 896, 224, 336, 896, 896, 224, 904, 336, 792, 336, 336, 896, 224, 896, 336, 896, 784, 336, 896, 336, 224, 896, 896, 336, 784, 336, 336, 896, 224, 896, 336, 896, 792, 336, 344, 784, 336, 896, 896, 224, 336, 896};

void setup() {
 pinMode(PIN, OUTPUT);
}

void loop() {
  for(int i=0;i<25;i++){
    digitalWrite(PIN, HIGH);
    delayMicroseconds(timing[i*2]);
    digitalWrite(PIN, LOW);
    delayMicroseconds(timing[i*2+1]);
  }
}