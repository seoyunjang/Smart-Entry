#include <Wire.h>

// 슬레이브 자기자신주소
#define SLAVE 4  

byte count = 0; 
char temp;

void setup() {
  Wire.begin(SLAVE);
  // 마스터로부터 데이터수신되었을대 호출될함수
  Wire.onReceive(receiveFromMaster); 
  //마스터로부터 데이터를 요청받았을때 호출될 함수 
  Wire.onRequest(sendToMaster);
  pinMode(13, OUTPUT);
  Serial.begin(9600);
}

void loop () {  
  if (temp == 'o') { // 받은데이터가 o라면 led 깜빡이기
    play();
  }
}

// 데이터 수신되엇을때
void receiveFromMaster(int bytes) {
  char ch[2];
  for (int i = 0 ; i < bytes ; i++) {    
    ch[i] = Wire.read(); 
  }
  temp = ch[0];
}

void play() { 
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  temp = 0;
}

// 데이터 요청받았을때
void sendToMaster() {  
  Wire.write("4th Arduino ");
}
