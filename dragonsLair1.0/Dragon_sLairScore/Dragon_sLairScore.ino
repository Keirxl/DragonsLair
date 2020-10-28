
#define RANDOMSCORE 200
#define BLINK_WAIT_TIME 700
#define BLINK_PAUSE_TIME 3000

int playerScore = 0;
byte hundreds;
byte tens;
byte ones;

Timer treasureSpawnTimer;
Timer dragonWaitTimer;

void setup() {
  randomize();
}

void loop() {
  
  if(buttonDoubleClicked()){
    //playerScore=random(200)+50;
    playerScore=542;
    hundreds =(playerScore-(playerScore%100))/100;
    tens = ((playerScore%100)-(playerScore%10))/10;
    ones = (playerScore%10);
  }

  scoreDisplay();

}

void scoreDisplay(){
        setColorOnFace(dim(YELLOW,120),0);
        setColorOnFace(dim(YELLOW,120),1);
        setColorOnFace(dim(GREEN,120),2);
        setColorOnFace(dim(GREEN,120),3);
        setColorOnFace(dim(RED,120),4);
        setColorOnFace(dim(RED,120),5);

  if(dragonWaitTimer.isExpired()){
    if(treasureSpawnTimer.isExpired()){
      if(hundreds>0){
        setColorOnFace(YELLOW,0);
        setColorOnFace(YELLOW,1);
        hundreds--;
      }else if(tens>0){
        setColorOnFace(GREEN,2);
        setColorOnFace(GREEN,3);
        tens--;
      }else if(ones>0){
        setColorOnFace(RED,4);
        setColorOnFace(RED,5);
        ones--;
      }else{
        dragonWaitTimer.set(BLINK_PAUSE_TIME);
        hundreds =(playerScore-(playerScore%100))/100;
        tens = ((playerScore%100)-(playerScore%10))/10;
        ones = (playerScore%10);
      }
      treasureSpawnTimer.set(BLINK_WAIT_TIME);
    }
  }
  
}
