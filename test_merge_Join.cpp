#include <bits/stdc++.h>
int MB;
int limit = floor((float)MB / 2.0) * TB;
class cmp
{
public:
    bool operator()(std::vector<std::string> &v1, std::vector<std::string> &v2)
    {
        return v1[0] > v2[0];
    }
};
int TB = 10;
std::string path_to_store = "C:\\Users\\atuluttam\\Documents\\new_visual\\project";
void wri_buf(std::vector<std::pair<std::string, std::string>> &vec, std::string path)
{
    std::ofstream tem_file(path);
    for (auto i : vec)
    {
        tem_file << i.second << "\n";
    }
    tem_file.close();
}
void open_sorted_runs_file(std::vector<std::string> const &sorted_runs_path, std::vector<std::ifstream *> &sorted_runs_file)
{
    int i = 0;
    while (i < sorted_runs_path.size())
    {
        sorted_runs_file.push_back(new std::ifstream(sorted_runs_path[i]));
        i++;
    }
}
void close_sorted_runs_file(std::vector<std::string> const &sorted_runs_path, std::vector<std::ifstream *> &sorted_runs_file)
{
    int i = 0;
    while (i < sorted_runs_path.size())
    {
        sorted_runs_file[i]->close();
        free(sorted_runs_file[i]);
        i++;
    }
    // tab_file.close();
}
bool full_container(int sorted_file_num, std::vector<std::vector<std::vector<std::string>>> &container_queue, std::vector<std::ifstream *> const &sorted_files, int B)
{
    std::string line;
    int i = 1;
    bool flag = false;
    while (getline(*sorted_files[sorted_file_num], line) && i <= TB * B)
    {
        flag = true;
        int offset = line.find_first_of(",");
        std::string key = line.substr(1, offset - 1);
        container_queue[sorted_file_num].push_back({key, std::to_string(sorted_file_num), line});
        i++;
    }
    return flag;
}
int main(int argc, char *argv[])
{
    int total_tuple = 0;
    int total_block = 0;
    int Mblock = 0;
    std::string file_path = argv[1];
    MB = std::stoi(argv[2]);
    std::ifstream file(file_path);
    if (!file.is_open())
    {
        exit(-1);
    }
    // file1.close();
    std::vector<std::pair<std::string, std::string>> file_vec;
    std::string line, key;
    int count = 0;
    int offset;
    std::vector<std::string> sorted_runs_path;
    std::vector<std::ifstream *> sorted_runs_file;
    int idx = 0;
    while (getline(file, line))
    {
        if (file.good() && !line.empty())
        {
            if (count == 0)
            {
                count++;
                continue;
            }
            //  std::cout << count << "\n";
            // std::cout << line << "\n";
            // std::cout << key << "\n";
            offset = line.find_first_of(",");
            key = line.substr(1, offset - 1);
            file_vec.push_back({key, line});
            count++;
            if (file_vec.size() == MB * TB)
            {
                std::cout << "we are here\n";
                sort(file_vec.begin(), file_vec.end());
                std::string pat = path_to_store + "\\" + "_" + std::to_string(idx) + ".txt";
                sorted_runs_path.push_back(pat);
                wri_buf(file_vec, pat);
                idx++;
                file_vec.clear();
            }
        }
    }
    // std::random_shuffle(file_vec.begin(),file_vec.end());
    if (file_vec.size() != 0)
    {
        sort(file_vec.begin(), file_vec.end());
        std::string pat = path_to_store + "\\" + "_" + std::to_string(idx) + ".txt";
        sorted_runs_path.push_back(pat);
        wri_buf(file_vec, pat);
        idx++;
        // file_vec.clear();
    }
    total_tuple = (idx - 1) * TB * MB + file_vec.size();
    file_vec.clear();
    total_block = ceil((float)total_tuple / (float)TB);
    std::cout << "\n"
              << total_tuple << "  " << total_block << "\n\n";
    // sort(file_vec.begin(),file_vec.end());
    // for (auto i : file_vec)
    // {
    //     std::cout << i.first << " " << i.second << "\n";
    // }
    // how many block do we need to read from each sorted_runs
    int block_to_read_from_sorted_run = floor(((float)(MB) / (float)sorted_runs_path.size()));
    std::cout << "total block to read from each sorted runs are: " << block_to_read_from_sorted_run << "\n\n\n\n";
    std::vector<std::vector<std::vector<std::string>>> container_queue(sorted_runs_path.size());
    // for (int i = 0; i < sorted_runs_path.size(); i++)
    // {
    //     container_queue.push_back(new std::vector<std::vector<std::string>>[block_to_read_from_sorted_run * TB]);
    // }
    std::priority_queue<std::vector<std::string>, std::vector<std::vector<std::string>>, cmp> final_sorted_que;
    int i = 0;
    open_sorted_runs_file(sorted_runs_path, sorted_runs_file);
    std::cout << "till 134\n";
    while (i < sorted_runs_path.size())
    {
        int j = 1;
        std::string line;
        while (std::getline(*sorted_runs_file[i], line) && j <= block_to_read_from_sorted_run * TB)
        {
            int offset = line.find_first_of(",");
            std::string key = line.substr(1, offset - 1);
            // key,num_sorted_run,line
            container_queue[i].push_back({key, std::to_string(i), line});
            std::cout << key << "\n";
            j++;
        }
        if (!container_queue[i].empty())
        {
            // access key value in it
            std::cout << container_queue[i][0][0] << "\n";

            container_queue[i].erase(container_queue[i].begin());
            final_sorted_que.push({container_queue[i][0]});
        }
        i++;
    }
    std::cout << "till 155 \n";
    std::string final_sorted_file_path = path_to_store + "\\" + "file_sorted_file.txt";
    std::ofstream final_sorted_file(final_sorted_file_path);
    while (!final_sorted_que.empty())
    {
        auto temp_vec = final_sorted_que.top();
        final_sorted_que.pop();
        final_sorted_file << temp_vec[2];
        final_sorted_file << "\n";
        if (!container_queue[stoi(temp_vec[1])].empty() || full_container(stoi(temp_vec[1]), container_queue, sorted_runs_file, block_to_read_from_sorted_run))
        {
            final_sorted_que.push({container_queue[stoi(temp_vec[1])][0]});
            container_queue[stoi(temp_vec[1])].erase(container_queue[stoi(temp_vec[1])].begin());
        }
    }
    std::cout << "till 173\n";
    close_sorted_runs_file(sorted_runs_path, sorted_runs_file);
    file.close();

    final_sorted_file.close();
    std::vector<std::pair<std::string, std::string>> r_set, s_set;
    int i = 1;
    // std::ifstream final_sorted_fi(final_sorted_file_path);
    std::ifstream fd(final_sorted_file_path);
    get_file_ready(r_set, &fd);
    get_file_ready(s_set, &fd);
    int st=0;
    int en=0;
    int i=0;
    int r_pointer=0;
    while(i<r_set.size()-1){
        if(r_set[st]==r_set[en+1]){
            en++;
            i++;
        }
        break;
    }
    std::ofstream joined_file(path_to_store + "\\" + "joined_file.txt");

    return 0;
}
void get_file_ready(std::vector<std::pair<std::string, std::string>> &t_set, std::ifstream *fd)
{
    int i = 1;
    std::string line, key;

    while (getline(*fd, line) && i <= limit)
    {
        int offset = line.find_first_of(",");
        key = line.substr(1, offset - 1);
        t_set.push_back({key, line});
    }

}