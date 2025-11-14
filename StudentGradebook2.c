//VERSION WITHOUT SQL database, storing data with txt file
#include<stdio.h>
#include<stdbool.h>

int addStudent(void)
{
    FILE *fp = fopen("students.txt", "a"); //this file is used to store the data of students
    if(!fp)
    {
        perror("File error");
        return 1;
    }

    int id, copy, digit;
    char name[30], last[30];
    printf("Enter 9-digit student ID: ");
    scanf("%d", &id);
    while (1) //while true
    {
        // Count digits
        digit = 0;
        copy = id;

        if (copy == 0)
        digit = 1;

        while (copy != 0) //if ID isn't equal to 0
        {
            copy /= 10; //divides the ID number until it reaches 0 and the # of divisions gets stored on digit
            digit++;
        }
        if (digit == 9) //will check digit count after dividing
        {
            break;   // if it is = 9, it will exit
        } 
        else 
        {
            printf("Please Enter a valid 9-digit ID: "); //Error message will loop until valid ID entered
            scanf("%d", &id);
        }
    }
    
    printf("Enter first name: ");
    scanf("%29s", name);

    printf("Enter last name: ");
    scanf("%29s", last);
    //fprintf(fp, "%-15s %-15s %-15s\n\n", "ID", "Name", "Last Name"); for other parts use
    fprintf(fp, "%-15d %-15s %-15s\n", id, name, last); //fprintf to write out on file, file stream
    fclose(fp);

    return 0;
}

int deleteStudent(void)
{
    int targetID;
    printf("Enter the ID of the student you wish to remove: ");
    scanf("%d", & targetID);

    FILE *fp = fopen("students.txt", "r");
    FILE *copy = fopen("copy.txt", "w");
    if(!fp || !copy)
    {
        perror("File error");
        return 1;
    }
    int id;
    char name[30], last[30];
    while(fscanf(fp, "%d %29s %29s", &id, name, last) == 3) //checks all three fields are there
    {
        if(id != targetID) //if any of the ID's in the file don't match the inputted id then the file is left the same making a copy
        {
            fprintf(copy, "%-15d %-15s %-15s\n", id, name, last);
        }
    }
    fclose(fp);
    fclose(copy);
    remove("students.txt"); //data gets removed if ID was found
    rename("copy.txt", "students.txt"); //The copy file gets renamed to its original
    printf("Record removed.\n"); //Needs to get fixed, will print regardless of removing or not. 
    return 0;
}

int main() //Just to test functions working
{
    printf("Testing AddStudent function...\n\n");

    addStudent();  
    deleteStudent();

    return 0;
}
