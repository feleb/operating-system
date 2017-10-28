#include <pthread.h>
#include<stdio.h>
#include "caltrain.h"
#include <math.h>
 
 
 
void
station_init(struct station *station)
{
    pthread_mutex_init(&station->mut,NULL);//pthread_mutex_init(&station->train,NULL);
    pthread_cond_init(&station->train_arrive,NULL);
    pthread_cond_init(&station->train_leave,NULL);
   station-> waiting_passengers=0;
    station->train_empty_seats=0;station->borded_count=0;
}
 
void
station_load_train(struct station *station, int count){
    if(!count || !station->waiting_passengers)
        return;
 
    station->train_empty_seats=station->totalSeats=count;
    station->borded_count=0;
 
    pthread_cond_broadcast(&station->train_arrive);
 
    pthread_mutex_lock(&station->mut);  
            pthread_cond_wait(&station->train_leave,&station->mut);
    pthread_mutex_unlock(&station->mut);
return;
}
 
void
station_wait_for_train(struct station *station)
{
    pthread_mutex_lock(&station->mut);
    station->waiting_passengers++;
 
    while (station->train_empty_seats==0)
        pthread_cond_wait(&station->train_arrive,&station->mut);
   
    station->train_empty_seats--;
    station->waiting_passengers--;  
    pthread_mutex_unlock(&station->mut);
}
 
void
station_on_board(struct station *station)
{
        pthread_mutex_lock(&station->mut);    
        station->borded_count++;
        if((!station->waiting_passengers && (station->totalSeats - station->train_empty_seats) == station->borded_count) || 
station->borded_count==station->totalSeats){
                pthread_cond_signal(&station->train_leave);//printf("%d",station->count);
        }
 
        pthread_mutex_unlock(&station->mut);
 
}
