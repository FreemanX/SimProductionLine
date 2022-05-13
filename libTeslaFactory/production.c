#include "production.h"
#include "robot_internal.h"
#include <errno.h>
#include <semaphore.h>
#include <time.h>

#define I_DEBUG
#ifdef I_DEBUG
#define I_DEBUG_HEAD debug_printf(__func__, "\n");
#else 
#define I_DEBUG_HEAD
#endif

static int _productionGoal;

static sem_t _space;
static int _space_limit;

static sem_t _producedSkeleton;
static sem_t _producedEngine;
static sem_t _producedChassis;
static sem_t _producedBody;
static sem_t _producedWindow;
static sem_t _producedTire;
static sem_t _producedBattery;
static sem_t _producedCar;

static int _hasInited = 0; 
void initProduction(int numCars, int numStorage) {
  I_DEBUG_HEAD
  if (numCars < 1) {
    err_printf(__func__, __LINE__, "Invalid production goal: %d\n", numCars);
    exit(-1);
  }
  if (numStorage < 1) {
    err_printf(__func__, __LINE__, "Invalid number of storage space: %d\n",
               numStorage);
    exit(-1);
  }
  if (_hasInited == 0) {
    _hasInited = 1;
    _productionGoal = numCars;
    _space_limit = numStorage;
    sem_init(&_space, 0, numStorage);
    sem_init(&_producedSkeleton, 0, 0);
    sem_init(&_producedEngine, 0, 0);
    sem_init(&_producedChassis, 0, 0);
    sem_init(&_producedBody, 0, 0);
    sem_init(&_producedWindow, 0, 0);
    sem_init(&_producedTire, 0, 0);
    sem_init(&_producedBattery, 0, 0);
    sem_init(&_producedCar, 0, 0);
#ifdef DEBUG
    debug_printf(
        __func__, "Production goal: %d %s, number of storage space: %d\n",
        _productionGoal, _productionGoal > 1 ? "cars" : "car", _space_limit);
#endif
  } else {
    err_printf(__func__, __LINE__, "Function %s() can only be called once!\n", __func__);
    err_printf(__func__, __LINE__ - 1, "Num Cars: %d, num storage: %d\n", 
        _productionGoal, _space_limit);
  }
}

static int _tryRequestSpace() {
  I_DEBUG_HEAD
#ifdef DEBUG
  int num_free_space;
  sem_getvalue(&_space, &num_free_space);
  debug_printf(__func__, "Requesting space, current space=%d...\n",
               num_free_space);
#endif
  int ret = sem_trywait(&_space);
#ifdef DEBUG
  if (ret == 0) {
    sem_getvalue(&_space, &num_free_space);
    debug_printf(__func__, "Space requested, current space=%d...\n",
                 num_free_space);
  } else {
    debug_printf(__func__, "Space request failed, no space available\n");
  }
#endif
  return ret;
}

static void _releaseSpace() {
  I_DEBUG_HEAD
  int num_free_space;
  sem_getvalue(&_space, &num_free_space);
  if (num_free_space < _space_limit) {
#ifdef DEBUG
    debug_printf(__func__, "Releasing space, current space=%d...\n",
                 num_free_space);
#endif
    sem_post(&_space);
#ifdef DEBUG
    sem_getvalue(&_space, &num_free_space);
    debug_printf(__func__, "Space released, current space=%d...\n",
                 num_free_space);
#endif
  } else {
    err_printf(__func__, __LINE__,
               "Fatial Error, releasing space that doesn't exist\n");
    exit(-1);
  }
}

static void _makeItemOnly(int makeTime, sem_t *item) {
  I_DEBUG_HEAD
  sleep(makeTime);
  sem_post(item);
}

static int _tryMakeItemWithSpace(int makeTime, sem_t *item) {
  I_DEBUG_HEAD
  int ret;
  if ((ret = _tryRequestSpace()) == 0)
    _makeItemOnly(makeTime, item);
  return ret;
}

static int _tryGetItem(sem_t *item) {
  I_DEBUG_HEAD
  int ret;
  if ((ret = sem_trywait(item)) == 0)
    _releaseSpace();
  return ret;
}

int getNumFreeSpace() {
  int sem_value;
  sem_getvalue(&_space, &sem_value);
  return sem_value;
}

#define GET_NUM_PRODUCED(What)                                                 \
  int getNumProduced##What() {                                                 \
    int sem_value;                                                             \
    sem_getvalue(&_produced##What, &sem_value);                                \
    return sem_value;                                                          \
  }

GET_NUM_PRODUCED(Skeleton)
GET_NUM_PRODUCED(Engine)
GET_NUM_PRODUCED(Chassis)
GET_NUM_PRODUCED(Body)
GET_NUM_PRODUCED(Window)
GET_NUM_PRODUCED(Tire)
GET_NUM_PRODUCED(Battery)
GET_NUM_PRODUCED(Car)

void printProductionProgress() {
  I_DEBUG_HEAD
  flushed_printf("Num free space: %d\n", getNumFreeSpace());
  flushed_printf("Produced Skeleton: %d\n", getNumProducedSkeleton());
  flushed_printf("Produced Engine: %d\n", getNumProducedEngine());
  flushed_printf("Produced Chassis: %d\n", getNumProducedChassis());
  flushed_printf("Produced Body: %d\n", getNumProducedBody());
  flushed_printf("Produced Window: %d\n", getNumProducedWindow());
  flushed_printf("Produced Tire: %d\n", getNumProducedTire());
  flushed_printf("Produced Battery: %d\n", getNumProducedBattery());
  flushed_printf("Produced Car: %d\n", getNumProducedCar());
  if (getNumProducedCar() == _productionGoal) {
    flushed_printf("Production task completed! %d %s produced.\n",
                   _productionGoal, _productionGoal > 1 ? "cars" : "car");
  }
}

#define TRY_MAKE(WHAT, What)                                                   \
  int tryMake##What(Robot robot) {                                             \
  I_DEBUG_HEAD\
    int ret = -1;                                                              \
    switch (robot->robotType) {                                                \
    case TypeA:                                                                \
      ret = _tryMakeItemWithSpace(TYPE_A_TIME_##WHAT, &_produced##What);       \
      break;                                                                   \
    case TypeB:                                                                \
      ret = _tryMakeItemWithSpace(TYPE_B_TIME_##WHAT, &_produced##What);       \
      break;                                                                   \
    case TypeC:                                                                \
      ret = _tryMakeItemWithSpace(TYPE_C_TIME_##WHAT, &_produced##What);       \
      break;                                                                   \
    default:                                                                   \
      break;                                                                   \
    }                                                                          \
    return ret;                                                                \
  }

TRY_MAKE(SKELETON, Skeleton)
TRY_MAKE(ENGINE, Engine)
TRY_MAKE(CHASSIS, Chassis)
TRY_MAKE(WINDOW, Window)
TRY_MAKE(TIRE, Tire)
TRY_MAKE(BATTERY, Battery)

int tryMakeBody(Robot robot) {
  I_DEBUG_HEAD
  int ret;

  if ((ret = _tryRequestSpace()) != 0)
    return ret;

  int req_skeleton = 1;
  int req_engine = 1;
  int req_chassis = 1;
  while (req_skeleton > 0 || req_engine > 0 || req_chassis > 0) {
    if (req_skeleton > 0 && _tryGetItem(&_producedSkeleton) == 0)
      req_skeleton--;
    if (req_engine > 0 && _tryGetItem(&_producedEngine) == 0)
      req_engine--;
    if (req_chassis > 0 && _tryGetItem(&_producedChassis) == 0)
      req_chassis--;
  }

  switch (robot->robotType) {
  case TypeA:
    _makeItemOnly(TYPE_A_TIME_BODY, &_producedBody);
    break;
  case TypeB:
    _makeItemOnly(TYPE_B_TIME_BODY, &_producedBody);
    break;
  case TypeC:
    _makeItemOnly(TYPE_C_TIME_BODY, &_producedBody);
    break;
  default:
    break;
  }

  return 0;
}

static int _requestSpace() {
  I_DEBUG_HEAD
#ifdef DEBUG
  int num_free_space;
  sem_getvalue(&_space, &num_free_space);
  debug_printf(__func__, "Requesting space, current space=%d...\n",
               num_free_space);
#endif
  int ret = sem_wait(&_space);
#ifdef DEBUG
  if (ret == 0) {
    sem_getvalue(&_space, &num_free_space);
    debug_printf(__func__, "Space requested, current space=%d...\n",
                 num_free_space);
  } else {
    debug_printf(__func__, "Space request failed, no space available\n");
  }
#endif
  return ret;
}

static int _makeItemWithSpace(int makeTime, sem_t *item) {
  I_DEBUG_HEAD
  int ret;
  if ((ret = _requestSpace()) == 0)
    _makeItemOnly(makeTime, item);
  return ret;
}

#define MAKE(WHAT, What)                                                       \
  void make##What(Robot robot) {                                               \
  I_DEBUG_HEAD\
    switch (robot->robotType) {                                                \
    case TypeA:                                                                \
      _makeItemWithSpace(TYPE_A_TIME_##WHAT, &_produced##What);                \
      break;                                                                   \
    case TypeB:                                                                \
      _makeItemWithSpace(TYPE_B_TIME_##WHAT, &_produced##What);                \
      break;                                                                   \
    case TypeC:                                                                \
      _makeItemWithSpace(TYPE_C_TIME_##WHAT, &_produced##What);                \
      break;                                                                   \
    default:                                                                   \
      break;                                                                   \
    }                                                                          \
  }

MAKE(SKELETON, Skeleton)
MAKE(ENGINE, Engine)
MAKE(CHASSIS, Chassis)
MAKE(WINDOW, Window)
MAKE(TIRE, Tire)
MAKE(BATTERY, Battery)

void makeBody(Robot robot) {
  I_DEBUG_HEAD

  _requestSpace();

  int req_skeleton = 1;
  int req_engine = 1;
  int req_chassis = 1;
  while (req_skeleton > 0 || req_engine > 0 || req_chassis > 0) {
    if (req_skeleton > 0 && _tryGetItem(&_producedSkeleton) == 0)
      req_skeleton--;
    if (req_engine > 0 && _tryGetItem(&_producedEngine) == 0)
      req_engine--;
    if (req_chassis > 0 && _tryGetItem(&_producedChassis) == 0)
      req_chassis--;
  }

  switch (robot->robotType) {
  case TypeA:
    _makeItemOnly(TYPE_A_TIME_BODY, &_producedBody);
    break;
  case TypeB:
    _makeItemOnly(TYPE_B_TIME_BODY, &_producedBody);
    break;
  case TypeC:
    _makeItemOnly(TYPE_C_TIME_BODY, &_producedBody);
    break;
  default:
    break;
  }
}

void makeCar(Robot robot) {
  I_DEBUG_HEAD
  int req_window = 7;
  int req_tire = 4;
  int req_battery = 1;
  int req_body = 1;

  while (req_window > 0 || req_tire > 0 || req_battery > 0 || req_body > 0) {
    if (req_window > 0 && _tryGetItem(&_producedWindow) == 0)
      req_window--;
    if (req_tire > 0 && _tryGetItem(&_producedTire) == 0)
      req_tire--;
    if (req_battery > 0 && _tryGetItem(&_producedBattery) == 0)
      req_battery--;
    if (req_body > 0 && _tryGetItem(&_producedBody) == 0)
      req_body--;
  }

  switch (robot->robotType) {
  case TypeA:
    _makeItemOnly(TYPE_A_TIME_CAR, &_producedCar);
    break;
  case TypeB:
    _makeItemOnly(TYPE_B_TIME_CAR, &_producedCar);
    break;
  case TypeC:
    _makeItemOnly(TYPE_C_TIME_CAR, &_producedCar);
    break;
  default:
    break;
  }
}

static struct timespec _ts;
static int _timedTryrequestSpace(int sec) {
  I_DEBUG_HEAD
  if (clock_gettime(CLOCK_REALTIME, &_ts) == -1) {
    err_printf("clock_gettime", __LINE__, "Failed to get time\n");
    return -1;
  }
  _ts.tv_sec += sec;

#ifdef DEBUG
  int num_free_space;
  sem_getvalue(&_space, &num_free_space);
  debug_printf(__func__, "Requesting space, current space=%d...\n",
               num_free_space);
#endif
  int s;
  while ((s = sem_timedwait(&_space, &_ts)) == -1 && errno == EINTR)
    continue;
#ifdef DEBUG
  if (s == 0) {
    sem_getvalue(&_space, &num_free_space);
    debug_printf(__func__, "Space requested, current space=%d...\n",
                 num_free_space);
  } else {
    debug_printf(__func__, "Space request failed, no space available\n");
  }
#endif
  return s;
}

static int _timedTryMakeItemWithSpace(int waitTime, int makeTime, sem_t *item) {
  I_DEBUG_HEAD
  int ret = -1;
  if ((ret = _timedTryrequestSpace(waitTime)) == 0)
    _makeItemOnly(makeTime, item);
  return ret;
}

#define TIMED_TRY_MAKE(WHAT, What)                                             \
  int timedTryMake##What(int waitTime, Robot robot) {                          \
  I_DEBUG_HEAD\
    int ret;                                                                   \
    switch (robot->robotType) {                                                \
    case TypeA:                                                                \
      ret = _timedTryMakeItemWithSpace(waitTime, TYPE_A_TIME_##WHAT,           \
                                       &_produced##What);                      \
      break;                                                                   \
    case TypeB:                                                                \
      ret = _timedTryMakeItemWithSpace(waitTime, TYPE_B_TIME_##WHAT,           \
                                       &_produced##What);                      \
      break;                                                                   \
    case TypeC:                                                                \
      ret = _timedTryMakeItemWithSpace(waitTime, TYPE_C_TIME_##WHAT,           \
                                       &_produced##What);                      \
      break;                                                                   \
    default:                                                                   \
      break;                                                                   \
    }                                                                          \
    return ret;                                                                \
  }

TIMED_TRY_MAKE(SKELETON, Skeleton)
TIMED_TRY_MAKE(ENGINE, Engine)
TIMED_TRY_MAKE(CHASSIS, Chassis)
TIMED_TRY_MAKE(WINDOW, Window)
TIMED_TRY_MAKE(TIRE, Tire)
TIMED_TRY_MAKE(BATTERY, Battery)

int timedTryMakeBody(int waitTime, Robot robot) {
  I_DEBUG_HEAD
  int ret = _timedTryrequestSpace(waitTime);
  if (ret != 0)
    return ret;
  int req_skeleton = 1;
  int req_engine = 1;
  int req_chassis = 1;
  while (req_skeleton > 0 || req_engine > 0 || req_chassis > 0) {
    if (req_skeleton > 0 && _tryGetItem(&_producedSkeleton) == 0)
      req_skeleton--;
    if (req_engine > 0 && _tryGetItem(&_producedEngine) == 0)
      req_engine--;
    if (req_chassis > 0 && _tryGetItem(&_producedChassis) == 0)
      req_chassis--;
  }

  switch (robot->robotType) {
  case TypeA:
    _makeItemOnly(TYPE_A_TIME_BODY, &_producedBody);
    break;
  case TypeB:
    _makeItemOnly(TYPE_B_TIME_BODY, &_producedBody);
    break;
  case TypeC:
    _makeItemOnly(TYPE_C_TIME_BODY, &_producedBody);
    break;
  default:
    break;
  }
  return ret;
}
