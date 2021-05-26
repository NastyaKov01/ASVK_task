#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include <map>
#include <sstream>
#include <cctype>
#include <cstdlib>
#include <algorithm>
#include <cstring>

using namespace std;

class node
{
    int id;
    string label;
    double lnt, ltd;    ///longitude; latitude
    map<int, double> routes;    ///the key is the id of a node that can be reached from the current node
                                ///the value is the distance between nodes

    map<int, double> delay;     ///the key is the id of a node that can be reached from the current node
                                ///the value is the delay on the edge
public:
    node(int pid = 0 , string st = "", double ln = 0, double lt = 0):
                        id{pid}, label{st}, lnt{ln}, ltd{lt}  {}
    int get_id() { return id; }     ///method that returns the node id
    string get_label () { return label; }   ///method that returns the node label
    double get_longitude () { return lnt; }     ///method that returns the node longitude
    double get_latitude () { return ltd; }      ///method that returns the node latitude
    void set_link (int dest, double dist, double del)   ///method that creates a new link
    {
        routes.insert(pair<int, double> (dest, dist));
        delay.insert(pair<int, double> (dest, del));
    }
    int links () { return routes.size(); }      ///method that returns the number of links
    double get_dist (int target)    ///method that returns the distance between the current node and the target node
    {
        return routes[target];
    }

    double get_del (int target)     ///method that returns the delay on the edge between
    {                               ///the current node and the target node
        return delay[target];
    }
    ///couple of methods that give access to the distance map
    auto route_beg() { return routes.begin(); }
    auto route_end() { return routes.end(); }
    ///couple of methods that give access to the delay map
    auto del_beg() { return delay.begin(); }
    auto del_end() { return delay.end(); }
    void rem (int key)  ///method that removes the link
    {
        routes.erase(key);
        delay.erase(key);
    }
};

///function that calculates the distance between two nodes
///the calculation formula:
///d = 2 * R arcsin(sqrt(sin^2((lt2 - lt1) / 2)  +  cos(lt1) * cos(lt2) * sin^2((lng2 - lng1) / 2)))
///R - radius of the Earth
///Such calculation takes into account the shape of the Earth
///and uses different trigonometry.
///Considering that Earth is not an ideal sphere, the level of the result error is 0.5%.
double distance (double lt1, double lng1, double lt2, double lng2)
{
    constexpr double R = 6371;
    lng1 = lng1 * M_PI / 180;
    lt1 = lt1 * M_PI / 180;
    lng2 = lng2 * M_PI / 180;
    lt2 = lt2 * M_PI / 180;
    double res = cos(lt1) * cos(lt2);
    res *= sin((lng2 - lng1) / 2) * sin((lng2 - lng1) / 2);
    res += sin((lt2 - lt1) / 2) * sin((lt2 - lt1) / 2);
    res = asin(sqrt(res));
    res *= 2 * R;
    return res;
}

///implementation of parsing
void parsing(vector <node> & data, string name)
{
    ifstream fin;
    fin.open(name); ///opening of the input file
    ///if the file can't be opened
    ///the corresponding message is displayed
    if (!fin.is_open()) {
        cout << "Cannot open the file " << name << endl;
        return;
    }
    string str, word;
    int id;
    string lab;
    double lng, lt;
    while (getline(fin, str)) {     ///line-by-line reading
        vector<string> words;
        istringstream sin(str);
        while (getline(sin, word, ' ')) {   ///splitting into words
            words.push_back(word);
        }
        auto k = words.begin();
        while (!isalpha((*k)[0])) {     ///removal of empty words
            words.erase(k);
            if ((*k) == "]") {
                break;
            }
        }
        if (words[0] == "node") {   ///analysis of information about the node
            for (int j = 0; j < 6; ++j) {
                getline(fin, str);
                vector<string> words;
                istringstream sin(str);
                while (getline(sin, word, ' ')) {   ///splitting into words
                    words.push_back(word);
                }
                k = words.begin();
                while (!isalpha((*k)[0])) {     ///removal of empty words
                    words.erase(k);
                    if ((*k) == "]") {
                        break;
                    }
                }
                stringstream ss;
                int z = words.size();
                switch (j) {
                case 0:     ///getting node id
                    ss << words[1];
                    ss >> id;
                    break;
                case 1:     ///getting node label
                    lab = words[1];
                    if (z > 2) {
                        for (int i = 2; i < z; ++i) {
                            lab += " " + words[i];
                        }
                    }
                    lab.erase(lab.begin()); ///removal of quotation marks
                    lab.erase(lab.end() - 1);
                    break;
                case 3:     ///getting node longitude
                    ss << words[1];
                    ss >> lng;
                    break;
                case 5:     ///getting node latitude
                    ss << words[1];
                    ss >> lt;
                    break;
                default:
                    break;
                }
            }
            node next(id, lab, lng, lt);    ///creation of a new node
            data.push_back(next);
        } else if (words[0] == "edge") {    ///analysis of information about links
            int source, target;
            constexpr double FIBER = 4.8;   ///delay per 1 km
            double dist, delay;
            for (int i = 0; i < 2; ++i) {
                getline(fin, str);
                vector<string> words;
                istringstream sin(str);
                while (getline(sin, word, ' ')) {   ///splitting into words
                    words.push_back(word);
                }
                k = words.begin();
                while (!isalpha((*k)[0])) {     ///removal of empty words
                    words.erase(k);
                    if ((*k) == "]") {
                        break;
                    }
                }
                stringstream ss;
                ss << words[1];
                if (i == 0) {
                    ss >> source;   ///starting point
                } else {
                    ss >> target;   ///endpoint
                }
            }
            ///calculation of distance between starting point and endpoint
            dist = distance(data[source].get_latitude(), data[source].get_longitude(),
                                data[target].get_latitude(), data[target].get_longitude());
            ///calculation of edge delay
            delay = dist * FIBER;
            ///adding new link to the topology
            data[source].set_link(target, dist, delay);
        }
    }
    fin.close();    ///closing of the input file
}

///function that creates the output file with topology
void print_topology (vector<node> data, string name)
{
    istringstream sin(name);
    getline(sin, name, '.');    ///creation of new file name
    name += "_topo.csv";
    ofstream fout;
    fout.open(name);    ///opening of the topology file
    ///if the file can't be opened
    ///the corresponding message is displayed
    if (!fout.is_open()) {
        cout << "Cannot open the file " << name << endl;
        return;
    }
    ///printing to the file
    fout << "Node 1 (id)" << ";" << "Node 1 (label)" << ";" << "Node 1 (longitude)" << ";" << "Node 1 (latitude)" << ";" ;
    fout << "Node 2 (id)" << ";" << "Node 2 (label)" << ";" << "Node 2 (longitude)" << ";" << "Node 2 (latitude)" << ";" ;
    fout << "Distance (km)" << ";" << "Delay (mks)" << endl;
    for (auto k: data) {
        fout << k.get_id() << ";" << k.get_label() << ";" << k.get_longitude() << ";" << k.get_latitude() << ";" ;
        if (k.links() == 0) {
            fout << endl;
        }
        auto start = k.route_beg();
        auto lend = k.route_end();
        for (auto i = start; i != lend; ++i) {
            int pid = i->first;
            if (i != start) {
                fout << k.get_id() << ";" << ";" << ";" << ";" ;
            }
            fout << pid << ";" << data[pid].get_label() << ";" << data[pid].get_longitude() << ";" << data[pid].get_latitude() <<";" ;
            fout << k.get_dist(pid) << ";" << k.get_del(pid) << endl;
        }
    }
    fout.close();   ///closing of the topology file
}

constexpr double INF = 40075 * 10000;   ///"infinity" value

///the realisation of floyd_warshall algorithm
void floyd_warshall (vector <node> data, int n, double d[], double p[])
{
    ///the filling of the matrixes
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j) {
                d[i * n + j] = 0;
            } else {
               d[i * n + j] = INF;
            }
            p[i * n + j] = -1;
        }
    }
    ///the filling of edge delays
    for (auto m: data) {
        int i = m.get_id();
        auto beg = m.del_beg();
        auto en = m.del_end();
        for (; beg != en; ++beg) {
            int j = beg->first;
            d[i * n + j] = m.get_del(j);
        }
    }
    ///building of the shortest paths
    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (d[i * n + k] < INF && d[k * n + j] < INF && (d[i * n + k] + d[k * n + j]) < d[i * n + j])
                {
                    d[i * n + j] = d[i * n + k] + d[k * n + j];
                    if (p[k * n + j] == -1) {
                        p[i * n + j] = k;
                    } else {
                        p[i * n + j] = p[k * n + j];
                    }
                 }
            }
        }
    }
}

///the creation of a reserve route between node i and node j
double reserve_route (vector <node> dat, vector<int> &path, int i, int j, int n)
{
    double d[n][n], p[n][n];
    floyd_warshall(dat, n, &d[0][0], &p[0][0]);
    if (d[i][j] != INF) {
        int k = j;
        while (k != -1) {
            path.push_back(k);
            k = p[i][k];
        }
        path.push_back(i);
        reverse(path.begin(), path.end());
        return d[i][j]; ///the function return the value of path delay
    } else {
        return 0;   ///if a reserve path can't be built the function returns 0
    }
}

///printing the route to the fres stream
template <typename T>
void print_route (T &fres, vector <int> v) {
    fres << "[" ;
    for (auto r = v.begin(); r != v.end(); ++r) {
        fres << *r;
        if (r + 1 != v.end()) {
            fres << ", ";
        }
    }
    fres << "]" ;
}

///the function constructs shortest paths
void find_routes (vector <node> data, string name, bool rf, int st, int fin)
{
    int n = data.size();
    double d[n][n], p[n][n];
    floyd_warshall(data, n, &d[0][0], &p[0][0]);    ///main paths are built
    ofstream fres;
    istringstream sin(name);
    getline(sin, name, '.');     ///creation of new file name
    name += "_routes.csv";
    fres.open(name);    ///opening of the output file
    ///if the file can't be opened
    ///the corresponding message is displayed
    if (!fres.is_open()) {
        cout << "Cannot open the file " << name << endl;
        return;
    }
    ///printing to the file
    fres << "Node 1 (id)" << ";" << "Node 2 (id)" << ";" << "Path type" << ";" ;
    fres << "Path" << ";" << "Delay (mks)" << endl;
    int cnt = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j) {
                continue;
            }
            ///if the path exists it is printed to the file
            if (d[i][j] != INF) {
                fres << i << ";" << j << ";" << "main" << ";" ;
                vector<int> mroute;
                int k = j;
                while (k != -1) {
                    mroute.push_back(k);
                    k = p[i][k];
                }
                mroute.push_back(i);
                reverse(mroute.begin(), mroute.end());
                print_route(fres, mroute);
                fres << ";" << d[i][j] << endl;
                if (rf) {   ///if the reserve paths are necessary
                    vector <int> resroute;
                    vector <int> tmp;
                    fres << i << ";" << j << ";" << "reserv" << ";" ;
                    vector<node> dat(data);
                    ///removal of the nodes used in the main path
                    for (auto k = mroute.begin() + 1; k != mroute.end() - 1; ++k) {
                        for (auto &it: dat) {
                            it.rem(*k);
                        }
                    }
                    double dist = reserve_route (dat, resroute, i, j, n);
                    double mindist = INF;
                    if (dist != 0 && dist!= d[i][j]) {  ///if the path is built, it is printed to the file
                        print_route(fres, resroute);
                        fres << ";" << dist << endl;
                    } else if (dist == d[i][j]) {   ///if the reserve route is just the edge,
                        resroute.clear();   ///we consider there is no reserve path
                        fres << "no" << endl;
                    } else {    ///trying to build the minimum path with one intersection node
                        for (auto k = mroute.begin() + 1; k != mroute.end() - 1; ++k) {
                            dat = data; ///the copy of the topology
                            ///removal of the nodes used in the main path except one
                            ///(the edges used in the main path are also removed)
                            for (auto t = mroute.begin() + 1; t != mroute.end() - 1; ++t) {
                                for (auto &it: dat) {
                                    if ((*k != *t) || (*k == *t && it.get_id() == *(k - 1))) {
                                        it.rem(*t);
                                    }
                                    if (*k == it.get_id() && k == mroute.end() - 2) {
                                        it.rem(*(mroute.end() - 1));
                                    }
                                }
                            }
                            dist = reserve_route (dat, tmp, i, j, n);
                            if (dist != 0 && dist < mindist) {  ///search for the minimum possible path
                                mindist = dist;
                                resroute = tmp;
                            }
                        }
                        if (mindist != INF) {   ///if the path is built, it is printed to the file
                            print_route(fres, resroute);
                            dist = mindist;
                            fres << ";" << mindist << endl;
                        } else {
                            resroute.clear();
                            fres << "no" << endl;
                        }
                    }
                    ///displaying of required paths(path) if they exist(it exists)
                    if (i == st && j == fin || i == st && fin == -1 || st == -1 && j == fin) {
                        cout << i << " " << j << " " << "main" << " " ;
                        print_route(cout, mroute);
                        cout << "  " << d[i][j] << endl;
                        if (resroute.size()) {
                            cout << i << " " << j << " " << "reserv" << " " ;
                            print_route(cout, resroute);
                            cout << "  " << dist << endl;
                        } else {
                            cout << i << " " << j << " " << "reserv" << " " ;
                            cout << "no" << endl;
                        }
                    }
                } else {    ///displaying of required paths(path) if they exist(it exists) without reserve routes
                    if (i == st && j == fin || i == st && fin == -1 || st == -1 && j == fin) {
                        cout << i << " " << j << " " << "main" << " " ;
                        print_route(cout, mroute);
                        cout << "  " << d[i][j] << endl;
                    }
                }
            } else {    ///no required route
                if (i == st && j == fin) {
                    cout << "no such route" << endl;
                }
                if (i == st || j == fin) {
                    ++cnt;
                }
            }
        }
    }
    if (cnt == n - 1) { ///no required routes
        cout << "no such routes" << endl;
    }
    if (st >= n || fin >= n) {  ///invalid node numbers
        cout << "The node number exceeds the maximum possible number" << endl;
    }
    fres.close();   ///closing of the output file
}

int main(int argc, char* argv[])
{
    bool rf = 0;    ///flag that shows the presence of -r option
    int st = -1, fin = -1, error = 0;   ///starting point; endpoint; error counter
    string name;    ///name of the input file
    for (int i = 1; i < argc; ++i) {    ///analysis of input options
        if (!strcmp(argv[i], "-t")) {
            ++i;
            name = argv[i];
        } else if (!strcmp(argv[i], "-r")) {
            rf = 1;
        } else if (!strcmp(argv[i], "-s")) {
            ++i;
            char *p;
            int j = 0;
            while (argv[i][j]) {    ///check of the node number type
                if (argv[i][j] == '.') {    ///double/float number is an error
                    ++error;
                    break;
                }
                ++j;
            }
            if (!error) {
                st = strtol(argv[i], &p, 10);
                if (st < 0) {   ///negative number is an error
                    ++error;
                }
            }
        } else if (!strcmp(argv[i], "-d")) {
            ++i;
            char *p;
            int j = 0;
            while (argv[i][j]) {    ///check of the node number type
                if (argv[i][j] == '.') {    ///double/float number is an error
                    ++error;
                    break;
                }
                j++;
            }
            if (!error) {
                fin = strtol(argv[i], &p, 10);
                if (fin < 0) {      ///negative number is an error
                    ++error;
                }
            }
        } else {    ///other input options are invalid
            ++error;
            break;
        }
    }
    if (error) {    ///in case of errors the corresponding message is displayed
        cout << "Invalid input" << endl;
        cout << "Try again" << endl;
        cout << "You can use the following options:" << endl;
        cout << "-t <name_of_the_topology_file> (to analyse the .gml file)" << endl;
        cout << "-r (to build the reserve routes)" << endl;
        cout << "-s <int_value> (to set the starting point)" << endl;
        cout << "-d <int_value> (to set the endpoint)" << endl;
        return 0;
    }
    vector <node> data;     ///vector of nodes of the graph
    parsing(data, name);    ///implementation of parsing
    print_topology(data, name);     ///the creation of topology file
    find_routes(data, name, rf, st, fin);   ///the construction of paths between nodes
    return 0;
}
