#define _GNU_SOURCE
#include "robot.h"
#include "robot_internal.h"
#include <dlfcn.h>

#define INTERNAL_DEBUG

static sem_t _typeA;
static sem_t _typeB;
static sem_t _typeC; 

static int _maxA;
static int _maxB;
static int _maxC;
static int _totalR;

static sem_t _pthreadCnt; 

char RobotTypeToChar(RobotType rt) {
  switch (rt) {
  case TypeA:
    return 'A';
  case TypeB:
    return 'B';
  case TypeC:
    return 'C';
  default:
    err_printf(__func__, __LINE__, "Converting unkonw robot type to char!\n");
    return 0;
  }
}

static int _hasInited = 0;
void initNumRobot(int numA, int numB, int numC) {
  if(_hasInited == 0){
    _maxA = numA;
    _maxB = numB;
    _maxC = numC;
    _totalR = numA + numB + numC;
    sem_init(&_typeA, 0, _maxA);
    sem_init(&_typeB, 0, _maxB);
    sem_init(&_typeC, 0, _maxC);
    sem_init(&_pthreadCnt, 0, _totalR);
#ifdef DEBUG
    debug_printf(__func__,
        "Initializing robots. TypeA: %d, TypeB: %d, TypeC: %d\n", numA,
        numB, numC);
#endif
    _hasInited = 1;
  } else {
    err_printf(__func__, __LINE__, "Function %s() can only be called once!\n", __func__);
    err_printf(__func__, __LINE__ - 1, "Max robot num -- TypeA: %d, TypeB: %d, TypeC: %d\n", 
        _maxA, _maxB, _maxC);
  }
}

static int robotCnt = 0;
Robot createRobot(RobotType rt, int numWork) {
  switch (rt) {
  case TypeA:
    if (sem_trywait(&_typeA) != 0)
      return NULL;
    break;
  case TypeB:
    if (sem_trywait(&_typeB) != 0)
      return NULL;
    break;
  case TypeC:
    if (sem_trywait(&_typeC) != 0)
      return NULL;
    break;
  default:
    err_printf(__func__, __LINE__, "Creating Robot with unknown type!\n");
    return NULL;
  }
  Robot r = (Robot)calloc(1, sizeof(Robot_));
  r->id = robotCnt++;
  r->robotType = rt;
  sem_init(&r->runningSemaphore, 0, 1);
  return r;
}

static void _checkAndReleaseRobot(RobotType rt, sem_t *rtSem, int max) {
  int n;
  sem_post(rtSem);
  sem_getvalue(rtSem, &n);
  if (n > max) {
    err_printf("releaseRobot(Fatial Error)", __LINE__,
               "Releasing robot type %c more than initialized! Current free: %d, Initialized: %d\n",
               RobotTypeToChar(rt), n, max);
    exit(-1);
  }
}

void releaseRobot(Robot robot) {
  RobotType rt = robot->robotType;
  sem_destroy(&robot->runningSemaphore);
  free(robot);
  switch (rt) {
  case TypeA:
    _checkAndReleaseRobot(rt, &_typeA, _maxA);
    break;
  case TypeB:
    _checkAndReleaseRobot(rt, &_typeB, _maxB);
    break;
  case TypeC:
    _checkAndReleaseRobot(rt, &_typeC, _maxC);
    break;
  default:
    err_printf(__func__, __LINE__, "Releasing Robot with unknown type!\n");
    return;
  }
}

int getNumFreeRobot(RobotType rt) {
  int n = -1;
  switch (rt) {
  case TypeA:
    sem_getvalue(&_typeA, &n);
    break;
  case TypeB:
    sem_getvalue(&_typeB, &n);
    break;
  case TypeC:
    sem_getvalue(&_typeC, &n);
    break;
  default:
    err_printf(__func__, -1, "Creating Robot with unknown type!\n");
    break;
  }
  return n;
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg){
#ifdef INTERNAL_DEBUG
  debug_printf(__func__, "\n");
#endif
  int ret;
  if ((ret = sem_trywait(&_pthreadCnt)) != 0){
    err_printf(__func__, __LINE__, "Thread creation failed. Max number(%d) of concurrent thread has been created!\n", _totalR);
    return ret;
  }
  int (*pc)(pthread_t *, const pthread_attr_t *, void *(*) (void *), void *) = dlsym(RTLD_NEXT, "pthread_create");
  return pc(thread, attr, start_routine, arg);
}

int pthread_join(pthread_t thread, void **retval){
#ifdef INTERNAL_DEBUG
  debug_printf(__func__, "\n");
#endif
  sem_post(&_pthreadCnt);
  int (*pj)(pthread_t, void **) = dlsym(RTLD_NEXT, "pthread_join");
  return pj(thread, retval);
}
