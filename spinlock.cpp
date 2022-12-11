# include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <chrono>

class Integer{
private:
    int value;
    pthread_spinlock_t* spinlock;
    int pshared;
public:
    Integer(int val):value(val){
        this->spinlock = new pthread_spinlock_t();
        pthread_spin_init(this->spinlock, pshared);
    }

    int get_value(){
        return this->value;
    }
    void set_value(int val){
        this->value = val;
    }
    void inc(){
        pthread_spin_lock(this->spinlock);
        this->value++;
        pthread_spin_unlock(this->spinlock);
    }
    ~Integer() {
        pthread_spin_destroy(this->spinlock);
        delete this->spinlock;
    }
};

void* inc(void* arg){
    Integer* integ = (Integer*) arg;
    int inc_num = 10000;
    for (int i = 0; i < inc_num; i++){
       integ->inc();
    }
    return NULL;
}

int main () {
    int thread_num = 56;
    Integer* integ = new Integer(0);
    pthread_t* thrds = new pthread_t[thread_num];


    auto start = std::chrono::high_resolution_clock::now();






    for (int i = 0; i < thread_num; i++){
        if (pthread_create(&thrds[i], NULL, inc, (void*)integ) != 0){
            return 1;
        }
    }

    for (int i = 0; i < thread_num; i++){
        if (pthread_join(thrds[i], NULL) != 0){
            return 2;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> duration = end - start;


    std::cout << integ->get_value() << "\n";

    delete [] thrds;
    delete integ;

    std::string file = "time.txt";
    std::string time = "Spinlock time is ";
    time += std::to_string(duration.count());
    int fld = open(file.c_str(), O_RDWR | O_CREAT, 0644);
    write(fld, time.c_str(), time.size());
    close(fld);






    return 0;
}

