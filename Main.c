#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// constant variables
char teamName[20] = "P10-08";
char fileName[20] = "P10-08-CMS.txt";

// defining constants
#define MAX_RECORDS 100
#define MAX_NAME_LEN 50
#define MAX_PROGRAM_LEN 50
#define MAX_CMD_LEN 128

// structure for a single record (in the text file)
typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    char programme[MAX_PROGRAM_LEN];
    float mark;
} Student;

// global array to store student data records and counter to count number of records is being stored
Student studentRecords[MAX_RECORDS];
int recordCount = 0;

// flag to check if database is opened
int DBopened = 0;

// operations Functions
void openDB();
void saveDB();
void showAll();
void showSummary();
void showAll_sort(char *command); 
void queryRecord(char *command);
void updateRecord(char *command);
void insertRecord();
void deleteRecord(char *command);

//Unique Features
void gradeAll();
void statsGPA();

// structure for Grading
typedef struct {
    int min_mark;         
    const char* letter;   
    float gpa;
    const char *remark;            
} GradeBand;

// GradeBand
static const GradeBand SIT_BANDS[] = {
    {85, "A",  5.0, "Excellent"},
    {80, "A-", 4.5, "Excellent"},
    {75, "B+", 4.0, "Good"},
    {70, "B",  3.5, "Good"},
    {65, "B-", 3.0, "Good"},
    {60, "C+", 2.5, "Average"},
    {55, "C",  2.0, "Average"},
    {50, "D+", 1.5, "Pass"},
    {45, "D",  1.0, "Pass"},
    {0,  "F",  0.0, "Fail"}
};

static const GradeBand* grade_of(float mark) {
    for (size_t i = 0; i < sizeof(SIT_BANDS) / sizeof(SIT_BANDS[0]); ++i) {
        if (mark >= SIT_BANDS[i].min_mark) return &SIT_BANDS[i];
    }
    return &SIT_BANDS[sizeof(SIT_BANDS) / sizeof(SIT_BANDS[0]) - 1];
}

void endStatement() {
    printf("====================================================================\n");
}

int main(){
    // DECLARATIONS
    printf("\"\n");
    printf("%*sDECLARATION%*s\n", (100 + 11) / 2, "", (100 - 11) / 2, "");
    printf("SIT's policy on copying does not allow the students to copy source code as well as assessment solutions from another person AI or other places. It is the students' responsibility to guarantee that their assessment solutions are their own work. Meanwhile, the students must also ensure that their work is not accessible by others. Where such plagiarism is detected, both of the assessments involved will receive ZERO mark.\n\n");
    printf("We hereby declare that:\n");
    printf("- We fully understand and agree to the abovementioned plagiarism policy.\n");
    printf("- We did not copy any code from others or from other places.\n");
    printf("- We did not share our codes with others or upload to any other places for public access and will not do that in the future.\n");
    printf("- We agree that our project will receive Zero mark if there is any plagiarism detected. \n");
    printf("- We agree that we will not disclose any information or material of the group project to others or upload to any other places for public access.\n");
    printf("- We agree that we did not copy any code directly from AI generated sources.\n\n");
    printf("Declared by: %s\n", teamName);
    printf("Team Members:\n");
    printf("1.Voo Geng Loong (2500633)\n");
    printf("2.Chang Wen Lin Sarah (2501932)\n");
    printf("3.Khor Jun Kit (2501028)\n");
    printf("4.Amos Tay Zhi Sheng (2500527)\n\n");
    printf("Date:(25 November 2025)\n\n");
    printf("\"\n");

    char command[MAX_CMD_LEN];
    while (1) {
        printf("%s (Enter \"HELP\" for assistance): ", teamName);
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }
        
        command[strcspn(command, "\n")] = 0;

        if (strcmp(command, "OPEN") == 0) {
            openDB();
        } 
        else if (strcmp(command, "SHOW ALL") == 0) {
            showAll();
        }
        else if (strncmp(command, "SHOW ALL SORT BY ", 17) == 0) { 
            showAll_sort(command);
        }
        else if (strcmp(command, "SHOW SUMMARY") == 0) {
            showSummary();
        }
        else if (strcmp(command, "INSERT") == 0) {
            insertRecord();
        }
        else if (strncmp(command, "QUERY", 5) == 0) {
            queryRecord(command);
        }    
        else if (strncmp(command, "UPDATE ID=", 10) == 0) {
            updateRecord(command);
        }
        else if (strncmp(command, "DELETE ID=", 10) == 0) {
            deleteRecord(command);
        }
        else if (strcmp(command, "EXIT") == 0) {
            printf("CMS: Exiting program.\n");
            break;
        }
        // unique features of grades and statistics added
        else if (strcmp(command, "GRADE ALL") == 0) {
            gradeAll();
        }
        else if (strcmp(command, "STATS GPA") == 0) {
            statsGPA();
        }
        else if (strcmp(command, "HELP") == 0) {
            printf("\n============================ HELP MENU =========================\n");
            printf("OPEN                - Open the database file and read in all the records\n");
            printf("SHOW ALL            - Display all current records in the data\n");
            printf("SHOW ALL SORT BY    - Display all current records sorted by ID or Mark (e.g., 'SHOW ALL BY SORT ID ASC', 'SHOW ALL SORT BY MARK DESC')\n");
            printf("SHOW SUMMARY        - Display the average, highest and lowest marks from the student records\n");
            printf("INSERT              - Add a new data record (e.g., 'INSERT ID=1234567', follow up by 'INSERT (Name Programme Marks) John Engineer 67.6'\n");
            printf("QUERY               - Search for record of student ID, or all records by Name or Programme. Example 'QUERY ID=2500111, QUERY Name=John'\n");
            printf("UPDATE ID=          - Update the data for a record with a given student ID. Example 'UPDATE ID=2500111 Mark=30'\n");
            printf("DELETE ID=          - Remove or delete a record of student ID\n");
            printf("GRADE ALL           - Show all records with grades and GPA\n");
            printf("STATS GPA           - Show average GPA and grades distribution\n");
            printf("EXIT                - Exit the program\n");
            printf("==================================================================\n\n");
        }
        else {
            printf("CMS: Error - Unknown command.\n");
            endStatement();
        }
    }
    return 0;
}

// [OPEN]: funtion to open the database txt file 
void openDB() {

    // check to see if the database file has already been opeend or not
    if (DBopened) {
        printf("CMS: P10-08 CMS file is already opened.\n");
        endStatement();
        return;
    }
    // attempt to open the file stored in "fileName" variable for reading
    FILE *file = fopen(fileName, "r");
    // if file does not exists, return error message
    if (file == NULL) {
        printf("CMS: Error - Could not open file '%s'.\n", fileName);
        endStatement();
        return;
    }

    // initialize buffer, line counter, and file password needed for password validation function
    char buffer[256]; 
    int lineCounter = 0;
    char filePassword[20];

    // loop through the database text file untill the line containing the password (line 3)
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        lineCounter++;
        if (lineCounter >= 3) {
            break; 
        }
    }

    // extract the password string from the line (in the db text file --> password: cassignment)
    char *passToken = strtok(buffer, ":");
    if (passToken != NULL) {
        passToken = strtok(NULL, "");
        if (passToken != NULL) {
            while (*passToken == ' ') {
                passToken++;
            }
            strncpy(filePassword, passToken, sizeof(filePassword) - 1);
            filePassword[sizeof(filePassword) - 1] = '\0';
        }
    }

    // initialize userInput, maxTries, authenticated needed for password authentication loop
    char userInput[50];
    int maxTries = 3;
    int authenticated = 0;

    // authentication loop to allow up to a maximum of 3 tries. after the third wrong passowrd, forcefully exit the program
    printf("CMS: Enter password to open the database: ");
    for (int tries = 0; tries < maxTries; tries++) {
        if (fgets(userInput, sizeof(userInput), stdin) != NULL) {
            if (strcmp(userInput, filePassword) == 0) {
                authenticated = 1;
                break; 
            } else {
                if (tries < maxTries - 1) {
                    printf("CMS: Incorrect password. You have %d tries left: ", maxTries - 1 - tries);
                }
            }
        } 
    }
    // authentication fails, exit the program
    if (authenticated == 0) {
        printf("\nCMS: Incorrect password entered 3 times. Program will now exit.\n");
        exit(1); 
    }
    
    // after authentication passes
    // reset file pointer and line counter
    rewind(file);
    lineCounter = 0;

    // indicate that database is opened
    DBopened = 1;
    // reset record count
    recordCount = 0;
    
    // skip first 6 lines to reach data records
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        lineCounter++;
        if (lineCounter >= 6) {
            break; 
        }
    }

    // read data records line by line
    while (recordCount < MAX_RECORDS && fgets(buffer, sizeof(buffer), file) != NULL) {
        // pointer for strtok
        char *fieldToken;
        char *recordData = buffer;

        // ID field
        fieldToken = strtok(recordData, "\t");
        // skip malformed lines
        if (fieldToken == NULL) continue; 
        studentRecords[recordCount].id = atoi(fieldToken);

        // Name field
        fieldToken = strtok(NULL, "\t");
        // skip malformed lines
        if (fieldToken == NULL) continue;
        strncpy(studentRecords[recordCount].name, fieldToken, sizeof(studentRecords[recordCount].name) - 1);
        studentRecords[recordCount].name[sizeof(studentRecords[recordCount].name) - 1] = '\0';

        // Programme field
        fieldToken = strtok(NULL, "\t");
        // skip malformed lines
        if (fieldToken == NULL) continue;
        strncpy(studentRecords[recordCount].programme, fieldToken, sizeof(studentRecords[recordCount].programme) - 1);
        studentRecords[recordCount].programme[sizeof(studentRecords[recordCount].programme) - 1] = '\0';

        // Mark field
        fieldToken = strtok(NULL, "\t");
        // skip malformed lines
        if (fieldToken == NULL) continue;
        studentRecords[recordCount].mark = atof(fieldToken);
        
        recordCount++;
    }
    fclose(file);
    printf("CMS: File '%s' is successfully opened. Loaded %d records.\n", fileName, recordCount); 
    endStatement();
}

// funtion to save the database txt file
void saveDB() {

    // attempt to open the file stored in "fileName" variable for overwriting
    FILE *file = fopen(fileName, "w");
    // if file does not exists, return error message
    if (file == NULL) {
        printf("CMS: Error - Could not open file '%s' for writing.\n", fileName);
        endStatement();
        return;
    }
    
    // write original metadata from the database txt file
    fprintf(file, "Database Name: P10-08 CMS\n");
    fprintf(file, "Authors: Amos, Jun Kit, Sarah, Geng Loong\n");
    fprintf(file, "Password: cassignment\n\n");
    fprintf(file, "Table Name: StudentRecords\n");
    
    // write header lines from the database txt file
    fprintf(file, "ID\tName\t\tProgramme\t\tMark\n");
    
    // write all records currently stored in the global memeory back to the file
    for (int i = 0; i < recordCount; i++) {
        // maintain the file's original structure
        fprintf(file, "%d\t%s\t\t%s\t\t%.1f\n", 
                studentRecords[i].id, 
                studentRecords[i].name, 
                studentRecords[i].programme, 
                studentRecords[i].mark);
    }
    
    fclose(file);
    printf("CMS: Changes saved to file '%s'.\n", fileName);
    endStatement();
}

// [SHOW ALL]: function to show all records in the database txt file
void showAll() {
    // ensure that the database file is opened, else prompt user to use the OPEN command first
    if (DBopened == 0) {
        printf("CMS: Error - No database file is currently open. Use the 'OPEN' command first.\n");
        endStatement();
        return;
    }

    // check if database is empty. if empty, prompt user to INSERT record first
    if (recordCount == 0) {
        printf("CMS: There are currently no records found in the database, insert some records first.\n");
        endStatement();
        return;
    } 

    // database is not empty, print all records
    printf("CMS: Here are all the records found in the table \"StudentRecords\".\n\n");

    // print header
    printf("%-10s %-20s %-30s %-1s\n", "ID", "Name", "Programme", "Mark");
    // print all records
    for (int i = 0; i < recordCount; i++) {
        printf("%-10d %-20s %-30s %.1f\n", 
            studentRecords[i].id, 
            studentRecords[i].name, 
            studentRecords[i].programme, 
            studentRecords[i].mark);
    }
    printf("\nCMS: Currently showing %d record(s).\n", recordCount);
    endStatement();
}

// [SHOW SUMMARY]: function to show summary statistics of student records
void showSummary() {
    // ensure that the database file is opened, else prompt user to use the OPEN command first
    if (DBopened == 0) {
        printf("CMS: Error - No database file is currently open. Use the 'OPEN' command first.\n");
        endStatement();
        return;
    }

    // check if database is empty. if empty, prompt user to INSERT record first
    if (recordCount == 0) {
        printf("CMS: There are currently no records found in the database, insert some records first.\n");
        endStatement();
        return;
    } 

    // initialize summary variables
    float totalMarks = 0.0;
    float highestMark = -1.0;
    float lowestMark = 101.0;
    char *highestName = NULL;
    char *lowestName = NULL;
    
    for (int i = 0; i < recordCount; i++) {

        // accumulate total marks
        totalMarks += studentRecords[i].mark;
        
        // check for highest mark
        if (studentRecords[i].mark > highestMark) {
            highestMark = studentRecords[i].mark;
            highestName = studentRecords[i].name;
        }
        // check for lowest mark
        if (studentRecords[i].mark < lowestMark) {
            lowestMark = studentRecords[i].mark;
            lowestName = studentRecords[i].name;
        }
    }

    // calculate average mark
    float averageMark = totalMarks / recordCount;

    // display summary
    printf("CMS: Here is the summary of Student Records\n\n");
    printf("Total Student: %d\n", recordCount);
    printf("Average Mark: %.2f\n", averageMark);
    printf("Highest Mark: %.2f (%s)\n", highestMark, highestName);
    printf("Lowest Mark: %.2f (%s)\n", lowestMark, lowestName);
    endStatement();
}

// [SHOW ALL SORT BY]: function to show all records sorted by ID or Mark in ASC or DESC order
void showAll_sort(char *command) {
    // ensure that the database file is opened, else prompt user to use the OPEN command first
    if (DBopened == 0) {
        printf("CMS: Error - No database file is currently open. Use the 'OPEN' command first.\n");
        endStatement();
        return;
    }

    // check if database is empty. if empty, prompt user to INSERT record first
    if (recordCount == 0) {
        printf("CMS: There are currently no records found in the database, insert some records first.\n");
        endStatement();
        return;
    } 

    // move past "SHOW ALL SORT BY " to get sorting arguments
    char *sortingArgs = command + 17;

    // check if sorting arguments are provided, show error message if sorting argument is an empty string
    if (strlen(sortingArgs) == 0) {
        printf("CMS: Error - Missing sorting arguments. Use 'SHOW ALL SORT BY <FIELD> <ORDER>'.\n");
        endStatement();
        return;
    }
    
    // parse sorting arguments into field and order
    char sortField[20] = {0};
    char sortOrder[5] = {0};
    sscanf(sortingArgs, "%19s %4s", sortField, sortOrder);

    // determine the sorting order
    int is_asc = 0; // default is_asc variable is 0 whihch indicates a DESC order
    // if sort order is ASC order, update is_asc variable to 1
    if (strcmp(sortOrder, "ASC") == 0) {
        is_asc = 1;
    } 
    // sort order is not ASC nor is it DESC, show error message that sort order has to be either ASC or DESC
    else if (strcmp(sortOrder, "DESC") != 0) {
        printf("CMS: Error - Invalid sort order '%s'. Use 'ASC' or 'DESC'.\n", sortOrder);
        endStatement();
        return;
    }

    // initialize a temporary array to hold sorted records to not change original order in file
    Student tempRecords[MAX_RECORDS];
    // copy all records in the global variable into the temporary sorting array
    memcpy(tempRecords, studentRecords, recordCount * sizeof(Student));

    // implementation of bubble sort algorithm
    // initialize temporary variable for swapping
    Student temp; 
    for (int i = 0; i < recordCount - 1; i++) {
        for (int j = 0; j < recordCount - i - 1; j++) {
            int shouldSwap = 0;
            
            // determine the sorting field and compare accordingly
            if (strcmp(sortField, "ID") == 0) {
                // check if a swap is needed for ID
                if (is_asc ? (tempRecords[j].id > tempRecords[j + 1].id)
                           : (tempRecords[j].id < tempRecords[j + 1].id)) {
                    shouldSwap = 1;
                }
            } 
            else if (strcmp(sortField, "MARK") == 0) {
                // check if a swap is needed for MARK
                if (is_asc ? (tempRecords[j].mark > tempRecords[j + 1].mark)
                           : (tempRecords[j].mark < tempRecords[j + 1].mark)) {
                    shouldSwap = 1;
                }
            } 
            // if sorting field is neither ID nor MARK, show error message
            else {
                printf("CMS: Error - Invalid sort field '%s'. Only 'ID' and 'MARK' are supported.\n", sortField);
                endStatement();
                return;
            }

            // perform the swap action
            if (shouldSwap) {
                temp = tempRecords[j];
                tempRecords[j] = tempRecords[j + 1];
                tempRecords[j + 1] = temp;
            }
        }
    }

    // display the sorted records
    if (strcmp(sortField, "ID") == 0 || strcmp(sortField, "MARK") == 0) {
        printf("CMS: Displaying records sorted by %s in %s order.\n\n", sortField, sortOrder);
        printf("%-10s %-20s %-30s %-1s\n", "ID", "Name", "Programme", "Mark");
        for (int i = 0; i < recordCount; i++) {
            printf("%-10d %-20s %-30s %.1f\n", 
                   tempRecords[i].id, 
                   tempRecords[i].name, 
                   tempRecords[i].programme, 
                   tempRecords[i].mark);
        }
        endStatement();
    }
}

// case-insensitive substring search
char *strcasestr(const char *haystack, const char *needle) {
    // check if the search string is empty. if empty return NULL
    if (!*needle) {
        return NULL;
    }

    // loop through every character in the haystack until haystack is the null terminator
    for (; *haystack; haystack++) {
        // initialize temporary pointer for comparison 
        const char *hPointer = haystack;
        const char *nPointer = needle;
        // comparison loop 
        while (*hPointer && *nPointer && tolower((unsigned char)*hPointer) == tolower((unsigned char)*nPointer)) {
            hPointer++; nPointer++;
        }
        // if needle pointer hits the null terminator, meaning every character in the needle was matched
        if (!*nPointer)
            return (char *)haystack;
    }
    // needle is not in the haystack
    return NULL;
}

// [QUERY]: function to query specific record by ID, or all records by Name, or Programme
void queryRecord(char *command) {
    // ensure that the database file is opened, else prompt user to use the OPEN command first
    if (DBopened == 0) {
        printf("CMS: Error - No database file is currently open. Use the 'OPEN' command first.\n");
        endStatement();
        return;
    }

    // check if database is empty. if empty, prompt user to INSERT record first
    if (recordCount == 0) {
        printf("CMS: There are currently no records found in the database, insert some records first.\n");
        endStatement();
        return;
    } 

    // extract the part after "QUERY FIELD= "
    char *queryArgs = strchr(command, '=');

    // ensure that a query argument is provided
    if (queryArgs == NULL) {
        printf("CMS: Error - Missing query argument. Expected QUERY \"Field\"=\"Value\".\n");
        endStatement();
        return;
    }

    // ensure no spacing before the '=' sign
    if (*(queryArgs - 1) == ' ') {
        printf("CMS: Error - Invalid format. Ensure no spacing before '='. Expected QUERY \"Field\"=\"Value\".\n");
        endStatement();
        return;
    }

    // extract the search value
    char *searchValueStart = queryArgs + 1;

    // set the search value pointer
    char *searchValue = searchValueStart;

    // ensure no spacing after the '=' sign
    if (*searchValueStart == ' ') {
        printf("CMS: Error - Invalid format. Ensure no spacing after '='. Expected QUERY \"Field\"=\"Value\".\n");
        endStatement();
        return;
    }

    // determine the field being queried (ID, Name or Programme)
    char field[15];
    const char *queryPrefix = "QUERY ";
    char *fieldStart = command + strlen(queryPrefix);

    // calculate length of field name
    int fieldLength = queryArgs - fieldStart;

    // copy field variable
    strncpy(field, fieldStart, fieldLength);

    // null-terminate the fieldLength integer string to only get the field name
    field[fieldLength] = '\0';

    int foundCount = 0;
    int is_id_query = (strcmp(field, "ID") == 0);
    int is_name_query = (strcmp(field, "Name") == 0);
    int is_programme_query = (strcmp(field, "Programme") == 0);

    // if query field is neither id, name, or programme, show an error message
    if (!is_id_query && !is_name_query && !is_programme_query) {
        printf("CMS: Error - Unknown query field '%s'. Use 'ID', 'Name' or 'Programme'.\n", field);
        endStatement();
        return;
    }

    // perform the search action
    printf("CMS: Searching for records where %s=\"%s\".\n\n", field, searchValue);
    // loop through every records in the global array to search for matching records
    for (int i = 0; i < recordCount; i++) {
        int match = 0;

        // perfroming id search
        if (is_id_query) {
            // check for exact ID match
            int searchID = atoi(searchValue);
            if (studentRecords[i].id == searchID) {
                match = 1;
            }
        }
        // performing name search
        else if (is_name_query) {
            // check if search_value exists anywhere within studentRecords[i].name
            if (strcasestr(studentRecords[i].name, searchValue) != NULL) {
                match = 1;
            }
        } 
        // performing programme search
        else if (is_programme_query) {
            // check if search_value exists anywhere within studentRecords[i].programme
            if (strcasestr(studentRecords[i].programme, searchValue) != NULL) {
                match = 1;
            }
        } 
        if (match) {
            if (foundCount == 0) {
                // print header
                printf("%-10s %-20s %-30s %-1s\n", "ID", "Name", "Programme", "Mark");
            }
            // print all matching records
            printf("%-10d %-20s %-30s %-1.1f\n", 
                   studentRecords[i].id, 
                   studentRecords[i].name, 
                   studentRecords[i].programme, 
                   studentRecords[i].mark);
            foundCount++;
            
        }
    }

    if (foundCount > 0) {
        printf("\nCMS: Found %d record(s) matching the criteria.\n", foundCount);
    } else {
        printf("CMS: Error - No record was found where %s=\"%s\".\n", field, searchValue);
    }
    endStatement();
}

// [UPDATE ID=]: function to update a single field of a specific record by ID
void updateRecord(char *command) {
    // ensure that the database file is opened, else prompt user to use the OPEN command first
    if (DBopened == 0) {
        printf("CMS: Error - No database file is currently open. Use the 'OPEN' command first.\n");
        endStatement();
        return;
    }

    // check if database is empty. if empty, prompt user to INSERT record first
    if (recordCount == 0) {
        printf("CMS: There are currently no records found in the database, insert some records first.\n");
        endStatement();
        return;
    } 

    // get the string arguments after "UPDATE ID="
    char *updateID = strstr(command, "UPDATE ID=");
    updateID += strlen("UPDATE ID=");

    // extract update arguments strings
    char *updateArgs = strchr(updateID, ' ');
    if (updateArgs == NULL) {
        printf("CMS: Error - Missing field argument. Expected UPDATE ID=\"ID\" \"<FIELD>\"=\"<VALUE>\" \n\n");
        endStatement();
        return;
    }

    // ensure single spacing after the ID value to be updated"
    if (*(updateArgs + 1) == ' ') {
        printf("CMS: Error - Invalid format. Ensure single spacing after the ID value.\n");
        endStatement();
        return;
    }
    
    // null terminate update arguments
    *updateArgs = '\0';

    // restore space for subsequent parsing
    *updateArgs = ' ';

    int searchID = atoi(updateID);
    if (searchID == 0) {
        printf("CMS: Error - Invalid ID value in UPDATE command.\n");
        endStatement();
        return;
    }

    // extract field and value pair
    char *fieldValuePair = updateArgs + 1;
    
    // extract "=value" of the field to be updated
    char *equalString = strchr(fieldValuePair, '=');
    if (equalString == NULL) {
        printf("CMS: Error - Malformed UPDATE command. Missing '=' sign.\n");
        endStatement();
        return;
    }

    // ensure no space before the '=' between the field value pair to be updated
    if (*(equalString - 1) == ' ') {
        printf("CMS: Error - Invalid format. No space allowed before '=' in the Field=Value pair.\n");
        endStatement();
        return;
    }

    // extract value to be updated
    char *value = equalString + 1;
    // ensure no space after the '=' between the field value pair to be updated
    if (*value == ' ') {
        printf("CMS: Error - Invalid format. No space allowed after '=' in the Field=Value pair.\n");
        endStatement();
        return;
    }

    // check to ensure the value of the update field is not empty
    if (strlen(value) == 0) {
        printf("CMS: Error - Invalid format. Missing update value.\n");
        endStatement();
        return;
    }

    // extract field to be updated
    *equalString = '\0';
    char *field = fieldValuePair;
    
    int found = 0;
    
    // loop through global array to search for the ID that was input in the command
    for (int i = 0; i < recordCount; i++) {
        if (studentRecords[i].id == searchID) {
            found = 1;
            
            // do not allow the update of ID field as it is a unique identifier
            if (strcmp(field, "ID") == 0) {
                printf("CMS: ID is unique and cannot be updated.\n");
                endStatement();
            }
            // check if field to be updated is Name
            else if (strcmp(field, "Name") == 0) {
                // update Name field
                strncpy(studentRecords[i].name, value, sizeof(studentRecords[i].name) - 1);
                studentRecords[i].name[sizeof(studentRecords[i].name) - 1] = '\0';
                printf("CMS: The record with ID=%d is successfully updated.\n", searchID);
                 // save changes to the database file
                saveDB();   
            } 
            // check if field to be updated is Programme
            else if (strcmp(field, "Programme") == 0) {
                // update Programme field
                strncpy(studentRecords[i].programme, value, sizeof(studentRecords[i].programme) - 1);
                studentRecords[i].programme[sizeof(studentRecords[i].programme) - 1] = '\0';
                printf("CMS: The record with ID=%d is successfully updated.\n", searchID);
                // save changes to the database file
                saveDB(); 
            } 
            // check if field to be updated is Mark
            else if (strcmp(field, "Mark") == 0) {
                char *endPointer;
                // convert value to a double and save pointer to the first undiscovered character in the endpointer. if endpointer points to a non-numeric number, if statement is used to return error message.
                double newMark = strtod(value, &endPointer);

                // first validation check to ensure the value to be updated is a numeric number
                if (*endPointer != '\0') {
                    printf("CMS: Error - Mark must be a numeric value.\n");
                    endStatement();
                } 
                
                // second validation check to ensure the value to be updated is a numeric number between 0 and 100
                else if (newMark < 0.0 || newMark > 100.0) {
                    printf("CMS: Error - Mark must be between 0 and 100.\n");
                    endStatement(); 
                }

                // update Mark field if all validation passed
                else {
                    studentRecords[i].mark = newMark;
                    printf("CMS: The record with ID=%d is successfully updated.\n", searchID);
                    endStatement();
                    // save changes to the database file
                    saveDB(); 
                }      
            }
            // invalid field name to be updated
            else {
                printf("CMS: Error - Invalid field '%s' for update.\n", field);
                endStatement();
            }
            break;
        }
    }
    if (!found) {
        printf("CMS: Error - Record with ID=%d not found.\n", searchID);
        endStatement();
    }
}

void insertRecord() {
    // ensure that the database file is opened
    if (DBopened == 0) {
        printf("CMS: Error - No database file is currently open. Use the 'OPEN' command first.\n");
        endStatement();
        return;
    }

    char idInput[20];
    int id;

    // Enter ID Number
    while (1) {
        printf("INSERT ID=");
        scanf("%19s", idInput);
        getchar(); // clear newline

        int len = strlen(idInput);
        int valid = 1;

        // Check: must be exactly 7 digits
        if (len != 7) {
            valid = 0;
        } else {
            for (int i = 0; i < len; i++) {
                if (!isdigit(idInput[i])) {
                    valid = 0;
                    break;
                }
            }
        }

        if (!valid) {
            printf("CMS: Invalid ID. Student ID must contain exactly 7 digits (e.g., 2301234).\n");
            continue;
        }

        id = atoi(idInput);

        // Check for duplicate ID
        int duplicate = 0;
        for (int i = 0; i < recordCount; i++) {
            if (studentRecords[i].id == id) {
                printf("CMS: The record with ID=%d already exists.\n", id);
                duplicate = 1;
                break;
            }
        }

        if (duplicate) return;

        break; // ID is valid and unique
    }

    Student newStudent;
    newStudent.id = id;

    // Validation of Name (49 Character)
    while (1) {
        printf("Enter Name: ");
        fgets(newStudent.name, sizeof(newStudent.name), stdin);
        newStudent.name[strcspn(newStudent.name, "\n")] = '\0'; // remove newline

        if (strlen(newStudent.name) == 0) {
            printf("CMS: Name cannot be empty.\n");
        } else if (strlen(newStudent.name) >= MAX_NAME_LEN) {
            printf("CMS: Name too long (max %d characters).\n", MAX_NAME_LEN - 1);
        } else {
            break;
        }
    }

    // Validation of Programme Name (49 Character)
    while (1) {
        printf("Enter Programme: ");
        fgets(newStudent.programme, sizeof(newStudent.programme), stdin);
        newStudent.programme[strcspn(newStudent.programme, "\n")] = '\0';

        if (strlen(newStudent.programme) == 0) {
            printf("CMS: Programme cannot be empty.\n");
        } else if (strlen(newStudent.programme) >= MAX_PROGRAM_LEN) {
            printf("CMS: Programme too long (max %d characters).\n", MAX_PROGRAM_LEN - 1);
        } else {
            break;
        }
    }

    // Validation of Marks ≥0 - ≤100
    while (1) {
        printf("Enter Mark (0 - 100): ");
        if (scanf("%f", &newStudent.mark) != 1) {
            printf("CMS: Invalid input. Please enter a numeric mark.\n");
            while (getchar() != '\n'); // clear invalid input
            continue;
        }
        if (newStudent.mark < 0 || newStudent.mark > 100) {
            printf("CMS: Invalid mark. Must be between 0 and 100.\n");
            continue;
        }
        getchar(); // consume leftover newline
        break;
    }

    // Save new record into database
    studentRecords[recordCount++] = newStudent;
    saveDB();

    printf("CMS: A new record with ID=%d is successfully added and saved.\n", newStudent.id);
    endStatement();
}

//[GRADE ALL FUNCTION]: Marks of every students by will be converted to their GPA and grades
void gradeAll() {
    if (DBopened == 0) {
        printf("CMS: Error - No database file is currently open. Use the 'OPEN' command first.\n");
        endStatement();
        return;
    }

    // check if database is empty
    if (recordCount == 0) {
        printf("CMS: There are currently no records found in the database, insert some records first.\n");
        endStatement();
        return;
    } 


    printf("CMS: All records with SIT grading:\n");
    printf("%-10s %-20s %-30s %-8s %-8s %-8s %-1s\n", "ID", "Name", "Programme", "Mark", "Grade", "GPA", "Remark");

    for (int i = 0; i < recordCount; i++) {
        const GradeBand* gb = grade_of(studentRecords[i].mark);
        printf("%-10d %-20s %-30s %-8.1f %-8s %-8.1f %-1s\n",
            studentRecords[i].id,
            studentRecords[i].name,
            studentRecords[i].programme,
            studentRecords[i].mark,
            gb->letter,
            gb->gpa,
            gb->remark);
    }
    endStatement();
}

// [DELETE FUNCTION]: delete a record in-memory, ask for confirmation
void deleteRecord(char* command) {
    // ensure that the database file is opened
    if (DBopened == 0) {
        printf("CMS: Error - No database file is currently open. Use the 'OPEN' command first.\n");
        endStatement();
        return;
    }

    // check if database is empty
    if (recordCount == 0) {
        printf("CMS: There are currently no records found in the database, insert some records first.\n");
        endStatement();
        return;
    } 

    // extract id after the command "DELETE ID="
    char* stringID = command + strlen("DELETE ID=");
    int search_id = atoi(stringID);
    printf("Searching for ID:%d\n", search_id);
    if (search_id == 0) {
        printf("CMS: Error - Invalid ID value in DELETE command.\n");
        endStatement();
        return;
    }

    int found_index = -1;
    for (int i = 0; i < recordCount; i++) {
        if (studentRecords[i].id == search_id) {
            found_index = i;
            break;
        }
    }

    if (found_index == -1) {
        printf("CMS: The record with ID=%d does not exist.\n", search_id);
        endStatement();
        return;
    }

    // Confirm deletion
    printf("CMS: Are you sure you want to delete record with ID=%d? Type \"Y\" to Confirm or type \"N\" to cancel.\n", search_id);
    printf("%s: ", teamName);

    char resp[16];
    if (fgets(resp, sizeof(resp), stdin) == NULL) {
        printf("CMS: The deletion is cancelled.\n");
        endStatement();
        return;
    }
    resp[strcspn(resp, "\n")] = 0;

    if (resp[0] == 'Y' || resp[0] == 'y') {
        // remove record by shifting remaining entries left
        for (int j = found_index; j < recordCount - 1; j++) {
            studentRecords[j] = studentRecords[j + 1];
        }
        recordCount--;
        printf("CMS: The record with ID=%d is successfully deleted.\n", search_id);
        endStatement();
        saveDB();
        // Note: Do not auto-save; user should call SAVE when ready.
    } else {
        printf("CMS: The deletion is cancelled.\n");
        endStatement();
    }
}

//[STATS GPA FUNCTIONS]: Showing the statistics of students average, top and lowest GPA as well as the distribution of grades
void statsGPA() {
    // Check if a database file is open
    if (DBopened == 0) {
        printf("CMS: Error - No database file is currently open. Use the 'OPEN' command first.\n");
        endStatement();
        return;
    }

    // Check if database has any records
    if (recordCount == 0) {
        printf("CMS: There are currently no records found in the database, insert some records first.\n");
        endStatement();
        return;
    } 

    // Initialize variables for tracking statistics
    int gradeBuckets[sizeof(SIT_BANDS) / sizeof(SIT_BANDS[0])] = { 0 };
    float totalGPA = 0.0f;
    float topGPA = -1e9f, lowestGPA = 1e9f;
    float topMark = -1e9f, lowestMark = 1e9f;
    const char* topStudentName = "";
    const char* lowestStudentName = "";

    // Loop through all student records
    for (int i = 0; i < recordCount; i++) {
        const GradeBand* currentGrade = grade_of(studentRecords[i].mark);
        
        // Find which grade bucket this student belongs to
        int bucketIndex = 0;
        for (int k = 0; k < (int)(sizeof(SIT_BANDS) / sizeof(SIT_BANDS[0])); ++k) {
            if (currentGrade == &SIT_BANDS[k]) { 
                bucketIndex = k; 
                break; 
            }
        }
        gradeBuckets[bucketIndex]++;
        totalGPA += currentGrade->gpa;

        // See top GPA student
        if (currentGrade->gpa > topGPA) { 
            topGPA = currentGrade->gpa; 
            topMark = studentRecords[i].mark; 
            topStudentName = studentRecords[i].name; 
        }
        
        // See lowest GPA student
        if (currentGrade->gpa < lowestGPA) { 
            lowestGPA = currentGrade->gpa; 
            lowestMark = studentRecords[i].mark; 
            lowestStudentName = studentRecords[i].name; 
        }
    }

    // Display the statistics of GPA
    printf("GPA STATISTICS (SIT)\n");
    printf("------------------------------\n");
    printf("Students     : %d\n", recordCount);
    printf("Average GPA  : %.2f\n", totalGPA / recordCount);
    printf("Top GPA      : %.1f (%.1f mark) %s\n", topGPA, topMark, topStudentName);
    printf("Lowest GPA   : %.1f (%.1f mark) %s\n\n", lowestGPA, lowestMark, lowestStudentName);

    // [Grade distribution]
    // compare marks to grade bands and prevent duplicate loops when counting
    int band_count = (int)(sizeof(SIT_BANDS) / sizeof(SIT_BANDS[0]));
    int buckets_percent[sizeof(SIT_BANDS) / sizeof(SIT_BANDS[0])] = { 0 };
    for (int i = 0; i < recordCount; i++) {
        float m = studentRecords[i].mark;
        for (int k = 0; k < band_count; ++k) {
            if (m >= SIT_BANDS[k].min_mark) { buckets_percent[k]++; break; }
        }
    }
    // display grade distribution with percentages
    printf("Grade distribution (%%):\n");
    for (int i = 0; i < band_count; ++i) {
        double percent = (recordCount > 0) ? (100.0 * buckets_percent[i] / recordCount) : 0.0;
        printf("  %-3s : %3d (%.1f%%)\n", SIT_BANDS[i].letter, buckets_percent[i], percent);
    }
    endStatement();
}