# SimProductionLine   
> _An assignment I designed for the course Principles of Operating Systems when I served as the tutor of that course. This is a simplied version._

## Objectives
• Use Pthread library to write multithreaded program    
• Use semaphores to handle thread synchronization   
• Use semaphores to limit resource usage   
• Learn parallel programming   
• Solve deadlock problem   

## Background Story
Tesla Motors is the leading company manufacturing electric cars and is also the first company who made electric cars so popular around the world. Tesla factory almost automated the whole production process by using a lot of smart trainable robot workers.       
Your job in this assignment is to write a program to simulate the production process of Tesla electric car and make the production process as efficient as you can but with limited resources.    

## System overview
To simplify the process of manufacturing Tesla electric cars, 7 car parts need to be built before the final assembly of a car. These parts are skeleton, engine, chassis, body, window, battery. The relationship among these parts can be found in the graph below:   
<img width="292" alt="image" src="https://user-images.githubusercontent.com/9710644/168219489-714ba1a3-4e12-401e-9aec-30787b9d688f.png">   
The parts which no arrow are pointing at depend on their own raw materials. We just assume that they are ready by default. The production rule is: 1 skeleton, 1 engine, and 1 chassis make 1 car body; 7 windows, 1 body, 4 tires, and 1 battery pack make 1 car. Each produced part except the car will consume 1 unit of storage space. The space will get released only when the part is consumed by someone. In another word, only the robot who is producing a chassis or a car will "produce" space. Each part, car, and storage space is represented by corresponding semaphore. These semaphores are not directly accessible to you. The semaphore values will be changed as you call functions defined in production.h. Please read through the source code files before you start to work on the assignment.    
## Q1 Complete the simple multithreaded version   
Follow the instructions in main.c and complete the production process (create num_typeB robots for type B robots and num_typeC robots for type C robots). Note that Job IDs are continuous numbers. You can use a for loop to enqueue Job IDs to the queue. You can also manually type “SKELETON”, “ENGINE”, … if you find it is clearer. Don’t forget to join all robot threads. You can assume there's unlimited amount of storage space in this question.   
## Q2 Implement a deadlock free multithreaded program
Copy the completed code from q1 to q2 and make it a deadlock free multithreaded program given limited amount of space. By the end of this question, your program should be able to produce any number of cars with storage space as little as **2** units and **2** robots.   
### Hints
1. You can use more queues and define your own semaphores to help you solve the deadlock problem.    
2. You can choose either deadlock prevention or deadlock detection strategy you've learnt in the lectures and tutorials. 
#### Deadlock prevention
The production process is executed in a way that we can be sure there won’t be any deadlock.    
E.g.: The hungry philosopher example in tutorial 4. Those philosophers signed an agreement to ensure there is no deadlock.
You can either have a clever scheduler to arrange the order of jobs so that there’s no deadlock or your robots are smart enough to pick jobs to ensure the whole production process is deadlock free.    
#### Deadlock detection
We don’t know if there will be a deadlock situation, but we can detect one.    
E.g.: If one robot has been waiting for a space for unreasonable amount of time, we can say that there is a deadlock. Then you may need to rearrange the jobs so that the deadlock can be broken. Note that you are not allowed to cancel an ongoing job when a robot has got a unit of space and starts the production process.    
Hint: You may find `timedTryMakeXXX()` functions useful. Be noted, you should determine a reasonable amount of waiting time. If you wait too long, deadlock detection will degrade the performance of production.   
## Q3 Make it run fast!
Create a directory named q3 and copy your deadlock free code from q2. You may have noticed that different types of robot can produce car parts and the car in different speed. To be specific the time needed for each types of robot is as the following:   
<img width="235" alt="screenRecording 2022-05-13 at 17 18 08@2x" src="https://user-images.githubusercontent.com/9710644/168231740-de82add6-937d-49e6-b905-10da418df0bc.png">    
Your task in q3 is to design an efficient deadlock-free production scheduling scheme with the given number of robots of each type. Your program will compete against your classmate. We'll test your program with various parameter combinations. Your mark of this question will depend on the time difference of your program and the fastest one in your class.   





