//Notes as of 10/20/2020
// Mining isn't quite there. the handoff isn't up and running
// correct connections get stuck in the correct connection loop (WHITE) and incorrect gets stuck in incorrect loop (BLUE)
// 

#define FIELD_COLOR makeColorHSB(200,60,100)
#define MAX_GAME_TIME 240000 //four Minutes
#define MAX_TIME_BETWEEN_ATTACKS 15000
#define FIRE_DURATION 1300
#define POISON_DURATION 3000
#define VOID_DURATION 5000
#define FIRE_DELAY_TIME 90
#define FIRE_EXTRA_TIME 1000
#define VOID_DELAY_TIME 90
#define VOID_EXTRA_TIME 3000
#define POISON_DELAY_TIME 500
#define POISON_EXTRA_TIME 4000
#define DRAGON_WAIT_TIME 6000
#define DRAGON_ATTACK_DURATION 1000
#define IGNORE_TIME 700
#define GOLD_MINE_TIME 6000 //cause half of that is 3 sec.
#define TREASURE_SPAWN_TIME 2000

//[A][B][C][D][E][F]

enum blinkType {FIELD,PLAYER}; //[A]
byte blinkType;
enum attackSignal {INERT,FIRE,POISON,VOID,RESOLVE,CORRECT,INCORRECT}; //[B][C][D]
byte attackSignal=INERT;
byte hiddenAttackSignal;
byte sendData;
byte playerFaceSignal[6]={FIRE,INERT,POISON,INERT,VOID,INERT}; //attacks correspond to treasure types for player pieces
byte permanentPlayerFaceType[6]={FIRE,INERT,POISON,INERT,VOID,INERT};
bool isDragon=false;

int playerScore=0;
byte ignoredFaces[6]={0,0,0,0,0,0};
byte luck=3;
bool isDead=false;

byte treasureType=0; // 1 for ruby, 2 for emerald, 3 for Gold
Color treasureColor[3]={RED,GREEN,YELLOW};

byte extraTime=0; //for setting the delay longer based on attack type

Timer delayTimer;
Timer attackDurationTimer;
Timer dragonWaitTimer;
Timer dragonAttackTimer;
Timer ignoreAttacksTimer;
Timer goldMineTimer;
Timer treasureSpawnTimer;

void setup() {
  randomize();
  byte randomTreasure=random(99);
  treasureType=(randomTreasure%3)+1;
  blinkType=FIELD;
}

void loop() {

  switch(attackSignal){
    case INERT:
      inertLoop();
      break;
    case FIRE:
      fireLoop();
      break;
    case POISON:
      poisonLoop();
      break;
    case VOID:
      voidLoop();
      break;
    case RESOLVE:
      resolveLoop();
      break;
    case CORRECT:
      correctLoop();
      break;
    case INCORRECT:
      incorrectLoop();
      break;
  }
  
  //sets the dragon
  if(buttonMultiClicked()){
    byte clicks=buttonClickCount();
    if(clicks==3){
      isDragon=!isDragon;
      dragonWaitTimer.set(DRAGON_WAIT_TIME);
    }
  }

  //dragonAttacks
  //CHANGE THIS:
  // add time to the DRAGON_WAIT_TIME according to which attack you are doing
  // because you want to give void longer to dissapate and less time to fire
  //also add something so that there's a chance that no attack will happen at all
  
  //PLAYER Pieces
  //sets the player piece
  if(isAlone()){
    if(buttonDoubleClicked()){
        blinkType=PLAYER;
        isDead=false;
        luck=3;
    }
  }

  if(blinkType==FIELD){
   sendData = (blinkType<<5) + (attackSignal<<2);
   setValueSentOnAllFaces(sendData);
  }else if (blinkType==PLAYER){
    FOREACH_FACE(f){
      sendData = (blinkType<<5) + (playerFaceSignal[f]<<2);
      setValueSentOnFace(sendData,f);
    }
  }

   displayLoop();
  
}

void inertLoop(){

   if(isDragon){
     if(dragonWaitTimer.isExpired()){
      byte maybeAttack = random(100);
      if(maybeAttack>40){
        if(noNeighborsAttacking){
          byte whichAttack=random(99);
          whichAttack=(whichAttack%3)+1;
          if(whichAttack==1){
            attackSignal=POISON;
            extraTime=POISON_EXTRA_TIME;
          }else if(whichAttack==2){
            attackSignal=FIRE;
            extraTime=FIRE_EXTRA_TIME;
          }else if(whichAttack==3){
            attackSignal=VOID;
            extraTime=VOID_EXTRA_TIME;
          }
          attackDurationTimer.set(FIRE_DURATION);
        }
      }
     }
  }

  //All things not Player Piece related
  if(blinkType==FIELD){
  
    //treasure spawning
      if(treasureSpawnTimer.isExpired()){
        if(treasureType==0){
          treasureType=(random(99)%3)+1;
        }
      }

    //recieves attacks and delays sending them until it's time 
    if(ignoreAttacksTimer.isExpired()){
      FOREACH_FACE(f) {
        if (!isValueReceivedOnFaceExpired(f)) {//a neighbor!
          if(getBlinkType(getLastValueReceivedOnFace(f)) == FIELD){
            if (getAttackSignal(getLastValueReceivedOnFace(f)) == FIRE || getAttackSignal(getLastValueReceivedOnFace(f))==POISON || getAttackSignal(getLastValueReceivedOnFace(f))==VOID) {
              if(hiddenAttackSignal==INERT){
                hiddenAttackSignal=getAttackSignal(getLastValueReceivedOnFace(f));
                if(hiddenAttackSignal==FIRE){
                  //set timer and display fire but don't BE fire until timer is up
                  delayTimer.set(FIRE_DELAY_TIME);
                }else if(hiddenAttackSignal==POISON){
                  //setTimer for poisionDisplay but don't BE poison until timer is out
                  delayTimer.set(POISON_DELAY_TIME);
                }else if(hiddenAttackSignal==VOID){
                  //set timer and display void, but don't BE void until timer is out
                  delayTimer.set(VOID_DELAY_TIME);
                }
              }
            }
          }
        }
      }
    }
  
  //When the delay is over, it's time to send the signal and set the duration of the attack
   if(!isDragon){
     if(delayTimer.isExpired()){
       attackSignal=hiddenAttackSignal;
       switch(hiddenAttackSignal){
          case FIRE:
            attackDurationTimer.set(FIRE_DURATION);
            break;
          case POISON:
            attackDurationTimer.set(POISON_DURATION);
            break;
          case VOID:
            attackDurationTimer.set(VOID_DURATION);
            break;
       }
       hiddenAttackSignal=INERT;
     }
    }

   //Mining
   miningLoop();
 }
 
   if(blinkType==PLAYER){
      if(luck<1){
        isDead=true;
      }
      //listen for damage
      FOREACH_FACE(f) {
        if (!isValueReceivedOnFaceExpired(f)) {//a neighbor!
          if (getBlinkType(getLastValueReceivedOnFace(f)) == FIELD){
            if (getAttackSignal(getLastValueReceivedOnFace(f)) == FIRE || getAttackSignal(getLastValueReceivedOnFace(f))==POISON || getAttackSignal(getLastValueReceivedOnFace(f))==VOID) {
              if(ignoredFaces[f]==0){
                luck--;
                ignoredFaces[f]=1; 
              }
            }
          }
        }else{
          ignoredFaces[f]=0;
        }
      }
    // Player mining 
    FOREACH_FACE(f){
      if (!isValueReceivedOnFaceExpired(f)) {
        if (getBlinkType(getLastValueReceivedOnFace(f)) == FIELD){
          if (getAttackSignal(getLastValueReceivedOnFace(f)) == CORRECT){
            if(ignoredFaces[f]==0){
              if(permanentPlayerFaceType[f]==4){
                playerScore+=3;
              }else{
                playerScore++;
              }
              ignoredFaces[f]=1;
            }
            playerFaceSignal[f]=CORRECT;
          }else if(getAttackSignal(getLastValueReceivedOnFace(f)) == INCORRECT){
            if(ignoredFaces[f]==0){
              luck--;
              ignoredFaces[f]=1;
            }
            playerFaceSignal[f]=INCORRECT;
          }
        }
      }else{
        ignoredFaces[f]=0;
        playerFaceSignal[f]=permanentPlayerFaceType[f];
      }
    }
  }
}

void fireLoop(){
    if(attackDurationTimer.isExpired()){
      attackSignal=RESOLVE;
    }
    FOREACH_FACE(f) {
      if (!isValueReceivedOnFaceExpired(f)) {//a neighbor!
        if (getAttackSignal(getLastValueReceivedOnFace(f)) == FIELD){
          if (getAttackSignal(getLastValueReceivedOnFace(f)) == INERT){
            attackSignal = FIRE;
          }
        }
      }
    }
}

void poisonLoop(){
    if(attackDurationTimer.isExpired()){
      attackSignal=RESOLVE;
    }
    FOREACH_FACE(f) {
      if (!isValueReceivedOnFaceExpired(f)) {//a neighbor!
        if (getAttackSignal(getLastValueReceivedOnFace(f)) == FIELD){
          if (getAttackSignal(getLastValueReceivedOnFace(f)) == INERT){
            attackSignal = POISON;
          }
        }
      }
    }
}

// PUT miningLoop() IN VOID LOOP

void voidLoop(){
    if(attackDurationTimer.isExpired()){
      attackSignal=RESOLVE;
    }  
    FOREACH_FACE(f) {
      if (!isValueReceivedOnFaceExpired(f)) {//a neighbor!
        if (getAttackSignal(getLastValueReceivedOnFace(f)) == FIELD){
          if (getAttackSignal(getLastValueReceivedOnFace(f)) == INERT){
            attackSignal = VOID;
          }
        }
      }
    }
  
}

void resolveLoop(){
  attackSignal=INERT;

  ignoreAttacksTimer.set(IGNORE_TIME);

  byte increasingWaitTime = map(millis(),0,MAX_GAME_TIME,0,MAX_TIME_BETWEEN_ATTACKS);
  byte timeBetweenAttacks = (MAX_TIME_BETWEEN_ATTACKS-increasingWaitTime+extraTime);
  if(isDragon){
    dragonWaitTimer.set(timeBetweenAttacks);
  }
  
    FOREACH_FACE(f) {
      if (!isValueReceivedOnFaceExpired(f)) {//a neighbor!
        if (getAttackSignal(getLastValueReceivedOnFace(f)) == FIELD){
          if (getAttackSignal(getLastValueReceivedOnFace(f)) == FIRE || getAttackSignal(getLastValueReceivedOnFace(f))==POISON || getAttackSignal(getLastValueReceivedOnFace(f))==VOID) {//This neighbor isn't in RESOLVE. Stay in RESOLVE
            attackSignal = RESOLVE;
          }
        }
      }
    }
}

void correctLoop(){
  //if a piece is correctly mined and it hears a PLAYER mirror that, then resolve
  FOREACH_FACE(f){
    if (!isValueReceivedOnFaceExpired(f)){
      if (getBlinkType(getLastValueReceivedOnFace(f))==PLAYER){
        if (getAttackSignal(getLastValueReceivedOnFace(f))==CORRECT){
           treasureType=0;
           treasureSpawnTimer.set(TREASURE_SPAWN_TIME);
           attackSignal=INERT;
        }
      }
    }
  }
}

void incorrectLoop(){
  FOREACH_FACE(f){
    if (!isValueReceivedOnFaceExpired(f)){
      if (getBlinkType(getLastValueReceivedOnFace(f))==PLAYER){
        if (getAttackSignal(getLastValueReceivedOnFace(f))==INCORRECT){
           attackSignal=INERT;
        }
      }
    }
  }
}

void displayLoop(){
  if(isDragon){
    setColor(MAGENTA);
  }else if(blinkType==PLAYER){
    if(isDead){
      setColor(WHITE);
    }else{
      setColor(FIELD_COLOR);
      setColorOnFace(RED,0);
      setColorOnFace(GREEN,2);
      setColorOnFace(YELLOW,4);
    }
  }else if(blinkType==FIELD){
    switch(attackSignal){
      case FIRE:
        setColor(ORANGE);
        break;
      case POISON:
        setColor(GREEN);
        break;
      case VOID:
        setColor(OFF);
        break;
      case INERT:
        fieldDisplay();
        break;
      case RESOLVE:
        break;
    }
  }
  
  
}

void fieldDisplay(){
  FOREACH_FACE(f){
    if (!isValueReceivedOnFaceExpired(f)) {
        if (getBlinkType(getLastValueReceivedOnFace(f)) == FIELD){
          setColorOnFace(FIELD_COLOR,f);
        }else{
          if(!treasureSpawnTimer.isExpired()){
            setColorOnFace(FIELD_COLOR,f);
          }else{
            setColorOnFace(treasureColor[treasureType-1],f);
          }
        }
    }else{
      if(!treasureSpawnTimer.isExpired()){
        setColorOnFace(FIELD_COLOR,f);
      }else{
        setColorOnFace(treasureColor[treasureType-1],f);
      }
    }
  }
}

bool noNeighborsAttacking(){
  byte neighborsAttacking=0;
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) {//a neighbor!
      if (getAttackSignal(getLastValueReceivedOnFace(f)) == FIRE || getAttackSignal(getLastValueReceivedOnFace(f))==POISON || getAttackSignal(getLastValueReceivedOnFace(f))==VOID) {
        neighborsAttacking++;
      }
    }
  }
  if(neighborsAttacking==0){
    return true;
  }else{
    return false;
  }
}

void miningLoop(){


  // In this case I've used the attacks as treasure types, but only when
  // coupled with the PLAYER blinktype. 
  // FIRE is ruby, POISON is emerald, and VOID is gold
  FOREACH_FACE(f){
    if (!isValueReceivedOnFaceExpired(f)){
      if (getBlinkType(getLastValueReceivedOnFace(f)) == PLAYER){
        if (getAttackSignal(getLastValueReceivedOnFace(f)) == FIRE){
          if(treasureType==1){
            attackSignal=CORRECT;
          }else{
            attackSignal=INCORRECT;
          }
        }else if (getAttackSignal(getLastValueReceivedOnFace(f)) == POISON){
          if(treasureType==2){
            attackSignal=CORRECT;
          }else{
            attackSignal=INCORRECT;
          }
        }else if (getAttackSignal(getLastValueReceivedOnFace(f)) == VOID){
          if(treasureType==3){
            if(goldMineTimer.isExpired()){
              goldMineTimer.set(GOLD_MINE_TIME);
            }else if (goldMineTimer.getRemaining()<GOLD_MINE_TIME/2){
              attackSignal=CORRECT;
            }
          }else{
            attackSignal=INCORRECT;
          }
        }
      }
    }
  }
}

byte getBlinkType(byte data){
  return(data>>5);
}

byte getAttackSignal(byte data){
  return(data>>2&7);
}
