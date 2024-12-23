# Simple Ransomware

## Disclaimer
This project is strictly for educational and research purposes only. It is intended to help individuals understand the functionality and risks associated with ransomware attacks, and to provide insights into securing systems against such threats. Any malicious use of this software is strictly prohibited and may lead to legal consequences.

The authors of this project do not condone or encourage any illegal activity, including the unauthorized use of this software.

## Overview
This repository contains a ransomware simulator implemented in C. The purpose of this project is to:

Educate developers, security researchers, and IT professionals about the inner workings of ransomware.
Demonstrate encryption mechanisms and file traversal techniques.
Provide insights into how to secure systems against similar attacks.

## Key Features
- File Encryption: Encrypts files within a target directory, appending a custom extension.
- Selective Targeting: Processes specific file types based on predefined extensions.
- Recursive Directory Traversal: Traverses folders and encrypts files at all levels.
- Stack-Based File Handling: Uses a custom stack implementation to manage file paths.

## How It Works
- The program begins by populating two stacks: one for folders and another for files.
- It traverses the file system, identifying valid files based on their extensions.
- Files are encrypted using a bitwise rotation algorithm and stored with a new file extension.
- Original files are deleted after encryption.

