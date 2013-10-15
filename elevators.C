//
//    elevators.C
//
//    Function stubs for the missing parts of elevators.
//
#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <queue>
#include <assert.h>
#include "building.h"
#include "elevators.h"
using namespace std;




struct trip{
  const Person* who;
  int origin;
  int destination;
  sem_t wakeup;
};
trip *rider;

class PCQueue {
public:
  PCQueue();
  void enqueue(trip *it);
  trip *dequeue();
private:
  queue<trip *> q;
  pthread_mutex_t lock; // protects q
  sem_t Qsize;
};

PCQueue::PCQueue()
{
  pthread_mutex_init(&lock,NULL);
  sem_init(&Qsize,0,0);
}

// Add an trip to the queue
void PCQueue::enqueue(trip *it)
{
  pthread_mutex_lock(&lock);
  q.push(it);
  pthread_mutex_unlock(&lock);
  sem_post(&Qsize);
}

// Sleep until there is an trip on the queue, then return it.
trip *PCQueue::dequeue()
{
  trip *it;
  sem_wait(&Qsize);
  pthread_mutex_lock(&lock);
  assert(!q.empty());
  it = q.front();
  q.pop();
  pthread_mutex_unlock(&lock);
  return it;
}

PCQueue myqueue;

//need a queue to put trips on the queue
//
// Elevator constructor
//   Called once for each elevator before the thread is created.
//
Elevator::Elevator()
{ 
  // This is the place to perform any one-time initializations of
  // per-elevator data.
}

//
// Elevator::display_passengers()
//
//  Call display() for each Person on the elevator.
//  Return the number of riders.
//
//  Beware: calling message() from this function will hang the simulation.
//
int Elevator::display_passengers()
{
  rider->who.display();
  int riders =0;
  if (door_is_open() && onfloor()==rider->origin){
    riders++;
  }
  //if (door_is_open()
  //while(sem_wait(&rider->wakeup)){
    return riders;
    //}
}

//
// Elevator::run()
//
//   Main thread for an elevator.
//   Will be called at the beginning of the simulation, to put the
//   Elevator into operation.  run() should pick up and deliver Persons,
//   coordinating with other Elevators for efficient service.
//   run should never return.
//   
void Elevator::run()
{
  message("running");
  int origin, destination;
  sem_t wakeup;
  rider=myqueue.dequeue();
  origin=rider->origin;
  destination=rider->destination;
  wakeup=rider->wakeup;
  if (onfloor()!= origin){
    move_to_floor(origin);
  }
  open_door();
  close_door();
  move_to_floor(destination);
  sem_post(&wakeup);
  //wriet code to dequeue a trip, take floors info and move them
  // Pick up and drop off passengers.
}

//
//  take_elevator
//
//    A Person (who) calls this function to take an elevator from their
//    current floor (origin) to a different floor (destination).
//
void take_elevator(const Person *who, int origin, int destination)
{
  trip mytrip;
  mytrip.who=who;
  mytrip.origin=origin;
  mytrip.destination=destination;
  myqueue.enqueue(&mytrip);
  sem_init(&mytrip.wakeup,0, 0);
// enqueue "myqueue" &mytrip, elevator 
  
  sem_wait(&mytrip.wakeup);
// Should not return until the person has taken an elevator
  // to their destination floor.
}

