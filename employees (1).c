#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include "employees.h"

static Employee employees[MAX_EMPLOYEES];
static int employeeCount = 0;

static void clearInputBuffer(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {}
}

static void readLine(char *text, int size)
{
    if (fgets(text, size, stdin) == NULL) {
        printf("\nInput closed. Exiting program.\n");
        exit(EXIT_SUCCESS);
    }

    if (strchr(text, '\n') == NULL) {
        clearInputBuffer();
    } else {
        text[strcspn(text, "\n")] = '\0';
    }
}

static int hasNonWhitespace(const char *text)
{
    while (*text != '\0') {
        if (!isspace((unsigned char)*text))
            return 1;
        text++;
    }
    return 0;
}

static int readPositiveInt(const char *prompt)
{
    char input[100];
    char *end;
    long value;

    while (1) {
        printf("%s", prompt);
        readLine(input, sizeof(input));

        errno = 0;
        value = strtol(input, &end, 10);

        while (isspace((unsigned char)*end))
            end++;

        if (input[0] != '\0' && end != input && *end == '\0' &&
            errno == 0 && value > 0 && value <= 2147483647L)
            return (int)value;

        printf("Invalid input. Please enter a positive whole number.\n");
    }
}

static int readMenuChoice(const char *prompt)
{
    char input[100];
    char *end;
    long value;

    printf("%s", prompt);
    readLine(input, sizeof(input));

    errno = 0;
    value = strtol(input, &end, 10);

    while (isspace((unsigned char)*end))
        end++;

    if (input[0] == '\0' || end == input || *end != '\0' ||
        errno != 0 || value < -2147483647L || value > 2147483647L)
        return -1;      /* shown as "Invalid menu choice" by the switch */

    return (int)value;
}

static double readNonNegativeDouble(const char *prompt)
{
    char input[100];
    char *end;
    double value;

    while (1) {
        printf("%s", prompt);
        readLine(input, sizeof(input));

        errno = 0;
        value = strtod(input, &end);

        while (isspace((unsigned char)*end))
            end++;

        if (input[0] != '\0' && end != input && *end == '\0' &&
            errno == 0 && isfinite(value) && value >= 0.0)
            return value;

        printf("Invalid amount. Please enter a finite value of 0 or more.\n");
    }
}

static int findEmployeeById(int id)
{
    int i;
    for (i = 0; i < employeeCount; i++) {
        if (employees[i].id == id)
            return i;
    }
    return -1;
}

double calculateSalary(const Employee *employee)
{
    return employee->basicSalary
         + employee->housingAllowance
         + employee->transportAllowance
         + employee->otherAllowance;
}

void addEmployee(void)
{
    Employee *employee;

    if (employeeCount >= MAX_EMPLOYEES) {
        printf("\nEmployee storage is full.\n");
        return;
    }

    employee = &employees[employeeCount];

    printf("\n========== ADD EMPLOYEE ==========\n");

    while (1) {
        employee->id = readPositiveInt("Enter employee ID: ");

        if (findEmployeeById(employee->id) == -1)
            break;

        printf("That employee ID already exists. Please use another ID.\n");
    }

    do {
        printf("Enter employee name: ");
        readLine(employee->name, NAME_LEN);

        if (!hasNonWhitespace(employee->name))
            printf("Name cannot be empty or contain only spaces.\n");
    } while (!hasNonWhitespace(employee->name));

    do {
        printf("Enter department: ");
        readLine(employee->department, DEPT_LEN);

        if (!hasNonWhitespace(employee->department))
            printf("Department cannot be empty or contain only spaces.\n");
    } while (!hasNonWhitespace(employee->department));

    employee->basicSalary = readNonNegativeDouble("Enter basic salary (N$): ");
    employee->housingAllowance = readNonNegativeDouble("Enter housing allowance (N$): ");
    employee->transportAllowance = readNonNegativeDouble("Enter transport allowance (N$): ");
    employee->otherAllowance = readNonNegativeDouble("Enter other allowance (N$): ");

    employee->totalSalary = calculateSalary(employee);
    employeeCount++;

    printf("\nEmployee added successfully.\n");
    printf("Total salary: N$%.2f\n", employee->totalSalary);
}

void displayEmployees(void)
{
    int i;

    printf("\n========== EMPLOYEE LIST ==========\n");

    if (employeeCount == 0) {
        printf("No employees have been registered yet.\n");
        return;
    }

    for (i = 0; i < employeeCount; i++) {
        employees[i].totalSalary = calculateSalary(&employees[i]);

        printf("\nEmployee %d\n", i + 1);
        printf("ID:                 %d\n", employees[i].id);
        printf("Name:               %s\n", employees[i].name);
        printf("Department:         %s\n", employees[i].department);
        printf("Basic Salary:       N$%.2f\n", employees[i].basicSalary);
        printf("Housing Allowance:  N$%.2f\n", employees[i].housingAllowance);
        printf("Transport Allow.:   N$%.2f\n", employees[i].transportAllowance);
        printf("Other Allowance:    N$%.2f\n", employees[i].otherAllowance);
        printf("Total Salary:       N$%.2f\n", employees[i].totalSalary);
    }
}

void searchEmployee(void)
{
    int id;
    int index;

    printf("\n========== SEARCH EMPLOYEE ==========\n");

    id = readPositiveInt("Enter employee ID to search: ");
    index = findEmployeeById(id);

    if (index == -1) {
        printf("Employee with ID %d was not found.\n", id);
        return;
    }

    employees[index].totalSalary = calculateSalary(&employees[index]);

    printf("\nEmployee found!\n");
    printf("ID:                 %d\n", employees[index].id);
    printf("Name:               %s\n", employees[index].name);
    printf("Department:         %s\n", employees[index].department);
    printf("Basic Salary:       N$%.2f\n", employees[index].basicSalary);
    printf("Housing Allowance:  N$%.2f\n", employees[index].housingAllowance);
    printf("Transport Allow.:   N$%.2f\n", employees[index].transportAllowance);
    printf("Other Allowance:    N$%.2f\n", employees[index].otherAllowance);
    printf("Total Salary:       N$%.2f\n", employees[index].totalSalary);
}

void employeeMenu(void)
{
    int choice;

    do {
        printf("\n========================================\n");
        printf("         EMPLOYEE MANAGEMENT\n");
        printf("========================================\n");
        printf("1. Add Employee\n");
        printf("2. Display Employees\n");
        printf("3. Search for Employee\n");
        printf("4. Return to Main Menu\n");
        printf("========================================\n");

        choice = readMenuChoice("Enter your choice: ");

        switch (choice) {
            case 1: addEmployee(); break;
            case 2: displayEmployees(); break;
            case 3: searchEmployee(); break;
            case 4: printf("Returning to main menu...\n"); break;
            default: printf("Invalid menu choice. Please choose 1-4.\n");
        }
    } while (choice != 4);
}

/* Read-only access for other modules (e.g. reports.c) */
int getEmployeeCount(void)
{
    return employeeCount;
}

int getEmployeeId(int index)
{
    if (index < 0 || index >= employeeCount)
        return -1;
    return employees[index].id;
}

const char *getEmployeeName(int index)
{
    if (index < 0 || index >= employeeCount)
        return "";
    return employees[index].name;
}

const char *getEmployeeDepartment(int index)
{
    if (index < 0 || index >= employeeCount)
        return "";
    return employees[index].department;
}

double getEmployeeTotalSalary(int index)
{
    if (index < 0 || index >= employeeCount)
        return -1.0;
    return calculateSalary(&employees[index]);
}
