# Operating System Engineering

> Based on [MIT 6.S081](https://pdos.csail.mit.edu/6.828/2020/index.html)

***

## References

* [YouTube Unix Intro](https://www.youtube.com/watch?v=tc4ROCJYbm0)
* a 

***

## Intro

* Purpose of OS:
  * Abstract and manage hardware
  * Multiplex: share the hardware among multiple programs at the same time
  * Isolation
  * Sharing
  * Security
  * Performance
  * Generality
  
* OS organisation

  * *Kernel*: a special program that provides services to running programs

  * *Process*: running program

  * *Shell*: command line interface, an ordinary program that reads commands from the user and executes them

    ==NOTE==: shell is a user program

  * *System call*: API of kernel


![kernel_user](D:\OneDrive\NCL\Extracurricular content\MIT_6.s081\imgs\kernel_user.png)

* Why hard/interesting?

  * Unforgiving
  * Tensions
    * Efficient - Abstract
    * Powerful - Simple
    * Flexible - Secure
  * Lots of thoughts

* Xv6 system calls

  ![system_calls](D:\OneDrive\NCL\Extracurricular content\MIT_6.s081\imgs\system_calls.png)

* 
