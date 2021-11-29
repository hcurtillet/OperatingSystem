typedef struct job{
    int id;
    int arrival;
    int unblock;
    int exectime;
    float ioratio;
    struct job * next;
} job;

typedef struct spect{
    int arrival; 
    int exectime;
    float ioratio;
} spec;