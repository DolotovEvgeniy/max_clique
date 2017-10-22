#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <algorithm>
#include <functional>
#include <map>
#include <utility>
using namespace std;

typedef int Vertex;
typedef vector<Vertex> Clique;
typedef vector< vector<char> > AdjacencyMatrix;
typedef int Color;
typedef vector< vector<Vertex> > ColorSets;
bool operator<(Clique a, Clique b) {
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
        cout << "empty";
    }
    for (int i = 0; i < clique.size(); ++i) {
        cout << clique[i] << ' ';
    }
    cout << endl;
}
Color maxColor(ColorSets colors) {
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
        ColorSets colors = colorGraph();
        Color maxUsedColor = maxColor(colors);
        colorCount_ = maxUsedColor + 1;
        for (int i = 0; i < vertexCount(); i++) {
            neighborsCount_.push_back(neighborsCount(i));
        }
        float density = 2*edgeCount_/float(vertexCount_*(vertexCount_-1));
        cout << density << endl;
        //cout <<"bound: " << vertexCount_/float((1-density)*vertexCount_+) <<endl;

    }
    bool isNeighbors(Vertex a, Vertex b) const {
        return adjacencyMatrix_[a][b] == 1;
    }
    vector<int> vertexDegree(vector<Vertex> vertices) const {
        vector<int> verticesNeighborsCount;
        for (int i = 0; i < vertices.size(); i++) {
            verticesNeighborsCount.push_back(neighborsCount(vertices[i]));
        }
        return verticesNeighborsCount;
    }

    vector<Vertex> getNeighbors(Vertex v) const {
        vector<Vertex> neighbors;
        for (Vertex i = 0; i < vertexCount_; ++i) {
            if (isNeighbors(i, v)) {
                neighbors.push_back(i);
            }
        }
        return neighbors;
    }
    vector<Vertex> getNeighborsInSet(Vertex v, vector<Vertex> vertices) const {
        vector<Vertex> neighbors;
        for(int i = 0; i < vertices.size(); i++) {
            if(isNeighbors(v, vertices[i])) {
                neighbors.push_back(vertices[i]);
            }
        }
        return neighbors;
    }
    int neighborsInSetCount(Vertex v, vector<Vertex> vertices) const {
        return getNeighborsInSet(v, vertices).size();
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
        return getNeighbors(v).size();
    }

    int neighborsCount(vector<Vertex> vertices) const {
        return getNeighbors(vertices).size();
    }

    int vertexCount() const {
        return vertexCount_;
    }
    int colorCount() const {
        return colorCount_;
    }
    bool isClique(vector<Vertex> vertices) const {
        for (size_t i = 0; i < vertices.size(); ++i) {
            for (size_t j = i + 1; j < vertices.size(); ++j) {
                if ( !isNeighbors(vertices[i], vertices[j]) ) {
                    return false;
                }
            }
        }
        return true;
    }
    //private:
    vector<Vertex> getVertices() const {
        vector<Vertex> vertices;
        for (int i = 0; i < vertexCount_;i++) {
            vertices.push_back(i);
        }
        return vertices;
    }

    ColorSets colorVertices(const vector<Vertex>& vertices) const {
        //printClique(vertices);
        ColorSets colors;
        map<Vertex, Color> vertex2color;
        for(int i = 0; i < vertices.size(); i++) {
            vertex2color[vertices[i]] = -1;
        }

        for(Vertex v = 0; v < vertices.size(); ++v) {
            vector<Vertex> neighbors = vertices & getNeighbors(vertices[v]);

            vector<Color> neighborsColor(neighbors.size());
            for (int i = 0; i < neighbors.size(); ++i) {
                neighborsColor[i] = vertex2color[neighbors[i]];
            }
            neighborsColor.erase(remove(neighborsColor.begin(), neighborsColor.end(), -1),
                                 neighborsColor.end());
            Color maxColor = -1;
            if(!neighborsColor.empty()) {
                maxColor = *max_element(neighborsColor.begin(), neighborsColor.end());
            }
            bool isColored = false;
            for (Color c = 0; c < maxColor; c++) {
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
            }
        }
        return colors;
    }
    ColorSets colorGraph() const {
        vector<Vertex> vertices;
        for (int i = 0; i < vertexCount_;i++) {
            vertices.push_back(i);
        }
        return colorVertices(vertices);
    }

    Clique lowerBound() const {
        Vertex v = distance(neighborsCount_.begin(),
                            max_element(neighborsCount_.begin(),
                                        neighborsCount_.end()));
       // printClique(neighborsCount_);
        Clique clique;
        clique.push_back(v);
       // printClique(clique);
        vector<Vertex> neighbors = getNeighbors(clique);
        while(!neighbors.empty()) {
            vector<int> verticesNeighborsCount = vertexDegree(neighbors);
            int maxVertexIndex = distance(verticesNeighborsCount.begin(),
                                          max_element(verticesNeighborsCount.begin(),
                                                      verticesNeighborsCount.end()));
            clique.push_back(neighbors[maxVertexIndex]);
            vector<Vertex> new_neighbors = getNeighbors(clique);
            //if (new_neighbors == neighbors) {
            //    return clique.size();
            //} else {
            neighbors = new_neighbors;
            //}
            //cout << "c" << endl;
            //printClique(clique);
            //cout << "n" << endl;
            //printClique(neighbors);
            int gg;
            //cin >> gg;
        }
        return clique;
    }

    int vertexCount_;
    int edgeCount_;
    int colorCount_;
    vector<int> neighborsCount_;
    AdjacencyMatrix adjacencyMatrix_;

};

pair<Vertex, Color> vertexWithGreatestColor(ColorSets colors) {
    return pair<Vertex, Color>(colors[colors.size()-1][0], colors.size()-1);
}

Clique currClique;
Clique maxClique;
int theta = 4;


ColorSets reusingColors(const Graph & graph,
                        vector<Vertex> candidates, ColorSets colors) {
   // cout << "prev color" << endl;
   // for(int i = 0; i < colors.size(); i++) {
   //     printClique(colors[i]);
   // }
    ColorSets newColors(colors.size());
    for(int i = 0; i < colors.size(); i++) {
        newColors[i] = colors[i] & candidates;
    }
    // remove empty colors
    //cout << "colors" << endl;


    newColors.erase(remove_if(newColors.begin(), newColors.end(),
                              [](const vector<Vertex>& v) {return v.size() == 0;}), newColors.end());
    //cout << "colors" << endl;
    //for(int i = 0; i < newColors.size(); i++) {
    //    printClique(newColors[i]);
    //}
    ColorSets resultColors;
    for(int i = 0; i < newColors.size(); i++) {
        if(newColors[i].size() == 1) {
            Vertex w = newColors[i][0];
            bool canRepaint = false;
            for(int j = 0; j < newColors.size(); j++) {
                if((newColors[j] & graph.getNeighbors(w)).empty() && i!=j) {
                    newColors[j].push_back(w);
                    canRepaint = true;
                    break;
                }
            }
            if(!canRepaint){
                resultColors.push_back(newColors[i]);
            }
        } else {
            resultColors.push_back(newColors[i]);
        }
    }
    sort(resultColors.begin(), resultColors.end(),
         [](const vector<Vertex> &v1, const vector<Vertex> &v2){return v1.size()>v2.size();});

    return resultColors;
}

ColorSets recoloring(const Graph & graph,
                     vector<Vertex> candidates) {
    return {};
}

void findMaxClique(const Graph & graph,
                   vector<Vertex> candidates, ColorSets colors) {
    int i;
    // cin >> i;
    //cout << maxClique.size() << endl;
    //cout << " max clique start candidates:";
    //printClique(candidates);
    //cout << "max clique colors" << endl;
    //for(int i = 0; i < colors.size(); i++) {
    //    printClique(colors[i]);
   // }
    while(!candidates.empty()) {
        pair<Vertex, Color> vertexAndColor = vertexWithGreatestColor(colors);
        Vertex v = vertexAndColor.first;
        int vColor = vertexAndColor.second;
        //cout<< "use vertex: " << v <<endl;
        if(currClique.size()+ vColor <= maxClique.size()) return;
        currClique = currClique | v;
        vector<Vertex> newCandidates = candidates & graph.getNeighbors(v);

        if(!newCandidates.empty()) {
            ColorSets newColors;
            if(currClique.size()-1+ vColor <= maxClique.size()+theta) {
            //    printClique(newCandidates);
              //  cout << " start reusing" << endl;

                newColors = reusingColors(graph, newCandidates, colors);
                //cout << "reusing end" << endl;
            } else {
                //cout << "start color vertices" << endl;
                newColors = graph.colorVertices(newCandidates);//recoloring(graph, newCandidates);
                //cout << "new colors" << endl;
                //for(int i = 0; i < newColors.size(); i++) {
                //    printClique(newColors[i]);
               // }
               // cout << "end color vertices" << endl;
            }
            //cout << "max begin" << endl;

            findMaxClique(graph, newCandidates, newColors);
            //cout << "max end" << endl;
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
    maxClique  = {};
    vector<Vertex> candidates = graph.getVertices();
    ColorSets colors = graph.colorVertices(candidates);
    findMaxClique(graph, candidates, colors);
}

int main(int argc, char** argv) {
    Graph graph(argv[1]);
    findMaxClique(graph);
    printClique(maxClique);
    cout << maxClique.size();
    //printClique(maxClique);
    return 0;
}
