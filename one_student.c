#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    char* student_name = argv[1];
    char* student_grade_str;
    int student_grade_curr;
    int student_grade_count = argc - 2;
    float student_grade_sum = 0;
    float student_grade_avg;

    for (int i = 2; i < argc; i++) {
        student_grade_str = argv[i];
        student_grade_curr = atoi(student_grade_str);
        student_grade_sum += student_grade_curr;
    }

    student_grade_avg = student_grade_sum / student_grade_count;

    printf("%s %.1f\n", student_name, student_grade_avg);
}