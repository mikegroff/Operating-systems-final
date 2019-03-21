/**
 * Scheduler
 *
 * COP4600
 * @author Matthew Balwant
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_FILE_NAME "processes.in"    // as defined in program spec
#define DEFAULT_OUT_NAME "processes.out"  // as defined in program spec
#define ALGO_SHORTNAME_MAX_LEN 4            // fcfs = len 4
#define ALGO_RR 0
#define ALGO_FCFS 1
#define ALGO_SJF 2
#define DEBUG = 0;

/**
 * Process stuct
 */
typedef struct process {
    char name[100];
    int arrival;
    int burst;
    int lastRun;
    int waiting;
    int turnaround;
} Process;

/**
 * Queue struct
 */
typedef struct queue {
    int head, tail, size;
    unsigned int capacity;
    Process *processes;
} Queue;

// parses the input file and will create the process queue for us
Queue *parse(FILE *input);

// queue functions
Queue *initQueue(unsigned int capacity);
void enqueue(Queue *q, Process p);
Process get(Queue *q);
int isFull(Queue *queue);
void destroyQueue(Queue *queue);

// queue comparators
int sort_by_arrival(const void *p1, const void *p2);

// algo functions
void round_robin(Queue *arrival, FILE *output);
void first_come_first_serve(Queue *arrival, FILE *output);
void shortest_job_first(Queue *arrivalQueue, FILE *output);

// fields
const char *algo_shortnames[] = { "rr", "fcfs", "sjf" };
const char *algo_names[] = { "Round-Robin", "First-Come First-Serve", "Shortest Job First (Pre)" };
const char algo_count = sizeof(algo_shortnames) / sizeof(algo_shortnames[0]);
int algorithm = -1;

int processcount = 0;
int runfor = 0;
int quantum = -1;

int main(int argc, char *argv[])
{

    // input file
    FILE *input;
    FILE *output;

    // Determine where to read from
    if (argc == 1) {
        // There are no arguements passed in, attempt to read from static file
        input = fopen(DEFAULT_FILE_NAME, "r");
        output = fopen(DEFAULT_OUT_NAME, "w");
    } else if (argc == 2) {
        // There is an extra parameter, this should be a file
        input = fopen(argv[1], "r");
        output = fopen(strcat(argv[1], ".out"), "w");
    } else {
        // printf("[ERROR]: Invalid number of arguements.\n");
        // no input file could be found, just exit here
        return 1;
    }

    // output = stdout;

    // make sure file exists
    if (!input || !output) {
        // printf("[ERROR]: File could not be opened.\n");
        // input file did not exist, return with error code 1
        return 1;
    }

    // create a list of all processes from input file
    Queue *arrivalQueue = parse(input);

    // print header
    fprintf(output, "%d processes\n", processcount);
    fprintf(output, "Using %s\n", algo_names[algorithm]);

    if (algorithm == ALGO_RR)
    {
        fprintf(output, "Quantum %d\n", quantum);
        round_robin(arrivalQueue, output);
    } else if (algorithm == ALGO_FCFS) {
        qsort(arrivalQueue->processes, (size_t) processcount, sizeof(Process), sort_by_arrival);
        first_come_first_serve(arrivalQueue, output);
    } else if (algorithm == ALGO_SJF) {
        shortest_job_first(arrivalQueue, output);
    }

    destroyQueue(arrivalQueue);

    return 0;

}

Queue *parse(FILE *input)
{

    // character buffer to store input lines
    char buffer[1000];

    Queue *arrivalQueue = NULL;

    while (fgets(buffer, sizeof buffer, input) != NULL) {

        // we only care about the characters before #
        strtok(buffer, "#\n");

        // probably a stupid way to do this, but whatever
        char *tokens = malloc(sizeof(char) * 1000);
        strncpy(tokens, buffer, strlen(buffer));
        strtok(tokens, " ");

        if (!strcmp(tokens, "processcount")) {

            // we are assuming that this will ALWAYS be before process list
            sscanf(buffer, "processcount %d", &processcount);
            arrivalQueue = initQueue((const unsigned int) processcount);

        } else if (!strcmp(tokens, "runfor")) {

            sscanf(buffer, "runfor %d", &runfor);

        } else if (!strcmp(tokens, "use")) {

            // read in the shortname, assuming max length is fcfs.
            char shortname[ALGO_SHORTNAME_MAX_LEN];
            sscanf(buffer, "use %s", shortname);
            // use read in short name to get algorithm id
            int i;
            for (i = 0; i < algo_count; ++i) {
                if (!strcmp(algo_shortnames[i], shortname)) {
                    algorithm = i;
                    break;
                }
            }

        } else if (!strcmp(tokens, "quantum")) {

            sscanf(buffer, "quantum %d", &quantum);

        } else if (!strcmp(tokens, "process")) {

            Process process;
            sscanf(buffer, "process name %s arrival %d burst %d",
                   &process.name,
                   &process.arrival,
                   &process.burst
            );
            process.waiting = 0;
            process.turnaround = -1;
            process.lastRun = -1;
            enqueue(arrivalQueue, process);

        } else if (!strcmp(tokens, "end")) {

            return arrivalQueue;

        } else {

            // printf("[WARNING]: Unrecognized command: \"%s\"\n", buffer);

        }

    }

    // if the program gets to here it means that there was no `end` command found
    return arrivalQueue;

}

Queue *initQueue(const unsigned int capacity) {
    Queue *queue = (Queue *) malloc(sizeof(queue));
    queue->capacity = capacity;
    queue->head = 0;
    queue->size = 0;
    queue->tail = -1;
    queue->processes = (Process *)malloc(sizeof(queue) * capacity);
    return queue;
}

void enqueue(Queue *q, Process p) {
    if (!isFull(q)) {
        q->processes[(++q->tail % q->capacity)] = p;
        q->size++;
    }
}

Process get(Queue *q) {
    Process p = q->processes[q->head];
    q->head = (q->head + 1) % q->capacity;
    q->size--;
    return p;
}

Process peek(Queue *q) {
    Process p = q->processes[q->head];
    return p;
}

int isFull(Queue *queue) {
    return (queue->size == queue->capacity);
}

int isEmpty(Queue *queue) {
    return (queue->size == 0);
}

void destroyQueue(Queue *queue) {
    free(queue->processes);
    free(queue);
}

int sort_by_arrival(const void *p1, const void *p2) {
    return ((Process *) p1)->arrival - ((Process *)p2)->arrival;
}

void processArrivals(Queue *arrivals, Queue *q, int time, FILE *output) {
    int arrivalSize = arrivals->size;
    int i;
    for (i = 0; i < arrivalSize; ++i) {
        Process p = get(arrivals);
        if (p.arrival == time) {
            fprintf(output, "Time %d: %s arrived\n", time, p.name);
            p.waiting = 0;
            enqueue(q, p);
        } else enqueue(arrivals, p);
    }
}

void shortest_job_first(Queue *arrival, FILE *output) {

    fprintf(output, "\n");

    Queue *q = initQueue(arrival->capacity);
    Process result[arrival->capacity];
    int r = 0;
    int next = 0;
    int time;
    char name[100];
    memset(name, '\0', sizeof(name));
    for (time = 0; time < runfor; ++time) {

        if (!isEmpty(arrival)) processArrivals(arrival, q, time, output);

        if (isEmpty(q)) {
            fprintf(output, "Time %d: IDLE\n", time);
            continue;
        }

        Process p = peek(q);
        if (next <= time && p.burst <= 0) {
            fprintf(output, "Time %d: %s finished\n", time, p.name);
            p = get(q);
            p.turnaround = time - p.arrival;
            result[r++] = p;
        }

        if (isEmpty(q)) {
            fprintf(output, "Time %d: IDLE\n", time);
            continue;
        }

        // size is 1, just take it and go
        if (q->size == 1) {
            p= peek(q);
            if (strcmp(name, p.name) != 0) {
                fprintf(output, "Time %d: %s selected (burst %d)\n", time, p.name, p.burst);
                memset(name, '\0', sizeof(name));
                strncpy(name, p.name, strlen(p.name));
                if (p.lastRun != -1) {
                    q->processes[q->head].waiting += (time - p.lastRun - 1);
                }
            }
            q->processes[q->head].lastRun = time;
            q->processes[q->head].burst--;
        } else {
            // there has already been an empty check, so we know it is not empty
            // now this is a bad way to do it, but we really want the first value
            // to be shortest
            const unsigned int count = (const unsigned int) q->size;
            Process processes[count];
            int i;
            int lowest = 0;
            for (i = 0; i < count; ++i) {
                processes[i] = get(q);
                //  printf("%s (%d)\n", processes[i].name, processes[i].burst);
                // we only want to change if burst is lower, remember context switching
                // has overhead, so if bursts are identical, might as well stay with current
                // low
                if (processes[i].burst < processes[lowest].burst) {
                    lowest = i;
                }
            }
            // new lowest is:
            // printf("New lowest is: %s (%d)\n", processes[lowest].name, processes[lowest].burst);
            // put them back with lowest 1st
            enqueue(q, processes[lowest]);
            for (i= 0; i < count; ++i) {
                if (i == lowest) continue;
                enqueue(q, processes[i]);
            }
            p = peek(q);
            if (strcmp(name, p.name) != 0) {
                fprintf(output, "Time %d: %s selected (burst %d)\n", time, p.name, p.burst);
                strncpy(name, p.name, strlen(p.name));
                if (p.lastRun != -1) {
                    q->processes[q->head].waiting += (time - p.lastRun - 1);
                }
            }
            q->processes[q->head].lastRun = time;
            q->processes[q->head].burst--;
        }

    }

    fprintf(output, "Finished at time %d\n", time);

    fprintf(output, "\n");
    int i;
    for (i = 0; i < r; ++i) {
        Process p = result[i];
        fprintf(output, "%s wait %d turnaround %d\n", p.name, p.waiting, p.turnaround);
    }
}

void first_come_first_serve(Queue *arrival, FILE *output) {

    fprintf(output, "\n");

    Queue *q = initQueue(arrival->capacity);
    Process result[arrival->capacity];
    int r = 0;
    int time;
    int next = 0;
    for (time = 0; time <= runfor; ++time) {

        const unsigned int count = (const unsigned int) q->size;

        if (!isEmpty(arrival)) processArrivals(arrival, q, time, output);

        if (isEmpty(q)) {
            fprintf(output, "Time %d: IDLE\n", time);
            continue;
        }

        Process p = peek(q);
        if (next <= time && p.burst <= 0) {
            fprintf(output, "Time %d: %s finished\n", time, p.name);
            p = get(q);
            p.turnaround = time - p.arrival;
            result[r++] = p;
        }

        if (isEmpty(q)) {
            fprintf(output, "Time %d: IDLE\n", time);
            continue;
        }

        p = peek(q);
        if (next <= time && p.burst > 0) {
            fprintf(output, "Time %d: %s selected (burst %d)\n", time, p.name, p.burst);
            next = time + p.burst;
            q->processes[q->head].burst = 0;
            q->processes[q->head].waiting = (time - p.arrival);
        }

    }

    fprintf(output, "Finished at time %d\n", time - 1);

    fprintf(output, "\n");
    int i;
    for (i = 0; i < r; ++i) {
        Process p = result[i];
        fprintf(output, "%s wait %d turnaround %d\n", p.name, p.waiting, p.turnaround);
    }


}

void round_robin(Queue *arrival, FILE *output)
{
    fprintf(output, "\n");

    Queue *q = initQueue(arrival->capacity);
    Process result[arrival->capacity];
    int r = 0;

    int time;
    int next = 0;
    for (time = 0; time < runfor; ++time) {

        const unsigned int count = (const unsigned int) q->size;

        if (!isEmpty(arrival)) processArrivals(arrival, q, time, output);

        // hack lol
        int i;
        for (i = 0; i < count; ++i) {
            Process p = get(q);
            if (p.burst <= 0) {
                fprintf(output, "Time %d: %s finished\n", time, p.name);
                p.turnaround = time - p.arrival;
                next = time;
                result[r++] = p;
            } else enqueue(q, p);
        }

        if (isEmpty(q)) {
            fprintf(output, "Time %d: IDLE\n", time);
            continue;
        } else if (next <= time && !isEmpty(q)) {
            Process p = get(q);
            fprintf(output, "Time %d: %s selected (burst %d)\n", time, p.name, p.burst);
            if (p.lastRun != -1) {
                p.waiting += (time - p.lastRun - quantum);
            } else if (time >= p.arrival) {
                p.waiting = (time - p.arrival);
            }
            p.lastRun = time;
            p.burst -= quantum;
            next = time + quantum;
            enqueue(q, p);
        }

    }

    fprintf(output, "Finished at time %d\n", time);

    fprintf(output, "\n");
    int i;
    for (i = 0; i < r; ++i) {
        Process p = result[i];
        fprintf(output, "%s wait %d turnaround %d\n", p.name, p.waiting, p.turnaround);
    }

}