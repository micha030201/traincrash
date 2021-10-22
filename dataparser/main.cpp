#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <chrono>
#include <thread>


using namespace std;

struct paket {
    uint8_t pos;
    uint16_t cur;
    uint16_t volt;
    uint8_t check;
};

void read_record(vector<vector<int>>& data, char *source) {
    fstream fin;

    fin.open(source, ios::in);

    vector<string> row;
    string line, word, temp;

    while (fin >> temp) {
        row.clear();
        getline(fin, line);
        stringstream s(line);

        while (getline(s, word, ',')) row.push_back(word);

        if (row.size() != 0) {
            //cout << row[1] << " " << row[6] << " " << row[7] << endl;
            data[0].push_back(stoi(row[1]));
            data[1].push_back(stoi(row[6]));
            data[2].push_back(stoi(row[7]));
        }
    }
}

int main(int argc, char **argv) {
    vector<vector<int>> data1(3, vector<int>());
    read_record(data1, argv[1]);

    vector<vector<int>> data2(3, vector<int>());
    read_record(data2, argv[3]);

    FILE *file1;
    struct paket tmp1;
    file1 = fopen(argv[2], "w");

    FILE *file2;
    struct paket tmp2;
    file2 = fopen(argv[4], "w");

    for (int j = 0; j < data1[0].size(); ++j) {
            tmp1.pos = static_cast<uint8_t>(data1[0][j]);
            tmp1.cur = static_cast<uint16_t>(data1[1][j]);
            tmp1.volt = static_cast<uint16_t>(data1[2][j]);
            tmp1.check = tmp1.pos + tmp1.cur + tmp1.volt;

            tmp2.pos = static_cast<uint8_t>(data2[0][j]);
            tmp2.cur = static_cast<uint16_t>(data2[1][j]);
            tmp2.volt = static_cast<uint16_t>(data2[2][j]);
            tmp2.check = tmp2.pos + tmp2.cur + tmp2.volt;

            unsigned char cur11 = ((tmp1.cur >> 8) & 255);
            unsigned char cur12 = (tmp1.cur & 255);
            unsigned char volt11 = ((tmp1.volt >> 8) & 255);
            unsigned char volt12 = (tmp1.volt & 255);

            unsigned char cur21 = ((tmp2.cur >> 8) & 255);
            unsigned char cur22 = (tmp2.cur & 255);
            unsigned char volt21 = ((tmp2.volt >> 8) & 255);
            unsigned char volt22 = (tmp2.volt & 255);

            //printf("%d %d %d\n", tmp.pos, tmp.cur, tmp.volt);

            fprintf(file1, "%c%c%c%c%c%c\n", tmp1.pos, cur12, cur11, volt12, volt11, tmp1.check);
            fprintf(file2, "%c%c%c%c%c%c\n", tmp2.pos, cur22, cur21, volt22, volt21, tmp2.check);

            std::this_thread::sleep_for(std::chrono::milliseconds(30));}

    fclose(file1);
    fclose(file2);

    return 0;
}
