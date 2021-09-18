#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>

using namespace std;

struct paket {
    uint8_t pos;
    uint16_t cur;
    uint16_t volt;
    uint8_t check;
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
        
        while (getline(s, word, ',')) row.push_back(word);

        if (row.size() != 0) {
            data[0].push_back(stoi(row[1]));
            data[1].push_back(stoi(row[6]));
            data[2].push_back(stoi(row[7]));
        }
    }
}

int main() {
    vector<vector<int>> data(3, vector<int>());
    read_record(data);

    FILE *file;
    struct paket tmp;
    file = fopen("/dev/ttyUSB0", "w");

    for (int j = 0; j < data[0].size(); ++j) {
        
            tmp.pos = static_cast<uint8_t>(data[0][j]);
            tmp.cur = static_cast<uint16_t>(data[1][j]);
            tmp.volt = static_cast<uint16_t>(data[2][j]);
            tmp.check = tmp.pos + tmp.cur + tmp.volt;
        
            unsigned char cur1 = ((tmp.cur >> 8) & 255);
            unsigned char cur2 = (tmp.cur & 255);

            unsigned char volt1 = ((tmp.volt >> 8) & 255);
            unsigned char volt2 = (tmp.volt & 255);

            printf("%d %d %d\n", tmp.pos, tmp.cur, tmp.volt);
        
            fprintf(file, "%c%c%c%c%c%c\n", tmp.pos, cur2, cur1, volt2, volt1, tmp.check); 
    }

    fclose(file);

    return 0;
}