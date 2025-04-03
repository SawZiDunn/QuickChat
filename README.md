# QuickChat  

A modern chat application built with Qt6 that supports both private and group chat functionality.  

This project is our **first-year final project**, developed to demonstrate our understanding of C++ programming, database management, and GUI design using Qt.  

## Overview  

QuickChat is a messaging app designed for easy and efficient communication. It supports both private and group chats, providing a simple and user-friendly interface. Messages are stored securely in a local SQLite database, ensuring they remain accessible whenever needed.

### Key Highlights  
- Intuitive user interface with separate windows for private and group chats  
- Persistent message storage  
- Easy-to-use group chat management  
- Users can view both their created groups and the groups they have joined  
- Modern desktop integration  

## 🚀 Technologies Used  

### **Framework**  
- **Qt6 (6.5+)**  
  - Qt Widgets for the GUI components  
  - Qt SQL for database operations  

### **Backend**  
- **C++17**  
- **SQLite** for data persistence  
- Custom database handler for efficient message management  

### **Build System**  
- **CMake 3.19+**  
- Qt's build tools and deployment system  

### **Architecture**  
- **Widget-based UI components**  
- **Event-driven communication system**  
- **Modular design** with separate components for chat, database, and UI management  

## ✨ Features  

- **Private chat messaging**  
- **Group chat functionality**  
- **View created groups and joined groups**  
- **Modern and intuitive user interface**  
- **SQLite database for message storage**  

## 📦 Installation  

### **Prerequisites**  
Ensure you have the following installed on your system:  
- Qt 6.5+  
- CMake 3.19+  
- A C++17 compatible compiler  

### **Build & Run**  

```sh
git clone https://github.com/yourusername/QuickChat.git
cd QuickChat
mkdir build && cd build
cmake ..
make
./QuickChat
