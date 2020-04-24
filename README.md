# OS_Project

Mục tiêu hiểu về Linux kernel module và hệ thống quản lý file và device trong linux, giao tiếp giữa tiến trình ở user space và code kernel space 


+ Viết một module dùng để tạo ra số ngẫu nhiên. 
+ Module này sẽ tạo một character device để cho phép các tiến trình ở userspace có thể open và read các số ngẫu nhiên.

## To run this program

In order to run this program, please following these below instructions:  
```console
cd src/randNumber
make
```
- Create kernel module  
`sudo insmod randNum_chardev.ko`  


- In order to check whether device is created  
`cat /proc/devices | grep random_device`  


- In order to check our device file is created  
`ls -la /dev/random_dev`  
Makesure all users can read and write but cannot execute the file/folder  
`sudo chmod 666 /dev/random_dev`  


- In order to see *MAJOR,MINOR* of device number,constructor,destructor announcemence 
`dmesg`  


- To use this kernel mode through userspace:  
`cd user_app/`  
`make`  
`./user_test`  

The console screen output would be:  
```console
Select below options:
        o (to open a device node)
        c (to close the device node)
        r (to read a generated random number)
        q (to quit the application)
Enter your option:
```

## To clean files and stop this module

- Stop this module  
`sudo rmmod randNum_chardev`  

- In **user_app** and **randNumber** folder  
`make clean`  

