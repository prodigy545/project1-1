#include <WiFi101.h>
// --- Motor Pin Definitions ---
const int WINC_CS  = 8, WINC_IRQ = 7, WINC_RST = 4, WINC_EN = 2;

const int FL_PWM = 6,   FL_DIR = 5;   // Front Left:  E=6,  M=5
const int FR_PWM = 9,   FR_DIR = 10;  // Front Right: E=9,  M=10
const int BL_PWM = A4,  BL_DIR = A5;  // Back Left:   E=A4, M=A5
const int BR_PWM = 11,  BR_DIR = 12;  // Back Right:  E=11, M=12

const int PIN_IR_LEFT_DIGITAL = A0;
const int PIN_IR_LEFT_ANALOG = A1; 
const int PIN_IR_RIGHT_DIGITAL = A2; 
const int PIN_IR_RIGHT_ANALOG = A3;



const int TRIG_PIN = 0;
const int ECHO_PIN = 1;


// --- Variables ---
int motorSpeed = 90;           // Default speed 
char lastMotionCmd = 'x';   // Stores the last command 

bool autolinemodeon = false;
bool manualMode = true;


float Ki = 0.0;
float Kd = 1.0;
float Kp = 4.0;
int lineFollowBaseSpeed = 75;

float error = 0;
float lastError = 0;
// float Integral = 0;
float Derivative = 0;

int irLeftDigital = 0;
int irRightDigital = 0;
int irLeftAnalog = 0;
int irRightAnalog = 0;




const char ssid[] = "FeatherAP";
const char pass[] = "test1234";     
WiFiServer server(80);

String ipToString(const IPAddress& ip) {
  return String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]);
}

// --- Setup Function for Each Motor ---
void setupMotor(int pwm, int dir) {
  pinMode(pwm, OUTPUT);  
  pinMode(dir, OUTPUT);
  
}

// --- Function to Drive a Motor ---
void setMotor(int pwm, int dir, int speed, bool forward) {
  digitalWrite(dir, forward ? HIGH : LOW); 
  analogWrite(pwm, speed);                 
}



// --- Arduino Setup Function ---

void setup() {
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(PIN_IR_LEFT_DIGITAL, INPUT);
  pinMode(PIN_IR_RIGHT_DIGITAL, INPUT);
  pinMode(PIN_IR_LEFT_ANALOG, INPUT);
  pinMode(PIN_IR_RIGHT_ANALOG, INPUT);


  Serial.begin(115200);  

  WiFi.setPins(WINC_CS, WINC_IRQ, WINC_RST, WINC_EN);


  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WINC1500 not detected"); while (1) {}
  }

    Serial.print("FW: "); Serial.println(WiFi.firmwareVersion());


  Serial.println("Starting AP…");
  int s = WiFi.beginAP(ssid, pass, 6);            
  if (s != WL_AP_LISTENING) {
    Serial.print("WPA2 AP failed ("); Serial.print(s); Serial.println("). Trying OPEN…");
    s = WiFi.beginAP(ssid, 6);                    
    if (s != WL_AP_LISTENING) { Serial.println("AP failed"); while (1) {} }
  }

  delay(8000); 

   Serial.print("AP IP: "); Serial.println(ipToString(WiFi.localIP())); 
  server.begin();


  // Initialize all four motors
  setupMotor(FL_PWM, FL_DIR);
  setupMotor(FR_PWM, FR_DIR);
  setupMotor(BR_PWM, BR_DIR);
  setupMotor(BL_PWM, BL_DIR);

  // Stop all motors 
  stopAllMotors();

}


// --- Function to Stop All Motors ---
void stopAllMotors() {
  lastMotionCmd = 's';
  setMotor(FL_PWM, FL_DIR, 0, true); 
  setMotor(FR_PWM, FR_DIR, 0, true);
  setMotor(BR_PWM, BR_DIR, 0, true);
  setMotor(BL_PWM, BL_DIR, 0, true);
  Serial.println("Stopping all motors");
}

// --- Movement Functions (called when a motion command is received) ---
void moveForward() {
  lastMotionCmd = 'f';
  setMotor(FL_PWM, FL_DIR, motorSpeed, false );
  setMotor(FR_PWM, FR_DIR, motorSpeed, true );
  setMotor(BR_PWM, BR_DIR, motorSpeed, true );
  setMotor(BL_PWM, BL_DIR, motorSpeed, false );
}

void moveBackwards() {
  lastMotionCmd = 'b';
  setMotor(FL_PWM, FL_DIR, motorSpeed, true);
  setMotor(FR_PWM, FR_DIR, motorSpeed, false);
  setMotor(BR_PWM, BR_DIR, motorSpeed, false );
  setMotor(BL_PWM, BL_DIR, motorSpeed, true);
}

void moveRight() {
  lastMotionCmd = 'r';
  setMotor(FL_PWM, FL_DIR ,motorSpeed, false);
  setMotor(BL_PWM, BL_DIR, motorSpeed, false );
  setMotor(FR_PWM, FR_DIR, motorSpeed-40, true);
  setMotor(BR_PWM, BR_DIR, motorSpeed-40, true);
}

void moveLeft() {
  lastMotionCmd = 'l';
  setMotor(FR_PWM, FR_DIR, motorSpeed, true);
  setMotor(BR_PWM, BR_DIR, motorSpeed, true);
  setMotor(BL_PWM, BL_DIR, motorSpeed-40, false);
  setMotor(FL_PWM, FL_DIR, motorSpeed-40,false);
}

void rotateCounterClockwise(){
  lastMotionCmd = 'cc';
  setMotor(FL_PWM, FL_DIR, motorSpeed, true);
  setMotor(FR_PWM, FR_DIR, motorSpeed, true);
  setMotor(BR_PWM, BR_DIR, motorSpeed, true);
  setMotor(BL_PWM, BL_DIR, motorSpeed, true);
}

void rotateClockwise(){
  lastMotionCmd = 'c';
  setMotor(FL_PWM, FL_DIR, motorSpeed, false);
  setMotor(FR_PWM, FR_DIR, motorSpeed, false);
  setMotor(BR_PWM, BR_DIR, motorSpeed, false);
  setMotor(BL_PWM, BL_DIR, motorSpeed, false);
}

void moveSidewaysLeft(){
  lastMotionCmd = 'sl';
  setMotor(FL_PWM, FL_DIR, motorSpeed, false);
  setMotor(FR_PWM, FR_DIR, motorSpeed, false);
  setMotor(BR_PWM, BR_DIR, motorSpeed, true);
  setMotor(BL_PWM, BL_DIR, motorSpeed, true);
}

void moveSidewaysRight(){
  lastMotionCmd = 'sr';
  setMotor(FL_PWM, FL_DIR, motorSpeed, true);
  setMotor(FR_PWM, FR_DIR, motorSpeed, true);
  setMotor(BR_PWM, BR_DIR, motorSpeed, false);
  setMotor(BL_PWM, BL_DIR, motorSpeed, false);
}

long getDistanceCM(){
  
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN , HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long speed = pulseIn(ECHO_PIN, HIGH, 38000);

  long distance = speed * 0.017; 

  return distance;
}



void emergencyStop(){
  if( (lastMotionCmd == 'f' || lastMotionCmd == 'r' || lastMotionCmd == 'l') && getDistanceCM()<45){
    stopAllMotors();
  }
}





void autoLineFollowing(){
    if(autolinemodeon){
      irLeftAnalog = analogRead(PIN_IR_LEFT_ANALOG);
      irRightAnalog = analogRead(PIN_IR_RIGHT_ANALOG);
      
      // PID calculation 
      error = irRightAnalog - irLeftAnalog;
      Derivative = error - lastError;
      
      float correction = Kp * error + Kd * Derivative; 

      // Use base speed for line following
      int leftMotorSpeed = lineFollowBaseSpeed + correction;
      int rightMotorSpeed = lineFollowBaseSpeed - correction;

      // If statements instead of constrain
      if (leftMotorSpeed > 90) leftMotorSpeed = 90;
      if (leftMotorSpeed < 0) leftMotorSpeed = 0;
      if (rightMotorSpeed > 90) rightMotorSpeed = 90;
      if (rightMotorSpeed < 0) rightMotorSpeed = 0;

      setMotor(FL_PWM, FL_DIR, leftMotorSpeed, false);
      setMotor(FR_PWM, FR_DIR, rightMotorSpeed, true);
      setMotor(BR_PWM, BR_DIR, rightMotorSpeed, true);
      setMotor(BL_PWM, BL_DIR, leftMotorSpeed, false);

      lastError = error;
    } else {
      stopAllMotors();
    }
}
  


void serve(WiFiClient& c){
  c.setTimeout(1500);
  String rl=c.readStringUntil('\n');        
  int sp1=rl.indexOf(' '), sp2=rl.indexOf(' ',sp1+1);
  String uri=(sp1>0&&sp2>sp1)?rl.substring(sp1+1,sp2):"/";
  int q=uri.indexOf('?'); String pth=(q>=0)?uri.substring(0,q):uri; String qry=(q>=0)?uri.substring(q+1):"";
  while(true){ String h=c.readStringUntil('\n'); if(h.length()==0||h=="\r") break; } 
  route(c,pth,qry);
}

void route(WiFiClient& c,const String& path,const String& q){
  if(path=="/"||path=="") { handleRoot(c); return; }
  if(path=="/forward")    { handleForward(c); return; }
  if(path=="/stop")      {autolinemodeon = false; manualMode = true; stop(c); return;}
  if(path=="/backwards") {handleBackwards(c); return;}
  if(path=="/left")       {handleLeft(c); return;}
  if(path=="/right")      {handleRight(c); return;}
  if(path=="/clockwise") {handleClockwise(c); return;}
  if(path=="/movesidewaysleft") {handleMoveSidewaysLeft(c); return;}
  if(path=="/movesidewaysright") {handleMoveSidewaysRight(c); return;}
  if(path=="/counterclockwise") {handleCounterClockwise(c); return;}
  if(path=="/autolineon") {autolinemodeon = true; manualMode = false; handleAutoLineOn(c); return;}
  if(path=="/autolineoff") {autolinemodeon = false; manualMode = true; handleAutolineOff(c); return;} 
  if(path.startsWith("/setspeed")) {handleSetSpeed(c, q); return;}
  // sendText(c,"404\n");
}

void handleRoot(WiFiClient& client){
   const char body[] = "Select an option";

  client.print("HTTP/1.1 200 OK\r\n");
  client.print("Content-Type: text/html\r\n");
  client.print("Connection: close\r\n");
  client.print("Content-Length: "); client.print(sizeof(body) - 1); client.print("\r\n\r\n");
  client.print(body);
    delay(1);
}

void handleForward(WiFiClient& client){
  moveForward();

  const char body[] = "Moved Forward";

  client.print("HTTP/1.1 200 OK\r\n");
  client.print("Content-Type: text/html\r\n");
  client.print("Connection: close\r\n");
  client.print("Content-Length: "); client.print(sizeof(body) - 1); client.print("\r\n\r\n");
  client.print(body);
    delay(1);
}

void stop(WiFiClient& client){
  stopAllMotors();
  const char body[] = "Stopped All Motors";

  client.print("HTTP/1.1 200 OK\r\n");
  client.print("Content-Type: text/html\r\n");
  client.print("Connection: close\r\n");
  client.print("Content-Length: "); client.print(sizeof(body) - 1); client.print("\r\n\r\n");
  client.print(body);
  delay(1);
    
}

void handleBackwards(WiFiClient& client){
  moveBackwards();
  const char body[] = "Moved Backwards";

  client.print("HTTP/1.1 200 OK\r\n");
  client.print("Content-Type: text/html\r\n");
  client.print("Connection: close\r\n");
  client.print("Content-Length: "); client.print(sizeof(body) - 1); client.print("\r\n\r\n");
  client.print(body);
  delay(1);
}

void handleLeft(WiFiClient& client){
  moveLeft();
  const char body[] = "Moved Left";

  client.print("HTTP/1.1 200 OK\r\n");
  client.print("Content-Type: text/html\r\n");
  client.print("Connection: close\r\n");
  client.print("Content-Length: "); client.print(sizeof(body) - 1); client.print("\r\n\r\n");
  client.print(body);
  delay(1);
}

void handleRight(WiFiClient& client){
  moveRight();
  const char body[] = "Moved Right.";

  client.print("HTTP/1.1 200 OK\r\n");
  client.print("Content-Type: text/html\r\n");
  client.print("Connection: close\r\n");
  client.print("Content-Length: "); client.print(sizeof(body) - 1); client.print("\r\n\r\n");
  client.print(body);
  delay(1);
}

void handleClockwise(WiFiClient& client){
  rotateClockwise();
  const char body[] = "Moved Clockwise.";

  client.print("HTTP/1.1 200 OK\r\n");
  client.print("Content-Type: text/html\r\n");
  client.print("Connection: close\r\n");
  client.print("Content-Length: "); client.print(sizeof(body) - 1); client.print("\r\n\r\n");
  client.print(body);
  delay(1);
}

void handleCounterClockwise(WiFiClient& client){
  rotateCounterClockwise();
  const char body[] = "Moved Counter Clockwise ";

  client.print("HTTP/1.1 200 OK\r\n");
  client.print("Content-Type: text/html\r\n");
  client.print("Connection: close\r\n");
  client.print("Content-Length: "); client.print(sizeof(body) - 1); client.print("\r\n\r\n");
  client.print(body);
  delay(1);
}

void handleMoveSidewaysLeft(WiFiClient& client){
  moveSidewaysLeft();
  const char body[] = "Moved Sideways Left";

  client.print("HTTP/1.1 200 OK\r\n");
  client.print("Content-Type: text/html\r\n");
  client.print("Connection: close\r\n");
  client.print("Content-Length: "); client.print(sizeof(body) - 1); client.print("\r\n\r\n");
  client.print(body);
  delay(1);
}

void handleMoveSidewaysRight(WiFiClient& client){
  moveSidewaysRight();
  const char body[] = "Moved Sideways Right";

  client.print("HTTP/1.1 200 OK\r\n");
  client.print("Content-Type: text/html\r\n");
  client.print("Connection: close\r\n");
  client.print("Content-Length: "); client.print(sizeof(body) - 1); client.print("\r\n\r\n");
  client.print(body);
  delay(1);
}

void handleSetSpeed(WiFiClient& client, const String& path) {
  int newSpeed = 90;
  int index = path.indexOf("value=");

  if (index != -1) {
    newSpeed = path.substring(index + 6).toInt();
  }

  if (newSpeed >= 0 && newSpeed <= 255) {
    Serial.print("Setting speed to : ");
    Serial.println(newSpeed);
    motorSpeed = newSpeed;
  }
  
  String body = "Speed set to " + String(newSpeed);
  client.print("HTTP/1.1 200 OK\r\n");
  client.print("Content-Type: text/html\r\n");
  client.print("Connection: close\r\n");
  client.print("Content-Length: "); client.print(body.length()); client.print("\r\n\r\n");
  client.print(body);
}

void handleAutoLineOn(WiFiClient& client){
  autolinemodeon = true;
  
 const char body[] = "Auto line mode ON";

  client.print("HTTP/1.1 200 OK\r\n");
  client.print("Content-Type: text/html\r\n");
  client.print("Connection: close\r\n");
  client.print("Content-Length: ");
  client.print(sizeof(body) - 1);
  client.print("\r\n\r\n");
  client.print(body);
}

void handleAutolineOff(WiFiClient& client){
  autolinemodeon = false;
  stopAllMotors();
  const char body[] = "Auto line mode OFF";

  client.print("HTTP/1.1 200 OK\r\n");
  client.print("Content-Type: text/html\r\n");
  client.print("Connection: close\r\n");
  client.print("Content-Length: ");
  client.print(sizeof(body) - 1);
  client.print("\r\n\r\n");
  client.print(body);
}


 

void loop() {
  irLeftAnalog = analogRead(PIN_IR_LEFT_ANALOG);
  irRightAnalog = analogRead(PIN_IR_RIGHT_ANALOG);

  // Serial.print("Left A="); Serial.print(irLeftAnalog);
  // Serial.print("Right A="); Serial.print(irRightAnalog);


  getDistanceCM();
  // emergencyStop();
  if(autolinemodeon){
  autoLineFollowing();

  }else if(manualMode){
    
  }

  delay(50);
 

  WiFiClient client = server.available();
  if (!client) return;

  client.setTimeout(2000); 
  serve(client);
  client.stop();
} 

h