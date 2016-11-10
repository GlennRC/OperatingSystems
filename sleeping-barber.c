#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>


pthread_mutex_t barber_access;
pthread_mutex_t seat_access;
int customer_access = 0;
int num_free_seats = 5;
bool shop_open = true;

// wait implementation for the counting semaphore: customer_access
void wait_for_customer() {
    while(customer_access <= 0) {
        ;
    }
    customer_access -= 1;
}

// signal impl for semaphore
void customer_available() {
    customer_access += 1;
}

void* barber() {
    // While the show is still open, take in customers
    while(shop_open) {
        // waiting for customer to leave the waiting room
        printf("[Barber]: waiting for customer\n");
        wait_for_customer();
        // waiting to access num_free_seats
        pthread_mutex_lock(&seat_access);
        // Add a free seat 
        num_free_seats++;
        printf("[Barber]: calling customer over\n");
        //Barber is ready to cut
        pthread_mutex_unlock(&barber_access);
        // release num_of_seats
        pthread_mutex_unlock(&seat_access);
        // Cutting hair
        printf("[Barber]: cutting hair\n");
        // cutting hair takes time
        sleep(3);
    }
    return NULL;
}

void* customer(void* rank) {
    // Check if there are seats available
    pthread_mutex_lock(&seat_access);
    if(num_free_seats > 0) {
        // There are so sit down
        num_free_seats--;
        printf("[%d]: found seat\n", (int)rank);
        // Signal the customer_access semaphore,
        // so that the barber knows a customer is waiting
        customer_available();
        pthread_mutex_unlock(&seat_access);
        printf("[%d]: waiting for barber\n", (int)rank);
        pthread_mutex_lock(&barber_access);
        printf("[%d]: leaving\n", (int)rank);
        //The barber is finished cutting hair
    }
    else {
        // No seats are available so leave
        pthread_mutex_unlock(&seat_access);
        printf("no seats available\n");
    }
    return NULL; 
}

int main(int argc, char* argv[]) { 
    int num_customers = 10;

    if(argc == 2)
        num_customers = atoi(argv[1]);
    
    pthread_mutex_init(&barber_access, NULL);
    pthread_mutex_init(&seat_access, NULL);
 
    pthread_t* barber_handle;
    pthread_create((void*)&barber_handle, NULL, barber, NULL);
    pthread_mutex_lock(&barber_access);
    
    pthread_t* cust_handles = (pthread_t*) malloc(num_customers*sizeof(pthread_t*));
    for(long thread=0; thread<num_customers; thread++) {
        sleep(rand() % 5);
        pthread_create((void*)&cust_handles[thread], NULL, customer, (void*)thread);
    }   

    for(int thread=0; thread<num_customers; thread++) {
        pthread_join(cust_handles[thread], NULL);

    }
    
    printf("Shop closed\n");
    shop_open = false;
    pthread_join((void*)barber_handle, NULL);
    
    return 0;
}
