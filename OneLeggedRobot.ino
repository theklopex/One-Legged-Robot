#include <Servo.h> 
 
Servo hip;   // create servo object to control a servo 
             // a maximum of eight servo objects can be created 
Servo knee;  // create servo object to control a servo 
 
#define SERVOMIN  0
#define SERVOMAX  180

typedef struct Position
{
  uint16_t hip;
  uint16_t knee;
} Position;

Position curPos = {0};

Position straightLeg(Position);

void setup() {
  bool debug = false;
  Serial.begin(9600);
  Serial.println("\n\n\nThe one legged robot wants to walk!");

  curPos = straightLeg(curPos);

  hip.attach(8);  // attaches the servo on pin 8 to the servo object 
  knee.attach(9);  // attaches the servo on pin 9 to the servo object 

  delay(10);
  delay(1000);
}

void
printPosition(
  Position pos)
{
    Serial.print("hip = ");
    Serial.print(pos.hip);
    Serial.print(", knee = ");
    Serial.print(pos.knee);
}

uint16_t
fixBounds(
  uint16_t pulselen)
{
  if (pulselen > SERVOMAX)
  {
    return SERVOMAX;
  }
  if (pulselen < SERVOMIN)
  {
    return SERVOMIN;
  }
  return pulselen;
}

void
stepTo(
  Position pos)
{
  uint16_t sensorValue;

  pos.hip = fixBounds(pos.hip);
  hip.write(pos.hip);              // tell servo to go to position in variable 'pos' 
  delay(15);                       // waits 15ms for the servo to reach the position 

  pos.knee = fixBounds(pos.knee);
  knee.write(pos.knee);              // tell servo to go to position in variable 'pos' 
  delay(15);                       // waits 15ms for the servo to reach the position 
}

uint8_t stepAmount = 10;

Position
advancePosition(
  Position oldPos,
  Position newPos)
{
  Position nextPos;
  bool debug = false;

  if (debug)
  {
    Serial.print("oldPos = ");
    printPosition(oldPos);
    Serial.print(" newPos = ");
    printPosition(newPos);
  }
  
  nextPos.hip = oldPos.hip;
  if (newPos.hip > oldPos.hip)
  {
    nextPos.hip += stepAmount;
    if (nextPos.hip > newPos.hip)
    {
      // Fix it if we overstep.
      nextPos.hip = newPos.hip;
    }
  }
  if (newPos.hip < oldPos.hip)
  {
    // This avoids going "negative" with unsigned integers
    if ((oldPos.hip - newPos.hip) < stepAmount)
    {
      // Fix it if we overstep.
      nextPos.hip = newPos.hip;
    }
    else
    {
      nextPos.hip -= stepAmount;
    }
  }

  nextPos.knee = oldPos.knee;
  if (newPos.knee > oldPos.knee)
  {
    nextPos.knee += stepAmount;
    if (nextPos.knee > newPos.knee)
    {
      // Fix it if we overstep.
      nextPos.knee = newPos.knee;
    }
  }
  if (newPos.knee < oldPos.knee)
  {
    // This avoids going "negative" with unsigned integers
    if ((oldPos.knee - newPos.knee) < stepAmount)
    {
      // Fix it if we overstep.
      nextPos.knee = newPos.knee;
    }
    else
    {
      nextPos.knee -= stepAmount;
    }
  }

  if (debug)
  {
    Serial.print("nextPos = ");
    printPosition(nextPos);
    Serial.print("\n");
  }
  return nextPos;
}

bool
operator==(
  Position pLeft,
  Position pRight)
{
  if (pLeft.hip != pRight.hip)
  {
    return false;
  }
  if (pLeft.knee != pRight.knee)
  {
    return false;
  }

  return true;
}

Position
glideTo(
  Position oldPos,
  Position newPos)
{
  bool debug = false;
  Position curPos;

  if (debug)
  {
    Serial.print("glideTo(");
    printPosition(oldPos);
    Serial.print("->");
    printPosition(newPos);
    Serial.print("\n");
  }
  if (oldPos.hip == newPos.hip)
  {
    // Apparently, they are equal.  Make them unequal to guarantee that we set the servo.
    if (debug)
    {
      Serial.println("They were equal");
    }
    oldPos.hip += stepAmount;
  }

  curPos = oldPos;
  do
  {
    if (debug)
    {
      Serial.print("cur = ");
      printPosition(curPos);
      Serial.print("new = ");
      printPosition(newPos);
    }

    curPos = advancePosition(curPos, newPos);
    if (debug)
    {
      Serial.print("cur = ");
      printPosition(curPos);
      Serial.print("\n");
    }
    stepTo(curPos);
  } while (!(newPos == curPos));

  if (debug)
  {
    Serial.print("New Position = ");
    printPosition(curPos);
    Serial.print("\n");
  }
  return curPos;
}

Position
straightLeg(
  Position curPos)
{
  Position straightPosition;
  Serial.print("straightPosition()  ");

  straightPosition.hip = (SERVOMIN + SERVOMAX) / 2;
  straightPosition.knee = SERVOMIN;
 
  printPosition(straightPosition);
  Serial.print("\n");

  straightPosition = glideTo(curPos, straightPosition);

  return straightPosition;
}

Position
raiseLeg(
  Position curPos)
{
  Position raisedPosition;
  Serial.print("raisedPosition()  ");

  raisedPosition.hip = SERVOMAX;
  raisedPosition.knee = curPos.knee;
 
  printPosition(raisedPosition);
  Serial.print("\n");

  raisedPosition = glideTo(curPos, raisedPosition);
  printPosition(raisedPosition);
  Serial.print("\n");

  return raisedPosition;
}

Position
straightKnee(
  Position curPos)
{
  Position straightKnee;
  Serial.print("straightKnee()  ");

  straightKnee.hip = curPos.hip;
  straightKnee.knee = SERVOMIN;

  printPosition(straightKnee);
  Serial.print("\n");

  straightKnee = glideTo(curPos, straightKnee);

  return straightKnee;
}

Position
bendKnee(
  Position curPos)
{
  Position bendKnee;
  Serial.print("bendKnee()  ");

  bendKnee.hip = 10 + (SERVOMIN + SERVOMAX) / 2;
  bendKnee.knee = 20 + (SERVOMIN + SERVOMAX) / 2;

  printPosition(bendKnee);
  Serial.print("\n");
 
  bendKnee = glideTo(curPos, bendKnee);

  return bendKnee;
}

void loop()
{
  curPos = straightLeg(curPos);
  delay(1000);
  curPos = bendKnee(curPos);
  delay(1000);
  curPos = raiseLeg(curPos);
  delay(1000);
  curPos = straightKnee(curPos);
  delay(1000);
}
