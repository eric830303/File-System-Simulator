# File System Simulator
---
## Inroduction
  This is a file system simulator running on **Linux Ubuntu**.
  The program is written using **C/C++**. Note that, the standard of C++ should be C++11 (or later version)

---
## Check Before Compiling
* **makefile** must exist. You cannot compile without the file.
* **src** (a directory) must exist. It contains all source code.
* **obj** (a firectory) must exist. It will contain object code.
---
## Check Before Running
* **setting** (a directory) must exists. And it contains following files:
 1. **setting/user.txt** must exist. It stores the information of users. It must exist.
 2. **setting/tree.txt** must exist. It stores the structure of file system. It must exist. Otherwise, the program cannot work
 3. **setting/file.txt** must exist. It stores the contents of files.
---
## Run
* Run command **make** on your terminal

```
user@NCTU:~/FileSystem: make
[Info] Complete! Execution file: fs
```
* Then, the executable, **fs**, will be made.
* Run **./fs** . Then, you will see following info on your screen.

```
User@NCTU:~/FileSystem: ./fs
	***********************************************************
	*               File System                               *
	*               1. Registration for New User              *
	*               2. Sign in                                *
	*               3. Help                                   *
	*               4. Exit                                   *
	**********************************************************t
	Please choice: 

```



