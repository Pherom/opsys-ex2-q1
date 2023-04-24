#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <math.h>
#include <string.h>

#define GRADES_FILE "all_std.log"
#define ERR_OUT_WRITE "Failed to open output file for writing"
#define ERR_IN_READ "Failed to open input file for reading"
#define FINAL_MSG_FORMAT "grade calculation for %d students is done\n"

void createAllStdFile(int* pid_arr, size_t pid_count);
void report_data_summary(int num_stud);
char* getTempFileName(int pid);

int main(int argc, char* argv[]) {
    FILE* readers_msg_file;
    FILE* curr_reader_file;
    char* readers_msg_file_name = argv[1];
    char* curr_reader_file_name;
    char* read_grades_argv[2];
    int temp_std_out;
    int temp_std_in;
    int pid;
    int* pid_arr;
    int wstatus;
    int pid_index = 0;
    int std_count = 0;


    temp_std_out = dup(1);
    close(1);
    readers_msg_file = fopen(readers_msg_file_name, "w");

    read_grades_argv[0] = "read_grades";
    read_grades_argv[1] = NULL;

    for (int i = 2; i < argc; i++) {
        curr_reader_file_name = argv[i];

        temp_std_in = dup(0);
        close(0);

        curr_reader_file = fopen(curr_reader_file_name, "r");

        if (fork() == 0) {
            exit(execve(read_grades_argv[0], read_grades_argv, NULL));
        }

        fclose(curr_reader_file);
        dup(temp_std_in);
        close(temp_std_in);
    }

    fclose(readers_msg_file);
    dup(temp_std_out);
    close(temp_std_out);

    pid_arr = (int*)malloc(sizeof(int) * (argc - 2));

    while((pid = wait(&wstatus)) != -1) {
        pid_arr[pid_index] = pid;
        std_count += WEXITSTATUS(wstatus);
        pid_index++;
    }

    pid = fork();
    if (pid == 0) {
        createAllStdFile(pid_arr, argc - 2);
    }

    if (pid != 0) {
        wait(NULL);
        report_data_summary(std_count);
        free(pid_arr);
    }
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

void createAllStdFile(int* pid_arr, size_t pid_count) {
    char* in_file_name;
    char* curr_line;
    FILE* in_file;
    FILE* out_file;
    size_t curr_line_len;

    out_file = fopen(GRADES_FILE, "w");
    
    if (out_file == NULL) {
        perror(ERR_OUT_WRITE);
    }

    else {
        for (int i = 0; i < pid_count; i++) {
            in_file_name = getTempFileName(pid_arr[i]);

            if (in_file_name != NULL) {
                in_file = fopen(in_file_name, "r");
                if (in_file == NULL) {
                    perror(ERR_IN_READ);
                }
                else {
                    while(getline(&curr_line, &curr_line_len, in_file) != -1) {
                        fprintf(out_file, "%s", curr_line);
                    }
                    fclose(in_file);
                }
                free(in_file_name);
            }
        }
        fclose(out_file);
    }
}

void report_data_summary(int num_stud) {
    fprintf(stderr, FINAL_MSG_FORMAT, num_stud);
}
