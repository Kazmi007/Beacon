#include "Graph.h"
#include "GraphExceptions.h"

#include <iostream>
#include <iomanip>
#include <queue>
#include <fstream>
#include <sstream>
#include <cstdlib>

Graph::Graph()
{}

Graph::Graph(const std::string& filePath)
{
    // Tokens
    std::string tokens[3];

    std::ifstream mapFile(filePath.c_str());
    // Read line by line
    std::string line;
    while (std::getline(mapFile, line))
    {
        // Empty Line Skip
        if(line.empty()) continue;
        // Comment Skip
        if(line[0] == '#') continue;

        // Tokenize the line
        int i = 0;
        std::istringstream stream(line);
        while(stream >> tokens[i]) i++;

        // Single token (Meaning it is a vertex)
        if(i == 1)
        {
            InsertVertex(tokens[0]);
        }
        // Exactly three tokens (Meaning it is an edge)
        else if(i == 3)
        {
            int weight = std::atoi(tokens[0].c_str());
            if(!ConnectVertices(tokens[1], tokens[2], weight))
            {
                std::cerr << "Duplicate edge on "
                          << tokens[0] << "-"
                          << tokens[1] << std::endl;
            }
        }
        else std::cerr << "Token Size Mismatch" << std::endl;
    }
}

void Graph::InsertVertex(const std::string& vertexName)
{
    for (int i=0; i<vertexList.size(); i++)
        if (vertexList[i].name == vertexName)
            throw DuplicateVertexNameException();
    GraphVertex v;
    v.edgeCount = 0;
    v.name = vertexName;
    vertexList.push_back(v);
}

bool Graph::ConnectVertices(const std::string& fromVertexName,
                            const std::string& toVertexName,
                            int weight)
{
    if (fromVertexName == toVertexName)
        return false;
    int v0 = -1, v1 = -1;
    for (int i=0; i<vertexList.size(); i++) {
        if (vertexList[i].name == fromVertexName)
            v0 = i;
        if (vertexList[i].name == toVertexName)
            v1 = i;
    }
    if ((v0 == -1) || (v1 == -1))
        throw VertexNotFoundException();
    if ((vertexList[v0].edgeCount + 1 > MAX_EDGE_PER_VERTEX) || (vertexList[v1].edgeCount + 1 > MAX_EDGE_PER_VERTEX))
        throw TooManyEdgeOnVertexExecption();
    for (int i=0; i<edgeList.size(); i++)
        if ((edgeList[i].vertexId0==v0 && edgeList[i].vertexId1==v1) || (edgeList[i].vertexId0==v1 && edgeList[i].vertexId1==v0))
            return false;
    vertexList[v0].edgeIds[vertexList[v0].edgeCount] = edgeList.size();
    vertexList[v1].edgeIds[vertexList[v1].edgeCount] = edgeList.size();
    vertexList[v0].edgeCount = vertexList[v0].edgeCount + 1;
    vertexList[v1].edgeCount = vertexList[v1].edgeCount + 1;
    GraphEdge e;
    e.vertexId0 = v0;
    e.vertexId1 = v1;
    e.masked = false;
    e.weight = weight;
    edgeList.push_back(e);
    return true;
}

bool Graph::ShortestPath(std::vector<int>& orderedVertexIdList,
                         const std::string& from,
                         const std::string& to) const
{
    int v0 = -1, v1 = -1;
    for (int i=0; i<vertexList.size(); i++) {
        if (vertexList[i].name == from)
            v0 = i;
        if (vertexList[i].name == to)
            v1 = i;
    }
    if ((v0 == -1) || (v1 == -1)) 
        throw VertexNotFoundException();
    if (from == to)
        return false;
    int flag = 1;
    std::vector<int> prev;
    std::vector<bool> vst; 
    std::priority_queue<DistanceVertexIdPair> q;
    std::vector<DistanceVertexIdPair> vert;
    for (int i = 0; i < vertexList.size(); i++) {
        DistanceVertexIdPair v(i, LARGE_NUMBER);
        vert.push_back(v);
        prev.push_back(-1);
        vst.push_back(false);
    }
    vert[v0].currentWeight = 0;
    q.push(vert[v0]);
    while (!q.empty()) {
        int v = q.top().vId;
        q.pop();
        vst[v] = true;
        for (int i=0; i<edgeList.size(); i++) {
            if (edgeList[i].masked == true)
                continue;
            else if (edgeList[i].vertexId0 == v) {
                int va = edgeList[i].vertexId1;
                if ((!vst[va]) && (vert[va].currentWeight > vert[v].currentWeight + edgeList[i].weight)) {
                    vert[va].currentWeight = vert[v].currentWeight + edgeList[i].weight;
                    prev[va] = v;
                    q.push(vert[va]);
                    if (va == v1)
                        flag = 0;
                }
            }
            else if (edgeList[i].vertexId1 == v) {
                int va = edgeList[i].vertexId0;
                if ((!vst[va]) && (vert[va].currentWeight > vert[v].currentWeight + edgeList[i].weight)) {
                    vert[va].currentWeight = vert[v].currentWeight + edgeList[i].weight;
                    prev[va] = v;
                    q.push(vert[va]);
                    if (va == v1)
                        flag = 0;
                }
            }
        }
    }
    if (flag)
        return false;
    std::vector<int> path;
    int crawl = v1;
    while (crawl != -1) {
        path.push_back(crawl);
        crawl = prev[crawl];
    }
    while (!path.empty()) {
        orderedVertexIdList.push_back(path.back());
        path.pop_back();
    }
    return true;
}


int Graph::MultipleShortPaths(std::vector<std::vector<int> >& orderedVertexIdList,
                              const std::string& from,
                              const std::string& to,
                              int numberOfShortestPaths)
{
    int v0 = -1, v1 = -1;
    for (int i=0; i<vertexList.size(); i++) {
        if (vertexList[i].name == from)
            v0 = i;
        if (vertexList[i].name == to)
            v1 = i;
    }
    if ((v0 == -1) || (v1 == -1)) 
        throw VertexNotFoundException();
    if (from == to)
        return 0;
    this->UnMaskAllEdges();
    std::vector<int> lpath;
    int spaths = 0;
    if (!this->ShortestPath(lpath, from, to))
        return spaths;
    spaths++;
    orderedVertexIdList.push_back(lpath);
    for (int i=0; i<numberOfShortestPaths-1; i++) {
        int hweight = -1;
        int hwid;
        for (int j=0; j<edgeList.size(); j++) {
            for (int k=0; k<lpath.size()-1; k++) {
                if ((edgeList[j].vertexId0==lpath[k] && edgeList[j].vertexId1==lpath[k+1]) || (edgeList[j].vertexId0==lpath[k+1] && edgeList[j].vertexId1==lpath[k])) {
                    if (edgeList[j].weight > hweight) {
                        hweight = edgeList[j].weight;
                        hwid = j;
                    }
                }
            }
        }
        edgeList[hwid].masked = true;
        lpath.clear();
        if (!this->ShortestPath(lpath, from, to))
            return spaths;
        spaths++; 
        orderedVertexIdList.push_back(lpath);
    }
    this->UnMaskAllEdges();
    return spaths;
}

void Graph::MaskEdges(const std::vector<StringPair>& vertexNames)
{
    for (int i=0;i<vertexNames.size();i++) {
        int v0 = -1, v1 = -1;
        for (int j=0; j<vertexList.size(); j++) {
            if (vertexList[j].name == vertexNames[i].s0)
                v0 = j;
            if (vertexList[j].name == vertexNames[i].s1)
                v1 = j;
        }   
        if ((v0 == -1) || (v1 == -1))
            throw VertexNotFoundException();
        for (int j=0; j<edgeList.size(); j++) 
            if ((edgeList[j].vertexId0==v0 && edgeList[j].vertexId1==v1) || (edgeList[j].vertexId0==v1 && edgeList[j].vertexId1==v0))
                edgeList[j].masked = true;
    }
}

void Graph::UnMaskEdges(const std::vector<StringPair>& vertexNames)
{
    for (int i=0;i<vertexNames.size();i++) {
        int v0 = -1, v1 = -1;
        for (int j=0; j<vertexList.size(); j++) {
            if (vertexList[j].name == vertexNames[i].s0)
                v0 = j;
            if (vertexList[j].name == vertexNames[i].s1)
                v1 = j;
        }   
        if ((v0 == -1) || (v1 == -1))
            throw VertexNotFoundException();
        for (int j=0; j<edgeList.size(); j++) 
            if ((edgeList[j].vertexId0==v0 && edgeList[j].vertexId1==v1) || (edgeList[j].vertexId0==v1 && edgeList[j].vertexId1==v0)) 
                edgeList[j].masked = false;
    }
}

void Graph::UnMaskAllEdges()
{
    for (int i=0; i<edgeList.size(); i++) 
        edgeList[i].masked = false;
}

void Graph::MaskVertexEdges(const std::string& name)
{
    int v = -1;
    for (int i=0; i<vertexList.size(); i++) 
        if (vertexList[i].name == name)
            v = i;
    if (v == -1)
        throw VertexNotFoundException();
    for (int i=0; i<edgeList.size(); i++)
        if (edgeList[i].vertexId0==v || edgeList[i].vertexId1==v) 
            edgeList[i].masked = true;
}

void Graph::UnMaskVertexEdges(const std::string& name)
{
    int v = -1;
    for (int i=0; i<vertexList.size(); i++) 
        if (vertexList[i].name == name)
            v = i;
    if (v == -1)
        throw VertexNotFoundException();
    for (int i=0; i<edgeList.size(); i++)
        if (edgeList[i].vertexId0==v || edgeList[i].vertexId1==v) 
            edgeList[i].masked = false;
}

void Graph::ModifyEdge(const std::string& vName0,
                       const std::string& vName1,
                       float newWeight)
{
    int v0 = -1, v1 = -1;
    for (int i=0; i<vertexList.size(); i++) {
        if (vertexList[i].name == vName0)
            v0 = i;
        if (vertexList[i].name == vName1)
            v1 = i;
    }   
    if ((v0 == -1) || (v1 == -1))
        throw VertexNotFoundException();
    for (int i=0; i<edgeList.size(); i++) 
        if ((edgeList[i].vertexId0==v0 && edgeList[i].vertexId1==v1) || (edgeList[i].vertexId0==v1 && edgeList[i].vertexId1==v0))
            edgeList[i].weight = newWeight;
}

void Graph::ModifyEdge(int vId0, int vId1,
                       float newWeight)
{
    for (int i=0; i<edgeList.size(); i++) 
        if ((edgeList[i].vertexId0==vId0 && edgeList[i].vertexId1==vId1) || (edgeList[i].vertexId0==vId1 && edgeList[i].vertexId1==vId0))
            edgeList[i].weight = newWeight;
}

void Graph::PrintAll() const
{
    for(size_t i = 0; i < vertexList.size(); i++)
    {
        const GraphVertex& v = vertexList[i];
        std::cout << v.name << "\n";
        for(int j = 0; j < v.edgeCount; j++)
        {
            int edgeId = v.edgeIds[j];
            const GraphEdge& edge = edgeList[edgeId];
            // Skip printing this edge if it is masked
            if(edge.masked)
                continue;

            // List the all vertex names and weight
            std::cout << "-" << std::setfill('-')
                             << std::setw(2) << edge.weight
                             << "-> ";
            int neigVertexId = (static_cast<int>(i) == edge.vertexId0)
                                 ? edge.vertexId1
                                 : edge.vertexId0;
            std::cout << vertexList[neigVertexId].name << "\n";
        }
    }
    // Reset fill value because it "sticks" to the std out
    std::cout << std::setfill(' ');
    std::cout.flush();
}

void Graph::PrintPath(const std::vector<int>& orderedVertexIdList,
                      bool sameLine) const
{
    for(size_t i = 0; i < orderedVertexIdList.size(); i++)
    {
        int vertexId = orderedVertexIdList[i];
        if(vertexId >= static_cast<int>(vertexList.size()))
            throw VertexNotFoundException();

        const GraphVertex& vertex = vertexList[vertexId];
        std::cout << vertex.name;
        if(!sameLine) std::cout << "\n";
        // Only find and print the weight if next is available
        if(i == orderedVertexIdList.size() - 1) break;
        int nextVertexId = orderedVertexIdList[i + 1];
        if(nextVertexId >= static_cast<int>(vertexList.size()))
            throw VertexNotFoundException();

        // Find the edge between these two vertices
        int edgeId = INVALID_INDEX;
        if(vertexId     < static_cast<int>(vertexList.size()) &&
           nextVertexId < static_cast<int>(vertexList.size()))
        {
            // Check all of the edges of vertex
            // and try to find
            const GraphVertex& fromVert = vertexList[vertexId];
            for(int i = 0; i < fromVert.edgeCount; i++)
            {
                int eId = fromVert.edgeIds[i];
                // Since the graph is not directional
                // check the both ends
                if((edgeList[eId].vertexId0 == vertexId &&
                    edgeList[eId].vertexId1 == nextVertexId)
                ||
                   (edgeList[eId].vertexId0 == nextVertexId &&
                    edgeList[eId].vertexId1 == vertexId))
                {
                    edgeId = eId;
                    break;
                }
            }
        }
        if(edgeId != INVALID_INDEX)
        {
            const GraphEdge& edge = edgeList[edgeId];
            std::cout << "-" << std::setfill('-')
                      << std::setw(2)
                      << edge.weight << "->";
        }
        else
        {
            std::cout << "-##-> ";
        }
    }
    // Print endline on the last vertex if same line is set
    if(sameLine) std::cout << "\n";
    // Reset fill value because it "sticks" to the std out
    std::cout << std::setfill(' ');
    std::cout.flush();
}

int Graph::TotalVertexCount() const
{
    return vertexList.size();
}

int Graph::TotalEdgeCount() const
{
    return edgeList.size();
}

std::string Graph::VertexName(int vertexId) const
{
    if (vertexId >= 0 && vertexId < vertexList.size())
        return vertexList[vertexId].name;
    return ""; 
}

int Graph::TotalWeightInBetween(std::vector<int>& orderedVertexIdList)
{
    int total = 0;
    for (int i=0; i < orderedVertexIdList.size() - 1; i++) {
        if ((orderedVertexIdList[i]<0) || (orderedVertexIdList[i]>=vertexList.size()))  
            throw VertexNotFoundException();
        if ((orderedVertexIdList[i+1]<0) || (orderedVertexIdList[i+1]>=vertexList.size()))  
            throw VertexNotFoundException();
        int v0 = orderedVertexIdList[i], v1 = orderedVertexIdList[i+1], flag = 1;
        for (int j=0; j<edgeList.size(); j++) { 
            if ((edgeList[j].vertexId0==v0 && edgeList[j].vertexId1==v1) || (edgeList[j].vertexId0==v1 && edgeList[j].vertexId1==v0)) {
                total += edgeList[j].weight;
                flag = 0;
            }
        }
        if (flag)
            return -1;
    }
    return total;
}