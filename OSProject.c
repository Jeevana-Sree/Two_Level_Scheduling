#include <stdio.h>
#include <stdlib.h>

// Define the structure of a process
typedef struct process {
    int pid; // Process ID
    int at; // Arrival time
    int bt; // Burst time
    int pr; // Priority
    int ct; // Completion time
    int tat; // Turnaround time
    int wt; // Waiting time
    int rem; // Remaining time
    struct process *next; // Pointer to the next process in the queue
} process;

// Define the structure of a queue
typedef struct queue {
    process *front; // Pointer to the front of the queue
    process *rear; // Pointer to the rear of the queue
} queue;

// Create a new process with given parameters and return its pointer
process *new_process(int pid, int at, int bt, int pr) {
    process *p = (process *)malloc(sizeof(process)); // Allocate memory for the process
    p->pid = pid; // Assign process ID
    p->at = at; // Assign arrival time
    p->bt = bt; // Assign burst time
    p->pr = pr; // Assign priority
    p->ct = 0; // Initialize completion time to 0
    p->tat = 0; // Initialize turnaround time to 0
    p->wt = 0; // Initialize waiting time to 0
    p->rem = bt; // Initialize remaining time to burst time
    p->next = NULL; // Initialize next pointer to NULL
    return p; // Return the pointer to the process
}

// Create an empty queue and return its pointer
queue *new_queue() {
    queue *q = (queue *)malloc(sizeof(queue)); // Allocate memory for the queue
    q->front = NULL; // Initialize front pointer to NULL
    q->rear = NULL; // Initialize rear pointer to NULL
    return q; // Return the pointer to the queue
}

// Check if a queue is empty or not, return 1 if empty, 0 otherwise
int is_empty(queue *q) {
    return (q->front == NULL); // Return true if front pointer is NULL, false otherwise
}

// Enqueue a process to the rear of a queue, update the front and rear pointers accordingly
void enqueue(queue *q, process *p) {
    if (is_empty(q)) { // If the queue is empty, make the process the front and rear of the queue
        q->front = p;
        q->rear = p;
    } else { // If the queue is not empty, append the process to the rear of the queue and update the rear pointer
        q->rear->next = p;
        q->rear = p;
    }
}

// Dequeue a process from the front of a queue, update the front and rear pointers accordingly, and return the pointer to the dequeued process
process *dequeue(queue *q) {
    if (is_empty(q)) { // If the queue is empty, return NULL
        return NULL;
    } else { // If the queue is not empty, remove the front process from the queue and update the front pointer, return the pointer to the removed process
        process *p = q->front;
        q->front = q->front->next;
        if (q->front == NULL) { // If the queue becomes empty after dequeuing, update the rear pointer to NULL as well
            q->rear = NULL;
        }
        p->next = NULL; // Set the next pointer of the dequeued process to NULL
        return p;
    }
}

// Peek at the front process of a queue without removing it, and return its pointer, return NULL if the queue is empty
process *peek(queue *q) {
    if (is_empty(q)) { // If the queue is empty, return NULL
        return NULL;
    } else { // If the queue is not empty, return the pointer to the front process of the queue without removing it
        return q->front;
    }
}

// Print the details of a process in a tabular format
void print_process(process *p) {
    printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\n", p->pid, p->at, p->bt, p->pr, p->ct, p->tat, p->wt); // Print process ID, arrival time, burst time, priority, completion time, turnaround time and waiting time separated by tabs 
}

// Print the details of all processes in a queue in a tabular format, along with their average turnaround time and average waiting time 
void print_queue(queue *q) {
    printf("PID\tAT\tBT\tPR\tCT\tTAT\tWT\n"); // Print the header row with labels for each column
    process *p = q->front; // Initialize a pointer to the front of the queue
    int n = 0; // Initialize a counter for the number of processes
    int total_tat = 0; // Initialize a variable for the total turnaround time
    int total_wt = 0; // Initialize a variable for the total waiting time
    while (p != NULL) { // Loop through the queue until the pointer reaches NULL
        print_process(p); // Print the details of the current process
        n++; // Increment the counter for the number of processes
        total_tat += p->tat; // Add the turnaround time of the current process to the total turnaround time
        total_wt += p->wt; // Add the waiting time of the current process to the total waiting time
        p = p->next; // Move the pointer to the next process in the queue
    }
    printf("Average Turn Around Time: %.2f\n", (float)total_tat / n); // Print the average turnaround time by dividing the total turnaround time by the number of processes
    printf("Average Waiting Time: %.2f\n", (float)total_wt / n); // Print the average waiting time by dividing the total waiting time by the number of processes
}

// Sort a queue by priority in ascending order using insertion sort, return the pointer to the sorted queue
queue *sort_by_priority(queue *q) {
    queue *sorted = new_queue(); // Create a new empty queue to store the sorted processes
    process *p = dequeue(q); // Dequeue a process from the original queue
    while (p != NULL) { // Loop until the original queue becomes empty
        process *temp = peek(sorted); // Peek at the front process of the sorted queue without removing it
        if (temp == NULL || p->pr < temp->pr) { // If the sorted queue is empty or the current process has lower priority than the front process of the sorted queue, enqueue it to the front of the sorted queue
            p->next = sorted->front;
            sorted->front = p;
            if (sorted->rear == NULL) { // If the sorted queue was empty, update the rear pointer as well
                sorted->rear = p;
            }
        } else { // If the current process has higher or equal priority than the front process of the sorted queue, find its correct position in the sorted queue and insert it there
            while (temp->next != NULL && p->pr >= temp->next->pr) { // Loop until either reaching the end of the sorted queue or finding a process with higher priority than the current process 
                temp = temp->next; // Move to the next process in the sorted queue
            }
            p->next = temp->next; // Insert the current process after the found process in the sorted queue
            temp->next = p;
            if (temp == sorted->rear) { // If inserting at the end of the sorted queue, update the rear pointer as well
                sorted->rear = p;
            }
        }
        p = dequeue(q); // Dequeue another process from the original queue and repeat
    }
    return sorted; // Return the pointer to the sorted queue
}

// Simulate a two-level queue with fixed priority preemptive scheduling for level 1 and round robin scheduling for level 2, given an array of processes, their number, and a time quantum for round robin scheduling, print their execution order and final details 
void two_level_queue(process *p[], int n, int tq) {
    queue *q1 = new_queue(); // Create a new empty queue for level 1 processes
    queue *q2 = new_queue(); // Create a new empty queue for level 2 processes
    queue *completed = new_queue(); // Create a new empty queue for completed processes

    int time = 0; // Initialize a variable for keeping track of time
	int i;
	
    printf("Execution order:\n"); // Print a label for execution order

    while (1) { // Loop until both queues become empty

        for (i = 0; i < n; i++) { // Loop through all processes in the array 
            if (p[i]->at == time) { // If a process has arrived at this time, enqueue it to level 1 queue 
                enqueue(q1, p[i]);
            }
        }

        q1 = sort_by_priority(q1); // Sort level 1 queue by priority in ascending order

        if (!is_empty(q1)) { // If level 1 queue is not empty, execute its front process with fixed priority preemptive scheduling 
            process *current = peek(q1); // Peek at the front process of level 1 queue without removing it 
            printf("%d ", current->pid); // Print its process ID as part of the execution order
            current->rem--; // Decrement its remaining time by 1
            time++; // Increment the time by 1
            if (current->rem == 0) { // If the process has completed its execution, dequeue it from level 1 queue and enqueue it to completed queue, update its completion time, turnaround time and waiting time 
                dequeue(q1);
                enqueue(completed, current);
                current->ct = time;
                current->tat = current->ct - current->at;
                current->wt = current->tat - current->bt;
            } else { // If the process has not completed its execution, check if there is any higher priority process arrived in level 1 queue, if yes, preempt the current process and enqueue it to level 2 queue, otherwise continue executing it 
                for (i = 0; i < n; i++) { // Loop through all processes in the array 
                    if (p[i]->at == time && p[i]->pr < current->pr) { // If a higher priority process has arrived at this time, preempt the current process and enqueue it to level 2 queue 
                        dequeue(q1);
                        enqueue(q2, current);
                        break;
                    }
                }
            }
        } else if (!is_empty(q2)) { // If level 1 queue is empty and level 2 queue is not empty, execute its front process with round robin scheduling 
            process *current = peek(q2); // Peek at the front process of level 2 queue without removing it 
            printf("%d ", current->pid); // Print its process ID as part of the execution order
            int t = 0; // Initialize a variable for keeping track of the time quantum
            while (t < tq && current->rem > 0) { // Loop until either reaching the time quantum or completing the process execution 
                current->rem--; // Decrement its remaining time by 1
                time++; // Increment the time by 1
                t++; // Increment the time quantum by 1
                for (i = 0; i < n; i++) { // Loop through all processes in the array 
                    if (p[i]->at == time) { // If a process has arrived at this time, enqueue it to level 1 queue 
                        enqueue(q1, p[i]);
                    }
                }
            }
            if (current->rem == 0) { // If the process has completed its execution, dequeue it from level 2 queue and enqueue it to completed queue, update its completion time, turnaround time and waiting time 
                dequeue(q2);
                enqueue(completed, current);
                current->ct = time;
                current->tat = current->ct - current->at;
                current->wt = current->tat - current->bt;
            } else { // If the process has not completed its execution, dequeue it from level 2 queue and enqueue it to the rear of level 2 queue 
                dequeue(q2);
                enqueue(q2, current);
            }
        } else { // If both queues are empty, break the loop 
            break;
        }
    }

    printf("\n"); // Print a new line after the execution order

    print_queue(completed); // Print the details of all completed processes along with their average turnaround time and average waiting time 

}

// Main function to test the two-level queue simulation with some sample processes
int main() {
	
    int n; // Number of processes
    int tq; // Time quantum for round robin scheduling
	int i;
	
    printf("Enter the number of processes: ");
    scanf("%d", &n);

    printf("Enter the time quantum for round robin scheduling: ");
    scanf("%d", &tq);

    // Dynamically allocate memory for an array of processes based on the user input
    process **p = (process **)malloc(n * sizeof(process *));
    
    printf("Enter arrival time, burst time, and priority for each process:\n");

    for (i = 0; i < n; i++) {
        int pid, at, bt, pr;
        printf("Process %d:\n", i + 1);
        printf("Arrival Time: ");
        scanf("%d", &at);
        printf("Burst Time: ");
        scanf("%d", &bt);
        printf("Priority: ");
        scanf("%d", &pr);
        // Create a new process with user input and store its pointer in the array
        p[i] = new_process(i + 1, at, bt, pr);
    }

    // Simulate the two-level queue with user input processes and time quantum
    two_level_queue(p, n, tq);

    // Free the allocated memory for the array of processes
    for (i = 0; i < n; i++) {
        free(p[i]);
    }
    free(p);

    return 0; // Return from the main function
}


