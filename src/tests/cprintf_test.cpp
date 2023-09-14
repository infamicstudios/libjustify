// Copyright 2023 Lawrence Livermore National Security, LLC and other
// libjustify Project Developers. See the top-level LICENSE file for details.
//
// SPDX-License-Identifier: MIT

#include <cprintf.h>
#include <unistd.h> //dup
#include <cstdio>
#include <fcntl.h> //open
#include <getopt.h> // Getopt 
#include <gtest/gtest.h>
#include <unistd.h>


bool print_output = false;
int test_to_print = -1;  // global variable to specify the test to print
static const char *Writefilename = "output.txt";

class OutputTest : public ::testing::Test {
protected:    
    char buffer[1024];
    int fd_out;
    int fd_stdout;

    virtual void SetUp() {
        fd_out = open(Writefilename, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
        fd_stdout = dup(STDOUT_FILENO);
        dup2(fd_out, STDOUT_FILENO);
    }

    virtual void TearDown() {
        dup2(fd_stdout, STDOUT_FILENO);
        close(fd_out);
    }

    void StoreAndCheckBuff(){
        memset(buffer, 0, sizeof(buffer));
        lseek(fd_out, 0, SEEK_SET);
        ssize_t bytes_read = read(fd_out, buffer, sizeof(buffer));
        if (bytes_read < 0) {
            close(fd_out);
            perror("read");
            exit(1);
        }
    }

    virtual void PrintOutput() {
        if (print_output) {
            StoreAndCheckBuff();
            dprintf(fd_stdout, "output: %s\n", buffer);
        }
    }  

    // Helper function for retrieving output
    const char* GetOutput() {
        StoreAndCheckBuff();
        return buffer;
    }
};



void printtestHeader(int testnum){
    printf("\n\n\n");
    printf("=====================================\n");
    printf("Test %d\n", testnum);
    printf("=====================================\n");
}

class Hello_World: public OutputTest {};

TEST_F(Hello_World, Test1) {
    printtestHeader(1);
    cfprintf(stdout, "%-s, %s!\n", "Hello", "world");
    cflush();
    const char* cprintf_output = GetOutput();

    fprintf(stdout, "Hello, %s!\n", "world");
    const char* printf_output = GetOutput();

    ASSERT_STREQ(cprintf_output, printf_output);
}

class Test_Int_Specifier: public OutputTest {};

TEST_F(Test_Int_Specifier, Test2) {

    printtestHeader(2);
    cfprintf(stdout, "%d, %d\n", 123, -456);
    cfprintf(stdout, "%i, %i\n", 789, -321);
    cflush();
    const char* cprintf_output = GetOutput();

    fprintf(stdout, "%d, %d\n", 123, -456);
    fprintf(stdout, "%i, %i\n", 789, -321);
    const char* printf_output = GetOutput();

    ASSERT_STREQ(cprintf_output, printf_output);
}


class Strings_Two_lines : public OutputTest {};

TEST_F(Strings_Two_lines, Test3) {
    printtestHeader(3);
    cfprintf(stdout, "%-s %s!\n", "Hello", "world");
    cfprintf(stdout, "%s %s!\n", "It works on my", "machine");
    cflush();
    const char* cprintf_output = GetOutput();

    fprintf(stdout, "Hello             world!\n");
    fprintf(stdout, "It works on my machine!\n");
    const char* printf_output = GetOutput();

    ASSERT_STREQ(cprintf_output, printf_output);
}

class Test_Two_line_Integer_and_Float : public OutputTest {};

TEST_F(Test_Two_line_Integer_and_Float, Test4) {
    printtestHeader(4);
    cfprintf(stdout, "%d %d %d\n", 1, 2, 3);
    cfprintf(stdout, "%f %f %e\n", 3.14159, 1.618033, 299792458.0);
    cflush();
    const char* cprintf_output = GetOutput();

    fprintf(stdout, "   1        2            3\n");
    fprintf(stdout, "%f %f %e\n", 3.14159, 1.618033, 299792458.0);
    const char* printf_output = GetOutput();

    ASSERT_STREQ(cprintf_output, printf_output); 
}


class TestBackToBack : public OutputTest {};

TEST_F(TestBackToBack, Test5) {
  printtestHeader(7);
  cprintf("%s%s x %s\n", "hello to everybody", "world", "!!!");
  cprintf("%#08x %08x\n", 4096, 1024);
  cprintf("%4d.%02d\n", 2048, 31);
  cfprintf(stdout, "%d%d %d\n", 1, 2, 3);
  cfprintf(stdout, "%f %f%e\n", 3.14159, 1.618033, 299792458.0);
  cprintf("%s %s%s %s\n", "Threads", "HWThread", "Core", "Socket");
  cflush();
  const char* cprintf_output = GetOutput();


  fprintf(stdout, "hello to everybodyworld x !!!\n");
  fprintf(stdout, "0x001000 00000400\n");
  fprintf(stdout, "2048.31\n");
  fprintf(stdout, "12 3\n");
  fprintf(stdout, "3.141590 1.6180332.997925e+08\n");
  fprintf(stdout, "Threads HWThreadCore Socket\n");
  const char* printf_output = GetOutput();

  ASSERT_STREQ(cprintf_output, printf_output);
}


int main(int argc, char **argv) {

    printf("Redirecting output to %s\n", Writefilename);

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}