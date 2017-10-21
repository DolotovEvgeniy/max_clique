#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <algorithm>
#include <functional>

using namespace std;

typedef int Vertex;
typedef vector<Vertex> Clique;
typedef vector< vector<char> > AdjacencyMatrix;

bool operator<(Clique a, Clique b) {
    return a.size() < b.size();
}
void printClique(const Clique &clique) {
    cout << "--------------" << endl;
    for (int i = 0; i < clique.size(); ++i) {
        cout << clique[i] << endl;
    }
    cout << "--------------" << endl;
}

class Graph {
public:
    Graph(const string &filename) {
        ifstream file(filename.c_str());
        string line;

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
        /*for (int i = 0; i < vertexCount_; ++i) {
            for (int j = 0; j < vertexCount_; j++) {
                cout << (int)adjacencyMatrix_[i][j];
            }
            cout << endl;
        }*/
    }
    bool isNeighbors(Vertex a, Vertex b) const {
        return adjacencyMatrix_[a][b] == 1;
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

private:
    int vertexCount_;
    int edgeCount_;
    AdjacencyMatrix adjacencyMatrix_;

};

Clique increaseToMaxClique(Clique clique, const Graph& graph) {
    Vertex maxVertex = *max_element(clique.begin(), clique.end());
    vector<Vertex> neighbors = graph.getNeighbors(clique);
    remove_if(neighbors.begin(), neighbors.end(), bind2nd(less<int>(), maxVertex));
    vector<Clique> cliques;
    cliques.push_back(clique);
    for (int i = 0; i < neighbors.size(); ++i) {
        Clique newClique = clique;
        newClique.push_back(neighbors[i]);
        if ( graph.isClique(newClique) ) {
            cliques.push_back(increaseToMaxClique(newClique, graph));
        }
    }
   cout << max_element(cliques.begin(), cliques.end())->size() << endl;
    return *max_element(cliques.begin(), cliques.end());
}

Clique findMaxClique(const Graph& graph) {
    vector<Clique> cliques;
    for (Vertex i = 0; i < graph.vertexCount(); ++i) {
        Clique clique;
        clique.push_back(i);
        cliques.push_back(increaseToMaxClique(clique, graph));
    }
    return *max_element(cliques.begin(), cliques.end());
}

int main(int argc, char** argv) {
    Graph graph(argv[1]);
    cout << findMaxClique(graph).size();
    return 0;
}
