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
        fd_out = open(Writefilename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
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

class Single_Line_Single_String_NoTab : public OutputTest {};

TEST_F(Single_Line_Single_String_NoTab, Test1) {

    cfprintf(stdout, "Hello, %s!\n", "world");
    cflush();

    const char* cprintf_output = GetOutput();
    fprintf(stdout, "Hello, %s!\n", "world");
    const char* printf_output = GetOutput();

    ASSERT_STREQ(cprintf_output, printf_output);
}

class Two_Line_Single_String_Tab : public OutputTest {};

TEST_F(Two_Line_Single_String_Tab, Test2) {
    cfprintf(stdout, "Hello, %s!\n", "world");
    cfprintf(stdout, "It works on my %s!\n", "machine");
    cflush();
}

class Three_Line_Two_Single_String_Three_Int_Tab : public OutputTest {};

TEST_F(Three_Line_Two_Single_String_Three_Int_Tab, Test2) {
    cfprintf(stdout, "Hello, %-s!\n", "world");
    cfprintf(stdout, "It works on my %s!\n", "machine");
    cfprintf(stdout, "%d %d %d\n", 1, 2, 3);
    cflush();
}

int main(int argc, char **argv) {

    int opt;
    static const char *optstring = "oht:";
    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
        case 'o':
            print_output = true;
            printf("Printing output from each test\n");
            break;
        case 'h':
            printf("Options:\n\t-o: Print output\n\t-h: Print this help message\n");
            exit(0);
        default: /* '?' */
            fprintf(stderr, "Usage: %s [-p][-h]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    printf("Redirecting output to %s\n", Writefilename);

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
