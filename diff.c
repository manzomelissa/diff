//
//  main.c
//  diff
//
//  Created by William McCarthy on 4/29/19.
//  Copyright © 2019 William McCarthy. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "diff.h"
#include "Para.c"
#include "Util.c"


#define ARGC_ERROR 1
#define TOOMANYFILES_ERROR 2
#define CONFLICTING_OUTPUT_OPTIONS 3
//#define BUFLEN 256
#define MAXSTRINGS 1024
#define MAXPARAS 4096
#define HASHLEN 200


void version(void) {
    printf("\n\n\ndiff (CSUF diffutils) 1.0.0\n");
    printf("Copyright (C) 2014 CSUF\n");
    printf("This program comes with NO WARRANTY, to the extent permitted by law.\n");
    printf("You may redistribute copies of this program\n");
    printf("under the terms of the GNU General Public License.\n");
    printf("For more information about these matters, see the file named COPYING.\n");
    printf("Written by Richard Cruz\n");
}

char buf[BUFLEN];
char *strings1[MAXSTRINGS], *strings2[MAXSTRINGS];
typedef struct options options;
char filename1[BUFLEN], filename2[BUFLEN];

int count1 = 0, count2 = 0;

void loadfiles(const char* filename1, const char* filename2) {
    memset(buf, 0, sizeof(buf));
    memset(strings1, 0, sizeof(strings1));
    memset(strings2, 0, sizeof(strings2));
    
    FILE *fin1 = openfile(filename1, "r");
    FILE *fin2 = openfile(filename2, "r");
    
    while (!feof(fin1) && fgets(buf, BUFLEN, fin1) != NULL) { strings1[count1++] = strdup(buf); }
    while (!feof(fin2) && fgets(buf, BUFLEN, fin2) != NULL) { strings2[count2++] = strdup(buf); }  fclose(fin2); fclose(fin1);
}

void print_option(const char* name, int value) { printf("%17s: %s\n", name, yesorno(value)); }

void diff_output_conflict_error(void) {
    fprintf(stderr, "diff: conflicting output style options\n");
    fprintf(stderr, "diff: Try `diff --help' for more information.)\n");
    exit(CONFLICTING_OUTPUT_OPTIONS);
}

void setoption(const char* arg, const char* s, const char* t, int n, struct options* op) {
    if ((strcmp(arg, s) == 0) || (t != NULL && strcmp(arg, t) == 0)) {
        setop(op, n);
    }
}

void showoptions(const char* file1, const char* file2, struct options op) {
    printf("diff options...\n");
    print_option("show_version", op.options[0]);
    print_option("show_brief", op.options[1]);
    print_option("ignorecase", op.options[2]);
    print_option("report_identical", op.options[3]);
    print_option("show_sidebyside", op.options[4]);
    print_option("show_leftcolumn", op.options[5]);
    print_option("show_unified", op.options[6]);
    print_option("showcontext", op.options[7]);
    print_option("suppresscommon", op.options[8]);
    print_option("diffnormal", op.options[9]);
    
    printf("file1: %s,  file2: %s\n\n\n", file1, file2);
    
    printline();
}

int file_contents_identical(void) {
    para* p = para_first(strings1, count1);
    para* q = para_first(strings2, count2);
    while (p != NULL && q != NULL && para_equal(p, q)) {
        p = para_next(p);
        q = para_next(q);
    }
    return p == NULL && q == NULL;
}

int check_identical(const char* filename1, const char* filename2) {
    return strcmp(filename1, filename2) == 0 || file_contents_identical() == 1;
}

void init_options_files(int argc, const char* argv[]) {
    int cnt = 0;
    const char* files[2] = { NULL, NULL };
    
    while (argc-- > 0) {
        const char* arg = *argv;
        setoption(arg, "-v",       "--version",                  &showversion);
        setoption(arg, "-q",       "--brief",                    &showbrief);
        setoption(arg, "-i",       "--ignore-case",              &ignorecase);
        setoption(arg, "-s",       "--report-identical-files",   &report_identical);
        setoption(arg, "--normal", NULL,                         &diffnormal);
        setoption(arg, "-y",       "--side-by-side",             &showsidebyside);
        setoption(arg, "--left-column", NULL,                    &showleftcolumn);
        setoption(arg, "--suppress-common-lines", NULL,          &suppresscommon);
        setoption(arg, "-c",       "--context",                  &showcontext);
        setoption(arg, "-u",       "showunified",                &showunified);
        if (arg[0] != '-') {
            if (cnt == 2) {
                fprintf(stderr, "apologies, this version of diff only handles two files\n");
                fprintf(stderr, "Usage: ./diff [options] file1 file2\n");
                exit(TOOMANYFILES_ERROR);
            } else { files[cnt++] = arg; }
        }
        ++argv;   // DEBUG only;  move increment up to top of switch at release
    }
    
    if (!showcontext && !showunified && !showsidebyside && !showleftcolumn) {
        diffnormal = 1;
    }
    
    if (showversion) { version();  exit(0); }
    
    if (((showsidebyside || showleftcolumn) && (diffnormal || showcontext || showunified)) ||
        (showcontext && showunified) || (diffnormal && (showcontext || showunified))) {
        
        diff_output_conflict_error();
    }

    if(showsidebyside){
        loadfiles(files[0], files[1]);
        para* p = para_first(strings1, count1);
        para* q = para_first(strings2, count2);
        int foundmatch = 0;
        
        para* qlast = q;
        while (p != NULL) {
            qlast = q;
            foundmatch = 0;
            while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
                q = para_next(q);
            }
            q = qlast;
            
            if (foundmatch) {
                while ((foundmatch = para_equal(p, q)) == 0) {
                    para_print(q, printright);
                    q = para_next(q);
                    qlast = q;
                }
                para_print(q, printboth);
                p = para_next(p);
                q = para_next(q);
            } else {
                para_print(p, printleft);
                p = para_next(p);
            }
        }
        while (q != NULL) {
            para_print(q, printright);
            q = para_next(q);
        }
        exit(0);
    }
    
char* yesorno(int condition) { return condition == 0 ? "no" : "YES"; }



int main(int argc, const char * argv[]) {
    char buf[BUFLEN];
    char *strings1[MAXSTRINGS], *strings2[MAXSTRINGS];
    
    memset(buf, 0, sizeof(buf));
    memset(strings1, 0, sizeof(strings1));
    memset(strings2, 0, sizeof(strings2));
    
    if (argc != 3) { fprintf(stderr, "Usage: ./diff file1 file2\n");  exit(ARGC_ERROR); }
    
    FILE *fin1 = openfile(argv[1], "r");
    FILE *fin2 = openfile(argv[2], "r");
    
    int count1 = 0, count2 = 0;
    while (!feof(fin1) && fgets(buf, BUFLEN, fin1) != NULL) { strings1[count1++] = strdup(buf); }
    while (!feof(fin2) && fgets(buf, BUFLEN, fin2) != NULL) { strings2[count2++] = strdup(buf); }
    
    para* p = para_first(strings1, count1);
    para* q = para_first(strings2, count2);
    if (para_equal(p, q)) { para_print(p, printboth);
    } else {
        para_print(q, printright);
        
        while ((q = para_next(q)) != NULL) {
            int equal = para_equal(p, q);
            para_print(q, equal ? printboth : printright);
        }
    }
    
    //  printf("p is: %s", para_info(p));
    //  printf("q is: %s", para_info(q));
    //  para_print(p, printleft);
    //  para_print(q, printright);
    //  printf("p and q are equal: %s\n\n", yesorno(para_equal(p, q)));
    
    printf("\nTODO: check line by line in a paragraph, using '|' for differences");
    printf("\nTODO: this starter code does not yet handle printing all of fin1's paragraphs.");
    printf("\nTODO: handle the rest of diff's options\n");
    printf("\tAs Tolkien said it, '...and miles to go before I sleep.'\n\n");
    fclose(fin1);
    fclose(fin2);
    return 0;
}
