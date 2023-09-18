#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <string>
#include <algorithm>
#include <random>
#include <climits>

class Set
{
public:
    int index;
    int distance;

    Set(int index = 0, int distance = INT_MAX) : index(index), distance(distance) {}

    std::string str()
    {
        return "i: " + std::to_string(index) + " d: " + std::to_string(distance);
    }
};

class Solution
{
public:
    std::vector<Set> sets;
    int total_distance;

    Solution(std::vector<Set> sets = {}, int total_distance = 0) : sets(sets), total_distance(total_distance) {}

    std::string str()
    {
        return "total distance: " + std::to_string(total_distance) + ", n_candidates: " + std::to_string(sets.size());
    }
};

int size = 0;
std::vector<int> x;
std::vector<int> y;
std::string _type = "";
std::vector<std::vector<int>> distance_matrix;
std::vector<Solution> solutions; // Define solutions globally

void read_file(const std::string &filename)
{
    // ... (unchanged)
}

void get_solutions()
{
    solutions.resize(size);

    for (int i = 0; i < size; ++i)
    {
        int count = 1;
        std::vector<bool> visited(size, false);
        visited[i] = true;
        int k = i;

        while (count < size)
        {
            Set best;
            best.distance = INT_MAX;
            for (int j = 0; j < size; ++j)
            {
                if (distance_matrix[k][j] < best.distance && !visited[j])
                {
                    best.distance = distance_matrix[k][j];
                    best.index = j;
                }
            }
            solutions[i].sets.push_back(best);
            solutions[i].total_distance += best.distance;
            k = best.index;
            visited[k] = true;
            count++;
        }

        Set return_city(i, distance_matrix[k][i]);
        solutions[i].sets.push_back(return_city);
        solutions[i].total_distance += return_city.distance;
    }
}

Solution update(Solution solution, int index, std::vector<int> _random)
{
    int update_tour = 0;
    for (int i = 0; i < size - 1; ++i)
    {
        update_tour += distance_matrix[solution.sets[i].index][solution.sets[i + 1].index];
    }
    solution.total_distance = update_tour;
    return solution;
}

Solution tweak(Solution solution)
{
    int a = rand() % (size - 2);
    int b = rand() % (size - 2);
    while (a == b)
    {
        b = rand() % (size - 2);
    }
    std::swap(solution.sets[a], solution.sets[b]);
    return solution;
}

void simulated_annealing(int index)
{
    int step = 1;
    int max_steps = size * 1000000000;
    double energy = 200;

    // Generate a and b within proper range
    int a = rand() % (size - 1);
    int b = rand() % (size - 1);
    if (a >= b)
        std::swap(a, b);

    Solution initial_solution = solutions[index];
    Solution best_solution = initial_solution;

    while (step < max_steps)
    {
        ++step;
        if (energy <= 0)
        {
            break;
        }

        Solution solution = tweak(initial_solution);
        Solution new_solution = update(solution, index, {a, b});
        int result1 = new_solution.total_distance;
        int result2 = initial_solution.total_distance;
        double result3 = static_cast<double>(rand()) / RAND_MAX;
        int res = result2 - result1;
        double result4 = exp(static_cast<double>(res) / energy);

        if (result1 < result2 || result3 < result4)
        {
            initial_solution = new_solution;
        }

        energy -= step / (1 + 0.8 * step);

        if (best_solution.total_distance > initial_solution.total_distance)
        {
            best_solution = initial_solution;
        }
    }

    solutions[index] = best_solution;
}

void print_tela(int index)
{
    std::cout << "Solution " << index << ": ";
    for (int k = 0; k < size; ++k)
    {
        std::cout << solutions[index].sets[k].index << " ";
    }
    std::cout << "total: " << solutions[index].total_distance << std::endl;
}

int calculate_tour_distance(std::vector<int> _tour)
{
    int dist = 0;
    for (int i = 0; i < size - 1; ++i)
    {
        dist += distance_matrix[_tour[i]][_tour[i + 1]];
    }
    dist += distance_matrix[_tour[size - 1]][_tour[0]];
    return dist;
}

int main()
{
    std::string filename = "path_to_your_tsp_fileeil51.tsp";  // Change this to the actual path
    std::cout << filename << std::endl;
    read_file(filename);

    std::vector<int> tour(size);
    for (int i = 0; i < size; ++i)
        tour[i] = i;

    std::cout << "Route length: " << calculate_tour_distance(tour) << std::endl;
    solutions.resize(size);
    get_solutions();

    std::cout << "Greedy Algorithm" << std::endl;
    Solution best_solution_greedy = solutions[0];
    for (int i = 0; i < size; ++i)
    {
        int distance = solutions[i].total_distance;
        if (distance < best_solution_greedy.total_distance)
            best_solution_greedy = solutions[i];
    }

    std::cout << "Best Solution " << best_solution_greedy.total_distance << ": ";
    for (int j = 0; j < size; ++j)
        std::cout << best_solution_greedy.sets[j].index << " ";
    std::cout << std::endl;

    std::vector<Solution> solutions_sa(size);
    std::cout << "Simulated Annealing" << std::endl;

    for (int i = 0; i < size; ++i)
        simulated_annealing(i);

    Solution best_solution_sa = solutions_sa[0];
    for (int i = 0; i < size; ++i)
    {
        int distance = solutions_sa[i].total_distance;
        if (distance < best_solution_sa.total_distance)
            best_solution_sa = solutions_sa[i];
    }

    std::cout << "Best Solution " << best_solution_sa.total_distance << ": ";
    for (int j = 0; j < size; ++j)
        std::cout << best_solution_sa.sets[j].index << " ";
    std::cout << std::endl;

    return 0;
}
