## WishShell - A Custom Unix-Style Shell in C

Wish is a lightweight Unix-like shell implemented in C, featuring standard shell behavior along with several built-in utilities such as duplicate file detection, network diagnostics, reminders, to-do management, and file cleanup.
This project demonstrates system-level programming using process control, threads, filesystem traversal, and network calls.

---
## Features
- Core Shell Functionality
- Built-in commands: cd, exit
- Supports batch mode and interactive mode

---
## Extended Build-in Commands
- **DupFinder**: Recursively scans a directory to detect duplicate files using XOR-based file checksums.
- **NetStatus**: Displays internet connectivity status, public IP, and ping latency.
- **Remindme** : Runs reminders asynchronously using POSIX threads.
- **Todocli** : CLI-based to do list manager that add, remove and list taks.
- **wcleanup** : Cleans up source file by Removing extra whitespaces , normalizing tabs and stripping trailing spaces.

---
## Build & Run


      make       # Builds the project
      ./wish
      make clean # Clean build files
