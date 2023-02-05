#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <random>

// This class represents a configuration of n queens on an n by n chess board.
// There are two publically available variables, the fitness score, which always falls between 0 and 100
// and the board itself, as this is dynamically allocated and needs to be freed

// There is an overloaded constructor. When only the size is input, it will randomly generate a configuration
// of a board sized n by n. It will also automatically calculate its fitness score.

// The second constructor will take in the size and the configuration of two other boards of the same size
// as well as a desired avg number of iterations per mutation.
// It will combine them by taking a section from both by a crossover point, and taking a side from each. There 
// should also be a random change every number of iterations given in the function.
// There are also commented out lines of code that call a function to print the board in each constructor
class Board {
    
    public:
        double fitnessScore; // To evaluate the ability of the board
        std::vector<int> chessBoard; // To represent the arrangement
        Board (int n){ // randomly generate board, and calculate fitness
            this->n = n;
            std::random_device rd;
            std::uniform_int_distribution<int> dist(0, n-1);
            for (int i = 0; i < n; i++){
                chessBoard.push_back(dist(rd));
            }
            fitness(chessBoard, n);
            //std::cout << safeQueens << std::endl;
            //printBoard(chessBoard, n);           
            //std::cout << fitnessScore << std::endl;
        }
        Board (int n, std::vector<int> board1, std::vector<int> board2, int avgTimesPerMutation){ // generate board from two others, chance of mutation
            this->n = n;
            std::random_device rd;
            std::uniform_int_distribution<int> dist(0, n-1);
            std::uniform_int_distribution<int> mutate(0, avgTimesPerMutation);
            int crossoverPoint = dist(rd);
            for (int i = 0; i < crossoverPoint; i++){
                if (mutate(rd) == 0){

                    chessBoard.push_back(dist(rd));
                    continue;
                }
                chessBoard.push_back(board1[i]);
            }
            for (int i = crossoverPoint; i < n; i++){
                if (mutate(rd) == 0){
                    chessBoard.push_back(dist(rd));
                    continue;
                }
                chessBoard.push_back(board2[i]);
            }
            fitness(chessBoard, n);
            //printBoard(chessBoard, n);
            //std::cout << fitnessScore << std::endl;
        }
        void printBoard(){ // function to print the board as an array
            for (int i = 0; i < n; i++){
                std::cout << chessBoard[i] << " ";
            }
            std::cout << std::endl;
        }
        
        void drawBoard(){ // To draw the board on the screen
            for (int i = 0; i < n; i++){ // Go through the rows
                printLine(); // Boundry between rows
                for (int j = 0; j < n; j++){ // Go through the columns, see if a space or a queen belongs at the row and column
                    queensAndSpaces(i, j);
                }
                nextRow(); // Goes to the next row
            }
            printLine(); // Boundry between rows
        } 
        bool operator <(Board obj) const { // For using std::sort to sort by fitness score in descending order
            return fitnessScore > obj.fitnessScore;
        }
    private:
        int n; // The size of the board and the number of queens 
        int safeQueens; // To help measure the fitness score, measures the number of queens not in a conflict
        
        bool conflict(std::vector<int> board, int i, int j){ // a function to see if two queens are in conflict
            if ((board[i] == board[j]) || (abs(i-j) == abs(board[i] - board[j]))){
                return true;
            } else {
                return false;
            }
        }
        bool inConflict (std::vector<int> board, int i, const int size){ // sees if the queen at index i of board is in conflict with any other queen in board
            for (int j = 0; j < size; j++){
                if (j == i){
                    continue;
                }
                if (conflict(board, i, j)){
                    return true;
                }
            }
            return false;
        }
        void numSafeQueens(std::vector<int> board, const int size){ // function to see how many queens are in conflict
            safeQueens = 0;
            for (int i = 0; i < size; i++){
                if (!inConflict(board, i, size)){
                    safeQueens++;
                }
            }
        }
        void fitness (std::vector<int> board, const int size){ // finds the fitness score
            numSafeQueens(board, size);
            fitnessScore = 100 * safeQueens / (double)size;
        }  
        
        // The following lines of code are used in the drawBoard() function
        void printLine(){
            std::cout << " ";
            for (int k = 0; k < n; k++){
                std::cout << "--- ";
            }
            std::cout << "\n";
        }
        void queensAndSpaces(int i, int j){
            std::cout << "|";
            if (chessBoard[i] == j){
                std::cout << " Q ";
            }else {
                std::cout << "   ";
            }
        }
        void nextRow(){
            std::cout << "|\n";
        }          
};


// The following class represents a population of Boards. Is made up of a vector of boards. Constructors automatically
// compute the fitness scores of each board using the constructors in the Board class. 
class Population {
    public:
        double bestFitness; // The highest fitness score in the population, so we know when we have found the solution, and can give the user updates
        int n; // The size of the board / # Queens
        std::vector<Board> population; // Vector to represent the population
        std::vector<Board> fittest; // Vector to represent the members of the population that will go on to the next generation


        // This constructor will randomly generate a population with "size" members of candiate solutions to the n queens problem
        Population(int size, int n){
            this->size = size;
            this->n = n;
            for (int i = 0; i < size; i++){
                population.push_back(Board(n));
            }
            std::sort(population.begin(), population.end());            
            bestFitness = population[0].fitnessScore;
            for (int i = 0; i < size/2; i++){
                fittest.push_back(population[i]);
            }
        }
        
        // This constructor will combine survivors of the previous generation (and randomly mutate) to create "size" candidate solutions to the n queens problem
        Population(int size, int n, std::vector<Board> survivors, int avgTimesPerMutation){
            this->size = size;
            this->n = n;
            for (int i = 0; i < 3; i++){
                population.push_back(survivors[i]);
            }
            for (int i = 1; i < size/2; i++){
                population.push_back(Board(n, survivors[0].chessBoard,survivors[i].chessBoard, avgTimesPerMutation));
            }
            for (int i = 2; i < size/2; i++){
                population.push_back(Board(n, survivors[1].chessBoard, survivors[i].chessBoard, avgTimesPerMutation));
            }
            std::sort(population.begin(), population.end());
            bestFitness = population[0].fitnessScore;
            for (int i = 0; i < size/2; i++){
                fittest.push_back(population[i]);
            }
        }
         
    private:
        int size; // The size of the population

        // The following functions were just used for debugging purposes
        void printBest(){
            std::sort(population.begin(), population.end());
            for (int i = 0; i < n; i++){  
                std::cout << population[0].chessBoard[i] << " ";
            }
            std::cout << std::endl;
        }
        void printFittest(){
            for (int i = 0; i < 5; i++){
                fittest[i].printBoard();
                std::cout << fittest[i].fitnessScore << std::endl;
            }
        }
        void printPopulation(){
            for (int i = 0; i < size; i++){
                population[i].printBoard();
                std::cout << population[i].fitnessScore << std::endl;
            }
        }
        
};

int main(){

    // Take in user input
    int populationSize, n, avgTimesPerMutation, numGenerations, updatePeriod;
    double probMutation;
    
    std::cout << "Here you will use a genetic algorithm to solve the N-Queens Problem. For how many queens do you want to find a solution?" << std::endl;
    std::cin >> n;

    std::cout << "How large would you like the population of each generation to be?" << std::endl;
    std::cin >> populationSize;

    std::cout << "What should the probablity of a row mutating be? (above zero, less than or equal to 1)" << std::endl;
    std::cin >> probMutation;

    std::cout << "What is the maximum number of generations you would like to run?" << std::endl;
    std::cin >> numGenerations;

    std::cout << "How many generations would you like to happen between each update?" << std::endl;
    std::cin >> updatePeriod;

    avgTimesPerMutation = 1 / probMutation;

    // Create initial Population
    Population pop(populationSize, n);

    int i = 0;
    
    std::cout << "Best Starting Board (of randomly generated population):" << std::endl;
    pop.population[0].drawBoard();

    std::cout << "Associated Starting Score: " << pop.population[0].fitnessScore << std::endl << std::endl;
   
    // Run Genetic Algorithm
    while(pop.bestFitness < 100 && i < numGenerations){
        i++;
        Population nextGen(populationSize, n, pop.fittest, avgTimesPerMutation);
        pop = nextGen;
        if (i % updatePeriod == 0){
            std::cout << "Best Board So Far: " << std::endl;
            nextGen.population[0].drawBoard();
            std::cout << "And Associated Fitness: " << nextGen.bestFitness << std::endl;  
            std::cout << std::endl << std::endl << std::endl;     
        }
    }
    std::cout << "Generations Done: " << i << std::endl;
    if (pop.bestFitness == 100){
        std::cout << "Success!!! Best Achieved: " << pop.bestFitness << "%" << std::endl;
        std::cout << "Here is your solution: " << std::endl;
    }else {
        std::cout << "Not quite! Best Achieved: " << pop.bestFitness << "%" << std::endl;
        std::cout << "Maybe try again. Sometimes it gets stuck at a local maximum" << std::endl;
        std::cout << "If that doesn't work, try changing the parameters you gave" << std::endl;
        std::cout << "Here is the closest solution we found: " << std::endl;
    }
    
    pop.population[0].drawBoard();
    return 0;
}