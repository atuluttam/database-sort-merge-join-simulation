#include <bits/stdc++.h>

using namespace std;
#define vi vector<int>
#define vs vector<string>
#define vvs vector<vs>
#define vvvs vector<vvs>
#define ps pair<string, string>
#define vps vector<ps>
#define vf vector<ifstream *>
#define fi first
#define se second
// total cost in term of disc block
int ALL_cost = 0;
string path_to_store = "C:\\Users\\atuluttam\\Documents\\new_visual\\project";
// #define path_to_store "C:\\Users\\atuluttam\\Documents\\new_visual\\project"

// comparator
class cmp
{
public:
    bool operator()(vector<string> &v1, vector<string> &v2)
    {
        return v1[0] > v2[0];
    }
};

class Relation
{
    int TB = 10; // number of tupple per block
    int MB;      // total mem block
    int b;       // total number of block in relation
    int n;       // total number of tupple in relation
    int dup;
    int cost = 0; //
    string tab_file_path;
    ifstream tab_file;
    string table_name;
    string col_info;
    string table_nam;
    int block_to_re_fro_sor_run; // how many block do we need to read from each sorted_runs(as per the mem block cap)
    vps file_vec;                // temp vector which will store block(some) of relation while reading from file
    vs sorted_runs_path;         // store the sorted runs file's path
    vf sorted_runs_file;
    // vi frez_count;

public:
    void dis()
    {
        cout << "Info about table :\n";
        cout << table_nam << "\n";
        cout << col_info << "\n";
        cout << "Total number of Blocks : " << b << "\n";
        cout << "Total number of Tupples : " << n << "\n";
        cout << "Total sorted runs created : " << sorted_runs_path.size() << "\n";
        cout << "Total cost of sorting in term of disc block reads : " << cost << "\n";
        ALL_cost += cost;
    }
    Relation(string path, int mb, string tab_nam)
    {
        MB = mb;
        tab_file_path = path;
        tab_file.open(path);
        table_name = tab_nam;
    }
    void create_sorted_run()
    {
        int offset;
        int count;
        int idx = 0;
        string line, key;

        // runs till the end of table
        while (getline(tab_file, line))
        {
            if (tab_file.good() && !line.empty())
            {
                if (count == 0 || count == 1)
                {
                    if (count == 0)
                        table_nam = line;
                    else
                        col_info = line;
                    count++;
                    continue;
                }
                //  cout << count << "\n";
                // cout << line << "\n";
                // cout << key << "\n";
                offset = line.find_first_of(",");
                key = line.substr(1, offset - 1);
                file_vec.push_back({key, line});
                count++;
                if (file_vec.size() == MB * TB)
                {
                    // sorting the tuple inside the vector
                    sort(file_vec.begin(), file_vec.end());
                    string pat = path_to_store + "\\" + table_name + "\\" + "_" + to_string(idx) + ".txt";
                    sorted_runs_path.push_back(pat);
                    wri_buf(file_vec, pat);
                    idx++;
                    file_vec.clear(); // flush
                }
            }
        }
        if (file_vec.size() != 0)
        {
            sort(file_vec.begin(), file_vec.end());
            string pat = path_to_store + "\\" + table_name + "\\" + "_" + to_string(idx) + ".txt";
            sorted_runs_path.push_back(pat);
            wri_buf(file_vec, pat);
            idx++;
            // file_vec.clear();
        }
        n = (idx - 1) * TB * MB + file_vec.size();
        file_vec.clear();
        b = ceil((float)n / (float)TB);
        cost += b * 2;
    }

    void wri_buf(vector<pair<string, string>> &vec, string path)
    {
        ofstream tem_file(path);
        for (auto i : vec)
        {
            tem_file << i.second << "\n";
        }
        tem_file.close();
    }

    void open_sorted_runs_file(vs const &sorted_runs_path, vector<ifstream *> &sorted_runs_file)
    {
        int i = 0;
        while (i < sorted_runs_path.size())
        {
            sorted_runs_file.push_back(new ifstream(sorted_runs_path[i]));
            i++;
        }
    }
    void close_sorted_runs_file()
    {
        int i = 0;
        while (i < sorted_runs_path.size())
        {
            sorted_runs_file[i]->close();
            free(sorted_runs_file[i]); // deallocate mem
            i++;
        }
    }

    bool full_container(int sorted_file_num, vvvs &container_vec, vector<ifstream *> const &sorted_files, int B)
    {
        string line;
        int i = 1;
        bool flag = false;
        while (getline(*sorted_files[sorted_file_num], line) && i <= TB * B)
        {
            flag = true;
            int offset = line.find_first_of(",");
            string key = line.substr(1, offset - 1);
            container_vec[sorted_file_num].push_back({key, to_string(sorted_file_num), line});
            i++;
        }
        cost += (ceil((float)(i - 1) / (float)TB)) * 2; // multiply by 2 because we have to write back also

        return flag;
    }
    string merge_sorted_runs()
    {
        string final_sorted_file_path;
        // reserving some block for writing the update value ( for min heap)
        int block_to_read_from_sorted_run = floor(((float)(MB - ceil((float)sorted_runs_file.size() / (float)TB)) / (float)sorted_runs_path.size()));

        vvvs container_vec(sorted_runs_path.size());
        priority_queue<vs, vvs, cmp> final_sorted_que; // min heap
        // used to open files of sorted runs
        open_sorted_runs_file(sorted_runs_path, sorted_runs_file);
        //
        int i = 0; // specify sorted runs num
        // if we have only one sorted run ,we don't have to do anything just give same sorted run as final sorted file
        // check memory constraint
        if (sorted_runs_file.size() >= MB)
        {
            cout << "Memory constraint violeted !";
            exit(-1);
        }
        if (sorted_runs_path.size() == 1)
        {
            final_sorted_file_path = sorted_runs_path[0];
            return final_sorted_file_path;
        }
        while (i < sorted_runs_path.size())
        {
            int j = 1;
            string line;
            while (getline(*sorted_runs_file[i], line) && j <= block_to_read_from_sorted_run * TB)
            {
                int offset = line.find_first_of(",");
                string key = line.substr(1, offset - 1);
                // key,num_sorted_run,line
                container_vec[i].push_back({key, to_string(i), line});
                // cout << key << "\n";
                j++;
            }
            // adding block read into cost when we are filling buffer coresponding to each run file at the beginning
            cost += ceil((j - 1) / TB) * 2; // multiply by 2 b/c we write back
            if (!container_vec[i].empty())
            {

                // picking the first tuple from the sorted run and pushing into min heap
                final_sorted_que.push({container_vec[i][0]});
                // erasing the same tuple from the corresponding buffer of sorted run
                container_vec[i].erase(container_vec[i].begin());
            }
            i++;
        }

        final_sorted_file_path = path_to_store + "\\" + table_name + "\\" + "sorted_Table_" + table_name + ".txt";
        ofstream final_sorted_file(final_sorted_file_path);
        while (!final_sorted_que.empty())
        {
            auto temp_vec = final_sorted_que.top();
            final_sorted_que.pop();
            final_sorted_file << temp_vec[2];
            final_sorted_file << "\n";
            if (!container_vec[stoi(temp_vec[1])].empty() || full_container(stoi(temp_vec[1]), container_vec, sorted_runs_file, block_to_read_from_sorted_run))
            {
                final_sorted_que.push({container_vec[stoi(temp_vec[1])][0]});
                container_vec[stoi(temp_vec[1])].erase(container_vec[stoi(temp_vec[1])].begin());
            }
        }
        close_sorted_runs_file();
        return final_sorted_file_path;
    }
};

class sortMergeJoin
{
    int const TB = 10;
    int MB;
    int cost = 0;
    int b = 0; // total number of block in merged table
    int n = 0; // total number of tupple in merge talble
    int tab1_st = 0;
    int tab2_st = 0;
    int st;
    int en;
    // how many block should i read from each sorted file such that mem block shoudn't full
    int block_of_each_rel;
    ifstream tab1_file;
    ifstream tab2_file;
    string merge_table_path;
    ofstream fin_merg_tab;
    vps tab1_buff, tab2_buff;

public:
    int Cost() { return cost; }
    void dis()
    {
        cout << "\n\nMerge Table Info : \n";
        cout << "total number of block : " << b;
        cout << "\ntotal number ot tupple : " << n << "\n";
        cout << "Total cost of join : " << cost << "\n";
        cout << "Joined Table Path : " << merge_table_path << "\n\n";
        ALL_cost += cost;
    }
    sortMergeJoin(string path1, string path2, int m)
    {
        MB = m;

        block_of_each_rel = (MB - 1) / 2; // 1 block reserved for output buffer
        tab1_file.open(path1);
        tab2_file.open(path2);
        // finally merged table path
        merge_table_path = path_to_store + "\\" + "merged_tab" + ".txt";
        fin_merg_tab.open(path_to_store + "\\" + "merged_tab" + ".txt");
        // cout << "\ntable 1 is being filled\n";
        fill_buff(&tab1_file, tab1_buff);
        // cout << "\ntable 2 is being filled\n";
        fill_buff(&tab2_file, tab2_buff); // to fill tab2 buffer
        join_the_buffer();
        dis();
    }
    void fill_buff(ifstream *tab, vps &buff)
    {
        string line, key;
        int offset;
        int i = 1;
        buff.clear();
        while (getline(*tab, line) && i <= block_of_each_rel * TB)
        {
            offset = line.find_first_of(",");
            key = line.substr(1, offset - 1);
            buff.push_back({key, line});
            i++;
        }
        cost += (ceil((float)(i - 1) / (float)TB)) * 2;
        // cout<<tab.eof()<<"\n";
        // cout << "block size : " << buff.size() / TB << "\n";
        // cout << "buffer size :" << buff.size() << "\n";
    }
    void join_the_buffer()
    {
        // cout << tab1_file.eof() << "\n";
        // cout << tab2_file.eof() << "\n";
        int k = 1;
        while (tab1_st < tab1_buff.size() && tab2_st < tab2_buff.size())
        {
            st = en = tab2_st;
            //             // if(tab1_buff[tab1_st]>tab2_buff[st]){
            //     break;//break b/c there won't be same value on common attribute
            // }
            // creating group of duplicates having the same join attribute value
            while (/*!tab2_file.eof() && */ tab2_buff.size() != 1 && en < tab2_buff.size() && tab2_buff[st].fi == tab2_buff[en + 1].fi)
            {
                // cout << "line 244\n";
                en++;
            }
            while (tab1_st < tab1_buff.size() && tab1_buff[tab1_st].fi <= tab2_buff[st].fi)
            {
                //                 if (tab1_buff[tab1_st].fi == tab2_buff[st].fi)
                {
                    string line = tab1_buff[tab1_st].se;

                    for (int i = st; i <= en; i++)
                    {
                        fin_merg_tab << line.substr(0, line.size() - 2);
                        string lin = tab2_buff[i].se;
                        int offset = lin.find_first_of(",");
                        // cout << lin;
                        fin_merg_tab << lin.substr(offset, lin.size());
                        fin_merg_tab << "\n";
                        k++;
                    }
                    // inctreament the table 1 pointer
                    tab1_st++;
                }
                if (tab1_buff.size() == tab1_st)
                {
                    tab1_st = 0;
                    // cout << "\ntable 1 is being filled\n";
                    fill_buff(&tab1_file, tab1_buff);
                }
            }
            // increamenting the second table pointer
            tab2_st = en + 1;
            if (tab2_buff.size() == tab2_st)
            {
                tab2_st = 0;
                // cout << "\ntable 2 is being filled\n";
                // just to check if some duplicate of previous group exist in next block we read from disc block
                tab1_st--;
                fill_buff(&tab2_file, tab2_buff);
            }
        }
        // calculating the tuple in finally mergedjoined table
        n = k;
        // block calculate
        b = ceil((float)k / (float)TB);
    }
};
string Sort_the_table(Relation *table)
{
    // phaseOne of External sort Merge
    table->create_sorted_run();
    // phaseTwo of External sort Merge
    string temp = table->merge_sorted_runs();
    table->dis();
    return temp;
}
int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        cerr << "Usage : " << argv[0] << " <R_path> <S_path>  <M>" << endl;
        exit(-1);
    }

    string R_path, S_path;
    // first input first file path
    R_path = argv[1];
    // second input second file path
    S_path = argv[2];
    // third input  memory block available
    int M = stoi(argv[3]);

    ifstream fd1(R_path);
    ifstream fd2(S_path);

    // check if this file path is valid
    if (!fd1.good() && !fd2.good() && M <= 0)
    {
        cerr << "Invalid input arguments/n";
        cerr << "Usage : " << argv[0] << " <R_path> <S_path> <sort/hash> <M>/n";
        fd1.close();
        fd2.close();
        exit(-1);
    }
    fd1.close();
    fd2.close();

    // creating object of Relation class for R_table
    // R_table is pointer pointing to the Relation class object having all info regarding the r table
    Relation *R_table = new Relation(R_path, M, "R");

    Relation *S_table = new Relation(S_path, M, "S");
    // Relation S_table(S_path, M, "S");

    // sort the table and return the final sorted table path
    cout << "\n\n\n=================================================================================================================================================\n\n";
    cout << "Assumption :\n\n";
    cout << "1. Total Main Memory available in term of Frames is : " << M << "\n";
    cout << "2. Each disc Block can fit into Frame of MM\n";

    cout << "\n\tRelation R sorting started :\n";
    string final_r = Sort_the_table(R_table);
    cout << "Relation R's sorted file path : " << final_r << "\n\n\n";

    cout << "\tRelation S sorting started :\n";
    string final_s = Sort_the_table(S_table);
    cout << "Relation S's sorted file path : " << final_s << "\n\n ";
    cout << "Total cost of sorting two Relation is : " << ALL_cost << "\n\n";

    cout << "Joining the tables : \n";
    sortMergeJoin(final_r, final_s, M);
    cout << "Total cost of SortMergeJoin : " << ALL_cost << "\n";
    cout << "\n\n\n=================================================================================================================================================\n\n";
    return 0;
}