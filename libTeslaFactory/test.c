#include "robot.h"
#include "production.h"
#include "utils.h"

int main(int argc, char *argv[]) {
  int numA = 3;
  int numB = 2;
  int numC = 1;
  initNumRobot(numA, numB, numC);
  initProduction(3, 20);
  initProduction(3, 20);
  
  Robot rA[numA];
  Robot rB[numB];
  Robot rC[numC];
  for (int i = 0; i < numA; ++i) {
    rA[i] = createRobot(TypeA, 10);
    debug_printf(__func__, "creating robot%c[%d], num free robot: %d\n", 
        RobotTypeToChar(rA[i]->robotType), rA[i]->id, getNumFreeRobot(TypeA));
  }
  for (int i = 0; i < numB; ++i) {
    rB[i] = createRobot(TypeB, 10);
    debug_printf(__func__, "creating robot%c[%d], num free robot: %d\n", 
        RobotTypeToChar(rB[i]->robotType), rB[i]->id, getNumFreeRobot(TypeB));
  }
  for (int i = 0; i < numC; ++i) {
    rC[i] = createRobot(TypeC, 10);
    debug_printf(__func__, "creating robot%c[%d], num free robot: %d\n", 
        RobotTypeToChar(rC[i]->robotType), rC[i]->id, getNumFreeRobot(TypeC));
  }

  Robot r = rA[0];
  double timer = getCurrentTime();
  makeSkeleton(r);
  makeEngine(r);
  makeChassis(r);
  makeBody(r);
  for (int i = 0; i < 7; ++i) {
    makeWindow(r); 
  }
  for (int i = 0; i < 4; ++i) { 
    makeTire(r);
  }
  makeBattery(r);
  makeCar(r); 
  printProductionProgress();
  debug_printf(__func__, "Time: %lf\n", getCurrentTime() - timer);

  r = rB[0];
  timer = getCurrentTime();
  tryMakeSkeleton(r);
  tryMakeEngine(r);
  tryMakeChassis(r);
  tryMakeBody(r);
  for (int i = 0; i < 7; ++i) {
    tryMakeWindow(r); 
  }
  for (int i = 0; i < 4; ++i) { 
    tryMakeTire(r);
  }
  tryMakeBattery(r);
  makeCar(r); 
  printProductionProgress();
  debug_printf(__func__, "Time: %lf\n", getCurrentTime() - timer);

  r = rC[0];
  timer = getCurrentTime();
  timedTryMakeSkeleton(1, r);
  timedTryMakeEngine(1, r);
  timedTryMakeChassis(1, r);
  timedTryMakeBody(1, r);
  for (int i = 0; i < 7; ++i) {
    timedTryMakeWindow(1, r); 
  }
  for (int i = 0; i < 4; ++i) { 
    timedTryMakeTire(1, r);
  }
  timedTryMakeBattery(1, r);
  makeCar(r); 
  printProductionProgress();
  debug_printf(__func__, "Time: %lf\n", getCurrentTime() - timer);

  for (int i = 0; i < numA; ++i) {
    debug_printf(__func__, "Releasing typeA robot[%d].\n", rA[i]->id);
    releaseRobot(rA[i]);
  }
  for (int i = 0; i < numB; ++i) {
    debug_printf(__func__, "Releasing typeB robot[%d].\n", rB[i]->id);
    releaseRobot(rB[i]);
  }
  for (int i = 0; i < numC; ++i) {
    debug_printf(__func__, "Releasing typeC robot[%d].\n", rC[i]->id);
    releaseRobot(rC[i]);
  }

  initNumRobot(4, 5, 6);

  Robot cheatRobot = calloc(1, sizeof(Robot_));
  cheatRobot->robotType = TypeA;
  cheatRobot->jobQ = queueCreate(100);
  releaseRobot(cheatRobot);

  return 0;
}
