# CRTOS
Multi-tasking kernel for small Microchip MCUs `PIC10` `PIC12` `PIC16` `PIC18`

Time-slicing cooperative multitasking `C` language source code for system engineering and lowest footprint. 
The CRTOS scheme ensure real-time performance. The code files are test-built using: 
```
Microchip MPLAB X IDE v5.25
Microchip XC8 v1.42 compiler
```
The origin of `CRTOS` began from the `B Knudsen Data` `cc5x` compiler who inspired the use of the `computed goto` code construct. When applied to the Microchip MIPS core ultra-efficient jump table will be produced. It is also noteworthy that `cc5x` can be integrated seamlessly into MPLAB X. 

`CRTOS` comprises these files:
```
ALS.h
CRTOS.h
CRTOS.c
main.c
```
The `CRTOS` tick ('heartbeat') uses `TMR0`. While low cost PIC variants do not feature interrupt, our `CRTOS` schema performs equally well on them. This is the first MCU resource needed:
```
TMR0 with or without interrupt
```
The scheduler is implemented in `main()` by hard-coding. For maximum achievable code efficiency, the `goto` jump construct is used. On the Microchip MIPS core the code structure will compile into "computed goto" jump table. In the following example at 0x2E and 0x2F `case`s in a `swtich()` translate into assembler `GOTO`s. 
```
!        switch(i)
0x2B: MOVF i, W
0x2C: XORLW 0x0
0x2D: BTFSC STATUS, 0x2
0x2E: GOTO 0x28
0x2F: GOTO 0x18
!        {
!            case 0: t = task0(); break;
0x28: CALL 0x5A
0x29: MOVWF t
0x2A: GOTO 0x30
```
Doing it this way is proven to be exceptionally efficient. Code size is sub-compact and code execution is fast and reliable. Much code execution context will be left available for more accurate timing and better responsiveness to events. 

Take the `PIC12F675` as an example, on this target if we put in 3 tasks and 3 re-entrance point per task, MCU resource usage was found to be: 
```
RAM 14 bytes
FLASH ROM 170 code words
```
Adding each task 
```
RAM +2 bytes
FLASH ROM +5 code words
```
As extreme compactness is demonstrated, inter-process communication uses global variables. Prevailing concepts such as semaphores, mutex and events are forgone. 

The term 'task' loosely refer to a code block of a certain functionality e.g. an LED task. Within a task there is at least one "self-blocking" point at which controls reliquish to the OS scheduler - the essence of cooperative multitasking (as opposed to preamption, for example, which needs significantly more resources for "context switching"). 
```
   <task code block begin>
   .
   .
   .
   <return code execution to OS>
   <task code block end>
```
In `CTROS` two macros that return code execution to OS are available:
```
CRTOSRelinquish2NextTaskMacro(t,i)
```
When code executes to this line, the task is suspended temporarily, and it will resume execution at point (`t`,`i`). Before concrete examples are given, the next paragraph explains parameters `t` and `i`.

Each task is referred to by its unique identifier `t`. Parameter `i` is the "re-entrance index". While giving up local program control flow returning execution to the OS scheduler, the macro specifies a re-entrance point using these parameters. By this mechanism, the task "cooperatively" execute in small chunks ensuring other task has a chance to run (prevents contention). On the next round control is given back to our task, it will resumes to (continue running code from) point(`t`,`i`). There is also a three-parameter macro with which the task can be suspended, but for a definitive time period `d`: 
```
CRTOSTaskDelayMacro(t,i,d)
```
This macro suspends the task into the `delay` state. The OS scheduler keeps track of time `d` count(s) of system tick. After this delay time period has expired, the task will become eligible to continue execution. `CRTOS` utilizes the round-robin scheduling method, on the next round the task resumes at the appropriate position. 

Concrete example:
```
   <task 5 begin>
   .
   CRTOSRelinquish2NextTaskMacro(5,2); // --------> program execution is given back to OS scheduler
   T5L2:                               // <-------- resume to this point the next scheduling round
   .
   .
   <task 5 code block end>
````
The macro suspends the task, and on the next round execution will continue at label `T5L2`. Recap that there are two version of task yield macro. When the non-delaying version is used, the task becomes eligible to execute in the next scheduling round immediately after it has relinquished control. 

Concrete example:
```
   <task 7 begin>
   .
   T7L1:                               // <-------- resume to this point the next scheduling round
   .
   CRTOSRelinquish2NextTaskMacro(7,1); // --------> program execution is given back to OS scheduler
   .
   .
   <task 7 code block end>
````
The macro suspends the task, and the next round execution will continue at label `T7L1`. Since ths label is placed above (but not after) the macro, the construct is actually a loop that "multi-threads".

This repo includes a "User Guide". Building up from the information presented here, the UG walks the reader through creating an `MPLAB X` project (from scratch) that incorporates `CRTOS`.  

As illustrated above, the idea is to split a long piece of code, provide a reliable mechanism to escape to the OS, and at the same time have a facility for code execution to rejoin the "thread". Apart from just being able to resume execution where control was left off last time, provision is made so that at the point of relinquish the coder can specify the location at which the next rejoin point. 

`CRTOS` reserved two bytes per task. One of these stores the state of each task in bit flags, for example, if the task is suspended or if it is eligible to be rescheduled are stored in these flags. At the time that it is designed, the bit flags are put in place to allow for flexibility. Extending the implementation to include more advanced RTOS functionality such like task changing the state of one another is now considered of limited practical value. 

The `CRTOS2` scheme gave up the bit flag byte. `CRTOS2` consumes only one byte per task. Tasks are either suspended when its delay counter is non-zero or they are eligible when its delay counter is zero. Further, tasks are put into functions (template `task0.c`). This way they are modularised into more readable units for easier code maintenance. `CRTOS2` files set comprises:
```
CRTOS2.h
CRTOS2.c
task0.c
mainCRTOS2.c
```
`CRTOS2` no longer has the two macros discussed above. The mechanics of task yielding can be said to have been "simplified". To keep track, a function return value and a local variable are used instead.

In `CRTOS2` tasks are written as function. The function return value would be the amount of OS tick delay that the coder desires. File scope variable stores the task "re-entrance index". By setting this variable to the correct value prior to relinquisition, the next round the task function is called, the `switch()` block near the beginning of the function automatically select the designated label to jump to (the act of "re-joining" a "thread"). To give an idea how much MCU resource will be needed, the footprint of this newer `CRTOS2` (with one task):
```
RAM 10 bytes
FLASH ROM 106 code words
```
For each task
```
RAM +1 byte
FLASH ROM +1 code words
```
Although the figures given in this text do not compare directly, they do give some sense of the compactness of both the implementations of `CRTOS` and of `CRTOS2`. Some metrics are listed below:
```
                    CRTOS      CRTOS2
number of task N    0..255     0..255
task delay          0..255     0..255
label per task      0..16      0..255
flavour             small N   larger N
raw speed           extreme     high
```
"Flavour" refers to the way the OSes manage their tasks. `CRTOS` uses hard code to scan through the list of tasks.  `CRTOS2`, however, loops through the list instead. The former uses less RAM and it is also the fastest way to do such work. The latter consumes two bytes RAM but the code does not grow even when the number of task increases (because only the loop variable need changing to accommodate increase). 

`CRTOS` and `CRTOS2` borrow from the Real-Time Operating System. They are very useful tool to achieve near real-time performance and responsiveness on low cost hardware. In essence:
```
Provides a framework on which code can be developed quickly
Extremely peformance-, footprint- and RAM-aware. 
Scheduler core together with code exit and re-enter mechanism mimics aspect of RTOS
Ideal for PIC10, PIC12, PIC16
```
Users of higher spec `PIC16` `PIC18` models may consider `cocoOS`. 16-bit `PIC24` and `dsPIC` would probably find `FreeRTOS` an attractive upgrade. 
"# 06SENSE_PULSE" 
