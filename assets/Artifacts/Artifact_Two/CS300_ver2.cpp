/*
Author: Misty Tutkavul
Date: 01/2026
Course: CS-499 Computer Science Capstone
Milestone Three – Enhancement Two: Algorithms and Data Structures

Description:
Enhanced Course Advising System originally developed in CS-300.
This version demonstrates best coding practices and algorithmic design by:
- Using a Binary Search Tree (BST) for ordered traversal
- Using a hash map for fast course lookup
- Validating logical program flow and input data
- Improving readability, maintainability, and correctness
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <limits>

using namespace std;

/*
Represents a single course record.
This structure is intentionally simple and focused only on data storage.
*/
struct Course {
    string courseNumber;                 // Unique course identifier (e.g., CS300)
    string courseTitle;                  // Human-readable course title
    vector<string> prerequisites;        // List of prerequisite course numbers
};

/*
Node structure used by the Binary Search Tree.
Each node stores a Course and pointers to left/right children.
*/
struct Node {
    Course course;
    Node* left;
    Node* right;

    // Constructor initializes node with no children
    Node(const Course& newCourse)
        : course(newCourse), left(nullptr), right(nullptr) {
    }
};

/*
Binary Search Tree class.
Purpose:
- Maintain courses in sorted order by course number
- Support in-order traversal for displaying a structured course list

Note:
- BST is NOT used for searching in this enhanced version
- Lookup responsibility is intentionally delegated to a hash map
*/
class CourseBST {
private:
    Node* root;

    /*
    Recursive insertion function.
    Courses are ordered lexicographically by course number.
    Average complexity: O(log n)
    Worst case: O(n)
    */
    void insertNode(Node*& node, const Course& course) {
        if (node == nullptr) {
            node = new Node(course);
        }
        else if (course.courseNumber < node->course.courseNumber) {
            insertNode(node->left, course);
        }
        else {
            insertNode(node->right, course);
        }
    }

    /*
    In-order traversal prints courses in sorted order.
    This is the primary reason the BST exists in the enhanced design.
    */
    void inOrderTraversal(Node* node) const {
        if (node != nullptr) {
            inOrderTraversal(node->left);
            cout << node->course.courseNumber << ", "
                << node->course.courseTitle << endl;
            inOrderTraversal(node->right);
        }
    }

public:
    CourseBST() : root(nullptr) {}

    // Public insert method hides recursive implementation details
    void Insert(const Course& course) {
        insertNode(root, course);
    }

    // Prints all courses in sorted order
    void PrintSortedCourses() const {
        inOrderTraversal(root);
    }
};

/*
Loads course data from a CSV file.
Courses are stored in:
- BST for sorted traversal
- Hash map for fast lookup

This hybrid approach demonstrates algorithmic trade-offs.
*/
void LoadCourses(
    const string& filename,
    CourseBST& bst,
    unordered_map<string, Course>& courseMap
) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Unable to open file " << filename << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue; // Skip empty lines

        stringstream ss(line);
        Course course;
        string token;

        // Parse course number and title
        getline(ss, course.courseNumber, ',');
        getline(ss, course.courseTitle, ',');

        // Parse prerequisite list
        while (getline(ss, token, ',')) {
            token.erase(0, token.find_first_not_of(" "));
            token.erase(token.find_last_not_of(" ") + 1);
            if (!token.empty()) {
                course.prerequisites.push_back(token);
            }
        }

        // Insert into both data structures
        bst.Insert(course);
        courseMap[course.courseNumber] = course;
    }

    file.close();

    /*
    Validate prerequisite references.
    This defensive check prevents silent logical flaws
    caused by missing or incorrect prerequisite data.
    */
    for (const auto& pair : courseMap) {
        for (const auto& prereq : pair.second.prerequisites) {
            if (courseMap.find(prereq) == courseMap.end()) {
                cout << "Warning: Course " << pair.first
                    << " references missing prerequisite "
                    << prereq << endl;
            }
        }
    }
}

/*
Prints detailed information for a single course.
Uses hash map lookup for O(1) average-time access.
*/
void PrintCourseDetails(
    const string& courseNumber,
    const unordered_map<string, Course>& courseMap
) {
    auto it = courseMap.find(courseNumber);
    if (it == courseMap.end()) {
        cout << "Course not found." << endl;
        return;
    }

    const Course& course = it->second;
    cout << course.courseNumber << ", " << course.courseTitle << endl;

    if (course.prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
    }
    else {
        cout << "Prerequisites: ";
        for (size_t i = 0; i < course.prerequisites.size(); ++i) {
            cout << course.prerequisites[i];
            if (i < course.prerequisites.size() - 1) cout << ", ";
        }
        cout << endl;
    }
}

/*
Main program loop.
Includes input validation and logical flow checks
to prevent user actions before data is loaded.
*/
int main() {
    CourseBST bst;
    unordered_map<string, Course> courseMap;
    bool dataLoaded = false;   // Prevents invalid operations

    int choice;
    string filename;
    string courseInput;
    const string defaultFile = "CS 300 ABCU_Advising_Program_Input.csv";

    cout << "Welcome to the course planner." << endl;

    while (true) {
        cout << "\n1. Load Data Structure" << endl;
        cout << "2. Print Course List" << endl;
        cout << "3. Print Course" << endl;
        cout << "9. Exit" << endl;
        cout << "\nWhat would you like to do? ";

        // Validate numeric input
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number." << endl;
            continue;
        }

        cin.ignore();

        switch (choice) {
        case 1:
            cout << "Enter file name (press Enter for default): ";
            getline(cin, filename);
            if (filename.empty()) filename = defaultFile;
            LoadCourses(filename, bst, courseMap);
            dataLoaded = true;
            cout << "Course data loaded successfully." << endl;
            break;

        case 2:
            if (!dataLoaded) {
                cout << "\nError: No course data loaded. Please load data first.\n";
                break;
            }
            cout << "\nHere is a sample schedule:\n" << endl;
            bst.PrintSortedCourses();
            break;

        case 3:
            if (!dataLoaded) {
                cout << "\nError: No course data loaded. Please load data first.\n";
                break;
            }
            cout << "What course do you want to know about? ";
            getline(cin, courseInput);
            transform(courseInput.begin(), courseInput.end(),
                courseInput.begin(), ::toupper);
            PrintCourseDetails(courseInput, courseMap);
            break;

        case 9:
            cout << "Thank you for using the course planner!" << endl;
            return 0;

        default:
            cout << "Invalid option." << endl;
        }
    }
}
