
#define RANDOMSCORE 200

int playerScore = 0;

void setup() {
  randomize();
}

void loop() {
  
  if(buttonDoubleClicked()){
    playerScore=random(200)+50;
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
}
