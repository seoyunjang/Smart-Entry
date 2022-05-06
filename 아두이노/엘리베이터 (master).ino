

#define         MOT_DIR           4
#define         MOT_PWM           5

#define         SENSOR_1          A0
#define         SENSOR_2          A1
#define         SENSOR_3          A2

#include <Wire.h>
#include <SoftwareSerial.h>
#include <Adafruit_MLX90614.h>

SoftwareSerial ble(2, 3);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

int SLAVE[2] = {0, 1}; 

unsigned long prev_time;

void setup() {     
  Serial.begin(9600);        ble.begin(9600);
  //mlx.begin();
  Wire.begin();
  pinMode(MOT_DIR, OUTPUT);  pinMode(MOT_PWM, OUTPUT);
  pinMode(SENSOR_1, INPUT);  pinMode(SENSOR_2, INPUT);  pinMode(SENSOR_3, INPUT);
  Serial.println("wow");
  move_ele(1);
}

void loop() {

  // 1초마다 온도값 읽기, 센서값읽기, 슬레이브에서 데이터요청
  if( (millis()-prev_time) > 1000){
    //read_mlx();
    Serial.println("prev_time_start");
    Serial.print("1 :: ");
    Serial.println(digitalRead(SENSOR_1));
    Serial.print("2 :: ");
    Serial.println(digitalRead(SENSOR_2));
    Serial.print("3 :: ");
    Serial.println(digitalRead(SENSOR_3));    
    Wire.requestFrom(SLAVE[0], 1);
    req_slave();
    Wire.requestFrom(SLAVE[1], 1);
    req_slave();
    Serial.println("prev_time_end");
    prev_time = millis();
  }
  
  read_ble();
}

// 온도 읽기
void read_mlx(){
  
  Serial.print("Ambient = ");       Serial.print(mlx.readAmbientTempC()); 
  Serial.print("*C\tObject = ");    Serial.print(mlx.readObjectTempC()); Serial.println("*C");
  Serial.print("Ambient = ");       Serial.print(mlx.readAmbientTempF()); 
  Serial.print("*F\tObject = ");    Serial.print(mlx.readObjectTempF()); Serial.println("*F");
  
  if(mlx.readObjectTempC()>50)send_slave(0,'c');
  else if(mlx.readObjectTempC()<50)send_slave(0,'d');
}

// 슬레이브에서 데이터읽기
void req_slave(){
  //Serial.println("req_slave_start");
  //Wire.requestFrom(SLAVE[i], 1);   
  if(Wire.available()){
    char data = Wire.read();
    Serial.print("req_data:: ");
    Serial.println(data);
    if(data == 'a')move_ele(1);
    else if(data == 'b')move_ele(3);     
  }  
  //Serial.println("req_slave_end");
}

// 슬레이브로 데이터 보내기
void send_slave(int slave, char data){
  Wire.beginTransmission(SLAVE[slave]);      
  Wire.write(data);  
  Wire.endTransmission(SLAVE[slave]);
}

// 어플에서 데이터 읽기
void read_ble(){
  if(ble.available()){
    char data = ble.read();
    Serial.println(data);
    if(data=='a'){          // 1층
      move_ele(1);
    }else if(data=='b'){    // 2층
      move_ele(2);
    }else if(data=='c'){    // 3층
      move_ele(3);
    }else if(data=='d'){    // 1층문개폐
      send_slave(0, 'a');
    }else if(data=='e'){    // 3층문개폐
      send_slave(1, 'a');
    }
  }
}

// 엘레베이터 제어
void move_ele(int num){   // MOT_DIR : HIGH 밑으로   MOT_DIR: LOW 위로
  Serial.println("move");
  if(num == 1){
    while(digitalRead(SENSOR_1)==LOW){
      digitalWrite(MOT_DIR, HIGH);      digitalWrite(MOT_PWM, HIGH);
    }
    digitalWrite(MOT_PWM, LOW);
  }else if(num == 2){
    if(digitalRead(SENSOR_1)==HIGH){
      while(digitalRead(SENSOR_2)==LOW){
        digitalWrite(MOT_DIR, LOW);      digitalWrite(MOT_PWM, HIGH);
      }  
      digitalWrite(MOT_PWM, LOW);
    }else if(digitalRead(SENSOR_3)==HIGH){
      while(digitalRead(SENSOR_2)==LOW){
        digitalWrite(MOT_DIR, HIGH);      digitalWrite(MOT_PWM, HIGH);
      }  
      digitalWrite(MOT_PWM, LOW);
    }
  }else if(num == 3){
    while(digitalRead(SENSOR_3)==LOW){
      digitalWrite(MOT_DIR, LOW);      digitalWrite(MOT_PWM, HIGH);
    }
    digitalWrite(MOT_PWM, LOW);
  }
}
