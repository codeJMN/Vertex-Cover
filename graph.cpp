// #pragma once
#include <algorithm> 
#include <climits>
#include <iostream>
#include <list>
// #include <math.h>
#include <memory> // defined std::unique_ptr
#include <sstream>
#include <vector>



// defines Var and Lit
#include "minisat/core/SolverTypes.h"
// defines Solver
#include "minisat/core/Solver.h"


#include "ece650-proj.hpp"

bool verbose2 = false;

Graph::Graph() : m_num_vertices(0) {}
void Graph::set_num_vertices(int t_num_vertices) { m_num_vertices = t_num_vertices;}
int Graph::get_num_vertices() { return m_num_vertices; }

bool Graph::ini_adjacency_matrix() {
    std::vector<int> tem(m_num_vertices, m_num_vertices*m_num_vertices);
    m_adj_mat.assign(m_num_vertices, tem);
    if (verbose2){
        std::cout << "Your initialized adjacency matrix: \n";
        for (std::vector<int> &i : m_adj_mat) {
            std::vector<int> vec = i;
            for (int j : vec) {
                std::cout << j << " ";
            }
        std::cout << std::endl;
        }
    }
    return true;
}

void Graph::set_edges(std::list<unsigned> &t_edges) { m_edges = t_edges;}

bool Graph::adjacency_matrix(std::string &err_msg) {
    int num1;
    int num2;
    std::list<unsigned> nums = m_edges;
    int V = m_num_vertices;
    ini_adjacency_matrix();
    if (nums.size() % 2 == 0){
        int nums_size = nums.size();
        for (int i = 0; i != (nums_size/2); i++){

            num1 = nums.front();
            nums.pop_front();
        

            num2 = nums.front();
            nums.pop_front();

            if (verbose2){
                std::cout << "num1: " << num1 << "num2: " << num2 << std::endl;
            }

            if (num1 < V && num2 < V){
                m_adj_mat[num1][num2] = 1;
                m_adj_mat[num2][num1] = 1;
            } else {
                err_msg = "Vertices out of bounds";
                return false;
            } 
        }  
        if (verbose2) {

            std::cout << "Your populated adjacency matrix: \n";
            for (std::vector<int> &i : m_adj_mat) {
                std::vector<int> vec = i;
                for (int j : vec) {
                    std::cout << j << " ";
                }
            std::cout << std::endl;
            }
        } 
          
        return true;
        
    } 
    err_msg = "Missing argument for vertices";
    return false;
}

bool shortest_path (std::vector<int> prev, int source, int target, std::vector<unsigned> &short_path, std::string &err_msg){
    int u = target;

    if (verbose2) {
        std::cout << "Entered shortest_path" << std::endl;
    }
    
    if (prev[u] != -1 || u == source){
        while (u != -1){
            short_path.push_back(u);
            u = prev[u];
        }
        if (verbose2) {
            std::cout << "Short Path:" << std::endl;
            for (int i : short_path) {
                std::cout << i << " ";
            }
            std::cout << std::endl;
        }
        return true;
    } 
    err_msg = "Unreachable vertex";
    return false;
    
}

bool Graph::dijkstra( std::list<unsigned> coordinates, std::vector<unsigned> &short_path, std::string &err_msg){

    if (coordinates.size() == 2) { 

        int source  = coordinates.front();
        coordinates.pop_front();


        int target = coordinates.front();
        coordinates.pop_front();

        std::list<unsigned> nums = m_edges;
        int V = get_num_vertices();
        if (source > V || target > V) {
            err_msg = "error: vertices out of bounds";
            return false;
        }

        if (source == target) {
            short_path.push_back(target);
            short_path.push_back(source);
            return true;
        }

        std::list<unsigned> q;
        std::vector<int> dist(V, V*V);
        std::vector<int> prev(V, -1);

        if (verbose2) {
            std::cout << "nums.begin(): " << *nums.begin() << " nums.end(): " << *nums.end() << std::endl;
        }

        // for (auto i = nums.begin(); i != nums.end(); ++i){

        //     dist[*i] = INT_MAX;
        //     prev[*i] = -1;
        //     q.push_back(*i);
        
        //     if (verbose2) {
        //         std::cout << "dijkstra" << std::endl;
        //         std::cout << "i: "<< *i << std::endl;
        //     }
        // }

        for ( int i = 0; i < V; i++){
            q.push_back(i);
        }

        // q.push_back(source);

        dist.at(source) = 0;

        if (verbose2) {
            std::cout << "dist with source " << source <<  " = 0 \n" << std::endl;
        }

        int u = 0;
        while (!q.empty()) {
            int distance = INT_MAX;
            

            for (int i : q){
                if (dist[i] < distance) {
                    distance = dist[i];
                    u = i;
                }
            }

            if (verbose2) {
                std::cout << "assigned u = " << u << " as the one with the shortest distance to source: " << u << std::endl;
            }

            if (u == target){

                break;
            }

            q.remove(u);

            if (verbose2) {
                std::cout << "removed u = " << u << "from q" << std::endl;
            }

            int alt;
            if (verbose2) {
                std::cout << "nums.begin(): " << *nums.begin() << " nums.edn(): " << *nums.end() << " u: " << u << std::endl;
            }

            for (int i : q){
                alt = dist[u] + m_adj_mat[u][i];
                if (alt < dist[i]){
                    dist.at(i) = alt;
                    prev.at(i) = u;
                }

            }
            if (verbose2) {
                std::cout << "New q using u: " << u << " are " << std::endl;
                for (int i : q){
                    std::cout << i << " ";
                }
                std::cout << std::endl;

                 std::cout << "New dist using u: " << u << " are " << std::endl;
                for (int i : dist){
                    std::cout << i << " ";
                }
                std::cout << std::endl;               


                std::cout << "New prev using u: " << u << " are " << std::endl;
                for (int i : prev){
                    std::cout << i << " ";
                }
                std::cout << std::endl;
            }
        }

        if (verbose2) {
            std::cout << "Ended calculating dist and prev" << std::endl;
        }
        
        if (verbose2) {
            std::cout << "Output prev " << std::endl;
            for (int i : prev) {
                std::cout << i << " ";
            }
        }
        return (shortest_path(prev, source, target, short_path, err_msg));
    }
    err_msg = "Missing or Bad argument for s";
    return false;
}

// Inspired by https://codingnest.com/modern-sat-solvers-fast-neat-underused-part-1-of-n/
Minisat::Var toVar(int value) {
    return value-1;
}

std::vector<int> Graph::vertex_cover(){
    bool satisfiable = false;
    int V = m_num_vertices;
    int k = 0;

    std::unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
    Minisat::vec<Minisat::Lit> literals_vector;
    bool res = false;

    std::vector<int> res_vec;

    while (!satisfiable && k<=V){
        k ++;
        // the next line de-allocates existing solver and allocates a new
        // one in its place.



        solver.reset (new Minisat::Solver());

        // Not possible because we are compiling in C++11. Check https://en.cppreference.com/w/cpp/memory/unique_ptr/make_unique
        // solver = std::make_unique<Minisat::Solver()>;





        // vec with V times zeros
        std::vector<int> vec;
        
        for (int i = 0; i < k*V ; ++i) {
        solver->newVar();
        }
        

        // First Clause
        if (verbose2){
            std::cout << "Clause 1" << std::endl;
        }
        for (int i = 1; i <= k ; ++i){
            for (int j = 1; j <= V; ++j){
                literals_vector.push(Minisat::mkLit(toVar((j*k)-(k-i))));
                vec.push_back((j*k)-(k-i));
            }
            solver->addClause(literals_vector);
            literals_vector.clear();

            if (verbose2){
                // for (auto i = vec.begin(); i != vec.end(); ++i){
                // // str += "<";
                // // str += std::to_string(*i);
                // std::cout << *i;
                // }
                // std::cout << std::endl;
                // vec.clear();
            }
        }

        // Second Clause
        if (verbose2){
            std::cout << "Clause 2" << std::endl;
        }
        vec.clear();
        for (int j = 1; j <= V; ++j){
            for (int i = 1; i < k ; ++i){            
                int temp = (j*k)-(k-i);
                for (int l = temp+1; l <= j*k; l++){
                    literals_vector.push(~Minisat::mkLit(toVar(temp)));
                    vec.push_back(0-temp);
                    literals_vector.push(~Minisat::mkLit(toVar(l)));
                    vec.push_back(0-l);
                    
                    solver->addClause(literals_vector);
                    literals_vector.clear();

                    if (verbose2){
                        // std::cout << "temp, l " << temp << l << std::endl;
                        for (unsigned int i = 0; i != vec.size(); ++i){
                            std::cout << vec[i];
                        }
                        // solver->toDimacs();
                        std::cout << std::endl;
                        vec.clear();
                    }                      
                }
            }
        }


        // Third clause
        if (verbose2){
            std::cout << "Clause 3" << std::endl;
        }
        vec.clear();
        for (int i = 1; i <= k ; ++i){
            for (int j = 1; j < V; ++j){                        
                int temp = (j*k)-(k-i);
                for (int l = temp+k; l <= V*k; l+=k){
                    literals_vector.push(~Minisat::mkLit(toVar(temp)));
                    vec.push_back(-temp);
                    literals_vector.push(~Minisat::mkLit(toVar(l)));
                    vec.push_back(-l);

                    solver->addClause(literals_vector);
                    literals_vector.clear();

                    if (verbose2){
                        // for (auto i = vec.begin(); i != vec.end(); ++i){

                        // std::cout << *i;
                        // }
                        // std::cout << std::endl;
                        // vec.clear();
                    }                      
                }
            }
        }

        // Fourth clause
        vec.clear();
        if (verbose2){
            std::cout << "Clause 4" << std::endl;
        }
        for (unsigned int i = 0 ; i < m_adj_mat.size(); i++){
            for (unsigned int j = i; j < m_adj_mat.size(); j++){
                if (m_adj_mat[i][j] == 1){
                    for (int l = 0; l < k; l++){
                        literals_vector.push(Minisat::mkLit(toVar((i+1)*k-l)));
                        // std::cout << "c4 " << i*l << std::endl;
                        vec.push_back(((i+1)*k-l));
                        literals_vector.push(Minisat::mkLit(toVar((j+1)*k-l)));
                        vec.push_back(((j+1)*k-l));
                    }
                    solver->addClause(literals_vector);
                    literals_vector.clear();

                    if (verbose2){
                        for (unsigned int i = 0; i != vec.size(); ++i){
                            std::cout << vec[i];
                        }
                        std::cout << std::endl;
                        vec.clear();
                    }                     
                }
            }
        }
        
        res = solver->solve();

        if (verbose2) {
            std::cout << "res is: " << res << std::endl;
        }

        if (res){
            if (verbose2){
                std::cout << "k * V: " << k*V << std::endl;
            }
            for (int i = 1; i <= k*V; i++){
                float j = i;
                if (verbose2) {
                    std::cout << "k: " << k << "\tV: " << V << std::endl;
                    std::cout << "Checking vertice: " << j << "\t";
                }
                if (Minisat::toInt(solver->modelValue(toVar(i))) == 0){
                    // Minisat::toLbool(
                    res_vec.push_back(ceil(j/k)-1);
                    if (verbose2) {
                        std::cout << "\nVertice: " << i << "\tVertice ceil: " << ceil(j/k)-1 << std::endl;
                    }
                }

                if (verbose2) {
                    std::cout << "after the if" << std::endl;
                }
            }
            satisfiable = true;            
        }
    }
    return res_vec;
}

    /// Print vertex cover
bool Graph::print_vertex_cover(std::vector<int> vertex_cover){
    std::sort(vertex_cover.begin(), vertex_cover.end());

    for (int i : vertex_cover){
        std::cout << i << " " ;
    }
    std::cout << std::endl;
    return true;
}




