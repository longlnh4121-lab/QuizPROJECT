C++ Quiz System Project
1. Introduction

The Quiz System is built with C++, supporting Admin/Student roles and automatic grading.

2. Technologies Used

Language: C++

Version Control: Git & GitHub

Packaging & Deployment: Docker

3. Installation & Deployment Guide

The project has been Dockerized to run on any environment.

Step 1: Clone the project
git clone https://github.com/longlnh4121-lab/QuizPROJECT.git
cd QuizPROJECT

Step 2: Build Docker Image
docker build -t quiz-app .

Step 3: Run the application
docker run -it --rm quiz-app

4. Demo Accounts (in users.txt)

Student: long / 1234

Admin: My / 1234
