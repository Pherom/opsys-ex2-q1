#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define LINE_MAX_LENGTH 100
#define MAX_GRADES_PER_STUDENT 10

void report_data_summary(int num_stud);
char* getTempFileName(int pid);

int main() {
    char* one_student_argv[MAX_GRADES_PER_STUDENT + 3];
    char curr_line[LINE_MAX_LENGTH];
    char* curr_token;
    char* pid_file_name;
    int one_student_argv_index;
    int student_count = 0;
    int temp_std_out = dup(1);
    FILE* pid_file;
    
    one_student_argv[0] = "one_student";

    close(1);
    pid_file_name = getTempFileName(getpid());
    pid_file = fopen(pid_file_name, "w");
    free(pid_file_name);

    while(fgets(curr_line, LINE_MAX_LENGTH, stdin) != NULL) {
        one_student_argv_index = 1;

        curr_token = strtok(curr_line, " \t");
        while (curr_token != NULL) { 
            one_student_argv[one_student_argv_index] = curr_token;
            one_student_argv_index++;
            curr_token = strtok(NULL, " \t");
        }
        one_student_argv[one_student_argv_index] = NULL;

        if (fork() == 0) {
            execve(one_student_argv[0], one_student_argv, NULL);
        }
        
        student_count++;
    }

    fclose(pid_file);
    dup(temp_std_out);
    close(temp_std_out);

    report_data_summary(student_count);

    while(wait(NULL) != -1);

    exit(student_count);
}

void report_data_summary(int num_stud)
{
    printf("process: %d: Completed grade calculation for %d students.\n",
    getpid(), num_stud);
}

char* getTempFileName(int pid) {
    //Temp file name dependant on length of pid therefore dynamic allocation used
    char* temp_file_name = (char*)malloc((int)((ceil(log10(pid))+6)*sizeof(char)));
    if (temp_file_name == NULL) {
        perror("Failed to allocate memory for output file name");
    }
    else {
        sprintf(temp_file_name, "%d.temp", pid);
    }
    return temp_file_name;
}