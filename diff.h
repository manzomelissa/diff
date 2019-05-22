//
//  diff.h
//  
//
//  Created by Melissa Manzo on 5/21/19.
//

#ifndef diff_h
#define diff_h

void version(void);
void todo_list(void);
void loadfiles(const char* filename1, const char* filename2);
void print_option(const char* name, int value);
void diff_output_conflict_error(void);
void setoption(const char* arg, const char* s, const char* t, int* value);
void showoptions(const char* file1, const char* file2);
int file_contents_identical(void);
int check_identical(const char* filename1, const char* filename2);
void init_options_files(int argc, const char* argv[]);


#endif /* diff_h */
