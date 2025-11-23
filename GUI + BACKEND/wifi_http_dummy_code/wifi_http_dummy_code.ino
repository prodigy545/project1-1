#include <WiFi101.h>

// Feather M0 WiFi (WINC1500) pins
const int WINC_CS  = 8, WINC_IRQ = 7, WINC_RST = 4, WINC_EN = 2;

const int FL_PWM = 6,  FL_DIR = 5;     // Front Left Motor
int motorSpeed = 80;           // Default speed for all motors (range: 0–255)
char lastMotionCmd = 'x';      // Stores the last direction command (e.g., 'f' for forward)

const char ssid[] = "FeatherAP";
const char pass[] = "test1234";     // >= 8 chars for WPA2
WiFiServer server(80);

// --------- Utility: IPAddress -> "A.B.C.D" -----
String ipToString(const IPAddress& ip) {
  return String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]);
}

// --- Setup Function for Each Motor ---
// Configures the direction and PWM pins for a motor
void setupMotor(int pwm, int dir) {
  pinMode(pwm, OUTPUT);  // Set PWM pin as output
  pinMode(dir, OUTPUT);  // Set direction pin as output
}

// --- Function to Stop All Motors ---
void stopAllMotors() {
  setMotor(FL_PWM, FL_DIR, 0, true);  // Speed 0 = stop
  //Add your code to control the other motors.
}

// --- Function to Drive a Motor ---
// 'speed' determines how fast, 'forward' determines direction
void setMotor(int pwm, int dir, int speed, bool forward) {
  digitalWrite(dir, forward ? HIGH : LOW);  // Set direction
  analogWrite(pwm, speed);                  // Set speed using PWM
}

void moveForward() {
  setMotor(FL_PWM, FL_DIR, motorSpeed, true);
  //Add your code to control the other motors.
  Serial.println("Moving forward");
}

void setup() {
  Serial.begin(115200);
  // DO NOT block on while(!Serial); we want it to run even without a PC attached

  WiFi.setPins(WINC_CS, WINC_IRQ, WINC_RST, WINC_EN);

  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WINC1500 not detected"); while (1) {}
  }

  Serial.print("FW: "); Serial.println(WiFi.firmwareVersion());

  Serial.println("Starting AP…");
  int s = WiFi.beginAP(ssid, pass, 6);            // WPA2, ch 6
  if (s != WL_AP_LISTENING) {
    Serial.print("WPA2 AP failed ("); Serial.print(s); Serial.println("). Trying OPEN…");
    s = WiFi.beginAP(ssid, 6);                    // OPEN AP fallback
    if (s != WL_AP_LISTENING) { Serial.println("AP failed"); while (1) {} }
  }

  delay(8000); // let AP + DHCP come up

  Serial.print("AP IP: "); Serial.println(ipToString(WiFi.localIP())); // usually 192.168.1.1
  server.begin();

    // Initialize all four motors
  setupMotor(FL_PWM, FL_DIR);
  //Add your code to control the other motors.

  // Stop all motors initially
  stopAllMotors();
}

void serve(WiFiClient& c){
  c.setTimeout(1500);
  String rl=c.readStringUntil('\n');        // "GET /path?query HTTP/1.1"
  int sp1=rl.indexOf(' '), sp2=rl.indexOf(' ',sp1+1);
  String uri=(sp1>0&&sp2>sp1)?rl.substring(sp1+1,sp2):"/";
  int q=uri.indexOf('?'); String pth=(q>=0)?uri.substring(0,q):uri; String qry=(q>=0)?uri.substring(q+1):"";
  while(true){ String h=c.readStringUntil('\n'); if(h.length()==0||h=="\r") break; } // headers
  route(c,pth,qry);
}

void route(WiFiClient& c,const String& path,const String& q){
  if(path=="/"||path=="") { handleRoot(c); return; }
  if(path=="/forward")    { handleForward(c); return; }
  // sendText(c,"404\n");
}

void handleRoot(WiFiClient& client){
  // Minimal well-formed HTTP response
  const char body[] = "Initial Page";

  client.print("HTTP/1.1 200 OK\r\n");
  client.print("Content-Type: text/html\r\n");
  client.print("Connection: close\r\n");
  client.print("Content-Length: "); client.print(sizeof(body) - 1); client.print("\r\n\r\n");
  client.print(body);
    delay(1);
}

void handleForward(WiFiClient& client){
  moveForward();
    // Minimal well-formed HTTP response
  const char body[] = "Moved Forward";

  client.print("HTTP/1.1 200 OK\r\n");
  client.print("Content-Type: text/html\r\n");
  client.print("Connection: close\r\n");
  client.print("Content-Length: "); client.print(sizeof(body) - 1); client.print("\r\n\r\n");
  client.print(body);
    delay(1);
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  client.setTimeout(2000); // 2s read timeout
  serve(client);
  client.stop();
}
