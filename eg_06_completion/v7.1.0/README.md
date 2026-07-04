# completion

Using completion variables is an easy way to synchronize between two tasks in
the kernel when one task needs to signal to the other that an event has
occurred. One task waits on the completion variable while another task performs
some work. When the other task has completed the work, it uses the completion
variable to wake up any waiting tasks.

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

Copy **completion.ko** and **load_module.sh** file to the target machine,
then run:

```bash
./load_module.sh
```

## test the module

In this example, we need two consoles in which we can perform read and write
separately. You can use any remote login tools, such as ssh or telnet, to open
another console. For users who followed my("d0u9") QEMU dev setup instruction, it
should already successfully have a telnetd daemon running. For more details,
check [00_preface/03_telnet_server.md](../00_preface/03_telnet_server.md) for
more details.

After the kernel module is loaded successfully. Execute the following command
in console 1:

```bash
cat /dev/completion
```

This command will block the current process, waiting for that if some other
process write something into the same file.

Here, in console 2, execute `echo "something" > /dev/completion`. After this,
the **cat** process previously executed in console 1 is waken up.

```
[  676.515274] :completion_open: invoked
[  676.515664] :completion_read: invoked
[  676.516053] :completion_read: process 278(cat) going to sleep
[  678.690938] :completion_open: invoked
[  678.691343] :completion_write: invoked
[  678.691344] :completion_write: process 236(sh) awakening the readers...
[  678.691383] :completion_read: awoken 278(cat)
```
---

### ¶ The end

