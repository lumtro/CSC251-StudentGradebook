#include<stdio.h>
#include <string.h>
#include "sqlite3.h" //Corrected, no need for sqlite/sqlite3 only sqlite 3
#include <stdbool.h>
#include <stdlib.h>
//int displayRecords(sqlite3 * db); should display all relevent data on based on ID
int addStudent(sqlite3 * db);       //functions take user input, problem when
int deleteStudent(sqlite3 * db);    //wrong data type is taken in, char into
                                    //studentID input
int listStudents(sqlite3 * db);     //display all students
int findStudent(sqlite3 * db);      //find student by ID
int updateStudent(sqlite3 * db);    //update student grade
int showStatistics(sqlite3 * db);   //show class statistics
int printRow(void *data, int argc, char **argv, char **colNames);  //helper for printing

int main(int argc, char const *argv[]){

    sqlite3 *db;
    char *err_msg = 0;
    int conn = sqlite3_open("students.db", &db);

    //Check #1
    if(conn != SQLITE_OK){
        fprintf(stderr, "Connection Failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        sqlite3_free(err_msg);
        return 1;
    }

    //Can add more columns for data
    char *sql_qry = "CREATE TABLE IF NOT EXISTS students"
    "(studentID INTEGER PRIMARY KEY, firstName TEXT, lastName TEXT, "
    "grade1 REAL, grade2 REAL, grade3 REAL, grade4 REAL, grade5 REAL);";
    //"INSERT INTO students VALUES (222333444, 'See', 'Sim');";

    conn = sqlite3_exec(db, sql_qry, 0, 0, &err_msg);

    //Check #2
    if(conn != SQLITE_OK)
    {
        fprintf(stderr, "Table Creation Failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        sqlite3_free(err_msg);
        return 1;
    }   else
    {
        fprintf(stderr, "Table Created Successfully\n");
    }
    //FUNCTIONS START HERE
    //probably have interactive switch here to select functions
    //Modifications made by Moises
    int option; //For switch
    do
    {
        printf("\n========== STUDENT GRADEBOOK ==========\n");
        printf("1) Add a student\n");
        printf("2) Remove a student\n");
        printf("3) Find a student\n");
        printf("4) Update a grade\n");
        printf("5) List all students\n");
        printf("6) Show statistics\n");
        printf("7) Exit the program\n");
        printf("Enter your option: ");
        scanf("%d", &option);
        switch(option)
        {
            case 1:
                addStudent(db);
                break;
            case 2:
                deleteStudent(db);
                break;
            case 3:
                findStudent(db);
                break;
            case 4:
                updateStudent(db);
                break;
            case 5:
                listStudents(db);
                break;
            case 6:
                showStatistics(db);
                break;
            case 7:
                printf("Thanks for using the database, Goodbye.\n");
                break;
            default:
                printf("Please enter a valid option.\n");
                break;
        }
    }while(option != 7);

    //FUNCTIONS END HERE

    sqlite3_close(db);

    return 0;
}
int callBack(void *data, int argc, char **argv, char **colNames) //this will be used to retieve any existing information, will be used to delete/update
{
    int *existsPtr = (int *) data;
    if(argv[0])
    {
        *existsPtr = atoi(argv[0]);
    }else
    {
        *existsPtr = 0;
    }
    return 0;
}

int addStudent(sqlite3 * db){
    char *err_msg = NULL;
    char firstName[30], lastName[30], sql_qry[512];
    char inputID[20];
    int digit;
    float g1, g2, g3, g4, g5;  //grade variables

    //Should take max of 30 characters
    fprintf(stderr, "Enter student first name (Max 30 Char): ");
    scanf("%29s", firstName); //Fixed, can't use & with char
    fprintf(stderr, "Enter student last name (Max 30 Char): ");
    scanf("%29s", lastName); //Fixed, can't use & with char

    //checks id is 9 digits, problem when ID starts with 0 (000001234)
    printf("Enter 9-digit student ID: ");
    scanf("%19s", inputID);
    //Optimized for rejecting numbers less than 9 digits and allows leading 0's (Moises)
    while (1) //while true
    {
        digit = 0; //Digit count
        while(inputID[digit] != '\0')
        {
            digit++;
        }
        if(digit == 9 && strspn(inputID, "0123456789") == 9) //checking the input is 9 digits and that it consists of number 0-9 and nothing else
        {
            break; //stops here after seeing ID is valid and moves on to the next thing
        }
        else
        {
            printf("Please enter a valid 9-digit ID: "); //error message if invalid
            scanf("%19s", inputID); //asks for input again until is right
        }
    }

    //Get 5 grades from user
    printf("Enter 5 grades (0-100, space-separated): ");
    scanf("%f %f %f %f %f", &g1, &g2, &g3, &g4, &g5);

    //snprintf for strings
    int numericID = atoi(inputID);
    snprintf(sql_qry, sizeof(sql_qry),
             "INSERT INTO students (studentID, firstName, lastName, grade1, grade2, grade3, grade4, grade5) VALUES"
             "(%d, '%s', '%s', %.2f, %.2f, %.2f, %.2f, %.2f);",
             numericID, firstName, lastName, g1, g2, g3, g4, g5);


    int conn = sqlite3_exec(db, sql_qry, 0, 0, &err_msg);
    if(conn != SQLITE_OK){
        fprintf(stderr, "Failed to Add Student: %s\n", sqlite3_errmsg(db));
        sqlite3_free(err_msg);
        return 1;
    }else{
        fprintf(stderr, "Student Added Successfully\n" );
    }

    return 0;
}

int deleteStudent(sqlite3 *db){
    char *err_msg = NULL;
    char sql_qry[256];
    int deleteID = 0, exists = 0;

    //checks if ID exists in database (Moises)
    do
    {
        fprintf(stderr, "Enter existing studentID to delete (9 digits): ");
        scanf("%d", &deleteID);

        snprintf(sql_qry, sizeof(sql_qry),
         "SELECT EXISTS(SELECT 1 FROM students WHERE studentID = %d) AS row_exists;",
         deleteID);
        
        exists = 0;
        int conn = sqlite3_exec(db, sql_qry, callBack, &exists, &err_msg); //we use callback to retrieve info and check that the ID exists from previous inputs
        if(conn != SQLITE_OK)
        {
            fprintf(stderr, "Failed to check student: %s\n", sqlite3_errmsg(db));
            sqlite3_free(err_msg);
            return 1;
        }
        if(!exists)
        {
            printf("Student ID does not exist. Try again.\n"); //If ID does not match then this error message will print
        }
    } while (!exists); //Loop gets repeated if the ID does not exists and it asks you to input a valid number

    //only integers so sprintf is fine
    sprintf(sql_qry, "DELETE FROM students WHERE studentID = %d;", deleteID);

    int conn = sqlite3_exec(db, sql_qry, 0, 0, &err_msg);
    if(conn != SQLITE_OK){
        fprintf(stderr, "Failed to Remove Student Record: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        sqlite3_free(err_msg);
        return 1;
    }
    
    fprintf(stderr, "Student Record Removed\n" );

    return 0;
}

//Helper function to print each row of student data
int printRow(void *data, int argc, char **argv, char **colNames) {
    printf("ID: %s | Name: %s %s", argv[0], argv[1], argv[2]);

    //Print grades if they exist
    if(argc > 3 && argv[3] != NULL) {
        float avg = (atof(argv[3]) + atof(argv[4]) + atof(argv[5]) + atof(argv[6]) + atof(argv[7])) / 5.0;
        printf(" | Grades: %s, %s, %s, %s, %s | Avg: %.2f",
               argv[3], argv[4], argv[5], argv[6], argv[7], avg);
    }
    printf("\n");
    return 0;
}

//List all students in the database
int listStudents(sqlite3 *db) {
    char *sql_qry = "SELECT * FROM students;";
    char *err_msg = NULL;

    printf("\n=== All Students ===\n");
    int conn = sqlite3_exec(db, sql_qry, printRow, NULL, &err_msg);

    if(conn != SQLITE_OK) {
        fprintf(stderr, "Failed to list students: %s\n", sqlite3_errmsg(db));
        sqlite3_free(err_msg);
        return 1;
    }
    printf("=== End of List ===\n");
    return 0;
}

//Find a student by their ID
int findStudent(sqlite3 *db) {
    int searchID;
    char sql_qry[256];
    char *err_msg = NULL;

    printf("Enter student ID to find: ");
    scanf("%d", &searchID);

    snprintf(sql_qry, sizeof(sql_qry),
             "SELECT * FROM students WHERE studentID = %d;", searchID);

    printf("\n=== Search Result ===\n");
    int conn = sqlite3_exec(db, sql_qry, printRow, NULL, &err_msg);

    if(conn != SQLITE_OK) {
        fprintf(stderr, "Search failed: %s\n", sqlite3_errmsg(db));
        sqlite3_free(err_msg);
        return 1;
    }
    printf("=== End of Search ===\n");
    return 0;
}

//Update a student's grade
int updateStudent(sqlite3 *db) {
    int updateID;
    float newGrade;
    int gradeNum;
    char sql_qry[256];
    char *err_msg = NULL;
    int exists = 0;

    printf("Enter student ID to update: ");
    scanf("%d", &updateID);

    //Check if student exists first
    snprintf(sql_qry, sizeof(sql_qry),
             "SELECT EXISTS(SELECT 1 FROM students WHERE studentID = %d);", updateID);

    sqlite3_exec(db, sql_qry, callBack, &exists, &err_msg);

    if(!exists) {
        printf("Student ID not found!\n");
        return 1;
    }

    printf("Which grade to update (1-5)? ");
    scanf("%d", &gradeNum);

    if(gradeNum < 1 || gradeNum > 5) {
        printf("Invalid grade number! Must be 1-5.\n");
        return 1;
    }

    printf("Enter new grade (0-100): ");
    scanf("%f", &newGrade);

    //Update the specific grade column
    snprintf(sql_qry, sizeof(sql_qry),
             "UPDATE students SET grade%d = %.2f WHERE studentID = %d;",
             gradeNum, newGrade, updateID);

    int conn = sqlite3_exec(db, sql_qry, NULL, NULL, &err_msg);
    if(conn != SQLITE_OK) {
        fprintf(stderr, "Update failed: %s\n", sqlite3_errmsg(db));
        sqlite3_free(err_msg);
        return 1;
    }
    printf("Grade updated successfully!\n");
    return 0;
}

//Helper function to print count
int printCount(void *data, int argc, char **argv, char **colNames) {
    printf("Total Students: %s\n", argv[0]);
    return 0;
}

//Helper function to print average
int printAverage(void *data, int argc, char **argv, char **colNames) {
    if(argv[0] != NULL) {
        printf("Class Average: %.2f\n", atof(argv[0]));
    } else {
        printf("Class Average: N/A (no students)\n");
    }
    return 0;
}

//Helper function to print highest grade
int printHighest(void *data, int argc, char **argv, char **colNames) {
    if(argv[0] != NULL) {
        printf("Highest Grade: %.2f\n", atof(argv[0]));
    } else {
        printf("Highest Grade: N/A\n");
    }
    return 0;
}

//Helper function to print lowest grade
int printLowest(void *data, int argc, char **argv, char **colNames) {
    if(argv[0] != NULL) {
        printf("Lowest Grade: %.2f\n", atof(argv[0]));
    } else {
        printf("Lowest Grade: N/A\n");
    }
    return 0;
}

//Show statistics for all students
int showStatistics(sqlite3 *db) {
    char *err_msg = NULL;

    printf("\n=== Class Statistics ===\n");

    //Count total students
    char *sql_count = "SELECT COUNT(*) FROM students;";
    sqlite3_exec(db, sql_count, printCount, NULL, &err_msg);

    //Calculate class average (average of all student averages)
    char *sql_avg = "SELECT AVG((grade1+grade2+grade3+grade4+grade5)/5.0) FROM students;";
    sqlite3_exec(db, sql_avg, printAverage, NULL, &err_msg);

    //Find highest individual grade
    char *sql_high = "SELECT MAX(max_grade) FROM "
                     "(SELECT MAX(grade1, grade2, grade3, grade4, grade5) as max_grade FROM students);";
    sqlite3_exec(db, sql_high, printHighest, NULL, &err_msg);

    //Find lowest individual grade
    char *sql_low = "SELECT MIN(min_grade) FROM "
                    "(SELECT MIN(grade1, grade2, grade3, grade4, grade5) as min_grade FROM students);";
    sqlite3_exec(db, sql_low, printLowest, NULL, &err_msg);

    printf("=== End of Statistics ===\n");
    return 0;
}
