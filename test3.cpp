#include <bits/stdc++.h>
using namespace std;

int main(int argc, char *argv[])
{
    string path1 = argv[1];
    string path2 = argv[2];
    ifstream file1(path1);
    ifstream file2(path2);
    vector<int> freq;
    vector<string> temp;
    string line;
    int key;
    int offset;
    while (getline(file1, line))
    {
        if (!line.empty())
        {
            offset = line.find_first_of(",");
            key = stoi(line.substr(1, offset - 1));
            freq.push_back(key);
        }
    }
    while (getline(file2, line))
    {
        if (!line.empty())
        {
            temp.push_back(line);
        }
    }
    random_shuffle(temp.begin(), temp.end());
    file2.close();
    ofstream of;
    of.open(path2, std::ofstream::out | std::ofstream::trunc);
    int i = 0;
    while (i < temp.size())
    {

        int idx = rand() % freq.size();
        string num = "(" + to_string(freq[idx]) + ",";
        of << num;
        of << temp[i].substr(1, temp[i].size()) << "\n";
        i++;
    }
    file1.close();
    of.close();
}