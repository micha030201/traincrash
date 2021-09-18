#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <time.h>

using namespace std;

struct paket {
    uint8_t pos;
    uint16_t cur;
    uint16_t volt;
};

void read_record(vector<vector<int>>& data) {
    fstream fin;
  
    fin.open("data.csv", ios::in);
  
    vector<string> row;
    string line, word, temp;
  
    while (fin >> temp) {
        row.clear();
        getline(fin, line);
        stringstream s(line);
        

        while (getline(s, word, ',')) {
            row.push_back(word);           
        }

        if (row.size() != 0) {
            data[0].push_back(stoi(row[1]));
            data[1].push_back(stoi(row[5]));
        }
    }
}

int main() {
    vector<vector<int>> data(2, vector<int>());
    read_record(data);

    int data1[1000];  //Random data we want to send
    FILE *file;
    struct paket tmp;
    srand( time(NULL) );
    file = fopen("/dev/ttyUSB0", "w");  //Opening device file
    int i = 0;

    for (int j = 0; j < 1000; ++j) {
	    data1[j] = j;
        tmp.pos = static_cast<uint8_t>(data1[j]);
        tmp.cur = static_cast<uint16_t>(data1[j]);
        tmp.volt = static_cast<uint16_t>(data1[j]);

        
        unsigned char cur1 = ((tmp.cur >> 8) & 255);
        unsigned char cur2 = (tmp.cur & 255);

        unsigned char volt1 = ((tmp.volt >> 8) & 255);
        unsigned char volt2 = (tmp.volt & 255);
        fprintf(file, "%c%c%c%c%c\n", tmp.pos, cur2, cur1, volt2, volt1); //Writing to the file
        //fflush(file);
        //printf("%hhd", data1)
        sleep(1);
    }
    fclose(file);

    return 0;
}