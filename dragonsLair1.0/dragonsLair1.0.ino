#define FIELD_COLOR makeColorHSB(200,60,100)
#define FIRE_DURATION 1300
#define POISON_DURATION 3000
#define VOID_DURATION 5000
#define FIRE_DELAY_TIME 175
#define VOID_DELAY_TIME 250
#define POISON_DELAY_TIME 750
#define DRAGON_WAIT_TIME 7000
#define DRAGON_ATTACK_DURATION 1000
//[A][B][C][D][E][F]

enum blinkType {FIELD,PLAYER}; //[A]
byte blinkType;
enum attackSignal {INERT,FIRE,POISON,VOID,RESOLVE}; //[B][C][D]
byte attackSignal=INERT;
byte hiddenAttackSignal;

byte sendData;

bool isDragon=false;

Timer delayTimer;
Timer attackDurationTimer;
Timer dragonWaitTimer;
Timer dragonAttackTimer;

void setup() {
  randomize();
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
  }
  
  //sets the dragon
  if(buttonMultiClicked()){
    byte clicks=buttonClickCount();
    if(clicks==3){
      isDragon=true;
      dragonWaitTimer.set(DRAGON_WAIT_TIME);
    }
  }

  //dragonAttacks
  //CHANGE THIS:
  // add time to the DRAGON_WAIT_TIME according to which attack you are doing
  // because you want to give void longer to dissapate and less time to fire
  if(isDragon){
     if(dragonWaitTimer.isExpired()){
        if(noNeighborsAttacking){
          dragonAttackTimer.set(DRAGON_ATTACK_DURATION);
          byte whichAttack=random(3);
          if(whichAttack==1){
            attackSignal=FIRE;
          }else if(whichAttack==2){
            attackSignal=VOID;
          }else if(whichAttack==3){
            attackSignal=POISON;
          }
        }
      dragonWaitTimer.set(DRAGON_WAIT_TIME);
     }
  }


   sendData = (blinkType<<5) + (attackSignal<<2);
   setValueSentOnAllFaces(sendData);

   displayLoop();
  
}

void inertLoop(){

  //recieves attacks and delays sending them until it's time 
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) {//a neighbor!
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

//When the delay is over, it's time to send the signal and set the duration of the attack
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

void fireLoop(){
  if(isDragon){
    if(dragonAttackTimer.isExpired()){
      attackSignal=INERT;
    }
  }else{
    if(attackDurationTimer.isExpired()){
      attackSignal=RESOLVE;
    }
  }
}

void poisonLoop(){
  if(isDragon){
    if(dragonAttackTimer.isExpired()){
      attackSignal=INERT;
    }
  }else{
    if(attackDurationTimer.isExpired()){
      attackSignal=RESOLVE;
    }
  }
}

void voidLoop(){
  if(isDragon){
    if(dragonAttackTimer.isExpired()){
      attackSignal=INERT;
    }
  }else{
    if(attackDurationTimer.isExpired()){
      attackSignal=RESOLVE;
    }
  }
}

void resolveLoop(){
  attackSignal=INERT;

  if(!isDragon){
    FOREACH_FACE(f) {
      if (!isValueReceivedOnFaceExpired(f)) {//a neighbor!
        if (getAttackSignal(getLastValueReceivedOnFace(f)) == FIRE || getAttackSignal(getLastValueReceivedOnFace(f))==POISON || getAttackSignal(getLastValueReceivedOnFace(f))==VOID) {//This neighbor isn't in RESOLVE. Stay in RESOLVE
          attackSignal = RESOLVE;
        }
      }
    }
  }
}

void displayLoop(){
  if(isDragon){
    setColor(MAGENTA);
  }else{
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
        setColor(FIELD_COLOR);
        break;
      case RESOLVE:
        setColor(BLUE);
        break;
    }
  }
  
  
}

bool noNeighborsAttacking(){
  byte neighborsAttacking=0;
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) {//a neighbor!
      if (getAttackSignal(getLastValueReceivedOnFace(f)) == FIRE || POISON || VOID) {
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

byte getBlinkType(byte data){
  return(data>>5);
}

byte getAttackSignal(byte data){
  return(data>>2&7);
}
