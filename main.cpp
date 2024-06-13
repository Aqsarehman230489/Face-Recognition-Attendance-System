#include <iostream>
#include <fstream>
#include <ctime>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iomanip>
#include <string>
#include <conio.h>
#include <limits>
#include <cctype>

using namespace std;
using namespace cv;

void changeConsoleColor() {
    system("color 5F");
}

void clearScreen() {
    system("cls");
}

void returnToMenu()
{
    changeConsoleColor();
    cout << "Press any key to return to the menu...";
    _getch();  // Wait for a key press
}

const string RESET = "\033[0m";
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string BLUE = "\033[34m";
const string MAGENTA = "\033[35m";
const string CYAN = "\033[36m";
const string WHITE = "\033[37m";
const string BOLD = "\033[1m";
const string UNDERLINE = "\033[4m";


bool isValidName(const string& name)
{
    if (name.empty()) {
        return false;
    }
    if (name.back()==' ') {
        return false;
    }
    for (char ch : name)
    {
       if (!isalpha(ch) && ch != ' ') {
                cout << "Name can only contain alphabetic characters and spaces." << endl;
                return false;
       }
    }
    return true;
}


class Person
{
public:
    Person(const string& name, int id)
        : name(name), id(id) {}

    virtual void markAttendance() const = 0;

protected:
    string name;
    int id;

    void markAttendance(const string& logFileName) const;
    void captureAndSavePhoto() const;
};

class Student : public Person
{
public:
    Student(const string& name, int rollNumber)
        : Person(name, rollNumber) {}

    void markAttendance() const override;
};

class Teacher : public Person
{
public:
    Teacher(const string& name, int regNumber, int shiftNumber)
        : Person(name, regNumber), shiftNumber(shiftNumber) {}

    void markAttendance() const override;

private:
    int shiftNumber;
};

class AttendanceSystem
{
public:
    void markStudentAttendance();
    void markTeacherAttendance();
    void viewStudentAttendance() const;
    void viewTeacherAttendance() const;

private:
    void displayAttendance(const string& logFileName) const;
};

void Person::markAttendance(const string& logFileName) const
{
    captureAndSavePhoto();

    time_t now = time(0);
    tm currentDate;
    localtime_s(&currentDate, &now);

    ofstream log(logFileName, ios::app);
    log << currentDate.tm_mday << "-" << currentDate.tm_mon + 1 << "-"
        << currentDate.tm_year + 1900 << " " << name << " " << id << " Photo: " << name + "_" + to_string(id) + ".jpg" << endl;
    log.close();

    cout << "Attendance marked for today: " << name << " " << id << endl;
}

void Person::captureAndSavePhoto() const
{
    VideoCapture cap(0);

    if (!cap.isOpened()) {
        cerr << "Error opening video capture." << endl;
        return;
    }

    Mat frame;
    namedWindow("Face Detection", WINDOW_NORMAL);
    resizeWindow("Face Detection", 1280, 720);

    String face_cascade_path = "haarcascade_frontalface_default.xml";
    CascadeClassifier face_cascade;

    if (!face_cascade.load(face_cascade_path)) {
        cerr << "Error loading face cascade." << endl;
        return;
    }

    time_t start_time = time(0);
    while (time(0) - start_time < 3) 
    {
        cap >> frame;

        if (frame.empty()) {
            cerr << "Error capturing frame." << endl;
            break;
        }

        Mat gray;
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        equalizeHist(gray, gray);

        vector<Rect> faces;
        face_cascade.detectMultiScale(gray, faces, 1.1, 3, 0, Size(30, 30));

        for (const Rect& face : faces) {
            rectangle(frame, face, Scalar(0, 255, 0), 2);

            
            stringstream ss;
            ss << name << " " << id;
            putText(frame, ss.str(), Point(face.x, face.y - 10), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 255, 0), 2);

            Mat detectedFace = frame(face);
            string photoPath = "attendance_photos/" + name + "_" + to_string(id) + ".jpg";
            imwrite(photoPath, detectedFace);
            cout << "Face image saved: " << photoPath << endl;
        }

        imshow("Face Detection", frame);

        if (waitKey(1) == 27) {
            destroyAllWindows();
            return;
        }
    }

    destroyAllWindows();
}

void Student::markAttendance() const
{
    Person::markAttendance("studentattendance_log.txt");
    clearScreen();
    cout << GREEN << "Attendance marked for today: " << name << " " << id << RESET << endl;
    returnToMenu();
}

void Teacher::markAttendance() const
{
    Person::markAttendance("teacherattendance_log.txt");
    clearScreen();
    cout << GREEN << "Attendance marked for today: " << name << " " << id << " Shift: " << shiftNumber << RESET << endl;
    returnToMenu();
}


void AttendanceSystem::markStudentAttendance()
{
    string name;
    int rollNumber;
    cout << endl;
    cout << CYAN << "\tEnter Name: " << RESET;
    getline(cin, name);
    while (!isValidName(name)) {
        cout << RED << "\tInvalid name! Name should only contain alphabets. Enter Name: " << RESET;
        getline(cin, name);
    }

    cout << CYAN << "\tEnter Roll Number: " << RESET;
    while (!(cin >> rollNumber)) {
        cout << RED << "\tInvalid roll number. Enter a valid integer: " << RESET;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    // Clear the input buffer
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    Student student(name, rollNumber);
    student.markAttendance();
}

void AttendanceSystem::markTeacherAttendance() 
{
    string name;
    int regNumber, shiftNumber;
    cout << endl;
    cout << CYAN << "\tEnter Name: " << RESET;
    getline(cin, name);
    while (!isValidName(name)) {
        cout << RED << "\tInvalid name. Name should only contain alphabets. Enter Name: " << RESET;
        getline(cin, name);
    }

    cout << CYAN << "\tEnter Registration Number: " << RESET;
    while (!(cin >> regNumber)) {
        cout << RED << "\tInvalid registration number. Enter a valid integer: " << RESET;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    cout << CYAN << "\tEnter Shift Number (0 or 1): " << RESET;
    while (!(cin >> shiftNumber) || (shiftNumber != 0 && shiftNumber != 1)) {
        cout << RED << "\tInvalid shift number. Enter 0 or 1: " << RESET;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    Teacher teacher(name, regNumber, shiftNumber);
    teacher.markAttendance();
}

void AttendanceSystem::viewStudentAttendance() const
{
    clearScreen();
    displayAttendance("studentattendance_log.txt");
    returnToMenu();
}

void AttendanceSystem::viewTeacherAttendance() const 
{
    clearScreen();
    displayAttendance("teacherattendance_log.txt");
    returnToMenu();
}

void AttendanceSystem::displayAttendance(const string& logFileName) const 
{
    ifstream log(logFileName);

    if (!log.is_open()) {
        cerr << "Error opening attendance log file." << endl;
        return;
    }

    cout << BOLD << "Attendance Log:\n" << RESET;
    string line;

    while (getline(log, line)) {
        cout << "\t" << line << endl;
    }

    log.close();
}

void displayMenu()
{
    clearScreen();
    cout << endl;
    cout << BOLD << YELLOW << "\t                                      " << RESET << endl;
    cout << BOLD << YELLOW << "\t======================================" << RESET << endl;
    cout << BOLD << YELLOW << "\t           Attendance System          " << RESET << endl;
    cout << BOLD << YELLOW << "\t======================================" << RESET << endl;
    cout << endl;
    cout << YELLOW << "\t1) Mark Student Attendance" << RESET << endl;
    cout << YELLOW << "\t2) Mark Teacher Attendance" << RESET << endl;
    cout << YELLOW << "\t3) View Student Attendance" << RESET << endl;
    cout << YELLOW << "\t4) View Teacher Attendance" << RESET << endl;
    cout << YELLOW << "\t5) Exit" << RESET << endl;
    cout << endl;
    cout << CYAN << "\tEnter your choice: " << RESET;
}


int main() {
    changeConsoleColor();
    AttendanceSystem attendanceSystem;
    int choice;

    do {
        displayMenu();
        while (!(cin >> choice) || choice < 1 || choice > 5) {
            cout << RED << "Invalid choice. Please enter a number between 1 and 5: " << RESET;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
        case 1:
            attendanceSystem.markStudentAttendance();
            break;
        case 2:
            attendanceSystem.markTeacherAttendance();
            break;
        case 3:
            attendanceSystem.viewStudentAttendance();
            break;
        case 4:
            attendanceSystem.viewTeacherAttendance();
            break;
        case 5:
            cout << GREEN << "Exiting program.\n" << RESET;
            break;
        default:
            cout << RED << "Invalid choice. Please try again.\n" << RESET;
        }

    }
    while (choice != 5);

    return 0;
}
