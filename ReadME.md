<b>This</b> is the source code of an operating system schedular made with C programming language

using forking , Message Queue and semephores 4 scheduling algorithms are simulated with process data in Process.txt.

Output is generated in 3 files :-

Schedular.perf: calculates CPU utilization, Average waiting turn-around time, Average Waiting time for each process. 

Schedular.log: represents the full scenario of all states of any process (Started, Stopped, Resumed, Finished) at any time.

Memory.log: represnts memory operations (Allocated and Deallocated) for all processess during the simulation.







#TOC
- How to Run
- Data Structures & Functions used
- Scheduling ALgorithms
 -- Shortest Job First (SJF)
 -- Highest Priority First (HPF)
 -- Round Robin (RR)
 -- Multilivel Feedback Loop (MLFL)
 
# Run
You can use Makefile to build and run your project

<h1>To run your project, use the command:

make run



<h1> Data Structures & Functions </h1>

Normal Node
to be used in all other data Structures
Linked-list
It is used to save the processes in it after reading them from the file.
Functions used with this data structure are
InsertInto_LinnkedList: Insert a node at the end of the LinkedList.
Queue
It is used with a multi-level algorithm where each level has a queue of processes to be run in that level.
Functions used with this data structure are
Insert_Into_Queue: Insert node at the beginning of the queue.
Delete_From_Queue: Dequeue from the queue and free the node

Circular Queue
It is used with round-robin algorithms where the rear of the queue points to the beginning of it so when we reach the end we can return to the beginning so that we can run the processes again.
Functions used with this data structure are
InsertInto_CircularQueue: Insert a node at the end of the circular queue.
Delete_From_Circular: Delete a node from any position from the circular queue.

Priority Queue
It is used in HPF and SJF algorithms, it has a pointer to a node that points to the front of the queue that will be used to pop this node when it finishes its execution.
Functions of Data structure:
insert_to_priority_queue: Insert the incoming process to the correct position.
Pop_from_priority_queue: Delete the node at the front of the Queue.
Is_empty_Queue: Check if the Priority queue is empty.

Multilevel Queue
It is a 2-D Queue of 11 levels where each is a queue and an extra buffer queue
Functions used with this data structure are:
pushIntoMultiLevel: It inserts a process in a level depending on its priority where each level is specific for a certain priority number (0-10).
pushIntoNextLevel: pops a process from its level and pushes it to the next level
AreAllLevelsEmpty: Check if all levels are empty
isMultiLevelEmpty: Check if all levels and buffer queue are empty
pushAllProcessBackToItsLevel: pushes all processes from the buffer back to its level
getNextProcessFromMultiLevel: returns a pointer on the next process that is going to run
runMultiLevelProcess: decrement process remaining time, calls the sophomore and check if the process is over 
Algorithms Explanations and Results

<h2>All Scheduling Algorithms will run on on the following example:-</h2>
  
  
![Process.txt](https://i.ibb.co/XXxHc5L/1-Input.png)

SJF
Explanation:
The SJF algorithm uses Priority Queue Data Structure which is sorted according to running time,  Since its a batch algorithm the first thing it does is dequeuing the head (which will run) and store it in a special memory then run it (Decrease Remaining Time) till it finishes (remaining time = 0 & process exit from its own as both scheduler and process are synchronized ) then it repeats the same process all over again.

Output:	
![Process.txt](https://ibb.co/SvvL6kh)


HPF
Explanation:
The HPF algorithm uses Priority Queue that rearranges the processes after each process arrives to queue and preempts the running process if the process that arrives at the scheduler has higher priority than it. Every clock checks on the remaining time and if it equals 0 → change the process state to be FINISHED. Define a pointer to the front of the priority queue and check the value that this pointer points to change this means → the process arrives at the scheduler and preempts the running process so, change the process that this pointer points to be STOPPED and these checks repeat every clock. 

Output:
![Process.txt](https://ibb.co/GT4m8GX)
![Process.txt](https://ibb.co/4VQr3JW)



RR
*Quantum used = 2;
Explanation:
The round-robin algorithm runs ready processes in a ready circular queue where a pointer traverses on the circular queue and runs a process for a quantum then goes to the next process and when it reaches the end it returns back to the beginning of the queue. 
The link between the scheduler and each process.c program is done by a semaphore specific for each process(semaphore is specified by process id), where each process is trying to down the semaphore to decrement the remaining time at the process.c program, on the other hand, once the RR function is called it decrements its data structure remaining time and up the semaphore so that process.c can down the semaphore.
When a process finishes its running time it is deleted from the ready circular queue and freed, also its process.c program terminates by itself.

Output:
![Process.txt](https://ibb.co/FJZrSQv)
![Process.txt](https://ibb.co/Y29xxPs)




Multi-Level Feedback Loop (MLFL)
Quantum used = 5;
Explanation:
The multiLevel Algorithm starts by getting the first process in the highest level with the lowest priority
After it finishes its quantum its pushed into the next level and a new process is brought by the same algorithm as before
If it was in level 10 and finished its quantum; it is pushed into the buffer
When all the levels are empty, all the processes in the buffer are returned their initial level
If a process is done, it is popped from its level and a new process is started with a refreshed quantum.



Output:
![Process.txt](https://ibb.co/HnHDf69)






