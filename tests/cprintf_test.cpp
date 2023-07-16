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

    //wrote a custom function for this... doesn't work for now.
    //TestSpecifiers("%d, %d\n", 123, -456);
    //TestSpecifiers("%i, %i\n", 789, -321);
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


class conversion_specs : public OutputTest {};

TEST_F(conversion_specs, Test3) {
    printtestHeader(3);
    cfprintf(stdout, "%-s %s!\n", "Hello", "world");
    cflush();
    const char* cprintf_output = GetOutput();

    cfprintf(stdout, "%s %s!\n", "It works on my", "machine");
    const char* printf_output = GetOutput();
    cflush();
}

class Three_Line_Two_Single_String_Three_Int_Tab : public OutputTest {};

TEST_F(Three_Line_Two_Single_String_Three_Int_Tab, Test4) {
    cfprintf(stdout, "%-s %s!\n", "Hello", "world");
    cfprintf(stdout, "%s %s!\n", "It works on my ", "machine");
    cfprintf(stdout, "%d %d %d\n", 1, 2, 3);
    cflush();
}

class LongTextInMiddle : public OutputTest {};

TEST_F(LongTextInMiddle, Test5) {
    cfprintf(stdout, "%s : %s\n", "Hyperthreading", "Enabled");
    //cfprintf(stdout, "It works on my %s %s\n", "machine", "Sometimes");
    cfprintf(stdout, "%d : %d : %d\n", 1, 2, 3);
    cfprintf(stdout, "%d : %d : %d : %d\n", 1, 2, 3, 4);
    //cfprintf(stdout, "This text is the longest : %d : %d\n\n\n",5, 10);
    cflush();
}

void print_children(unsigned int i) {
    unsigned ARR_SIZE = 32;

    if( i == 0 ) {
        cprintf("%s %s %s %s\n", "Thread", "HWThread", "Core", "Socket");
    }

    int socket = (i > ARR_SIZE/2) % 2;
    cfprintf(stdout, "%d %d %d %d\n", i, i, i, socket);

    i = i+1;
    if (i < ARR_SIZE) {
        print_children(i);
    } else {
        cflush();
    }
    
}

class TestTopology : public OutputTest {};

TEST_F(TestTopology, Test6) {
    printtestHeader(4);
    char *hostname = "quartz1234";
    int num_sockets = 2;
    int num_cores_per_socket = 18;
    int total_cores = num_sockets * num_cores_per_socket;
    int total_Threads = 36;
    int threads_per_core = 2;


    cfprintf(stdout, "=================\n");
    cfprintf(stdout, "Platform Topology\n");
    cfprintf(stdout, "=================\n");
    cflush();
    cfprintf(stdout, "%-s: %s\n", "Hostname", hostname);
    //cfprintf(stdout, "Num Sockets: %d\n", num_sockets);
    cfprintf(stdout, "%-s: %d\n", "Num Cores per Socket", num_cores_per_socket);

    if ( threads_per_core == 1)
    {
        cfprintf(stdout, "%-s: %s\n", "  Hyperthreading", "No");
    }
    else
    {
        cfprintf(stdout, "%-s: %s\n", "  Hyperthreading", "Yes");
    }

    
    cfprintf(stdout, "\n");
    cfprintf(stdout, "%-s: %d\n", "Total Num of Cores", total_cores);
    cfprintf(stdout, "%-s: %d\n", "Total Num of Threads", total_Threads);
    cfprintf(stdout, "\n");
    cfprintf(stdout, "Layout:\n");
    cfprintf(stdout, "-------\n");
    cflush();
    print_children(0);
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