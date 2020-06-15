//Libraries Required
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//OLED screen dimensions.
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


float bbw = .2; //BB weight (Grams)
float joules = 0; //Joules
float rof = 0; //Rate of fire
long rof_t0 = 0;
long rof_tprev = 0;
int rof_count = 0;
float avg_fps = 0; // Average Feet Per Second
float shot_num = 0; //Shot Counter
int disp_rot = 0; //Display Rotation (0 = Horizontal, 1 = Vertical)
long g1_time = 0; //Gate 1 recorded time
long g2_time = 0; //Gate 2 recorded time
long g1_slow_time = 0;
long g2_slow_time = 0;
long now = 0;
long display_time = 0;
bool g1_trip = false; //Gate 1 current state
bool g2_trip = false; //Gate 2 current state
bool g1_trip0 = false;
bool  g2_trip0 = false;
bool g1_trip_latch = false;
bool g2_trip_latch = false;
int g1_persist = 0;
int g2_persist = 0;
int g1_pin = 2; //Sensor Pin 0
int g2_pin = 3; //Sensor Pin 1
int latch_persist = 2;
float gate_dist = 51.0; //distance in millimeters
float fps = 0; //Frames Per Second
String fpstr = ""; //Frames per second in char form

//Record Gate 1 time
void gate1(){
  g1_time = micros();
  g1_trip_latch = true;
}

//Record Gate 2 time
void gate2(){
  g2_time = micros();
  g2_trip_latch = true;
}

//Set timer to current micros()
void timer(){
  now = micros();
}

//Display vertical
void display_vert(){
  display.setRotation(1);
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(12,10);
  display.print("FPS: ");
  display.setTextSize(2);
  display.setCursor(12,30);
  display.print(fpstr);
  display.setTextSize(1);
  display.setCursor(0,68);
  display.print("SHOT #:");
  display.print(int(shot_num));
  display.setCursor(0,94);
  display.print("AVGFPS:");
  display.print(int(avg_fps));
  display.setCursor(0,81);
  display.print("RND/S:");
  char buffer2[4];
  String srof = dtostrf(rof, 2, 1, buffer2);
  display.print(srof);
  display.setCursor(0,55);
  display.print(joules);
  display.print("J");
  if(g1_trip){
    display.setCursor(15,109);
    display.print("GATE 1");
  }
  if(g2_trip){
    display.setCursor(15,119);
    display.print("GATE 2");
  }
  display.display();
}

//Display horizontal
void display_horz(){
  display.setRotation(0);
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(12,12);
  display.print("FPS: ");
  display.print(fpstr);
  display.setTextSize(1);
  display.setCursor(42,46);
  display.print("SHOT # : ");
  display.print(int(shot_num));
  display.setCursor(42,35);
  display.print("AVG FPS: ");
  display.print(int(avg_fps));
  display.setCursor(42,57);
  display.print("RND/S  : ");
  char buffer[5];
  String srof = dtostrf(rof, 2, 1, buffer);
  display.print(srof);
  display.setCursor(2,35);
  display.print(joules);
  display.print("J");
  if(g1_trip){
    display.setCursor(2,46);
    display.print("GATE 1");
  }
  if(g2_trip){
    display.setCursor(2,57);
    display.print("GATE 2");
  }
  display.display();
}

//Setup (Run once)
void setup() {
  pinMode(10,OUTPUT);
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  attachInterrupt(0,gate1,RISING);
  attachInterrupt(1,gate2,RISING);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  delay(3000); // Pause for 2 seconds
  display.clearDisplay();
  display.setRotation(0);
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,0);
  if(disp_rot){
    display_vert();
  }
  else{
    display_horz();
  }
}

//Run forever
void loop() {
  timer();
  if((now-display_time) > 50000){
    display_time = now;
    g1_trip0 = g1_trip;
    g2_trip0 = g2_trip;
    g1_trip = !digitalRead(2);
    g2_trip = !digitalRead(3);
    
    g1_persist += g1_trip*latch_persist - !g1_trip;
    g1_persist = constrain(g1_persist,0,latch_persist);
    
    g2_persist += g2_trip*latch_persist - !g2_trip;
    g2_persist = constrain(g2_persist,0,latch_persist);

    if((g2_trip) && (!g2_trip0)){
      g2_slow_time = millis();
    }
    if((g1_trip) && (!g1_trip0)){
      g1_slow_time = millis();
    }
    
    
    
   //---------------------------------------------
    if((!g2_trip) && (g2_trip0)){
      if( (now/1000 - g2_slow_time) >1000){
        disp_rot++;
        disp_rot = disp_rot%2;
        g1_persist = 0;
        g2_persist = 0;
      }
    }
    if(disp_rot){
      display_vert();
    }
    else{
      display_horz();
    }
    //----------------------------------------------
    //Reset
    if((!g1_trip) && (g1_trip0)){
      if( (now/1000 - g1_slow_time) >1000){
        rof = 0;
        //rof_t0 = 0;
        //rof_tprev = 0;
        //rof_count = 0;
        avg_fps = 0;
        shot_num = 0;
        //disp_rot = 0;
        //g1_time = 0;
        //g2_time = 0;
        //g1_slow_time = 0;
        //g2_slow_time = 0;
        //now = 0;
        //display_time = 0;
        //g1_trip = false;
        //g2_trip = false;
        //g1_trip0 = false;
        //g2_trip0 = false;
        //g1_trip_latch = false;
        //g2_trip_latch = false;
        //g1_persist = 0;
        //g2_persist = 0;
      }
    }
    //----------------------------------------------
  }

  
  g1_persist += g1_trip_latch*latch_persist;
  g2_persist += g2_trip_latch*latch_persist;
  g1_trip_latch = false;
  
  if(g1_persist){
    if(g2_trip_latch){
      Serial.println("MEASURING");
      fps = 3281*gate_dist/(g2_time-g1_time);
      if (fps<0){
        fpstr = "ERR1";
      }
      else if (fps>420){
        fpstr = "ERR2";
      }
      else{
        fpstr = String(int(fps));
        avg_fps = (avg_fps*shot_num+fps)/(shot_num+1);
        shot_num++;
        if((g2_time-rof_tprev)>2000000){
          Serial.println("restarting count");
          rof_t0 = g2_time;
          rof_tprev = g2_time;
          rof_count = 0;
          rof = 0.0;
        }
        else{
          Serial.println("continuing count");
          rof_tprev = g2_time;
          rof_count++;
          rof = float(rof_count)*1000000/float(g2_time-rof_t0);
        }
      }
//------------------------
      //fps = 300; //Testing
      joules = 0.0005*bbw*sq(fps*0.3048);
      Serial.print("Joules: ");
      Serial.println(joules);
//------------------------
      Serial.println(fps);
      Serial.println(g2_time-g1_time);
      g1_persist = 0;
      g2_persist = 0;
      if(disp_rot){
        display_vert();
      }
      else{
        display_horz();
      }
    }
  }
  g2_trip_latch = false;
}
