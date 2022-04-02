# Operating System Engineering

> Based on [MIT 6.S081](https://pdos.csail.mit.edu/6.828/2020/index.html)

***

## References

* [YouTube Unix Intro](https://www.youtube.com/watch?v=tc4ROCJYbm0)
* a 

***

## Intro

* Purpose of OS:
  * Abstract the hardware for convenience and portability
  * Multiplex the hardware among many applications: share the hardware among multiple programs at the same time
  * Isolate applications in order to contain bugs
  * Allow sharing among cooperating applications
  * Control sharing for security
  * Don't get in the way of high performance
  * Generality: Support a wide range of applications

* Why hard/interesting?

  * Unforgiving environment
  * Tensions
    * Efficient - Abstract
    * Powerful - Simple
    * Flexible - Secure
  * Lots of thoughts

* OS organisation

  * *Kernel*: a special program that provides services to running programs

    * Services kernel provides:
      * process
      * memory allocation
      * file contents
      * file names, directories
      * access control (security)
      * many others: users, IPC, network, time, terminals

  * *Process*: a running program has user-space memory containing instructions, data, and a stack and pre-process state private to the kernel

    * A process alternates between executing in *user space* and *kernel space*
    * Time-sharing processes: transparently switches the available CPUs among the set of processes waiting to execute

  * *Shell*: command-line user                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   interface, an ordinary program that reads commands from the user and executes them

    ==NOTE==: shell is a user program

  * *System call*: API of kernel

    * Look like function call but **NOT**


![kernel_user](D:\OneDrive\NCL\Extracurricular content\MIT_6.s081\imgs\kernel_user.png)

* *File descriptor*

  * A small integer representing a kernel-managed object (a "file") that a process may read from or write to
  * A process may obtain a file descriptor by
    * opening a file, directory, or device
    * creating a pipe
    * duplicating an existing descriptor
  * the FD interface abstracts away the differences between files, pipes, and devices, making them all look like streams of bytes
  * FD indexes into a per-process table maintained by kernel
  * Different processes have different FD spaces of fds starting at zero
  * By convention, a process reads from FD 0 (standard input), writes output to FD 1 (standard output), and writes error messages to FD 2 (standard error)
  * The shell exploits it to implement I/O redirection and pipelines
  * The shell ensures that it always has three FD open, which are by default FDs for the console

* *Pipes*

  * **A small kernel buffer** exposed to processes as a pair of FDs, one for reading and one for writing
  * Writing data to one end of the pipe makes that data available for reading from other end of the pipe
  * Four advantages over temporary file:
    * pipes automatically clean themselves up
    * pipes can pass arbitrarily long streams of data
    * pipes allow for parallel execution of pipeline stages
    * efficient in IPC because of pipes' blocking reads and writes

* *File system*

  * File system provides 
    * data files, which contain uninterpreted byte arrays
    * directories, which contain named references to data files and other directories
  * The directories form a tree, starting at a special directory called the `root`
  * `path`: `/a/b/c` refers to the file or directory named `c` inside directory `b` inside directory `a` in the root directory `/`
  * Paths that don't begin with `/` are evaluated relative to the calling process's current directory, which can be changed with `chdir`
  * A file's name is distinct from the file itself: the same underlying file, called an *inode*, can have multiple names, called *links*
  * Each inode is identified by a unique inode number
  * Each link consists of an entry in a dir; the entry contains a file name and a reference to an inode
  * An inode holds *metadata* about a file, including
    * type (file or directory or device)
    * length
    * location of the file's content on disk
    * number of links to a file

* Xv6 system calls

  ![system_calls](D:\OneDrive\NCL\Extracurricular content\MIT_6.s081\imgs\system_calls.png)

  * `int fork()`

    * create a new process called the *child process*

    * child process has the **same memory contents** as the calling process called *parent process*

      the parent and child are executing with **different memory and different registers**, changing a variable in one does not affect the other

    * return child's PID in parent; return zero in child

  * `int exit(int status)`

    * cause the calling process to stop executing and to release resources such as memory and open files
    * `status`: conventionally `0` to indicate success and `1` to indicate failure
    * no return

  * `int wait(int* status)`

    * return the PID of an exited (killed) child of the current process and copies the exit status of the child to the `*status`
    * if none of the caller's children has exited, `wait` waits for one to do so
    * if the caller has no children, return `-1`
    * if the parent doesn't care about the exit status of a child, it can pass a `0` address to `wait`

  * `int exec(char* file, char* argv[])`

    * replace the calling process's memory with a new memory image loaded from a file stored in the file system
    * the file must have a particular format, which specifies which part of the file holds instructions, which part is data, at which instruction to start, etc.
    * when `exec` succeed, it doesn't return to the calling program; instead, the instructions loaded from the file start executing at the entry point declared in the file header
    * two arguments: name of the executable file, an array of string arguments
    * conventionally, the first element is the name of the program (executable file) that most programs ignore; the last element is `0`
    * To avoid the waste of creating a duplicate process and then immediately replacing it, os kernels optimize the implementation of `fork` for this use case by using virtual memory techniques such as copy-on-write

  * Xv6 allocates most user-space memory implicitly: 

    * `fork`
    * `exec`
    * `sbrk(n)`: a process needs more memory at run-time

  * `int read(int fd, char* buf, int n)`

    * read at most `n` bytes from the `fd`, copies them into `buf`, and returns the number of bytes read
    * File has an offset. `read` reads data from the current file offset and then advances it
    * When there are no more bytes to read, return `0` to indicate the end of the file

  * `int write(int fd, char* buf, int n)`

    * writes `n` bytes from `buf` to the `fd` and returns the number of bytes written
    * Fewer than `n` bytes are written only when an error occurs.
    * also has offset

  * `int close(int fd)`

    * releases a FD, making it free for reuse by a future `open`, `pipe`, or `dup`
    * A newly allocated FD is always the lowest-numbered unused descriptor of the current process

  * **I/O redirection**

    * FD and `fork` interact to make I/O redirection easy to implement
    * `exec` replaces the calling process's memory but preserves its file table
    * although `fork` copies the FD table, each underlying file offset is shared between parent and child; this behaviour helps produce sequential output

    ```c
    // simplified version of the code a shell runs for command 
    // cat < input.txt
    
    char* argv[2];
    
    argv[0] = "cat";
    argv[1] = 0;
    if(fork() == 0) {
        close(0);
        open("input.txt", O_RDONLY);
        exec("cat", argv);	  // cat execute with fd 0 (standard input) 
        					// referring to input.txt
    }
    ```

  * `int open(char* file, int flags)`

    * open a file, returns a FD
    * flags control what `open` does
      * `O_RDONLY`: for reading
      * `O_WRONLY`: for writing
      * `O_RDWR`: for both reading and writing
      * `O_CREATE`: to create the file if it doesn't exist
      * `O_TRUNC`: to truncate the file to zero length

  * Why `fork` and `exec` instead of `forkexec`

    * Between the two, the shell has a chance to redirect the child's I/O without disturbing the I/O setup of the main shell
    * A common UNIX idiom:
      * `fork` a child process;
      * `exec` a command in the child
      * parent `wait` for child to finish

  * `int dup(int fd)`

    * duplicates an existing FD, returns a new one that refers to the same file, sharing an offset

      ==Note==: the value of new FD is not equal to the old one

    * Two FDs share an offset if they were derived from the same original FD by `fork` and `dup` calls. Otherwise FDs don't share offsets even if they resulted from `open` calls for the same file.

  * `int pipe(int p[])`

    * Creates a new pipe and records the read and write FDs in the array `p`
    * If no data is available, a `read` on a pipe waits for either data to be written or for all FDs referring to the write end to be closed; in the latter case, `read` return `0`

  * `int chdir(char* dir)`

    * Change the current directory

  * `int mkdir(char* dir)`

    * Create a new directory

  * `int mknod(char* file, int, int)`

    * Create a new device file
    * Two `int` arguments: major and minor device numbers, which uniquely identify a kernel device
    * When a process later opens a device file, the kernel diverts `read` and `write` system calls to the kernel device implementation instead of passing them to the file system

  * `int fstat(int fd, struct stat* st)`

    * Place info about an open file into `*st`

  * `int link(char* file1, char* file2)`

    * Create another name (file 2) for the file 1
    * Reading or writing to file 1 is the same as to file 2

  * `int unlink(char* file)`

    * Removes a name from the file system

    * The file's inode and the disk space are only freed when the link count is zero and no FD refer to it

    * An idiomatic way to create a temporary inode with no name that will be cleaned up when the process closes `fd` or exists

      ```c
      fd = open("/tmp/xyz", O_CREATE | O_RDWR);
      unlink("/tmp/xyz");
      ```

* Real world
  * The idea sparked a culture of "software tools" that was responsible for much of UNIX's power and popularity
  * The shell was the first so-called "scripting language"
  * The UNIX system call interface has been standardized through the Portable Operating System Interface (POSIX) standard
  * Modern kernels evolve continuously and rapidly, and offer many features beyond POSIX
  * UNIX unified access to multiple types of resources (files, directories, and devices) with a single set of file-name and file-descriptor interface

***

