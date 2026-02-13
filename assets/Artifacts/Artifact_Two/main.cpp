/*
Author: Misty Tutkavul
Date: 10/18/2025
Description:
This program provides an advising assistance tool for the Computer Science Department at ABCU.
It loads course records from a comma separated values file into a Binary Search Tree structure.
The user can perform the following actions through a simple menu:
1. Load course data from a file
2. Display all courses in alphanumeric order by course number
3. Look up a course and view its title and its list of prerequisites
Notes:
The expected file format is one course per line with the course number first, the course title second, followed by zero or more prerequisite course numbers.
Example line: CS200, Data Structures, CS100, CS105
Whitespace around commas is not removed by this loader. Keep the file clean to avoid unintended spaces in course numbers.
All lookups compare exact strings. The menu uppercases user input before search to reduce mismatches.
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

/*
Represents a single course.
courseNumber holds the unique identifier such as CS200.
courseTitle holds the course name such as Data Structures.
prerequisites holds zero or more course numbers that must be completed earlier.
*/
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

/*
Represents a node in the Binary Search Tree.
Each node stores one Course and pointers to left and right children.
The tree is ordered by courseNumber using string comparison.
*/
struct Node {
    Course course;
    Node* left;
    Node* right;

    // Creates a node that stores the provided course and has no children yet
    Node(Course newCourse) {
        course = newCourse;
        left = nullptr;
        right = nullptr;
    }
};

/*
Manages the Binary Search Tree that indexes courses by course number.
Supported operations include insertion, in order traversal, and search.
*/
class CourseBST {
private:
    Node* root;

    /*
    Inserts a course into the tree at or below the provided node pointer.
    Uses standard Binary Search Tree insertion on courseNumber.
    When the pointer is null a new node is allocated.
    */
    void addNode(Node*& node, Course course) {
        if (node == nullptr) {
            node = new Node(course);
        }
        else if (course.courseNumber < node->course.courseNumber) {
            addNode(node->left, course);
        }
        else {
            addNode(node->right, course);
        }
    }

    /*
    Performs an in order traversal to print the list of courses in sorted order.
    This visits left child then current node then right child.
    */
    void inOrder(Node* node) {
        if (node != nullptr) {
            inOrder(node->left);
            cout << node->course.courseNumber << ", " << node->course.courseTitle << endl;
            inOrder(node->right);
        }
    }

    /*
    Searches for a course by courseNumber starting at the provided node.
    Returns a pointer to the matching node or null when not found.
    */
    Node* search(Node* node, string courseNumber) {
        if (node == nullptr || node->course.courseNumber == courseNumber) {
            return node;
        }
        else if (courseNumber < node->course.courseNumber) {
            return search(node->left, courseNumber);
        }
        else {
            return search(node->right, courseNumber);
        }
    }

public:
    // Creates an empty tree with a null root pointer
    CourseBST() { root = nullptr; }

    // Inserts a course record into the tree
    void Insert(Course course) {
        addNode(root, course);
    }

    // Prints all courses in sorted order by course number
    void PrintCourseList() {
        inOrder(root);
    }

    /*
    Prints details for a single course including its list of prerequisites.
    If the course is not present the function reports that the course is not found.
    */
    void PrintCourse(string courseNumber) {
        Node* node = search(root, courseNumber);
        if (node == nullptr) {
            cout << "Course not found." << endl;
        }
        else {
            cout << node->course.courseNumber << ", " << node->course.courseTitle << endl;
            if (node->course.prerequisites.empty()) {
                cout << "Prerequisites: None" << endl;
            }
            else {
                cout << "Prerequisites: ";
                for (size_t i = 0; i < node->course.prerequisites.size(); ++i) {
                    cout << node->course.prerequisites[i];
                    if (i < node->course.prerequisites.size() - 1) cout << ", ";
                }
                cout << endl;
            }
        }
    }
};

/*
Reads course data from a comma separated values file and loads it into the tree.
Each line must contain at least two fields which are course number and course title.
Any remaining fields on the same line are treated as prerequisite course numbers.
If the file cannot be opened the function prints an error and returns without changes.
*/
void LoadCourses(string filename, CourseBST& bst) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Unable to open file: " << filename << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) {
            // Skip empty lines for convenience
            continue;
        }

        stringstream ss(line);
        string token;
        Course course;

        // Read the course number then the course title
        getline(ss, course.courseNumber, ',');
        getline(ss, course.courseTitle, ',');

        // Read zero or more prerequisites that follow on the same line
        while (getline(ss, token, ',')) {
            // Remove any leading or trailing spaces
            if (!token.empty() && token.front() == ' ') {
                token.erase(token.begin());
            }
            if (!token.empty() && token.back() == ' ') {
                token.pop_back();
            }
            if (!token.empty()) {
                course.prerequisites.push_back(token);
            }
        }

        // Insert the parsed course into the tree
        bst.Insert(course);
    }

    file.close();
}

/*
Main program loop that presents a simple text menu.
Option one loads course data from a file provided by the user or uses a default file name when left blank.
Option two prints the full course list in sorted order.
Option three prints details for a requested course.
Option nine exits the program.
*/
int main() {
    CourseBST bst;
    int choice;
    string filename;
    string courseInput;
    const string defaultFile = "CS 300 ABCU_Advising_Program_Input.csv";

    cout << "Welcome to the course planner." << endl;

    while (true) {
        // Show the menu
        cout << "\n1. Load Data Structure." << endl;
        cout << "2. Print Course List." << endl;
        cout << "3. Print Course." << endl;
        cout << "9. Exit" << endl;
        cout << "\nWhat would you like to do? ";
        if (!(cin >> choice)) {
            // Handle non numeric input and clear the stream
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\nPlease enter a number from the menu.\n" << endl;
            continue;
        }
        cin.ignore();

        // Process the selected option
        switch (choice) {
        case 1:
            cout << "Enter the file name (press Enter to use default file - CS 300 ABCU_Advising_Program_Input.csv): ";
            getline(cin, filename);

            if (filename.empty()) {
                filename = defaultFile;
                cout << "Using default file: " << filename << endl;
            }

            LoadCourses(filename, bst);
            break;
        case 2:
            cout << "\nHere is a sample schedule:\n" << endl;
            bst.PrintCourseList();
            break;
        case 3:
            cout << "What course do you want to know about? ";
            getline(cin, courseInput);
            transform(courseInput.begin(), courseInput.end(), courseInput.begin(), ::toupper);
            bst.PrintCourse(courseInput);
            break;
        case 9:
            cout << "Thank you for using the course planner!" << endl;
            return 0;
        default:
            cout << "\n" << choice << " is not a valid option.\n" << endl;
            break;
        }
    }
}
