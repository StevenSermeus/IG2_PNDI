#ifndef PHASE2_H
#define PHASE2_H
    #include <stdio.h>
    #include <stdbool.h>
    #include <math.h>

    #define NB_MAX_CLASSES 20

    void displayResultsByClass(int realClasses[], int estimatedClasses[], int nbTests);
    void displayConfusionMatrix(int realClasses[], int estimatedClasses[], int nbTests);
    void displayAccuracy(int realClasses[], int estimatedClasses[], int nbTests);
    double computeAccuracy(int realClasses[], int estimatedClasses[], int nbTests);
    void classifiedValues(int realClasses[], int estimatedClasses[], int nbTests, int valuesMatrix[][NB_MAX_CLASSES]);
    int maxNumClasses(int realClasses[], int nbTests);
    void displayLine(int iLine, int nbTests, int matrix[][NB_MAX_CLASSES]);

    /**
     * @brief Generate the matrix of classified values
     * 
     * @param realClasses
     * @param estimatedClasses 
     * @param nbTests 
     * @param valuesMatrix confusion matrix 
     */
    void classifiedValues(int realClasses[], int estimatedClasses[], int nbTests, int valuesMatrix[][NB_MAX_CLASSES]) {
        for(int i = 0; i < nbTests; i++) {
            valuesMatrix[realClasses[i]-1][estimatedClasses[i]-1]++;
        }
    }

    /**
     * @brief 
     * 
     * @param realClasses 
     * @param nbTests 
     * @return int the highest classes number
     */
    int maxNumClasses(int realClasses[], int nbTests) {
        int max = -1;
        for (int i = 0; i < nbTests; i++) {
            if (realClasses[i] > max) {
                max = realClasses[i];
            }
        }
        return max;
    }

    /**
     * @brief display a line of the confusion matrix
     * 
     * @param iLine index of the line to display
     * @param nbTests 
     * @param matrix 
     */
    void displayLine(int iLine, int nbTests, int matrix[][NB_MAX_CLASSES]) {
        for (int iColumn = 0; iColumn < nbTests; iColumn++) {
            printf(" %d ", matrix[iLine][iColumn]);
        }
    }

    /**
     * @brief Diplaa the confusion matrix
     * 
     * @param realClasses 
     * @param estimatedClasses 
     * @param nbTests 
     */
    void displayConfusionMatrix(int realClasses[], int estimatedClasses[], int nbTests){
        int matrix[NB_MAX_CLASSES][NB_MAX_CLASSES] = {0};
        classifiedValues(realClasses, estimatedClasses, nbTests, matrix);
        int nbClasses = maxNumClasses(realClasses, nbTests);
        for (int iLine = 0; iLine < nbClasses; iLine++) {
            if (iLine == 0) {
                //        / ... ... ... \

                printf("/ ");
                displayLine(iLine, nbClasses, matrix);
                printf(" \\\n");
            } else {
                if (iLine == nbClasses -1) {
                    //    \ ... ... ... /

                    printf("\\ ");
                    displayLine(iLine, nbClasses, matrix);
                    printf(" /\n");
                } else {
                    //    | ... ... ... |

                    printf("| ");
                    displayLine(iLine, nbClasses, matrix);
                    printf(" |\n");
                }
            }
        }
    }

    /**
     * @brief display the global accuracy
     * 
     * @param realClasses 
     * @param estimatedClasses 
     * @param nbTests 
     */
    void displayAccuracy(int realClasses[], int estimatedClasses[], int nbTests){
        double accuracy = computeAccuracy(realClasses, estimatedClasses, nbTests);
        printf("Global accuracy: %.2f %%\n", accuracy * 100);
    }

    /**
     * @brief Compute the global accuracy
     * 
     * @param realClasses 
     * @param estimatedClasses 
     * @param nbTests 
     * @return (double) the accuracy
     */
    double computeAccuracy(int realClasses[], int estimatedClasses[], int nbTests){
        int nbClasses = maxNumClasses(realClasses, nbTests);
        int matrix[NB_MAX_CLASSES][NB_MAX_CLASSES] = {0};
        classifiedValues(realClasses, estimatedClasses, nbTests, matrix);

        int nbCorret = 0;
        for (int iLine = 0; iLine < nbClasses; iLine++) {
            for (int iColumn = 0; iColumn < nbClasses; iColumn++) {
                if (iLine == iColumn) {
                    nbCorret += matrix[iLine][iColumn];
                }
            }
        }
        return (double)nbCorret / nbTests;
    }

    /**
     * @brief display by classe the % of well classified values and the number of well classified values 
     * 
     * @param realClasses 
     * @param estimatedClasses 
     * @param nbTests 
     */
    void displayResultsByClass(int realClasses[], int estimatedClasses[], int nbTests){
        int matrix[NB_MAX_CLASSES][NB_MAX_CLASSES] = {0};
        classifiedValues(realClasses, estimatedClasses, nbTests, matrix);
        int nbClasses = maxNumClasses(realClasses, nbTests);
        printf("Numero classe  | Bien classes  | Total         | Pourcentage\n");
        printf("---------------+---------------+---------------+-------------\n");
        for (int iLine = 0; iLine < nbClasses; iLine++) {
            int total = 0;
            for (int iColumn = 0; iColumn < nbClasses; iColumn++) {
                total += matrix[iLine][iColumn];
            }
            if (total > 0) {
                printf("%2d             | ", iLine + 1);
                printf("%d             | ", matrix[iLine][iLine]);
                printf("%d             | ", total);
                printf("%.2f %%\n", (double)matrix[iLine][iLine] / total * 100);
            }
        }
    }
#endif