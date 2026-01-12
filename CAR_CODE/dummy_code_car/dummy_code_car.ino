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
  int motorSpeed = 90;  

  int searchForwardSpeed = 110;  
  int searchPivotSpeed = 110;   
  int searchTurnDuration = 950; 
  char lastMotionCmd = 'x';   

  bool autolinemodeon = false;
  bool manualMode = true;

  const int LINE_DETECTED_THRESHOLD = 200;

  // State Machine Variables
  int searchState = 0;          
  unsigned long searchTimer = 0; 

  float Ki = 0.2;
  float Kd = 1.5;
  float Kp = 5.0;
  int lineFollowBaseSpeed = 78;

  float error = 0;
  float lastError = 0;
  float Integral = 0;
  float Derivative = 0;

  bool searchMode = false;
  int searchStep = 0;
  int squareSize = 500;         
  int currentSide = 0; 

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
    if( (lastMotionCmd == 'f' || lastMotionCmd == 'r' || lastMotionCmd == 'l') && getDistanceCM()<15){
      stopAllMotors();
    }
  }


  // Check if either sensor sees a line
  bool foundLine() {
    irLeftAnalog = analogRead(PIN_IR_LEFT_ANALOG);
    irRightAnalog = analogRead(PIN_IR_RIGHT_ANALOG);
    
    bool lineFound = (irLeftAnalog > LINE_DETECTED_THRESHOLD || 
                      irRightAnalog > LINE_DETECTED_THRESHOLD);
    
    if (lineFound) {
      Serial.println("LINE FOUND!");
      Serial.print("Left: "); Serial.print(irLeftAnalog);
      Serial.print(" Right: "); Serial.println(irRightAnalog);
    }
    
    return lineFound;
  }

  // Check if robot is lost (both sensors see white)
  bool isLost() {
    irLeftAnalog = analogRead(PIN_IR_LEFT_ANALOG);
    irRightAnalog = analogRead(PIN_IR_RIGHT_ANALOG);
    
    return (irLeftAnalog < LINE_DETECTED_THRESHOLD && 
            irRightAnalog < LINE_DETECTED_THRESHOLD);
  }

  // Search for line using expanding squares
  void searchForLine() {
    if (!searchMode) return;
    
    // --- 1. Constant Check for Line ---
    // If found, stop everything immediately
    if (foundLine()) {
      Serial.println("=== SUCCESS! Switching to line following ===");
      searchMode = false;
      autolinemodeon = true;
      manualMode = false;
      stopAllMotors();
      return;
    }
    
    unsigned long currentTime = millis();

    // --- 2. State Machine Logic ---
    
    // STATE 0: MOVING FORWARD
    if (searchState == 0) {
      // Drive Forward with custom SLOW speed
      setMotor(FL_PWM, FL_DIR, searchForwardSpeed, false);
      setMotor(FR_PWM, FR_DIR, searchForwardSpeed, true);
      setMotor(BR_PWM, BR_DIR, searchForwardSpeed, true);
      setMotor(BL_PWM, BL_DIR, searchForwardSpeed, false);

      // Check if time is up for this side
      if ((currentTime - searchTimer)/1.3> squareSize) {
        stopAllMotors();
        searchState = 1;         // Switch to Turning state
        searchTimer = currentTime; // Reset timer
      }
    } 
    
    // STATE 1: TURNING (Pivoting)
    else if (searchState == 1) {
      // Pivot Clockwise with custom FAST speed
      // Note: All motors set to 'false' for clockwise based on your rotateClockwise() function
      setMotor(FL_PWM, FL_DIR, searchPivotSpeed, false);
      setMotor(FR_PWM, FR_DIR, searchPivotSpeed, false);
      setMotor(BR_PWM, BR_DIR, searchPivotSpeed, false);
      setMotor(BL_PWM, BL_DIR, searchPivotSpeed, false);

      // Check if time is up for the turn
      if (currentTime - searchTimer > searchTurnDuration) {
        stopAllMotors();
        
        // Setup for next forward move
        searchState = 0;         // Switch back to Moving Forward
        searchTimer = currentTime; // Reset timer
        currentSide++;
        
        // Expand square logic
        if (currentSide >= 4) {
          currentSide = 0;
          squareSize += 300;
          Serial.print("=== Expanding search - size: ");
          Serial.println(squareSize);
          
          // Safety limit (Search Timeout)
          if (squareSize > 2000) {
            Serial.println("=== SEARCH FAILED - Line not found ===");
            searchMode = false;
            stopAllMotors();
          }
        }
      }
    }
  }

  // Start the search
  void startKidnappedSearch() {
    Serial.println("\n========== STARTING KIDNAPPED ROBOT SEARCH ==========");
    searchMode = true;
    autolinemodeon = false;
    manualMode = false;
    searchStep = 0;
    squareSize = 600;
    currentSide = 0;
    searchState = 0;
    searchTimer = millis();
  }

  // Auto-detect if robot was kidnapped
  void autoDetectKidnapped() {
    static int lostCounter = 0;
    static unsigned long lastLineSeenTime = 0;
    
    if (autolinemodeon && !searchMode) {
      irLeftAnalog = analogRead(PIN_IR_LEFT_ANALOG);
      irRightAnalog = analogRead(PIN_IR_RIGHT_ANALOG);
      
      // Check if AT LEAST ONE sensor sees the line (black tape)
      bool canSeeLine = (irLeftAnalog > LINE_DETECTED_THRESHOLD || 
                        irRightAnalog > LINE_DETECTED_THRESHOLD);
      
      if (canSeeLine) {
        // Reset counter - we can still see the line
        lostCounter = 0;
        lastLineSeenTime = millis();
      } else {
        // Neither sensor sees black - might be lost
        lostCounter++;
        
        // Only trigger if lost for 1 full second (20 cycles × 50ms = 1000ms)
        if (lostCounter > 300) {
          Serial.println("=== ROBOT KIDNAPPED! Starting search... ===");
          Serial.print("Time since last line seen: ");
          Serial.print(millis() - lastLineSeenTime);
          Serial.println(" ms");
          startKidnappedSearch();
          lostCounter = 0;
        }
      }
    } else {
      lostCounter = 0; // Reset if not in auto-line mode
    }
  }



  void autoLineFollowing(){
      if(autolinemodeon){
        irLeftAnalog = analogRead(PIN_IR_LEFT_ANALOG);
        irRightAnalog = analogRead(PIN_IR_RIGHT_ANALOG);
        
        
        error = irRightAnalog - irLeftAnalog;
        Derivative = error - lastError;
        Integral += error;

        float correction = Kp * error + Kd * Derivative + Ki* Integral; 

        
        int leftMotorSpeed = lineFollowBaseSpeed + correction;
        int rightMotorSpeed = lineFollowBaseSpeed - correction;

      
      bool left_parity = false;
      bool right_parity = true;
      if (leftMotorSpeed > 90) leftMotorSpeed = 90;
      if (leftMotorSpeed < 0) {
        leftMotorSpeed = -leftMotorSpeed;
        left_parity = !left_parity;
      }
      if (rightMotorSpeed > 90) rightMotorSpeed = 90;
      if (rightMotorSpeed < 0) {
        rightMotorSpeed = -rightMotorSpeed;
        right_parity = !right_parity;
      }

      setMotor(FL_PWM, FL_DIR, leftMotorSpeed, left_parity);
      setMotor(FR_PWM, FR_DIR, rightMotorSpeed, right_parity);
      setMotor(BR_PWM, BR_DIR, rightMotorSpeed, right_parity);
      setMotor(BL_PWM, BL_DIR, leftMotorSpeed, left_parity);

        lastError = error;
      } else {
        stopAllMotors();
        Integral = 0;
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
    if(path=="/stop")      {autolinemodeon = false; manualMode = true;     searchMode = false;  stop(c); return;}
    if(path=="/backwards") {handleBackwards(c); return;}
    if(path=="/left")       {handleLeft(c); return;}
    if(path=="/right")      {handleRight(c); return;}
    if(path=="/clockwise") {handleClockwise(c); return;}
    if(path=="/movesidewaysleft") {handleMoveSidewaysLeft(c); return;}
    if(path=="/movesidewaysright") {handleMoveSidewaysRight(c); return;}
    if(path=="/counterclockwise") {handleCounterClockwise(c); return;}
    if(path=="/autolineon") {autolinemodeon = true; manualMode = false; handleAutoLineOn(c); return;}
    if(path=="/autolineoff") {autolinemodeon = false; manualMode = true; handleAutolineOff(c); return;}
    if(path=="/kidnappedstart") { handleKidnappedStart(c); return; }   // ADD THIS
    if(path=="/kidnappedstop") { handleKidnappedStop(c); return; }     // ADD THIS
    if(path.startsWith("/setspeed")) {handleSetSpeed(c, q); return;}
    
    if (path.startsWith("/setmap")) { handleSetMap(c, q); return; }
    if (path == "/getdata") { handleGetData(c); return; }
    

    // sendText(c,"404\n");
  }

  // Junction map inner awareness
  char memory[69] = {};

  void handleSetMap(WiFiClient& client, const String& path) {
    int index = path.indexOf("value=");

    // Default: empty map
    memory[0] = '\0';

    String value = "";
    if (index != -1) {
      value = path.substring(index + 6); // everything after "value="
    }

    // Stop at first separator if URL has more params like &foo=bar
    int amp = value.indexOf('&');
    if (amp != -1) value = value.substring(0, amp);

    // Copy only R/L into memory, up to 68 chars (leave room for '\0')
    int written = 0;
    for (int i = 0; i < value.length() && written < 68; i++) {
      char c = value.charAt(i);

      // Accept lowercase too
      if (c == 'r') c = 'R';
      if (c == 'l') c = 'L';

      if (c == 'R' || c == 'L') {
        memory[written++] = c;
      }
    }
    memory[written] = '\0';

    Serial.print("Setting map to: ");
    Serial.println(memory);

    String body = "Map set to " + String(memory) + " (len=" + String(written) + ")";
    client.print("HTTP/1.1 200 OK\r\n");
    client.print("Content-Type: text/html\r\n");
    client.print("Connection: close\r\n");
    client.print("Content-Length: "); client.print(body.length()); client.print("\r\n\r\n");
    client.print(body);
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


  void handleKidnappedStart(WiFiClient& client) {
    startKidnappedSearch();
    
    const char body[] = "Kidnapped robot search started";
    client.print("HTTP/1.1 200 OK\r\n");
    client.print("Content-Type: text/html\r\n");
    client.print("Connection: close\r\n");
    client.print("Content-Length: ");
    client.print(sizeof(body) - 1);
    client.print("\r\n\r\n");
    client.print(body);
  }

  void handleKidnappedStop(WiFiClient& client) {
    Serial.println("=== Search stopped manually ===");
    searchMode = false;
    stopAllMotors();
    
    const char body[] = "Search stopped";
    client.print("HTTP/1.1 200 OK\r\n");
    client.print("Content-Type: text/html\r\n");
    client.print("Connection: close\r\n");
    client.print("Content-Length: ");
    client.print(sizeof(body) - 1);
    client.print("\r\n\r\n");
    client.print(body);
  }

  double pos_x = 0.0, pos_y = 0.0, angle_rad = 1.5707963267948966;

  void handleGetData(WiFiClient& client) {
    // const char body[] = String(pos_x) + "\n" + String(pos_y) + "\n" + String(angle_rad) + "\n";
    char body[200] = {};
    snprintf(body, sizeof(body), "%f\n%f\n%f\n", pos_x, pos_y, angle_rad);

    client.print("HTTP/1.1 200 OK\r\n");
    client.print("Content-Type: text/html\r\n");
    client.print("Connection: close\r\n");
    int len = strlen(body);
    client.print("Content-Length: ");
    client.print(len);
    client.print("\r\n\r\n");
    client.print(body);
  }



  #define TIME_TO_COMPLETE_THE_FULL_CIRCLE_IN_SECONDS 5
  #define TIME_TO_COMPLETE_A_ROTAION 3
  #define CRAB_MULTIPLYER (0.5)

  

  void loop() {
    getDistanceCM();
    emergencyStop();
    
    // Priority order: search > auto-line > manual
    if (searchMode) {
      searchForLine();           // Searching for line
    } else if (autolinemodeon) {
      autoDetectKidnapped();     // Check if robot got lost
      autoLineFollowing();       // Normal line following
    } else if (manualMode) {
      // Manual control
    }


    // =======================
    // ODOMETRY FROM REAL MOTOR PINS (DIR + PWM DUTY)
    // Replaces the old lastMotionCmd-based position update.
    // =======================

    // For your wiring/parity (based on moveForward()):
    // FL forward => dir LOW,  FR forward => dir HIGH
    // BL forward => dir LOW,  BR forward => dir HIGH
    const uint8_t FL_FWD_LEVEL = LOW;
    const uint8_t FR_FWD_LEVEL = HIGH;
    const uint8_t BL_FWD_LEVEL = LOW;
    const uint8_t BR_FWD_LEVEL = HIGH;

    // Cache PWM reads so pulseIn() doesn't stall WiFi handling too much
    static unsigned long odoLastMs = 0;
    static unsigned long pwmSampleLastMs = 0;
    static uint8_t pwmFL = 0, pwmFR = 0, pwmBL = 0, pwmBR = 0;

    auto readPwmDuty255 = [](uint8_t pwmPin) -> uint8_t {
      // Measures PWM duty by timing HIGH/LOW pulses.
      // Works even if the pin is configured as OUTPUT (Arduino reads back port state).
      // Timeout tuned for ~490/980Hz PWM (period ~2.0ms / ~1.0ms).
      const unsigned long TIMEOUT_US = 3500;

      unsigned long hi = pulseIn(pwmPin, HIGH, TIMEOUT_US);
      unsigned long lo = pulseIn(pwmPin, LOW,  TIMEOUT_US);

      // If we couldn't measure a PWM cycle, treat it as DC HIGH/LOW
      if (hi == 0 && lo == 0) {
        return digitalRead(pwmPin) ? 255 : 0;
      }
      if (lo == 0) return 255;
      if (hi == 0) return 0;

      float duty = (float)hi / (float)(hi + lo);
      int v = (int)(duty * 255.0f + 0.5f);
      if (v < 0) v = 0;
      if (v > 255) v = 255;
      return (uint8_t)v;
    };

    auto signedWheel = [](uint8_t duty255, uint8_t dirPin, uint8_t fwdLevel) -> float {
      int sign = (digitalRead(dirPin) == fwdLevel) ? +1 : -1;
      return sign * ((float)duty255 / 255.0f); // [-1..+1]
    };

    unsigned long nowMs = millis();
    if (odoLastMs == 0) odoLastMs = nowMs;
    float dt = (nowMs - odoLastMs) * 0.001f;
    odoLastMs = nowMs;

    // Sample PWM duty only every 50ms (keeps loop responsive)
    if (nowMs - pwmSampleLastMs >= 50) {
      pwmSampleLastMs = nowMs;
      pwmFL = readPwmDuty255(FL_PWM);
      pwmFR = readPwmDuty255(FR_PWM);
      pwmBL = readPwmDuty255(BL_PWM);
      pwmBR = readPwmDuty255(BR_PWM);
    }

    // Wheel "commanded actual" values from pins (normalized)
    float wFL = signedWheel(pwmFL, FL_DIR, FL_FWD_LEVEL);
    float wFR = signedWheel(pwmFR, FR_DIR, FR_FWD_LEVEL);
    float wBL = signedWheel(pwmBL, BL_DIR, BL_FWD_LEVEL);
    float wBR = signedWheel(pwmBR, BR_DIR, BR_FWD_LEVEL);

    // Mecanum-ish kinematics in robot frame:
    // vF = forward, vL = left, wZ = CCW
    float vF = (wFL + wFR + wBL + wBR) * 0.25f;
    float vL = (-wFL + wFR + wBL - wBR) * 0.25f;
    float wZ = (-wFL + wFR - wBL + wBR) * 0.25f;

    // Scale to your old "units":
    // Previously: 0.05 units per tick at ~20Hz => ~1.0 units/sec at full speed.
    const float MAX_UNITS_PER_SEC = 1.0f;

    // Previously for rotation you used TIME_TO_COMPLETE_A_ROTAION (seconds for 2π).
    const float MAX_RAD_PER_SEC = (6.283185307179586f / (float)TIME_TO_COMPLETE_A_ROTAION);

    float vF_u = vF * MAX_UNITS_PER_SEC;
    float vL_u = vL * MAX_UNITS_PER_SEC;
    float wZ_r = wZ * MAX_RAD_PER_SEC;

    // Integrate pose in world frame
    angle_rad += wZ_r * dt;

    float ca = cos(angle_rad);
    float sa = sin(angle_rad);

    pos_x += (ca * vF_u - sa * vL_u) * dt;
    pos_y += (sa * vF_u + ca * vL_u) * dt;


    WiFiClient client = server.available();
    // static unsigned long lastDebug = 0;
    // irLeftAnalog = analogRead(PIN_IR_LEFT_ANALOG);
    // irRightAnalog = analogRead(PIN_IR_RIGHT_ANALOG);
    // Serial.print("L: "); Serial.print(irLeftAnalog);
    // Serial.print(" | R: "); Serial.print(irRightAnalog);
    

    if (!client) return;

    client.setTimeout(2000); 
    serve(client);
    client.stop();
  }

