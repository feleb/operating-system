#include <pthread.h>

struct station {
   
    pthread_cond_t train_arrive,train_leave;
    pthread_mutex_t mut , train;
    int waiting_passengers,train_empty_seats , borded_count , totalSeats;
};

void station_init(struct station *station);

void station_load_train(struct station *station, int count);

void station_wait_for_train(struct station *station);

void station_on_board(struct station *station);
