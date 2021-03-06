#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <algorithm>
#include <functional>
#include <map>
#include <utility>
#include <ctime>
using namespace std;

typedef int Vertex;
typedef vector<Vertex> Clique;
typedef vector< vector<char> > AdjacencyMatrix;
typedef int Color;
typedef vector< vector<Vertex> > ColorSets;


bool operator<(const Clique& a, const Clique& b) {
    return a.size() < b.size();
}

vector<Vertex> operator&(const vector<Vertex> &v1, const vector<Vertex> &v2) {
    vector<Vertex> result;
    for(int i = 0; i < v1.size(); ++i) {
        for(int j =0; j < v2.size(); ++j) {
            if(v1[i] == v2[j]) {
                result.push_back(v1[i]);
            }
        }
    }
    sort(result.begin(), result.end());
    return result;
}

vector<Vertex> operator&(const vector<Vertex> &v1, Vertex v) {
    vector<Vertex> v2 = {v};
    return v1&v2;
}

vector<Vertex> operator|(const vector<Vertex> &v1, const vector<Vertex> &v2) {
    vector<Vertex> result = v1;
    for(int i = 0; i < v2.size(); i++) {
        bool isContained = false;
        for(int j =0; j < v1.size(); j++) {
            if(v2[i] == v1[j]) {
                isContained = true;
            }
        }

        if(!isContained) {
            result.push_back(v2[i]);
        }
    }
    sort(result.begin(), result.end());
    return result;
}

vector<Vertex> operator|(const vector<Vertex> &v1, Vertex v) {
    vector<Vertex> v2 = {v};
    return v1|v2;
}

vector<Vertex> operator/(const vector<Vertex> &v1, const vector<Vertex> &v2) {
    vector<Vertex> result;
    for(int i = 0; i < v1.size(); i++) {
        bool isContained = false;
        for(int j =0; j < v2.size(); j++) {
            if(v1[i] == v2[j]) {
                isContained = true;
            }
        }

        if(!isContained) {
            result.push_back(v1[i]);
        }
    }
    sort(result.begin(), result.end());
    return result;
}

vector<Vertex> operator/(const vector<Vertex> &v1, Vertex v) {
    vector<Vertex> v2 = {v};
    return v1/v2;
}

void printClique(const Clique &clique) {
    if(clique.size()==0) {
        cout << 0;
        return;
    }

    cout << clique.size() << " ";
    for (int i = 0; i < clique.size(); ++i) {
        cout << clique[i]+1 << ' ';
    }
    cout << endl;
}
Color maxColor(const ColorSets& colors) {
    return  colors.size()-1;
}

class Graph {
public:
    Graph(const string &filename) {
        ifstream file(filename.c_str());
        string line;
        edgeCount_ = 0;
        while (getline(file, line)) {
            if (line[0] == 'c') {
                continue;
            } else if(line[0] == 'p') {
                sscanf(line.c_str(), "p edge %d %d", &vertexCount_, &edgeCount_);
                adjacencyMatrix_.resize(vertexCount_);
                for (int i = 0; i < vertexCount_; ++i) {
                    adjacencyMatrix_[i].resize(vertexCount_);
                }
            } else if(line[0] == 'e') {
                Vertex a,b;
                sscanf(line.c_str(), "e %d %d", &a, &b);
                adjacencyMatrix_[a-1][b-1] = 1;
                adjacencyMatrix_[b-1][a-1] = 1;
            } else {
                throw string("invalid file");
            }
        }
        for (int i = 0; i < vertexCount_; i++) {
            vector<Vertex> neighbors;
            for (Vertex j = 0; j < vertexCount_; ++j) {
                if (isNeighbors(i, j)) {
                    neighbors.push_back(j);
                }
            }
            neighbors_.push_back(neighbors);
            verticesDegree_.push_back(neighbors.size());
        }
    }
    bool isNeighbors(Vertex a, Vertex b) const {
        return adjacencyMatrix_[a][b] == 1;
    }
    vector<int> verticesDegree(vector<Vertex> vertices) const {
        vector<int> verticesNeighborsCount;
        for (int i = 0; i < vertices.size(); i++) {
            verticesNeighborsCount.push_back(neighbors_[vertices[i]].size());
        }
        return verticesNeighborsCount;
    }
    int vertexDegree(Vertex v) const {
        return verticesDegree_[v];
    }
    void sortByDegree(vector<Vertex>& vertices) const {
        sort(vertices.begin(), vertices.end(),
             [this](const Vertex &v1, const Vertex &v2) {
            return vertexDegree(v1) < vertexDegree(v2);
        });
    }

    vector<Vertex> getNeighbors(Vertex v) const {
        return neighbors_[v];
    }

    vector<Vertex> getNeighbors(vector<Vertex> vertices) const {
        vector<Vertex> neighbors;
        for (Vertex i = 0; i < vertexCount_; ++i) {
            bool isNeighbor = true;
            for (int j = 0; j < vertices.size(); ++j) {
                if (!isNeighbors(i, vertices[j])) {
                    isNeighbor = false;
                }
            }
            if (isNeighbor) {
                neighbors.push_back(i);
            }
        }
        return neighbors;
    }

    int neighborsCount(Vertex v) const {
        return neighbors_[v].size();
    }

    int neighborsCount(vector<Vertex> vertices) const {
        return getNeighbors(vertices).size();
    }

    vector<Vertex> getVertices() const {
        vector<Vertex> vertices;
        for (int i = 0; i < vertexCount_;i++) {
            vertices.push_back(i);
        }
        return vertices;
    }

    ColorSets colorVertices(const vector<Vertex>& vertices) const {
        ColorSets colors;
        map<Vertex, Color> vertex2color;
        for(int i = 0; i < vertices.size(); i++) {
            vertex2color[vertices[i]] = -1;
        }
        Color maxColor = -1;
        for(Vertex v = 0; v < vertices.size(); ++v) {
            vector<Vertex> neighbors = vertices & getNeighbors(vertices[v]);
            vector<Color> neighborsColor(neighbors.size());
            for (int i = 0; i < neighbors.size(); ++i) {
                neighborsColor[i] = vertex2color[neighbors[i]];
            }

            bool isColored = false;
            for (Color c = 0; c <= maxColor; c++) {
                if(find(neighborsColor.begin(), neighborsColor.end(), c) == neighborsColor.end()) {
                    colors[c].push_back(vertices[v]);
                    vertex2color[vertices[v]] = c;
                    isColored = true;
                    break;
                }
            }
            if (!isColored) {
                colors.push_back({vertices[v]});
                vertex2color[vertices[v]] = maxColor+1;
                maxColor++;
            }
        }
        return colors;
    }
    ColorSets colorGraph() const {
        return colorVertices(getVertices());
    }
    Clique lowerBound() const {
        Vertex v = distance(verticesDegree_.begin(),
                            max_element(verticesDegree_.begin(),
                                        verticesDegree_.end()));
        Clique clique;
        clique.push_back(v);

        vector<Vertex> neighbors = getNeighbors(clique);
        while(!neighbors.empty()) {
            vector<int> verticesNeighborsCount = verticesDegree(neighbors);
            int maxVertexIndex = distance(verticesNeighborsCount.begin(),
                                          max_element(verticesNeighborsCount.begin(),
                                                      verticesNeighborsCount.end()));
            clique.push_back(neighbors[maxVertexIndex]);
            vector<Vertex> new_neighbors = getNeighbors(clique);

            swap(neighbors, new_neighbors);
        }
        return clique;
    }
private:
    vector<int> verticesDegree_;
    vector < vector<Vertex> > neighbors_;
    int vertexCount_;
    int edgeCount_;
    AdjacencyMatrix adjacencyMatrix_;

};

pair<Vertex, Color> vertexWithGreatestColor(const ColorSets &colors) {
    return pair<Vertex, Color>(colors[colors.size()-1][0], colors.size()-1);
}

Clique currClique;
Clique maxClique;
int theta = 10;
int maxColorCount;
unsigned int startTime;
unsigned int maxTime;
ColorSets reusingColors(const Graph & graph,
                        const vector<Vertex> &candidates, const ColorSets &colors) {

    ColorSets newColors(colors.size());
    for(int i = 0; i < colors.size(); i++) {
        newColors[i] = candidates & colors[i];
    }

    newColors.erase(remove_if(newColors.begin(), newColors.end(),
                              [](const vector<Vertex>& v) {return v.size() == 0;}), newColors.end());
    for(int i = 0; i < newColors.size(); i++) {
        if(newColors[i].size() == 1) {
            Vertex w = newColors[i][0];
            bool canRepaint = false;
            for(int j = 0; j < newColors.size(); j++) {
                if((newColors[j] & graph.getNeighbors(w)).empty() && i!=j) {
                    newColors[j].push_back(w);
                    newColors[i].clear();
                    canRepaint = true;
                    break;
                }
            }
        }
    }

    ColorSets resultColors;
    for(int i = 0; i < newColors.size(); i++) {
        if(!newColors[i].empty()) {
            resultColors.push_back(newColors[i]);
        }
    }
    sort(resultColors.begin(), resultColors.end(),
         [](const vector<Vertex> &v1, const vector<Vertex> &v2){return v1.size()>v2.size();});
    return resultColors;
}

void findMaxClique(const Graph & graph,
                   vector<Vertex> candidates, ColorSets colors) {
    if(clock()-startTime > maxTime) {
        return;
    }
    if(maxColorCount == maxClique.size()) {
        return;
    }
    while(!candidates.empty()) {
        pair<Vertex, Color> vertexAndColor = vertexWithGreatestColor(colors);
        Vertex v = vertexAndColor.first;
        int vColor = vertexAndColor.second;
        if(currClique.size()+ vColor+1 <= maxClique.size()) return;
        currClique = currClique | v;
        vector<Vertex> newCandidates = candidates & graph.getNeighbors(v);
        if(!newCandidates.empty()) {
            ColorSets newColors;
            if(currClique.size() + vColor <= maxClique.size()+theta) {
                newColors = reusingColors(graph, newCandidates, colors);
            } else {
                newColors = graph.colorVertices(newCandidates);
            }
            findMaxClique(graph, newCandidates, newColors);
        } else if (currClique.size() > maxClique.size()) {
            maxClique = currClique;
        }
        currClique = currClique/v;
        if(colors[colors.size()-1].size() == 1) {
            colors.erase(colors.begin()+colors.size()-1);
        } else {
            colors.back().erase(remove(colors.back().begin(), colors.back().end(), v),
                                colors.back().end());
        }
        candidates = candidates/v;
    }
}

void findMaxClique(const Graph& graph) {
    // initial step
    currClique = {};
    maxClique  = graph.lowerBound();
    vector<Vertex> candidates = graph.getVertices();
    ColorSets colors = graph.colorGraph();
    maxColorCount = colors.size();
    findMaxClique(graph, candidates, colors);
}

int main(int argc, char** argv) {
    startTime = clock();
    maxTime = atoi(argv[2])*CLOCKS_PER_SEC;

    Graph graph(argv[1]);
    findMaxClique(graph);

    cout << (clock()-startTime)/double(CLOCKS_PER_SEC) << " ";

    sort(maxClique.begin(), maxClique.end());
    printClique(maxClique);
    return 0;
}
