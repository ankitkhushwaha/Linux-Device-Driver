# poll

The poll mechanism is a Linux kernel mechanism for I/O event notification. Its
function is to monitor multiplue file descriptors to see I/O is possible on
any of them. Different from the tranditional block I/O, poll mechanism promise
its users to monitor many file desciptors, normal file descriptrs or socket
file descriptors. If any interested event occurs on any file desciptor, for
example the file desciptor can be read or write, the poll will notify the user
process and tell it what happened.

In this example, we implement a simple char device to test our poll ability.
After the module is successfully loaded, Poll method checks whether buffer 
has data to read or has space to write. If write call write data to buffer,
it wakes up read `wake queue` and similary read call flushes out the data
after sending data to userspace and free `N` bytes and wakes up write
`wake queue`.

## build the module

To build this module, execute:

```bash
make KERNELDIR=/path/to/kernel/source/dir
```

If you have already set and exported `KERNELDIR` environment variable, simply
execute `make` is enough.

If neither `KERNELDIR` environment variable nor `KERNELDIR` option of make
are set, the current running kernel will be built against.

## Usage

Copy **load_module.sh** and **poll.ko** files to the target machine, then run:

```bash
sh load_module.sh
```

## test the module

After successfully load the driver, simply run the executable **poll.out** in
the **test** directory.

Each time the file descriptor is writable, we write the current loop counter
into the device file. Likewise, each time the file desciptor is readable, we
read the content from it and print to the screen.

As we described before, the file descriptor to the device file we opened will
be readable every 1 second, and will be writable every 2 seconds.

Sample output as below:

```
polling ...
[3] read: Hello World!
[3] write: Hello world! -> 4
polling ...
[3] read: Hello world! -> 4
polling ...
[3] read: Hello world! -> 4
[3] write: Hello world! -> 2
polling ...
[3] read: Hello world! -> 2
polling ...
[3] read: Hello world! -> 2
[3] write: Hello world! -> 0
```

---

### ¶ The end
