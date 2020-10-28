
#define RANDOMSCORE 800
#define BLINK_WAIT_TIME 700
#define BLINK_PAUSE_TIME 3000
#define BLINK_SHOW_TIME 200

int playerScore = 0;
byte hundreds;
byte tens;
byte ones;
byte whichScore=10;

Timer treasureSpawnTimer;
Timer dragonWaitTimer;
Timer attackDurationTimer;

void setup() {
  randomize();
}

void loop() {
  
  if(buttonDoubleClicked()){
    playerScore=random(RANDOMSCORE);
    hundreds =(playerScore-(playerScore%100))/100;
    tens = ((playerScore%100)-(playerScore%10))/10;
    ones = (playerScore%10);
  }

  scoreDisplay();

}

void scoreDisplay(){


  if(dragonWaitTimer.isExpired()){
    if(treasureSpawnTimer.isExpired()){
      if(hundreds>0){
        whichScore=0;
        hundreds--;
      }else if(tens>0){
        whichScore=2;
        tens--;
      }else if(ones>0){
        whichScore=4;
        ones--;
      }else{
        whichScore=10;
        dragonWaitTimer.set(BLINK_PAUSE_TIME); //how long to wait until starting over
        hundreds = (playerScore-(playerScore%100))/100;
        tens = ((playerScore%100)-(playerScore%10))/10;
        ones = (playerScore%10);
      }
      attackDurationTimer.set(BLINK_SHOW_TIME); //how long to show the flashes
      treasureSpawnTimer.set(BLINK_WAIT_TIME); // how long to wait to flash again
    }
  }
   setColorOnFace(dim(YELLOW,120),0);
   setColorOnFace(dim(YELLOW,120),1);
   setColorOnFace(dim(GREEN,120),2);
   setColorOnFace(dim(GREEN,120),3);
   setColorOnFace(dim(RED,120),4);
   setColorOnFace(dim(RED,120),5);

  if(!attackDurationTimer.isExpired()){
      if(whichScore==0){
        setColorOnFace(YELLOW,0);
        setColorOnFace(YELLOW,1);
      }else if(whichScore==2){
         setColorOnFace(GREEN,2);
         setColorOnFace(GREEN,3);
      }else if(whichScore==4){
        setColorOnFace(RED,4);
        setColorOnFace(RED,5);
      }
  }
  
}
