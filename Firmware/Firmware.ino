#define SetBit(result, bit_num) result |= (1<<bit_num)
#define ClearBit(result, bit_num) (result &= (~(1<<bit_num)))

#define FB_BS 2 //First Barrel Bottom Switch
#define FB_TS 3 //First Barrel Top Switch
#define SB_BS 4 //Second ..
#define SB_TS 5

#define FB_IN 8
#define FB_OUT 9
#define SB_IN 10
#define SB_OUT 11

int ssCode = 0; //system state code

struct Barrel {
  bool wasting;
  char name[15];
  byte bottomSwitch;
  byte topSwitch;
  byte inValve;
  byte outValve;
} barrel_one, barrel_two;


void setup() {
  Serial.begin(9600);

  init(&barrel_one);
  
  
  pinMode(FB_BS, INPUT_PULLUP);
  pinMode(FB_TS, INPUT_PULLUP);
  pinMode(SB_BS, INPUT_PULLUP);
  pinMode(SB_TS, INPUT_PULLUP);

  pinMode(FB_IN, OUTPUT);
  pinMode(FB_OUT, OUTPUT);
  pinMode(SB_IN, OUTPUT);
  pinMode(SB_OUT, OUTPUT);
  digitalWrite(FB_IN, HIGH);
  digitalWrite(FB_OUT, HIGH);
  digitalWrite(SB_IN, HIGH);
  digitalWrite(SB_OUT, HIGH);
}

void init(Barrel *barrel){
  barrel->wasting = true;
}

void loop() {
    getSystemState();
    switch(ssCode) {
        case 0:
            fillFirstBarrel();
            fillSecondBarrel();
            //activate a barrel
            break;
        case 3:
            switchToSecondBarrel();
            break;
        case 12:
            switchToFirstBarrel();
            break;
    }
    delay(1000);
    Serial.println(ssCode);
}

void getSystemState(){
    digitalRead(FB_BS) == 1 ? SetBit(ssCode, 3) : ClearBit(ssCode, 3);
    digitalRead(FB_TS) == 1 ? SetBit(ssCode, 2) : ClearBit(ssCode, 2);
    digitalRead(SB_BS) == 1 ? SetBit(ssCode, 1) : ClearBit(ssCode, 1);
    digitalRead(SB_TS) == 1 ? SetBit(ssCode, 0) : ClearBit(ssCode, 0);
}

void fillFirstBarrel(){
  digitalWrite(FB_IN, LOW);
  while(digitalRead(FB_TS) != 1){
    Serial.println("Waiting for the FIRST barrel to be filled...");
    delay(1000);
  }
  digitalWrite(FB_IN, HIGH);
}

void fillSecondBarrel(){
    digitalWrite(SB_IN, LOW);
  while(digitalRead(SB_TS) != 1){
    Serial.println("Waiting for the SECOND barrel to be filled...");
    delay(1000);
  }
  digitalWrite(SB_IN, HIGH);
}

void switchToFirstBarrel(){
  Serial.println("Switching to FIRST barrel. CODE: " + ssCode);
  digitalWrite(FB_OUT, LOW);
  Serial.println("FIRST OUT valve opened");
  delay(1000); 
  Serial.println("SECOND OUT valve closed");
  digitalWrite(SB_OUT, HIGH);  
  delay(1000);
}

void switchToSecondBarrel(){
  Serial.println("Switching to SECOND barrel. CODE: " + ssCode);
  digitalWrite(SB_OUT, LOW);
  Serial.println("SECOND OUT valve opened");
  delay(1000);
  digitalWrite(FB_OUT, HIGH);
  Serial.println("FIRST OUT valve closed");
  delay(1000);
}



