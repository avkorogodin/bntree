#include <algorithm>
#include <deque>
#include <string>
#include <vector>
#include <utility>
#include <chrono>
#include <iostream>
#include <fstream>
#include <map>

#include "bntree.hpp"

using namespace std;
using namespace chrono;

using write_sequence = vector<string>;

using test_pair = pair<uint64_t, string>;
using modify_sequence = vector<test_pair>;
using read_sequence = vector<test_pair>;

ifstream& operator >> (ifstream& _is, test_pair& _value)
{
    _is >> _value.first;
    _is >> _value.second;

    return _is;
}

template <typename S>
S get_sequence(const string& _file_name)
{
    ifstream infile(_file_name);
    S result;

    typename S::value_type item;

    while (infile >> item)
    {
        result.emplace_back(move(item));
    }

    return result;
}

extern size_t dpt, maxdpt, countdpt, totaldpt;  //  измеритель глубины

int main()
{
    uint64_t progress = 0;
    double percent = 0;
    time_point<system_clock> time;
    nanoseconds total_time(0);
    
    write_sequence write =  get_sequence<write_sequence>("./test/write.txt");
    modify_sequence modify =  get_sequence<modify_sequence>("./test/modify.txt");
    read_sequence read = get_sequence<read_sequence>("./test/read.txt");

    bntree st;
    //cout << "count: " << write.size() << endl;
    
    progress = 0;
    percent = write.size() / 100.;
    
    for (const string &item : write)
    {
        time = system_clock::now();
        total_time += system_clock::now() - time;
        
        st.insert(item);
       
        if (++progress % int(10 * percent) == 0)
        {
            cout << "time: " << duration_cast<milliseconds>(total_time).count()
                 << "ms progress: " << progress << " / " << write.size() << "\n";
        }
    }
    
    cout << "insert have done" << endl;
    
    cout << "\ncount: " << st.size() << " add end" << endl;
    //st.print();
    //return -1;
    
    progress = 0;
    percent = modify.size() / 100.;

    modify_sequence::const_iterator mitr = modify.begin();
    read_sequence::const_iterator ritr = read.begin();
    
    for (int i = 0; mitr != modify.end() && ritr != read.end(); ++mitr, ++ritr, i++)
    {
        time = system_clock::now();

        st.erase(mitr->first);
        st.insert(mitr->second);
        
        const char *d = st.get(ritr->first);
        
        total_time += system_clock::now() - time;
        if ( strcmp( ritr->second.data(), d) != 0)
        {
            cout << "test failed " << i << endl;
            return 1;
        }

        if (++progress % int(10 * percent) == 0)
        {
            cout << "time: " << duration_cast<milliseconds>(total_time).count()
                 << "ms progress: " << progress << " / " << modify.size() << "\n";
        }
    }

    return 0;
}

/*
int main() {
    
    bntree st;
    
    //cout << st.cpl2(0) << endl;
    //cout << st.cpl2(1) << endl;
    //cout << st.cpl2(2) << endl;
    //cout << st.cpl2(3) << endl;
    //cout << st.cpl2(4) << endl;
    //cout << st.cpl2(7) << endl;
    //cout << st.cpl2(8) << endl;
    //cout << st.cpl2(9) << endl;
    //long a = st.cpl2(2);
    //cout << st.ilog2(8) << endl;
    //cout << st.cpl2(4294967295) << endl;
    //cout << st.cpl2(4294967297) << endl;
    
    //return -1;
    
    //st.insert("50", "ssss");
    
    //st.insert("25");
    
    //st.insert("15");
    //st.insert("17");
    //st.insert("11");
    
    //st.insert("35");
    //st.insert("30");
    //st.insert("40");
    
    //st.insert("75");
    
    //st.insert("65");
    //st.insert("60");
    //st.insert("70");
    st.print();
    
    //st.insert("90");
    //st.print();
    //st.insert("80");
    //st.insert("95");    
    
    st.insert("8");
    st.insert("4");
    st.insert("30");
    st.insert("2");
    st.insert("6");
    st.insert("10");
    st.insert("36");
    st.insert("9");
    st.insert("12");
    st.insert("38");
    st.insert("13");
    st.insert("14");
    st.insert("15");
            
    cout << "size(): " << st.size() << endl;
    
    //for (int i = 0; i < st.size(); i++ )
    //    st.get(i);
    
    st.print();
    
    return -1;
    
    puts("\nerase");
    
    //st.erase(1);
    
    puts("\nerase ok");
    
    st.insert("50", "bbbb");
    
    st.print();
    
    //for (int i = 0; i < st.size(); i++ )
    //    st.get(i);
    
    puts("###############");
    
    //std::string str = "12345678";
    //cout << str.substr(0, 2) << endl;
    
    return -1;
    
    st.insert("888");
    st.insert("8876");
    st.insert("865");
    st.insert("854");
    st.print();
    st.insert("123");
    st.insert("1234");
    st.insert("1235");
    st.insert("132");
    st.print();
    
    cout << "0get " << st.get(0)->key << " ." << endl;
    cout << "1get " << st.get(1)->key << " ." << endl;
    cout << "2get " << st.get(2)->key << " ." << endl;
    cout << "3get " << st.get(3)->key << " ." << endl;
    cout << "4get " << st.get(4)->key << " ." << endl;
    
    //st.erase(4);
    //st.print();
    //st.get(5);
    
    //cout << "get " << st.get(2) << " ." << endl;
    //cout << "get " << st.get(2) << " ." << endl;
    //cout << "get " << st.get(3) << " ." << endl;
    //cout << "get " << st.get(4) << " ." << endl;
    //cout << "get " << st.get(5) << " ." << endl;
    puts("end");
    return -1;
    
    st.insert("888");
    st.print();
    st.insert("333");
    st.print();
    st.insert("999");
    st.print();
    st.insert("111");
    st.print();
    st.insert("222");
    st.print();
    st.insert("555");
    st.print();
    st.insert("444");
    st.print();
    st.insert("444444");
    st.print();
    st.insert("777");
    st.print();
    st.insert("000");
    st.print();
    
    
    return -1;
}
*/
