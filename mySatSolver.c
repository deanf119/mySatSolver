/**
 * Lab #5
 * 
 * Developers:
 * 1) Dean Fernandes
 *      301274908
 *      deanf@sfu.ca
 * 
 * Purpose: Using simple backtracking to solve the SAT problem.
 * 
 * Tasks:
 * 
 * Input Counter    : Done : Tested
 * Input Reader     : Done : Tested
 * Clause Evaluator : Done : Tested
 * Guess Finder     :      :
 * Back Track       :      :
 * Main             :      :
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAX_STRING_LENGTH 10
static const char fileName[] = "dubois20.cnf"; //----------------------------------------------> Define Test File Name


void argumentInput_Initializer(int* numberVariable, int* numberClause, int* numberLiteral);
void argumentInput_Reader(int list_Clause[], int list_Literal[]);
bool clause_Evaluator(const bool list_Guess[], const int list_Clause[], const int list_Literal[], const int numberClause, const int list_Guess_Size);
bool guess_Finder(const int numberVariable, const int numberClause, bool list_Guess[], bool status_of_list_Guess[], const int list_Clause[], const int list_Literal[]);
void guess_BackTrack(bool list_Guess[], bool status_of_list_Guess[], int* myGuessVariable, int* list_Guess_Size);



int main(){

    int numberVariable = 0;
    int numberClause = 0;
    int numberLiteral = 0;
    
    argumentInput_Initializer( &numberVariable, &numberClause, &numberLiteral);

    int list_Clause[numberClause];
    int list_Literal[numberLiteral];

    argumentInput_Reader(list_Clause, list_Literal);


    //Setting up the solution test arrays
    bool list_Guess[numberVariable];
    bool status_of_list_Guess[numberVariable];
    for (int i = 0; i<numberVariable; i++){
        list_Guess[i] = true;
        status_of_list_Guess[i] = true;
    }

    bool finalResult;
    finalResult = guess_Finder(numberVariable, numberClause, list_Guess, status_of_list_Guess, list_Clause, list_Literal);

    if(finalResult== true){
        printf("The result is true \n");
        printf("The result is: \n");
        int index_for_print = 1;
        for(int a = 0; a < numberVariable; a++){
            printf("X%d : %s\n", index_for_print, list_Guess[a]? "true": "false");
            index_for_print++;
        }
    }else{
        printf("The result is false \n");
    }

    return 0;
}


/**
 * Function: argumentInput_Initializer
 * Purpose: Find number of all the literals in the file
 * Input: 1) File Name (global variable)
 * Output: 1)Number of different variables 2) Number of clauses 3) Number of literals 
*/
void argumentInput_Initializer(int* numberVariable, int* numberClause, int* numberLiteral){
    
    char tempNum[MAX_STRING_LENGTH];
    FILE *myInputFile;
    myInputFile = fopen( fileName, "r" );

    //Read Header Line
    fscanf(myInputFile, "%s", tempNum); // p
    fscanf(myInputFile, "%s", tempNum); // cnf
    fscanf(myInputFile, "%s", tempNum); // variables
    *numberVariable = atoi(tempNum);
    fscanf(myInputFile, "%s", tempNum); // clauses
    *numberClause = atoi(tempNum);

    //Read all the clauses
    int temp = 0;
    int literalCount = 0;
    while(!feof(myInputFile)){

        fscanf(myInputFile, "%s", tempNum); // literals
        temp = atoi(tempNum);
        if (temp == 0){
            continue;
        }
        else{
            literalCount++;
        }
    }
    *numberLiteral = literalCount;
}

/**
 * Function: argumentInput_Reader
 * Purpose: Read and store all the literals in the file
 * Input: 1) array of clauses  2) array of literals 
 * Output: 1) array of clauses (provides #of literals for each clause) 2) array of literals (provides all literals) 
*/
void argumentInput_Reader(int list_Clause[], int list_Literal[]){
    
    char tempNum[MAX_STRING_LENGTH];
    FILE *myInputFile;
    myInputFile = fopen( fileName, "r" );

    //Skip Header Line
    fscanf(myInputFile, "%s", tempNum); // p
    fscanf(myInputFile, "%s", tempNum); // cnf
    fscanf(myInputFile, "%s", tempNum); // variables
    fscanf(myInputFile, "%s", tempNum); // clauses

    //Store all the literals and record nuber of literals for each clause
    int temp = 0;
    int clauseCounter = 0;
    int literalCounter = 0;
    int loopCounter = 0;
    while(!feof(myInputFile)){

        fscanf(myInputFile, "%s", tempNum); // literals
        temp = atoi(tempNum);
        if (temp == 0){
            list_Clause[clauseCounter] = literalCounter;
            literalCounter = 0;
            clauseCounter++;
        }
        else{
            list_Literal[loopCounter] = temp;
            literalCounter++;
            loopCounter++;
        }
    }

}

/**
 * Function: clause_Evaluator
 * Purpose: evaluate a clause, given a guess
 * Input: 1) array of clauses  2) array of literals 3) array of guesses 4) number of clauses 5) size of guess array
 * Output: boolean result if guess works or not
*/
bool clause_Evaluator(const bool list_Guess[], const int list_Clause[], const int list_Literal[], const int numberClause,  const int list_Guess_Size){

    int counter_in_listClause = 0;
    bool resultArray[numberClause];
    bool result;

    //Deal with or in each clause
    for(int i = 0; i<numberClause; i++){
        
        resultArray[i] = false;
        
        for(int j = 0; j<list_Clause[i]; j++){
            
            int currentLiteral = list_Literal[counter_in_listClause + j];
            int guessIndex = abs(currentLiteral) - 1;
            
            if(( currentLiteral > 0) && (list_Guess[guessIndex] == true)){
                resultArray[i] = true;
                break;
            }
            else if(( currentLiteral < 0) && (list_Guess[guessIndex] == false)){
                resultArray[i] = true;
                break;
            }
            else if ( guessIndex > list_Guess_Size - 1){
                resultArray[i] = true;
                break;                
            }
            else{
                resultArray[i] = false;
            }
        }
        counter_in_listClause += list_Clause[i];
    }

    //Deal with and between all the clauses
    for(int k = 0; k<numberClause; k++){
        if(resultArray[k]== false){
            result = false;
            break;
        }
        else{
            result = true;
        }
    }
    return result;
}


/**
 * Function: guess_Finder
 * Purpose: makes the next guess solution
 * Input: 1) array of clauses  2) array of literals 3) array of guesses 4) array of guess status 5) number of clauses 6) number of Variable
 * Output: 1) true if solution found.
*/
bool guess_Finder(const int numberVariable, const int numberClause, bool list_Guess[], bool status_of_list_Guess[], const int list_Clause[], const int list_Literal[]){
    
    int myGuessVariable = 0;
    int list_Guess_Size = 1;
    int back_track_counter_mine[numberVariable];

    for (int a = 0; a < numberVariable; a++){
        back_track_counter_mine[a] = 0;
    }

    bool return_boolean_value;
    bool final_bool_result;
    bool bypass_flag;

    while(myGuessVariable < numberVariable){

        //Backtracked to beginning of array and have not found any solution.
        if (list_Guess_Size == 0){
            final_bool_result = false;
            bypass_flag = true;
            break;
        }

        // Check right node
        return_boolean_value = clause_Evaluator(list_Guess, list_Clause, list_Literal, numberClause, list_Guess_Size);
                
        if (return_boolean_value == false){

            list_Guess[myGuessVariable] = !list_Guess[myGuessVariable];
            //status_of_list_Guess[myGuessVariable] = false;

            // Check left node
            return_boolean_value = clause_Evaluator(list_Guess, list_Clause, list_Literal, numberClause, list_Guess_Size);
            
            if (return_boolean_value == false){

                // Go back one node
                guess_BackTrack(list_Guess, status_of_list_Guess, &myGuessVariable, &list_Guess_Size);
                continue;
            }
        }

        back_track_counter_mine[myGuessVariable] = back_track_counter_mine[myGuessVariable] + 1;
        if (back_track_counter_mine[myGuessVariable] > 2){
            back_track_counter_mine[myGuessVariable] = 0;
            guess_BackTrack(list_Guess, status_of_list_Guess, &myGuessVariable, &list_Guess_Size);
            continue;
        }

        printf("Debug___guess: %d and Repetition: %d\n", myGuessVariable, back_track_counter_mine[myGuessVariable]);
        myGuessVariable++;
        list_Guess_Size++;
    }

    if(bypass_flag == true){ // do nothing
    }
    else{ final_bool_result = true;
    }
    
    return final_bool_result;
}



/**
 * Function: guess_BackTrack
 * Purpose: backTrack on current guess
 * Input: 
 * Output: 
*/

void guess_BackTrack(bool list_Guess[], bool status_of_list_Guess[], int* myGuessVariable, int* list_Guess_Size){
 
    // You have checked both child nodes.
    //status_of_list_Guess[*myGuessVariable] = true;

    *myGuessVariable = *myGuessVariable - 1;
    *list_Guess_Size = *list_Guess_Size - 1;
    
    status_of_list_Guess[*myGuessVariable] = false;
    list_Guess[*myGuessVariable] = !list_Guess[*myGuessVariable];

}


