# Navigation-App
Functional equivalent of a maps application (i.e. Google Maps, Yandex Maps etc.) that utilizes custom implementations of graphs and hash-tables. The project highly makes use of OOP and abstraction.

Description of the implementations and their functions are given below:


--**Graph Implementation**
The Graph data data structure that is used in this project, follows the adjacency list
style of implementation with slight differences. Graph Data Structure has non-negative weights
for its edges. Additionally, edges on this data structure is bi-directional meaning that graph is
not a directional graph. Additional functionality consists of masking an edge in the graph and
finding multiple “shortest” paths using masking functions. 

Instead of linked list, graph holds its data (in this case edges and vertices) in dynamic arrays. For
dynamic array implementation, class utilizes std::vector<T> class from the Standard Template
Library (STL). Graph class and its helper structures are declared in Graph.h header file and their
implementations are defined in Graph.cpp file.
  
1.1 GraphVertex Struct
GraphVertex structure holds the information about a vertex. A GraphVertex instance holds
edgeIds for which edges are originated from this vertex.
At most a vertex can have MAX EDGE PER VERTEX (currently this is set to 8) amount of
edges. Since maximum edge count is defined as a compile time constant, edgeIds variable is declared
as a static array. Thus, edgeCount variable holds the actual edge count in edgeIds array. Note that an
edgeId in edgeIds array is the index of the Graph::edgeList dynamic array. Finally, name variable holds the
unique name of that vertex.

1.2 GraphEdge Struct
GraphEdge structure holds the information about an edge. weight variable holds the weight of
the vertex, masked flag holds whether this edge is masked or not. Finally, vertexId0 and vertexId1
hold the vertex indices that this edge connects.
Just like edgeId, vertexId0 and vertexId1 are indices of the Graph::vertexList dynamic array.
  
1.3 Graph Class
Graph class implements non-directional, non-negative weighted graph. It only allows single edge
between two vertices. It utilizes two dynamic arrays for its data, and it is capable of some functionalities that are explained below.

1.3.1 Graph();
Default constructor constructs an empty graph. It relies on the default constructors of the std::vector.

1.3.2 Graph(const std::string& filePath);
This constructor reads a “.map” file and constructs the Graph data structure using the information
in the file. It relies on InsertVertex(...) and ConnectVertices(...) functions.
A “.map” file for the map of my university (METU) is included in the repo for reference.

1.3.3 void InsertVertex(const std::string& vertexName);
This function tries to insert a vertex named “vertexName” without any edge. However, it
throws “DuplicateVertexNameException” if a same named vertex already exists on the graph.

1.3.4 bool ConnectVertices(const std::string& fromVertexName,
const std::string& toVertexName, int weight);
This function tries to set an edge between two vertices. If such named vertex is not found, it
throws “VertexNotFoundException”.
If edge count will exceed the MAX EDGE PER VERTEX, it throws “TooManyEdgeOnVertexExecption”. Additionally,
this function returns true if the insertion is successful.
Insertion may fail when these two vertices already have an edge. Then this function will return
false.

1.3.5 Mask Functions
Unlike traditional graphs, this graph implementation will consist masking functions. Mask functions mask the
particular edge(s) in the graph. Masked edges are not used on the calculations of
“PrintAll” and “ShortestPath” algorithms. Masked edges do not alter the behavior of any
other function (i.e “ConnectVertices” function still returns false even if the edge between those
two vertices are masked).
There is no error when user tries to mask/unmask an already masked/unmasked edge or tries to
mask/unmask an unavailable edge.

1.3.6 void MaskEdges(const std::vector<StringPair>& vertexNames);
This function masks edges between vertex name pairs on the array vertexNames. 
If any of the names does not correspond to a vertex in the graph, this function should throw
“VertexNotFoundException”.
  
1.3.7 void UnMaskEdges(const std::vector<StringPair>& vertexNames);
This function does the complete opposite of the function explained in Section 1.3.6.

1.3.8 void UnMaskAllEdges();
This function unmasks all the edges in the graph.

1.3.9 MaskVertexEdges(const std::string& name);
This function masks all the edges coming out of the vertex with the name name. If the name does not
correspond to a vertex in the graph, this function throws “VertexNotFoundException”.

1.3.10 UnMaskVertexEdges(const std::string& name);
This function does the complete opposite of the function explained in Section 1.3.9

1.3.11 bool ShortestPath(std::vector<int>& orderedVertexIdList,
const std::string& from, const std::string& to) const
This function is used to find shortest path between two vertices. It returns the shortest path as an
ordered vertex id list in the orderedVertexIdList variable. If a shortest path exists, this variable’s
first element should be the vertex id of the vertex named “from” and the last element should be
the vertex id of the vertex named “to”. If a path cannot be found, then orderedVertexIdList should
remain empty. Return value of the function is true when a shortest path is found and false when
there is no path exists between those two vertices.
This function uses std::priority_queue STL library class.
In order remain consistent between implementations, it does update the path weight if it is
equal to previously found path weight. This means that the “first” shortest path will be
chosen as the shortest path if any other equally weighted shortest path exists.
While running, this function does not consider masked edges as a valid edge.
If either “to” or “from” is not available in the graph, function should throw “VertexNotFoundException”.
  
1.3.12 int MultipleShortPaths(std::vector<std::vector<int>>&
orderedVertexIdList, const std::string& from,
const std::string& to, int numberOfShortestPaths);
This function returns numberOfShortestPaths amount of short paths according to the algorithm
which will be defined below.
The algorithm can be expressed as follows:
• Unmask all the edges
• Find the shortest path normally
• Add the found path to the orderedVertexIdList
• Refer to it as the last found path
• Until numberOfShortestPaths is reached
  – Mask the highest weighted edge on the last found path 
  – Run shortest path on the graph
  – If any other shortest path could not be found; terminate.
  – Add the found path to the orderedVertexIdList
  – Refer the found path as last found path.
• Unmask all the edges
Argument orderedVertexIdList is a dynamic array of dynamic arrays of integers. Each dynamic array
of integers should hold a valid path as defined the the Section 1.3.11. It's assumed that orderedVertexIdList
is empty initially. This function returns the number of shortest paths available in the orderedVertexIdList.
If either “to” or “from” is not available in the graph, function throws “VertexNotFoundException”.

1.3.13 void ModifyEdge(const std::string& vName0,
const std::string& vName1, float newWeight);
Modifies the edge weight between two vertices. If either “to” or “from” is not available in the
graph, function throws “VertexNotFoundException”. If no edge is present between
these vertices, function fails silently.

1.3.14 void ModifyEdge(int vId0, int vId1,
float newWeight);
Modifies the edge weight between two vertices. If no edge is present between these vertices and/or
any of the ids are not valid, function fails silently.

1.3.15 void PrintAll() const;
Prints the graph on to the standard output. If an edge is masked, print function does not print
that edge. 

1.3.16 void PrintPath(const std::vector<int>& orderedVertexIdList,
bool sameLine = false) const;
Prints the orderedVertexIdList (which is the output of “ShortestPath” or “MultipleShortPaths”) to the standard output. 
Please notice that this function does not check if an edge exists between adjacent vertices on the list. 
If no edge exists between these vertices, it prints “-##->”. If sameLine is true, this function
prints on a single line.

1.3.17 int TotalVertexCount() const;
This function returns the total number of vertices in the graph.

1.3.18 int TotalEdgeCount() const;
This function returns the total number of edges in the graph.

1.3.19 std::string VertexName(int vertexId) const;
This function returns name of the vertex with the given id. If no such vertex is available,
this function returns an empty string.

1.3.20 int TotalWeightInBetween(std::vector<int>& orderedVertexIdList,
int start, int end);
Accumulates the edge weights between vertices in the orderedVertexIdList. If a vertex with the
given id is not available in the graph, this function throws “VertexNotFoundException”.
If no edge exists between any of the vertices, function returns -1.

--**Hash Table** 
In addition to the Graph data structure, the maps application requires a hash table for storing found
paths. This is implemented as the KeyedHashTable class which stores string and integer vector
pairs. The string will be used to determine the location of the data using hash function. 
Hash table will resolve its collisions using quadratic probing.
KeyedHashTable class and its helper structures are declared in HashTable.h header file and their
implementationsare defined in HashTable.cpp file.
  
2.1 KeyedHashTable Class
KeyedHashTable class has the main implementation. This class holds its data as an array on
a simple allocation which is pointed by the table variable. Entire capacity of the hash table is on
tableSize variable. Variable occupiedElementCount holds how many entries are on the hash table.
Empty entries are initialized with an empty string and an empty vector.

2.1.1 static int FindNearestLargerPrime(int requestedCapacity);
This function returns the nearest but larger prime from the PRIME LIST static variable.
It's assumed requestedCapacity is never greater or equal to 100th prime which is 541. Even if
requestedCapacity is a prime, this function should still return the next prime.

2.1.2 int Hash(const std::string& key) const;
This function returns the hashed value of a string. Equation below shows the mathematical formula of
the hash function:
hash = summation over all k in key indices -> string[k] ∗ PRIME_LIST[k]
hash = (hash mod tableSize)
It's assumed that the length of the key string never exceeds 100.

2.1.3 void ReHash();
Rehash function is called when (occupiedElementCount * EXPAND_THRESHOLD >= tableSize).
When called; this function allocates a new table with a new prime. This prime is found by using
int FindNearestLargerPrime(...). Argument for int FindNearestLargerPrime(...) will be
the doubled table size. Then it rehashes all of the entries from the old table to the new table.
Finally it deletes the old table.

2.1.4 KeyedHashTable();
Default constructor will create the hash table with the smallest prime number which is 2.

2.1.5 KeyedHashTable(int requestedCapacity);
This constructor will create the hash table with the nearest but larger prime of requestedCapacity.

2.1.6 KeyedHashTable(const KeyedHashTable&);
Copy constructor does a deep copy.

2.1.7 KeyedHashTable& operator=(const KeyedHashTable&);
Copy assignment operator does a deep copy.

2.1.8 ~KeyedHashTable();
Destructor deletes all the allocated data.

2.1.9 bool Insert(const std::string& key,
const std::vector<int>& intArray);
This function inserts a value with key _key_ and an integer array intArray. Collisions should be
resolved using quadratic probing. After insertion, this function calls void ReHash() when
(occupiedElementCount * EXPAND_THRESHOLD >= tableSize).
If insert operation is successful, function returns true. Insert operation may fail when there
is an entry with a same key. Then this function returns false.

2.1.10 bool Remove(const std::string& key);
This functions tries to remove an entry with a key named key. If there is no such keyed entry
present on the table it returns false. If the entry is found and deleted, this function returns true.
In order to “delete” an entry, its key should be set to empty string and its array should be set to
an empty array.

2.1.11 void ClearTable();
This function clears all of the entries in the table. This function does not deallocate the table. It
sets all of occupied entries to empty string and empty array.

2.1.12 bool Find(std::vector<int>& valueOut,
const std::string& key) const;
This function tries to find an entry with a key named key. If such entry is present, function copies
that entry’s integer array to the valueOut variable and returns true. If such entry is not available,
function returns false.

2.1.13 void Print() const;
Prints the hash table. This function only prints the occupied elements. 

3 Maps
“Maps” class combines the Graph data structure and KeyedHashTable data structure. Continuing with the
same pattern; Maps class is declared on Maps.h header file and its definitions are in Maps.cpp file.
  
3.1 Maps Class
Maps class is basically a state machine. This means that call order of the functions is
important. In order to properly use the class, user first sets its destination and starting location.
Then it should start the journey. During journey, user can update its current location and call
display function to see its current route and timing values. Finally, user should end the journey.
Maps is responsible to warn the user when such order is not satisfied using the print functions (Section 3.1.1).

3.1.1 Print Functions
There are many print operations in this class. In order to provide consistency, all print operations are
provided as functions. These functions are not explained here but all of which are self-explanatory.
These functions are:
• void PrintNotInJourney() const;
• void PrintUnableToChangeDestination() const;
• void PrintUnableToChangeStartingLoc() const;
• void PrintAlreadyInJourney() const;
• void PrintJourneyIsAlreadFinished() const;
• void PrintLocationNotFound() const;
• void PrintJourneyCompleted() const;
• void PrintCachedLocationFound(const std::string& location0,
const std::string& location1)
• void PrintCalculatingRoutes(const std::string& location0,
const std::string& location1) const;

3.1.2 static std::string GenerateKey(const std::string& location0,
const std::string& location1);
Generates a key that will be used in the hash table. 

3.1.3 Maps(int potentialPathCount, const std::string& mapFilePath);
Constructor will generate a graph using the string mapFilePath and sets the potentialPathCount
member variable to the same named argument. Initially class is not in a journey. Initially, hash
table is constructed with the multiplication of total location count in the map and potentialPathCount.

3.1.4 void SetDestination(const std::string& name);
This function tries to set destination. This function succeeds only if the user is not in a journey. If
this function is called during journey, it calls PrintUnableToChangeDestination().

3.1.5 void SetStartingLocation(const std::string& name);
This function tries to set startingLoc. This function succeeds only if the user is not in a journey. If
this function is called during journey it calls PrintUnableToChangeStartingLoc().

3.1.6 void StartJourney();
This function starts a journey. When a journey starts, this function finds the shortest paths between
startingLoc and destination. Before calculating the shortest paths, it calls PrintCalculatingRoutes(...).
How many paths that are going to be calculated is determined by the potentialPathCount variable.
After paths are found, this function caches these paths on to the hash table. Caching operation
will be explained in Section 3.1.10. Additionally, it sets currentLoc to startingLoc. Finally, it sets
the currentRoute to the first route that is found.
If this function is called during journey, it calls PrintAlreadyInJourney(). If either
startingLoc and destination not the map, it calls PrintLocationNotFound().

3.1.7 void EndJourney();
This function ends the journey. If this function is not called during a journey, it calls
PrintJourneyIsAlreadFinished(). Otherwise; this function should clear the cache table, destination,
startingLoc and currentLoc variables.

3.1.8 void UpdateLocation(const std::string& name);
This function will be called when user has reached a certain location on the map. User may or may
not call this from a location that is already on the current path. Algorithm-wise, this function will
check if the destination name between destination is already in the cache. If such a path is in the hash
table it calls PrintCachedLocationFound() and sets currentRoute to the cached route. If
location can not be found in the hash table it calls PrintCalculatingRoutes(...) then
it finds shortest path between name and destination just like in StartJourney() function.
Finally, this function caches found locations to the hash table which is explained in
Section 3.1.10.
If this function is not called during journey, it calls PrintNotInJourney(). If name is
not on the map it calls PrintLocationNotFound(). If user reached to his/her destination,
function calls PrintJourneyCompleted().

3.1.9 void Display();
This function prints the current journey parameters. This function should be called during journey
otherwise this function will call PrintNotInJourney().

3.1.10 Caching
When multiple short paths are found, Maps class caches these on to the hash table.
All sub-paths towards the destination should be cached. For example, assuming two paths
(A->B->C->D) and (A->E->C->D) are found, (A->B->C->D), (B->C->D), (C->D), (A->E->C->D),
(E->C->D), (C->D) should be cached on to the hash table. Key value of such paths are found
by using void GenerateKey() function. First argument on this function should be the starting
location on the sub path and second argument should be the destination (Key value of B->C->D
will be the output of GenerateKey(B, D). Duplicates are ignored.

  
Happy travels :)
