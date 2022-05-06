
#define             SLAVE             0

#define             SERVO_PIN         5
#define             IR_PIN            6

#include <Wire.h>
#include <Servo.h>
#include <SoftwareSerial.h>

Servo servo;
SoftwareSerial ble(3, 2);

unsigned long prev_time, ele_time;
byte recv_data[10];


char check_end[10]={'O','K','+','D','I','S','C','E'};
char check_start[20] = {'O','K','+','D','I','S','I','S','O','K','+','D','I','S','C'};

char ble_data[1023];
int  ble_count = 0;

int rssi = 0;

char req_data = ' ';

bool rssi_flag = false, temp_flag = true, servo_flag = false;
bool master_ele = false, read_flag = false;;
bool wire_flag = false;

void setup() {
  Serial.begin(9600);           ble.begin(9600);
  Serial.println("START_FIRST");
  Wire.begin(SLAVE);
  Wire.onReceive(receive_m);    Wire.onRequest(req_m); 
  
  pinMode(IR_PIN, INPUT);
  
  servo_exe(120);
  delay(500);
  servo_exe(0);

  
}

void loop () {

  // 5초마다 비콘 검색
  if( (millis() - prev_time) > 5000){
    Serial.println("AT5000");  
    for(int i = 0; i<=100; i++)ble_data[i]=' ';
    ble_count = 0;
    ble.print("AT+DISI?"); 
    read_flag = true;     
    ble_count = 0;  
    if(temp_flag && rssi_flag){
      Serial.println("PASS_BEACON");
      if(!servo_flag){    // 열기
        servo_exe(120);        
        req_data = 'a';
        master_ele = true;
        // 엘베 1층으로 부르기      
      }      
    }
    prev_time = millis();
  }

  // 문열려있으면 10초뒤 닫기
  if( ((millis() - ele_time) > 10000) && servo_flag){
    Serial.println("ele_time!");
    if(servo_flag)servo_exe(0);    
    ele_time = millis();
  }
  
  read_ir();
  read_rssi();
  wire_exe();
  
}

// 비콘 읽기
void read_rssi(){
  while( ble.available() ){
    char data = ble.read();  
    Serial.print(data);
    if(read_flag){
      ble_data[ble_count] = data;  
      ble_count++;
    }
    
    if(ble_data[ble_count] == 'E'){      
      check_acc();  
      for(int i = 0; i<1023; i++)ble_data[i]=' ';
      read_flag = false;
      ble_count = 0; 
    }    
  }//Serial.println("");
}

//주변에 비콘있는지 확인하기
void check_acc(){
  int start_count = 0, end_count = 0;
  for(int i = 0; i<1023; i++){
    if(ble_data[i] == '-'){
     rssi =  (ble_data[i+1]-48)*100 + (ble_data[i+2]-48)*10 + (ble_data[i+3]-48);
     Serial.println(rssi);
     Serial.println("complete");
     if(rssi <=60 )rssi_flag = true;
     else if(rssi > 60)rssi_flag = false;      
    }else{
      Serial.println("rssi false");
      rssi_flag = false;
    }          
  } 
}

void read_ir(){
  if( digitalRead(IR_PIN) == LOW){
    if(!servo_flag)servo_exe(120);//열기
    ele_time = millis();
  }
}

// 마스터데이터에따라 서보 여닫기
void wire_exe(){
  if(wire_flag){
    if(!servo_flag)servo_exe(120);        // 열기
    else if(servo_flag)servo_exe(0);      // 닫기
    wire_flag = false;
  }
}

// 마스터에서 보내는 데이터읽기
void receive_m(byte bytes) {  
  while( Wire.available()){
    char data = Wire.read();
    Serial.println(data);
    if(data == 'a'){      // 문개방
      Serial.print("WIRE DATA :: A");
      Serial.println(data);
      wire_flag = true;
    }else if(data == 'c'){    // 온도통과
      Serial.print("WIRE DATA :: C");
      Serial.println(data);
      temp_flag = true;      
    }else if(data == 'd'){    
      Serial.print("WIRE DATA :: D");
      Serial.println(data);
      temp_flag = false;      
    }
  } 
}

//  마스터로 보내기
void req_m() {  
  if(master_ele){
    Wire.write(req_data);
    req_data = ' ';
    master_ele = false;    
  }else if(!master_ele)Wire.write('x');   
  
}

void servo_exe(int degree){
  if(!servo_flag && (degree==120) ){    // 열기
    servo.attach(SERVO_PIN);  servo.write(120);
    delay(500);               servo.detach();
    servo_flag = true;
    ele_time = millis();
  }else if(servo_flag && (degree==0)){ // 닫기
    servo.attach(SERVO_PIN);  servo.write(0);
    delay(500);               servo.detach();
    servo_flag = false;
    ele_time = millis();
  }
}
