#include <iostream>
#include <unordered_map>
#include <vector>
#include <climits>
#include <time.h>
using namespace std;

#define pint pair<int,int>
#define vec2pint vector<vector<pint>>
#define umap unordered_map<int, int>
#define vec2int vector<vector<int>>
#define parent(x) ((x-1)/2)
#define leftchild(x) (2*x+1)
#define rightchild(x) (2*x+2)
//Constants for the Problem
int N = 5000; // Number of vertices in the network
int degree = 8; // Degree of each vertex in type-1 graph
int per100 = 20; //20 Percent
int max_weight = N/50;
int min_weight = N/500;

//Display the Graph
void Display(vec2pint& adjlist)
{
    int count=0;
    for(auto list:adjlist)
    {
        cout<<"Node-"<<++count<<" : ";
        for(auto ele:list)
            cout<<ele.first+1<<":"<<ele.second<<"  ";
        cout<<endl;
    }
}

//Creating a cyclic graph
void CreateCycle(vec2pint& adjlist, int n, vec2int& maplist)
{
    vector<int> map(n, 0);
    int previousindex = 0;
    for(int i=0;i<n-1;i++)
    {
        map[previousindex] = 1;
        int startindex = rand()%n;
        while(map[startindex] == 1)
            startindex = (startindex+1)%n;
        int wt = min_weight + rand()%(max_weight-min_weight);
        adjlist[previousindex].push_back(make_pair(startindex,wt));
        adjlist[startindex].push_back(make_pair(previousindex,wt));
        maplist[previousindex][startindex] = 1;
        maplist[startindex][previousindex] = 1;
        previousindex = startindex;
    }
    int wt = min_weight + rand()%(max_weight-min_weight);
    adjlist[previousindex].push_back(make_pair(0,wt));
    adjlist[0].push_back(make_pair(previousindex,wt));
    maplist[previousindex][0] = 1;
    maplist[0][previousindex] = 1;
    return;
}

//Type-1 Graph creation
void CreateType1Graph(vec2pint& adjlist, int n, int degree)
{
    vec2int maplist(n, vector<int>(n, 0));
    CreateCycle(adjlist, n, maplist);
    for(int l=0;l<n*degree/2-n;l++)
    {
        int rand1=0, rand2=0;
        do{
            rand1 = rand()%n;
            rand2 = rand()%n;
        }while(rand1==rand2 || maplist[rand1][rand2]==1);
        maplist[rand1][rand2] = 1;
        maplist[rand2][rand1] = 1;
        int wt = min_weight + rand()%(max_weight-min_weight);
        adjlist[rand1].push_back(make_pair(rand2,wt));
        adjlist[rand2].push_back(make_pair(rand1,wt));
    }
    return;
}

//Type-2 Graph creation
void CreateType2Graph(vec2pint& adjlist, int n, int per100)
{
    vec2int maplist(n, vector<int>(n, 0));
    CreateCycle(adjlist, n, maplist);
    for(int i=0;i<n;i++)
    {
        for(int j=0;j<n;j++)
        {
            int randval = rand()%100;
            if (maplist[i][j]==0 && i!=j && randval<per100/2)
            {
                int wt = min_weight + rand()%(max_weight-min_weight);
                adjlist[i].push_back(make_pair(j,wt));
                adjlist[j].push_back(make_pair(i,wt));
                maplist[i][j] = 1;
                maplist[j][i] = 1;
            }
        }
    }
    return;
}

//Heap functions
void Insert(vector<pint>& maxheap, int& heaplen, vector<int>& pos, pint input)
{
    maxheap[heaplen] = input;
    heaplen++;
    pos[input.first] = heaplen-1;
    int i = heaplen-1;
    while (i != 0 && maxheap[parent(i)].second < maxheap[i].second)
    {
        pos[maxheap[i].first] = parent(i);
        pos[maxheap[parent(i)].first] = i;
        swap(maxheap[i], maxheap[parent(i)]);
        i = parent(i);
    }
    return;
}

void MaxHeapify(vector<pint>& maxheap, int& heaplen, vector<int>& pos, int i)
{
    int l = leftchild(i);
    int r = rightchild(i);
    int largest = i;
    if (l < heaplen && maxheap[l].second > maxheap[i].second)
        largest = l;
    if (r < heaplen && maxheap[r].second > maxheap[largest].second)
        largest = r;
    if (largest != i)
    {
        pos[maxheap[i].first] = largest;
        pos[maxheap[largest].first] = i;
        swap(maxheap[i], maxheap[largest]);
        MaxHeapify(maxheap, heaplen, pos, largest);
    }
}

pint extractMax(vector<pint>& maxheap, int& heaplen, vector<int>& pos)
{
    if (heaplen == 1)
    {
        heaplen--;
        return maxheap[0];
    }
    pint ret = maxheap[0];
    pos[maxheap[0].first] = -1;
    maxheap[0] = maxheap[heaplen-1];
    pos[maxheap[0].first] = 0;
    heaplen--;
    MaxHeapify(maxheap, heaplen, pos, 0);
    return ret;
}

void Delete(vector<pint>& maxheap, int& heaplen, vector<int>& pos, int index)
{
    int i = index;
    maxheap[i].second = INT_MAX;
    while (i != 0 && maxheap[parent(i)].second < maxheap[i].second)
    {
        pos[maxheap[i].first] = parent(i);
        pos[maxheap[parent(i)].first] = i;
        swap(maxheap[i], maxheap[parent(i)]);
        i = parent(i);
    }
    extractMax(maxheap, heaplen, pos);
}

pint getmax(vector<pint>& maxheap, int& heaplen, vector<int>& pos)
{
    return maxheap[0];
}

int Dijkstra(vec2pint& adjlist, int& source, int& destination, vector<int>& path)
{
    int n = adjlist.size();
    vector<int> dad(n, 0), BW(n, INT_MIN), fringe, status(n, 0); //Unseen-0, Fringe-1 and In-tree-2
    status[source] = 2;
    for(auto ele:adjlist[source])
    {
        status[ele.first] = 1;
        fringe.push_back(ele.first);
        dad[ele.first] = source;
        BW[ele.first] = ele.second;
    }
    while(status[destination] != 2)
    {
        //Pick the fringe with the largest BW
        int fringeindex = -1, fringeBW = INT_MIN;
        for(int i=0;i<fringe.size();i++)
        {
            if (fringeBW < BW[fringe[i]])
            {
                fringeindex = i;
                fringeBW = BW[fringe[i]];
            }
        }
        int fringenode = fringe[fringeindex];
        fringe.erase(fringe.begin()+fringeindex);
        status[fringenode] = 2;
        for(auto ele:adjlist[fringenode])
        {   
            if (status[ele.first] == 0)
            {
                status[ele.first] = 1;
                fringe.push_back(ele.first);
                dad[ele.first] = fringenode;
                BW[ele.first] = min(BW[fringenode], ele.second);
            }
            else if (status[ele.first] == 1 && (BW[ele.first] < min(BW[fringenode],ele.second)))
            {
                BW[ele.first] = min(BW[fringenode], ele.second);
                dad[ele.first] = fringenode;
            }
        }
    }
    int currnode = destination;
    path.push_back(destination);
    while(currnode != source)
    {
        path.insert(path.begin(), dad[currnode]);
        currnode = dad[currnode];
    }
    return BW[destination];
}

int Dijkstra_1(vec2pint& adjlist, int& source, int& destination, vector<int>& path)
{
    int n = adjlist.size();
    vector<int> dad(n, 0), BW(n, INT_MIN), status(n, 0); //Unseen-0, Fringe-1 and In-tree-2
    status[source] = 2;
    for(auto ele:adjlist[source])
    {
        status[ele.first] = 1;
        dad[ele.first] = source;
        BW[ele.first] = ele.second;
    }
    while(status[destination] != 2)
    {
        //Pick the fringe with the largest BW
        int fringeindex = -1, fringeBW = INT_MIN;
        for(int i=0;i<n;i++)
        {
            if (status[i]==1 && fringeBW < BW[i])
            {
                fringeindex = i;
                fringeBW = BW[i];
            }
        }
        status[fringeindex] = 2;
        for(auto ele:adjlist[fringeindex])
        {   
            if (status[ele.first] == 0)
            {
                status[ele.first] = 1;
                dad[ele.first] = fringeindex;
                BW[ele.first] = min(BW[fringeindex], ele.second);
            }
            else if (status[ele.first] == 1 && (BW[ele.first] < min(BW[fringeindex],ele.second)))
            {
                BW[ele.first] = min(BW[fringeindex], ele.second);
                dad[ele.first] = fringeindex;
            }
        }
    }
    int currnode = destination;
    path.push_back(destination);
    while(currnode != source)
    {
        path.insert(path.begin(), dad[currnode]);
        currnode = dad[currnode];
    }
    return BW[destination];
}

int Dijkstra_Heap(vec2pint& adjlist, int& source, int& destination, vector<int>& path)
{
    int n = adjlist.size();
    vector<int> dad(n, 0), BW(n, INT_MIN), status(n, 0); //Unseen-0, Fringe-1 and In-tree-2
    vector<pint> fringe(n, make_pair(-1,INT_MIN)); int heaplen = 0;
    vector<int> pos(n, -1);
    status[source] = 2;
    for(auto ele:adjlist[source])
    {
        status[ele.first] = 1;
        Insert(fringe, heaplen, pos, ele);
        dad[ele.first] = source;
        BW[ele.first] = ele.second;
    }
    while(status[destination] != 2)
    {
        pint fringemax = extractMax(fringe, heaplen, pos);
        int fringenode = fringemax.first;
        status[fringenode] = 2;
        for(auto ele:adjlist[fringenode])
        {
            if (status[ele.first] == 0)
            {
                status[ele.first] = 1;
                dad[ele.first] = fringenode;
                BW[ele.first] = min(BW[fringenode], ele.second);
                Insert(fringe, heaplen, pos, make_pair(ele.first, BW[ele.first]));
            }
            else if (status[ele.first] == 1 && (BW[ele.first] < min(BW[fringenode],ele.second)))
            {
                Delete(fringe, heaplen, pos, pos[ele.first]);
                BW[ele.first] = min(BW[fringenode], ele.second);
                dad[ele.first] = fringenode;
                Insert(fringe, heaplen, pos, make_pair(ele.first, BW[ele.first]));
            }
        }
    }
    int currnode = destination;
    path.push_back(destination);
    while(currnode != source)
    {
        path.insert(path.begin(), dad[currnode]);
        currnode = dad[currnode];
    }
    return BW[destination];
}

void heapify(vector<pair<pint, int>>& arr, int n, int i)
{
    int largest = i;
    int l = 2*i + 1;
    int r = 2*i + 2;
 
    if (l < n && arr[l].second > arr[largest].second)
        largest = l;
 
    if (r < n && arr[r].second > arr[largest].second)
        largest = r;
 
    if (largest != i)
    {
        swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

void union12(int r1, int r2, vector<int>& parent, vector<int>& rank)
{
    if (rank[r1]>rank[r2])
        parent[r2] = r1;
    else if (rank[r2]>rank[r1])
        parent[r1] = r2;
    else
    {
        parent[r1] = r2;
        rank[r1]++;
    }
}

int findpc(int v, vector<int>& parent)
{
    int root = v, w = v, temp = 0;
    while(parent[root] != root)
        root = parent[root];
    while(parent[w] != w)
    {
        temp = w;
        w = parent[w];
        parent[temp] = root;
    }
    return w;
}

void Kruskal_MaxBW(vec2pint& adj, vec2pint& retadj)
{
    vector<pair<pint, int>> arr;
    for(int i=0;i<adj.size();i++)
    {
        for(int j=0;j<adj[i].size();j++)
        {
            if (i<adj[i][j].first)
            {
                arr.push_back(make_pair(make_pair(i, adj[i][j].first), adj[i][j].second));
            }
        }
    }
    int n = arr.size();
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);
    
    //Make Set
    int num = adj.size();
    vector<int> parent, rank(num, 0);
    for(int i=0;i<num;i++)
        parent.push_back(i);
    for(int i=0;i<adj.size();i++)
        retadj.push_back(vector<pint>());
    
    for (int i=n-1; i>=0; i--)
    {
        int v1 = arr[0].first.first;
        int v2 = arr[0].first.second;
        int p1 = findpc(v1, parent);
        int p2 = findpc(v2, parent);
        
        if (p1!=p2)
        {
            union12(p1, p2, parent, rank);
            retadj[v1].push_back(make_pair(v2,arr[0].second));
            retadj[v2].push_back(make_pair(v1,arr[0].second));
        }
        
        swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
}

int DFS(vec2pint& adj, int start, int destination, vector<int>& path, vector<int>& visit)
{
    if (start == destination)
    {
        path.push_back(destination);
        return INT_MAX;
    }
    visit[start] = 1;
    for(int i=0;i<adj[start].size();i++)
    {
        if (visit[adj[start][i].first]==0)
        {
            int bw = DFS(adj, adj[start][i].first, destination, path, visit);
            if (bw != -1)
            {
                path.insert(path.begin(), adj[start][i].first);
                return min(bw, adj[start][i].second);
            }
        }
    }
    return -1;
}

int main()
{
    srand ( time(NULL) );
    clock_t t1;
    
    vec2pint adjlist1(N, vector<pint>()), adjlist2(N, vector<pint>());
    CreateType1Graph(adjlist1, N, degree);
    CreateType2Graph(adjlist2, N, per100);
    //Display(adjlist);
    vec2pint adjlist_kruskal1, adjlist_kruskal2;
    
    for(int l=0;l<1;l++)
    {
        int source = rand()%N, destination = rand()%N;
        cout<<"Source is "<<source<<" and Destination is "<<destination<<endl;
        vector<int> path1,path2, path3, path4, path5, path6;
        t1 = clock();
        int maxBW1 = Dijkstra_1(adjlist1, source, destination, path1);
        t1 = clock()-t1;
        cout<<"Alg-1 for Type-1 Graph, BW is "<<maxBW1<<" and time taken is "<<((float)t1)/(CLOCKS_PER_SEC/1000.0)<<" ms"<<endl;
        /*for(auto p:path1)
            cout<<p<<" ";
        cout<<endl;*/
        t1 = clock();
        maxBW1 = Dijkstra_1(adjlist2, source, destination, path2);
        t1 = clock()-t1;
        cout<<"Alg-1 for Type-2 Graph, BW is "<<maxBW1<<" and time taken is "<<((float)t1)/(CLOCKS_PER_SEC/1000.0)<<" ms"<<endl;
        /*for(auto p:path2)
            cout<<p<<" ";
        cout<<endl;*/
        
        t1 = clock();
        int maxBW2 = Dijkstra_Heap(adjlist1, source, destination, path3);
        t1 = clock() - t1;
        cout<<"Alg-2 for Type-1 Graph, BW is "<<maxBW2<<" and time taken is "<<((float)t1)/(CLOCKS_PER_SEC/1000.0)<<" ms"<<endl;
        /*for(auto p:path3)
            cout<<p<<" ";
        cout<<endl;*/
        t1 = clock();
        maxBW1 = Dijkstra_Heap(adjlist2, source, destination, path4);
        t1 = clock()-t1;
        cout<<"Alg-2 for Type-2 Graph, BW is "<<maxBW1<<" and time taken is "<<((float)t1)/(CLOCKS_PER_SEC/1000.0)<<" ms"<<endl;
        /*for(auto p:path4)
            cout<<p<<" ";
        cout<<endl;*/
        
        if (l==0)
        {
            t1 = clock();
            Kruskal_MaxBW(adjlist1, adjlist_kruskal1);
            t1 = clock() - t1;
            cout<<"Alg-3: Running time for Kruskal's on Type-1 Graph is "<<((float)t1)/(CLOCKS_PER_SEC/1000.0)<<" ms"<<endl;
        }
        vector<int> visit1(adjlist_kruskal1.size(), 0);
        t1 = clock();
        int maxBW3 = DFS(adjlist_kruskal1, source, destination, path5, visit1);
        path5.insert(path5.begin(), source);
        t1 = clock() - t1;
        cout<<"Alg-3: Running time for DFS on Type-1 Graph, BW is "<<maxBW3<<" and time taken is "<<((float)t1)/(CLOCKS_PER_SEC/1000.0)<<" ms"<<endl;
        /*for(auto p:path5)
            cout<<p<<" ";
        cout<<endl;*/
        if (l==0)
        {
            t1 = clock();
            Kruskal_MaxBW(adjlist2, adjlist_kruskal2);
            t1 = clock() - t1;
            cout<<"Alg-3: Running time for Kruskal's on Type-2 Graph is "<<((float)t1)/(CLOCKS_PER_SEC/1000.0)<<" ms"<<endl;
        }
        vector<int> visit2(adjlist_kruskal2.size(), 0);
        t1 = clock();
        maxBW3 = DFS(adjlist_kruskal2, source, destination, path6, visit2);
        path6.insert(path6.begin(), source);
        t1 = clock() - t1;
        cout<<"Alg-3: Running time for DFS on Type-2 Graph, BW is "<<maxBW3<<" and time taken is "<<((float)t1)/(CLOCKS_PER_SEC/1000.0)<<" ms"<<endl;
        /*for(auto p:path6)
            cout<<p<<" ";
        cout<<endl;*/
        cout<<endl;
    }
    return 0;
}