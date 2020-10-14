#define FIELD_COLOR makeColorHSB(200,60,60)
#define FIRE_ATTACK_LENGTH 1500
#define FIRE_DELAY_TIME 500
#define POISON_DELAY_TIME 1000
//[A][B][C][D][E][F]

enum blinkType {FIELD,PLAYER}; //[A]
byte blinkType;
enum attackSignal {INERT,FIRE,POISON,VOID,RESOLVE}; //[B][C][D]
byte attackSignal=INERT;
byte hiddenAttackSignal;

byte sendData;

bool isDragon=false;

Timer fireAttackLingerTimer;
Timer fireDelayTimer;
Timer poisonDelayTimer;

void setup() {
  // put your setup code here, to run once:

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
    }
  }

   sendData = (blinkType<<5) + (attackSignal<<2);
   setValueSentOnAllFaces(sendData);
  
}

void inertLoop(){
  
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) {//a neighbor!
      if (getAttackSignal(getLastValueReceivedOnFace(f)) == FIRE || POISON || VOID) {
        if(hiddenAttackSignal==INERT){
          hiddenAttackSignal=getAttackSignal(getLastValueReceivedOnFace(f));
          if(hiddenAttackSignal==FIRE){
            //set timer and display fire but don't BE fire until timer is up
            fireDelayTimer.set(FIRE_DELAY_TIME);
            //set Timer for how long to be fire? or do in fire loop
          }else if(hiddenAttackSignal==POISON){
            //setTimer for poisionDisplay but don't BE poison until timer is out
            poisonDelayTimer.set(POISON_DELAY_TIME);
            //set timer for how long to be poision.
            
          }else if(hiddenAttackSignal==VOID){
            //set timer and display void, but don't BE void until timer is out
            fireDelayTimer.set(FIRE_DELAY_TIME);
            //set Timer for how long to be VOID
          }
        }
      }
    }
  }




  
}

void fireLoop(){
  
}

void poisonLoop(){
  
}

void voidLoop(){
  
}

void resolveLoop(){
  attackSignal=INERT;
  
  FOREACH_FACE(f) {
    if (!isValueReceivedOnFaceExpired(f)) {//a neighbor!
      if (getAttackSignal(getLastValueReceivedOnFace(f)) == FIRE || POISON || VOID) {//This neighbor isn't in RESOLVE. Stay in RESOLVE
        attackSignal = RESOLVE;
      }
    }
  }
}

void displayLoop(){
  if(isDragon){
    setColor(MAGENTA);
  }
  
}

byte getBlinkType(byte data){
  return(data>>5);
}

byte getAttackSignal(byte data){
  return(data>>2&7);
}
