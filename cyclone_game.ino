#include <Adafruit_CircuitPlayground.h>
#include <AsyncDelay.h>
AsyncDelay delay_Cs; 
AsyncDelay delay_5s;
AsyncDelay delay_1s;
bool sparkle = 0;
int LED = -1;
bool start = 0;
int slow = 500;
int time = 200;
int level = -1;
int Score = 0;
volatile const byte slideSwitch = 7;
volatile const byte Lbut = 4;
volatile const byte Rbut = 5;
bool Slowmoflag = 0;
bool clicking =0;
bool lost = 1;
bool Winner = 1;
bool countDown = 1;
int beepH = 440;
int beepC = 261;
bool winfuncflag = 0;
bool losefuncflag = 0;
bool sadflag = 0;
bool notlosing = 0;
bool done = 0;
int slowmoDelay[10] = {3000,2800,2600,2400,2200,2000,1800,1600,1400,1200};//array for the wait duration for the slow mo, in other words how long the slow-mo mode is active for.
int sloar[10]={600,550,500,450,400,350,300,250,200,150};//array for the slow-mo values
int timar[10]={200,190,180,170,160,150,140,130,120,110};// array for the normal run times
//int soundAry[5]={146.83,261.63,440,987.77,1760};
void setup() {
  // put your setup code here, to run once:
  CircuitPlayground.begin();
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(slideSwitch), startstop, CHANGE);//creates the interupt for switch
  attachInterrupt(digitalPinToInterrupt(Lbut), SlowMo, RISING);
  attachInterrupt(digitalPinToInterrupt(Rbut), click, RISING);
 
}

void loop() {
  // put your main code here, to run repeatedly:
  if (delay_5s.isExpired() && Winner == 1){//Will print "get ready to play" after 5 seconds then waits an additonal second for the user to get ready. Probably will make these into their own function.
      delay(1000);
      Winner = 0;
      countDown = 1;
      sparkle = 0;
    }
  if (delay_5s.isExpired() && lost == 1){//Will print "get ready to play" after 5 seconds then waits an additonal second for the user to get ready.  
    delay(1000);
    lost = 0;
    countDown = 1;
  }
  if (countDown == 1){
    Serial.println("Get Ready! LED to hit on is #7. ");
    for (int i = 3; i>0 && start == 1;i--){
      Serial.println(i);
      delay(1000); 
      if (i==1){
        Serial.println("GO");
        Nextlevel();
      }
    }
    countDown = 0;
  }
  if (start == 1 && countDown == 0 && !lost && !Winner){//where the game actually is. it only plays when the switch is on, countdown is done and lost is false and winner is false. 
    LED++;//selects the pixel linearly 1-10
    CircuitPlayground.setPixelColor(LED, 0, 0, 255);//LED color blue
    delay(time);//controls the speed of the game
    //Serial.println(time);//this is just for testing purposes
    CircuitPlayground.clearPixels();

    if (LED ==10){//resets the LED count to -1 so it can get all 10 LEDS
      LED = -1;
    }
    if (Slowmoflag == 1){//this is where the slowmo happens
      time = timar[level]+sloar[level];//choses normal time depending on level and choses slowmo time also depending on level and the sum of these values gets added to time 
      if (delay_Cs.isExpired()){//Waits the certain time depending on level, to change the time back to what the normal time should be depending on the level 
        time = timar[level];
        Slowmoflag = 0;
      }
    }
    int randNum = random(10);
    int randColor = random(150);
    for (int i = 0; i<=3 && sparkle == 1 && start == 1;i++){
      for (int i = 0; i<=9 && start == 1; i++){
        CircuitPlayground.setPixelColor(i,CircuitPlayground.colorWheel(randColor * 256/10));
        delay(50);
      }
      CircuitPlayground.clearPixels();
    }
  }
  if (sadflag == 1&& start == 1){
    CircuitPlayground.setPixelColor(0,255,0,0);
    CircuitPlayground.setPixelColor(1,255,0,0);
    CircuitPlayground.setPixelColor(2,255,0,0);
    CircuitPlayground.setPixelColor(3,255,0,0);
    CircuitPlayground.setPixelColor(4,255,0,0);
    CircuitPlayground.setPixelColor(5,255,0,0);
    CircuitPlayground.setPixelColor(6,255,0,0);
    CircuitPlayground.setPixelColor(7,255,0,0);
    CircuitPlayground.setPixelColor(8,255,0,0);
    CircuitPlayground.setPixelColor(9,255,0,0);
    CircuitPlayground.setPixelColor(10,255,0,0);
    CircuitPlayground.playTone(beepH,1000);
    delay(500);
    CircuitPlayground.clearPixels();
    delay(500);
    sadflag = 0;
  }
  clicking = 0;
  //notlosing = 0;
  if (done == 1 && delay_1s.isExpired()){
    
    notlosing = 0;
    done = 0;
  }
}

void startstop(){//starts the game when the switch is to the left and turns off when the switch is to the right
  CircuitPlayground.clearPixels();
  start = CircuitPlayground.slideSwitch();
  if (start == 1){
    countDown = 1;
    Serial.println("Game on");
    lost = 0;
    Winner = 0;
  }else if(start == 0){
    Serial.println("Game off");
    CircuitPlayground.clearPixels();
    level = -1;
    slow = 500;
    time = 200;
    Score = 0;
    lost = 1;
    Winner = 1;
  }
}
void SlowMo(){//left button interrupt which is the slow-mo button
  Serial.println("slowmo");
  Slowmoflag = 1;
  delay_Cs.start(slowmoDelay[level], AsyncDelay::MILLIS); // makes the timer cout up by 1 second
}
void click(){// right button interrupt and is where we detect if they click on the currect LED
 if (!clicking){
  clicking = 1;
  if (clicking == 1 && LED == 7){//only changes level when they have clicked on the correct LED which is the seventh LED
    //CircuitPlayground.clearPixels();
    CircuitPlayground.playTone(beepC,1000);
    Serial.println("Good hit!!");
    Nextlevel();
    delay_1s.start(1000, AsyncDelay::MILLIS);
    notlosing = 1;
  }else if(clicking == 1 && notlosing == 0){// if they miss it calls the lose function
    Lose();
  }
 }
}
void Nextlevel(){// determines the Level
  CircuitPlayground.clearPixels();
  Serial.println("Level: ");
  Serial.println(level+2);
  delay(1000);
  level = level + 1;//increases the level by one when the games first starts and when the get a correct hit.
  LED = -1;
  done = 1;
  Scoreboard();//calls score function
  time = timar[level]; //makes sure the correct game speed is set.
  if (level == 9){//When the user hits level 10 calls the win function
    colorFirework();
    win();
    reset();
  }
}
void Scoreboard(){//adds and prints the suers current score
  Score = Score + 20;
  Serial.print("Your score: ");
  Serial.println(Score);
}
void Lose(){
  CircuitPlayground.clearPixels();
  //CircuitPlayground.playTone(beepH,1000);
  Serial.println("You lost");
  lost = 1;//sets the lost flag to true, pausing the game 
  Serial.print("Your Final Score: ");
  Serial.println(Score);
  sadflag = 1;
  sad(); //this is going to play the sad tone and turn the LEDs to a sad color
  delay_5s.start(5000, AsyncDelay::MILLIS);// starts the 5 second timer
  losefuncflag = 1;
  reset();
}
void win(){
  Serial.println("YAY you won!!");
  colorFirework(); //this is going to be the celebreation lights and happy sound
  delay_5s.start(5000, AsyncDelay::MILLIS);//starts the 5 sec timer 
  winfuncflag = 1;
  Winner = 1;
  
}
void colorFirework(){
  sparkle = 1;
}
void sad(){
  sadflag = 1;
}
void reset(){
  if (winfuncflag == 1 && start == 1){
    level = -1;
    Score = 0;
    winfuncflag = 0;
  }
  else if (losefuncflag == 1 && start == 1){
    Score = 0;
    level = -1;
    losefuncflag = 0;
  }
}

