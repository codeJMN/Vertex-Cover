/** Main file */
#include <algorithm> 
// #include <climits>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <vector>

// defined std::unique_ptr
#include <memory>
// defines Var and Lit
#include "minisat/core/SolverTypes.h"
// defines Solver
#include "minisat/core/Solver.h"

#include "ece650-proj.hpp"

bool verbose = false;

void * thread_cnf(void *);
void * thread_approx1(void *);
void * thread_approx2(void *);
void * thread_out(void *);

std::unique_ptr<Minisat::Solver> solver;
std::vector<std::vector<int>> adjacency_matrix;

// Inspired by https://codingnest.com/modern-sat-solvers-fast-neat-underused-part-1-of-n/
Minisat::Var toVar(int value) {
    return value-1;
}

bool k_vertex_cover(int k, int V, std::vector<std::vector<int>> adj_mat_mod, std::vector<int> &res_vec){
    // int V = m_num_vertices;

    std::unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
    Minisat::vec<Minisat::Lit> literals_vector;
    // boolean that indicates satisfiability
    bool res = false;

    // vector with the satisfiable solution
    // std::vector<int> res_vec;
    solver.reset (new Minisat::Solver());

        // Not possible because we are compiling in C++11. Check https://en.cppreference.com/w/cpp/memory/unique_ptr/make_unique
        // solver = std::make_unique<Minisat::Solver()>;

    // vec for verbose
    std::vector<int> vec;
    
    for (int i = 0; i < k*V ; ++i) {
    solver->newVar();
    }
    
    // First Clause
    if (verbose){
        std::cout << "Clause 1" << std::endl;
    }
    for (int i = 1; i <= k ; ++i){
        for (int j = 1; j <= V; ++j){
            literals_vector.push(Minisat::mkLit(toVar((j*k)-(k-i))));
            vec.push_back((j*k)-(k-i));
        }
        solver->addClause(literals_vector);
        literals_vector.clear();

        if (verbose){
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
    if (verbose){
        std::cout << "Clause 2" << std::endl;
    }
    vec.clear();
    for (int j = 1; j <= V; ++j){
        for (int i = 1; i < k ; ++i){            
            int temp = (j*k)-(k-i);
            for (int l = temp+1; l <= j*k; ++l){
                literals_vector.push(~Minisat::mkLit(toVar(temp)));
                vec.push_back(0-temp);
                literals_vector.push(~Minisat::mkLit(toVar(l)));
                vec.push_back(0-l);
                
                solver->addClause(literals_vector);
                literals_vector.clear();

                if (verbose){
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
    if (verbose){
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

                if (verbose){
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
    if (verbose){
        std::cout << "Clause 4" << std::endl;
    }
    for (unsigned int i = 0 ; i < adj_mat_mod.size(); ++i){
        for (unsigned int j = i; j < adj_mat_mod.size(); ++j){
            if (adj_mat_mod[i][j] == 1){
                for (int l = 0; l < k; ++l){
                    literals_vector.push(Minisat::mkLit(toVar((i+1)*k-l)));
                    // std::cout << "c4 " << i*l << std::endl;
                    vec.push_back(((i+1)*k-l));
                    literals_vector.push(Minisat::mkLit(toVar((j+1)*k-l)));
                    vec.push_back(((j+1)*k-l));
                }
                solver->addClause(literals_vector);
                literals_vector.clear();

                if (verbose){
                    for (unsigned int i = 0; i != vec.size(); ++i){
                        std::cout << vec[i];
                    }
                    std::cout << std::endl;
                    vec.clear();
                }                     
            }
        }
    }
    
    Minisat::lbool res2;
    res2 = solver->solve();

    if (verbose) {
        std::cout << "res is: " << res << std::endl;
    }

    if (res){
        if (verbose){
            std::cout << "k * V: " << k*V << std::endl;
        }
        for (int i = 1; i <= k*V; ++i){
            float j = i;
            if (verbose) {
                std::cout << "k: " << k << "\tV: " << V << std::endl;
                std::cout << "Checking vertice: " << j << "\t";
            }
            if (Minisat::toInt(solver->modelValue(toVar(i))) == 0){
                // Minisat::toLbool(
                res_vec.push_back(ceil(j/k)-1);
                if (verbose) {
                    std::cout << "\nVertice: " << i << "\tVertice ceil: " << ceil(j/k)-1 << std::endl;
                }
            }

            if (verbose) {
                std::cout << "after the if" << std::endl;
            }
        }   
        return true;         
    }
    return false;
}

void * vertex_cover(void * res_vec_VC){
    bool satisfiable = false;

    int V = adjacency_matrix.size();

    int k = 0;

    std::unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
    Minisat::vec<Minisat::Lit> literals_vector;
    // bool res = false;

    std::vector<int> res_vec;
    
    // std::vector<int> res_vec_approx_2;

    while (!satisfiable && k<V){
        ++k;
        // the next line de-allocates existing solver and allocates a new
        // one in its place.
        
        if (k_vertex_cover(k, V, adjacency_matrix, res_vec)){
            satisfiable = true;
        }
    }
    *((std::vector<int>*)res_vec_VC) = res_vec; //https://www.geeksforgeeks.org/void-pointer-c-cpp/
    return NULL;
}

// Calculates the degree of each vertex
void get_vertex_degree(std::vector<std::vector<int>> &adj_mat){
    int degree = 0;
    for (unsigned i = 0; i < adj_mat[0].size(); ++i){
        for (unsigned j = 0; j < adj_mat.size()-2; ++j){
            degree = degree + adj_mat[j][i];
        }
        adj_mat[adj_mat.size()-1][i] = degree;
        degree = 0 ;
    }
}

// Adds a row with the index of each vertes and a row with the degree of each vertex
void fix_adj_mat(std::vector<std::vector<int>> &adj_mat_mod, int V){
    // std::vector<std::vector<int>> adj_mat_mod = m_adj_mat;
    std::vector<int> ver_num;
    std::vector<int> degree(V,0);

    for (unsigned i = 0; i < adj_mat_mod.size() ; ++i) {
        ver_num.push_back(i);
    }

    // Add a row that accounts for the vertex number
    adj_mat_mod.push_back(ver_num);

    // Add a row that accounts for the degree of the vertex
    adj_mat_mod.push_back(degree);

    // Calculate the degree of the adjaccency matrix
    get_vertex_degree(adj_mat_mod);
}

// inspired on http://www.cplusplus.com/forum/beginner/31363/#msg169817
// function to delete a column
bool delete_column (std::vector<std::vector<int>> &adj_mat_mod, unsigned column_to_delete){
    // unsigned columnToDelete = 2;
    bool is_column_deleted = false;
    for (unsigned i = 0; i < adj_mat_mod.size(); ++i){
        if (adj_mat_mod[i].size() > column_to_delete){
            adj_mat_mod[i].erase(adj_mat_mod[i].begin() + column_to_delete);
            is_column_deleted = true;
        }
    }
    return is_column_deleted;
}

// inspired on http://www.cplusplus.com/forum/beginner/31363/#msg169817
// function to delete a row
bool delete_row (std::vector<std::vector<int>> &adj_mat_mod, unsigned row_to_delete){
    bool is_row_deleted = false;
    if (adj_mat_mod.size() > row_to_delete){
        adj_mat_mod.erase( adj_mat_mod.begin() + row_to_delete );
        is_row_deleted = true;
    }
    return is_row_deleted;
}

int get_graph_degree(std::vector<std::vector<int>> &adj_mat_mod){
    int graph_degree = 0;
    for (int i:adj_mat_mod[adj_mat_mod.size()-1]){
        graph_degree = graph_degree + i;
    }
    return graph_degree;
}

// for debugging
void print_mat(std::vector<std::vector<int>> test_mat){
    for (std::vector<int> i:test_mat){
        for (int j : i){
            std::cout << j << " ";
        }
        std::cout << "\n";

    }
}

/// Get Approx-VC-1
void * approx_VC_1(void * res_vec_approx_1){
    // std::vector<int> res_vec_approx_1;
    std::vector<std::vector<int>> adj_mat_mod = adjacency_matrix;
    int V = adjacency_matrix.size();
    fix_adj_mat(adj_mat_mod, V);
    // print_mat(adj_mat_mod);

    int graph_degree = get_graph_degree(adj_mat_mod);
    int max_degree_vertex;
    int max_element_index;
    std::vector<int> res_vec;
    // int max;
    unsigned int tries = 0;
    // int initial_adj_mat_size = adj_mat_mod.size();

    if (verbose){
        std::cout << "inside approx_1" << std::endl;
    }

    while (graph_degree !=0){
        ++tries;
        if (tries > adj_mat_mod.size()-2){
            std::cerr << "Error: Inside approx_VC_1. Too many tries" << std::endl;
            // return res_vec_approx_1;
        }
        // max = *max_element(adj_mat_mod[adj_mat_mod.size()-1].begin(), adj_mat_mod[adj_mat_mod.size()-1].end());
        // Inspired by https://riptutorial.com/cplusplus/example/11151/find-max-and-min-element-and-respective-index-in-a-vector
        max_element_index = std::max_element(adj_mat_mod[adj_mat_mod.size()-1].begin(), adj_mat_mod[adj_mat_mod.size()-1].end()) - adj_mat_mod[adj_mat_mod.size()-1].begin();

        max_degree_vertex = adj_mat_mod[adj_mat_mod.size()-2][max_element_index];

        res_vec.push_back(max_degree_vertex);
        if (verbose) {
            std::cout << "pushed index " << max_degree_vertex << "into res_vec_approx_1" << std::endl;
        }

        // iterate over the max_element column and erase the rows that are != 0

        for (unsigned i = 0; i < adj_mat_mod.size()-2; ++i){
            if (adj_mat_mod[i][max_element_index] != 0){
                adj_mat_mod[i][max_element_index] = 0;
                adj_mat_mod[max_element_index][i] = 0;

                // if(delete_row(adj_mat_mod, i)){
                //     if(delete_column(adj_mat_mod, i)){
                //         --max_element_index;                    
                //         --i;}
                //     else{
                //         std::cout << "Error inside approx_VC_1 deleting a column" << std::endl;
                //     }
                // }
                // else{
                //     std::cout<< "Error inside approx_VC_1 deleting a row" << std::endl;
                // }
            } 
        }

        get_vertex_degree(adj_mat_mod);
        // print_mat(adj_mat_mod);
        graph_degree = get_graph_degree(adj_mat_mod);
        
        // std::cout << "graph_degree: " << graph_degree<< std::endl;
        
    }
    *((std::vector<int>*)res_vec_approx_1) = res_vec; // https://www.geeksforgeeks.org/void-pointer-c-cpp/
    return NULL;
}

std::array<int, 2> edge_pick (std::vector<std::vector<int>> &adj_mat_mod){
    std::array<int, 2> edge_picked = {0,0};
    for (unsigned i = 0; i < adj_mat_mod.size()-2; ++i){
        for (unsigned j = 0; j < adj_mat_mod.size()-2; ++j){
            if (adj_mat_mod[i][j]==1){
                edge_picked = {(int)i, (int)j};
                // std::cout << "edge_picked" << i << " " << j << std::endl;
                return edge_picked;
            }
        }
    }
    std::cout << "Error in edge_picked" << std::endl;
    return edge_picked;
}

/// Get Approx-VC-2
void * approx_VC_2(void * res_vec_approx_2){
    if (verbose){
        std::cout << "inside approx_2" << std::endl;
    }

    int V = adjacency_matrix.size();
    std::vector<int> res_vec;

    // std::vector<int> res_vec_approx_2;
    std::vector<std::vector<int>> adj_mat_mod = adjacency_matrix;

    fix_adj_mat(adj_mat_mod, V);

    unsigned int tries = 0;

    std::array<int, 2> edge_picked = {0,0};

    int graph_degree = get_graph_degree(adj_mat_mod);
    while (graph_degree !=0){
        ++tries;
        if (tries > adj_mat_mod.size()-2){
            std::cerr << "Error: Inside approx_VC_2. Too many tries" << std::endl;
            // return res_vec_approx_2;
        }
        edge_picked = edge_pick(adj_mat_mod);

        res_vec.push_back(edge_picked[0]);
        res_vec.push_back(edge_picked[1]);

        for (int i : edge_picked){
            for (unsigned j = 0; j < adj_mat_mod[i].size(); ++j){
                if (adj_mat_mod[i][j] == 1){
                    adj_mat_mod[i][j] = 0;
                    adj_mat_mod[j][i] = 0;
                }
            }
        }
        // std::cout << "\n";
        // print_mat(adj_mat_mod);
        get_vertex_degree(adj_mat_mod);

        graph_degree = get_graph_degree(adj_mat_mod);
    }    

    *((std::vector<int>*)res_vec_approx_2) = res_vec;
    return NULL;

}

void error (const std::string &err_msg){
    std::cout << "Error: " << err_msg << std::endl;
}

int main(
    // int argc, char** argv
    ) {

    //Checking arguments that were used when running the code
    // for (int i = 0; i < argc; ++i) {
    //     std::string arg = argv[i];
    //     if (arg == "--verbose"|| arg == "-v") {//Check if the verbose flag was indicated
    //         verbose = true; //If it was indicated, set the verbose to true
    //     }
    // }

    // Graph graph;
    Graph graph;

    std::string err_msg;
    char cmd;



    // read from stdin until EOF
    while (!std::cin.eof()) {

        // read a line of input until EOL and store in a string
        std::string line;
        std::getline(std::cin, line);

        // vector nums carries the args from parse_line
        std::list<unsigned> nums;

        //list to store shortest path
        std::vector<unsigned> short_path;

        // if nothing was read, go to top of the while to check for eof
        if (line.empty()) {
            continue;
        }

        std::string msg;        
        if (parse_line(line, cmd, nums, err_msg)) {
            switch (cmd) {
            case 'V':
                if (nums.size() == 1){
                    int num = nums.front();
                    graph.set_num_vertices(num);
                    if (verbose){
                        std::cout << "Num of vertices stored: "<< graph.get_num_vertices() << std::endl;
                    }
                    // int V = graph.get_num_vertices();
                    if (!graph.ini_adjacency_matrix()){
                        err_msg = "Something broke when creating the adjacency matrix";
                        error(err_msg);
                    }
                    break;
                }        
                error("Unexpected argument for V");                
            case 'E':
                // set the edges of the graph. nums is a list: each two consecutive numbers are an edge.
                graph.set_edges(nums);
                
                // Call adjacency_matrix 
                if (graph.adjacency_matrix(err_msg)){
                    adjacency_matrix = graph.get_adj_mat();
                    std::vector<int> vertex_cover_cnf;
                    std::vector<int> vertex_cover_approx_1;
                    std::vector<int> vertex_cover_approx_2;
                    // if (graph.vertex_cover(vertex_cover)){              
                    
                    // }
                    // else {
                    //     std::cerr << "CNF-SAT-VC timed-out" << std::endl;
                    //     break;
                    // }

                    pthread_t thread_cnf;
                    pthread_create(&thread_cnf, NULL, vertex_cover, &vertex_cover_cnf);

                    pthread_t thread_approx1;
                    pthread_create(&thread_approx1, NULL, approx_VC_1, &vertex_cover_approx_1);

                    pthread_t thread_approx2;
                    pthread_create(&thread_approx2, NULL, approx_VC_2, &vertex_cover_approx_2);

                    pthread_join(thread_cnf, NULL);
                    pthread_join(thread_approx1, NULL);
                    pthread_join(thread_approx2, NULL);

                    // std::vector<int> vertex_cover_approx_1 = graph.approx_VC_1();
                    

                    // std::vector<int> vertex_cover_approx_2 = graph.approx_VC_2();  

                    msg = "CNF-SAT-VC: ";
                    graph.print_vertex_cover(msg, vertex_cover_cnf);

                    msg = "APPROX-VC-1: ";
                    graph.print_vertex_cover(msg, vertex_cover_approx_1);

                    msg = "APPROX-VC-2: ";
                    graph.print_vertex_cover(msg, vertex_cover_approx_2);

                } else {
                    error(err_msg);
                };
                
                break;
            case 's': 
                // Call dijkstra. nums contains the source and target. Short_path will return the shortest path.               
                // if (graph.dijkstra(nums, short_path, err_msg)){
                //     if(print_short_path(short_path)){
                //         break;
                //     } 
                //     error("There was a problem when printing the shortest path");
                //     break;
                // }
                // else{
                //     error("Something went wrong");
                // }
                std::cout << "The s command has been disabilitated. Please contact the adminsitrators: Natalia and Juan" << std::endl;
            default: error(err_msg);
            }
        } else {
            error(err_msg);
        }
    }
}

