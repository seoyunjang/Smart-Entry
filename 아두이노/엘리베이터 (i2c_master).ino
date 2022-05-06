


#include <Wire.h>

// 슬레이브주소
int SLAVE[4] = {1, 2, 3, 4}; 

void setup() {  
  Wire.begin();   
  Serial.begin(9600); 
}

void loop() {

  // 시리얼데이터로 i2c데이터 보냄
  if (Serial.available()) {
    char e = Serial.read();
    byte c = e - 48;
    if (c < 5) {      
      Wire.beginTransmission(SLAVE[c-1]);      
      Wire.write('o');
      Wire.write('\n');
      Wire.endTransmission(SLAVE[c-1]);
      
      delay(10);      
      i2c_communication(c-1); 
      delay(10);
    }
  }
}

void i2c_communication(int i) {
  // 마스터에서 슬레이브로 데이터요청  
  Wire.requestFrom(SLAVE[i], 12);   
  for (int j = 0 ; j < 12 ; j++) {      
    char c = Wire.read();   
    // 데이터 읽어와서 시리얼모니터에 띄어줌  
    Serial.print(c); 
  }
  Serial.println();
}
