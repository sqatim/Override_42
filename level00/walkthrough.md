# Walkthrough

Upon connecting to the `level00` user, we discovered a binary file named `"level00"`. When executing it, we were prompted to enter a password for authentication.

```sh
level00@OverRide:~$ ./level00
***********************************
*             -Level00 -          *
***********************************
Password:AAAA

Invalid Password!
```

To better understand its behavior, we attempted to reverse it using `gdb`. We started by disassembling the `main` function, which displayed a message to the user and then awaited a password input using the `scanf` function.

```c
   0x0804849d <+9>: mov    DWORD PTR [esp],0x80485f0 // "***********************************"
   0x080484a4 <+16>: call   0x8048390 <puts@plt>
   0x080484a9 <+21>: mov    DWORD PTR [esp],0x8048614 // "* \t     -Level00 -\t\t  *"
   0x080484b0 <+28>: call   0x8048390 <puts@plt>
   0x080484b5 <+33>: mov    DWORD PTR [esp],0x80485f0 // "***********************************"
   0x080484bc <+40>: call   0x8048390 <puts@plt>
   0x080484c1 <+45>: mov    eax,0x804862c // Password:
   0x080484c6 <+50>: mov    DWORD PTR [esp],eax
   0x080484c9 <+53>: call   0x8048380 <printf@plt>
   0x080484ce <+58>: mov    eax,0x8048636
   0x080484d3 <+63>: lea    edx,[esp+0x1c]
   0x080484d7 <+67>: mov    DWORD PTR [esp+0x4],edx
   0x080484db <+71>: mov    DWORD PTR [esp],eax
   0x080484de <+74>: call   0x80483d0 <__isoc99_scanf@plt>
   0x080484e3 <+79>: mov    eax,DWORD PTR [esp+0x1c]
```

In the case where the password input was incorrect and not equal to `"5276"`, the program printed the message "\nInvalid Password!".

```c
   0x080484e7 <+83>: cmp    eax,0x149c
   0x080484ec <+88>: jne    0x804850d <main+121>
   --------------------------------------------------------------
   0x0804850d <+121>: mov    DWORD PTR [esp],0x8048651 // "\nInvalid Password!"
   0x08048514 <+128>: call   0x8048390 <puts@plt>
```

On the other hand, if the input matched the password `"5276"`, the program displayed the message `"Authenticated!"` and then started a new shell process `(/bin/sh)` within the current process.

```c 
   0x080484ee <+90>:	mov    DWORD PTR [esp],0x8048639 // "Authenticated!"
   0x080484f5 <+97>:	call   0x8048390 <puts@plt>
   0x080484fa <+102>:	mov    DWORD PTR [esp],0x8048649 // "/bin/sh"
   0x08048501 <+109>:	call   0x80483a0 <system@plt>
```

The following example is what happens after entering the correct password.

```sh
level00@OverRide:~$ ./level00
***********************************
*              -Level00 -         *
***********************************
Password:5276

Authenticated!
$ cd ../level01
$ ls -la
total 17
dr-xr-x---+ 1 level01 level01   80 Sep 13  2016 .
dr-x--x--x  1 root    root     260 Oct  2  2016 ..
-rw-r--r--  1 level01 level01  220 Sep 10  2016 .bash_logout
lrwxrwxrwx  1 root    root       7 Sep 13  2016 .bash_profile -> .bashrc
-rw-r--r--  1 level01 level01 3533 Sep 10  2016 .bashrc
-rwsr-s---+ 1 level02 users   7360 Sep 10  2016 level01
-rw-r--r--+ 1 level01 level01   41 Oct 19  2016 .pass
-rw-r--r--  1 level01 level01  675 Sep 10  2016 .profile
```
