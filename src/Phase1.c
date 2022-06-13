#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "Phase2.h"

#define trainSet_NAME "trainSet.csv"
#define testSet_NAME "testSet.csv"
#define LINE_SIZE 300
#define PATH_SIZE 50
#define NB_SUBJECT 24
#define VACC_SIZE 600
#define L_GPATH 10 
#define NB_REPOS 15

// struct des tester de qui sont dans le fichier
typedef struct {
    int code;
    int weight;
    int height;
    int age;
    int gender;
} Subject;

typedef struct {
    int time;
    double attitudeRoll;
    double attitudePitch;
    double attitudeYaw;
    double gravityX;
    double gravityY;
    double gravityZ;
    double rotationRateX;
    double rotationRateY;
    double rotationRateZ;
    double userAccelerationX;
    double userAccelerationY;
    double userAccelerationZ;
} LineDataSub;

typedef struct {
    int motion;
    int index;
    int gender;
    double vacc[VACC_SIZE];
} LineDataDataset;

void createDataSet(char repositories[][L_GPATH]);
void initGender(int gender[]);
void openSet(FILE** trainSet, FILE** testSet);
void createPath(char path[], char repositories[], int iFile);
int motionNumber(char motion[]);
void writeheaderLineCsv(FILE* file);
void writeLineCsv(LineDataDataset lineDataSet, FILE* file);

void main(void) {
    // PHASE 1
    char repositories[NB_REPOS][L_GPATH] = { "dws_1/","dws_2/","dws_11/","jog_9/","jog_16/","sit_5/","sit_13/" ,"std_6/","std_14/","ups_3/","ups_4/","ups_12/","wlk_7/","wlk_8/","wlk_15/"};

    createDataSet(repositories);

    // PHASE 2
    // int realClasses[30] = { 18, 4, 10, 11, 18, 19, 5, 3, 4, 2, 20, 6, 3, 18, 9, 5, 6, 16, 10, 20, 11, 8, 19, 8, 5, 16, 14, 17, 19, 6 };
    // int estimateClasses[30] = { 18, 17, 15, 16, 17, 20, 4, 10, 17, 2, 10, 10, 18, 12, 5, 6, 6, 10, 15, 12, 12, 14, 2, 19, 13, 15, 16, 6, 19, 17 };
    // int realClasses[8] = { 5, 2, 5, 3, 1, 3, 2, 4 };
    // int estimateClasses[8] = { 5, 5, 1, 2, 1, 3, 2, 4 };
    // int nbTests = 8;
    // displayConfusionMatrix(realClasses, estimateClasses, nbTests);
    // displayAccuracy(realClasses, estimateClasses, nbTests);
    // displayResultsByClass(realClasses,estimateClasses,nbTests);
}

/**
 * @brief process the gender
 *
 * @param gender int tab that is modify in the function to add the gender from the csv file data_subjects_info.csv
 * @return int code of error 0 all good 1 error file opening
 */
void initGender(int gender[]) {

    FILE* dataSubjectFile;
    fopen_s(&dataSubjectFile, "data_subjects_info.csv", "r");

    if (dataSubjectFile == NULL) {
        exit(100);
    }

    char line[LINE_SIZE];
    Subject subject;
    fgets(line, LINE_SIZE, dataSubjectFile); //title
    int iGender = 0;
    fgets(line, LINE_SIZE, dataSubjectFile);
    while (!feof(dataSubjectFile)) {
        if (sscanf_s(line, "%d,%d,%d,%d,%d", &subject.code, &subject.weight, &subject.height, &subject.age, &subject.gender) == 5) {
            gender[iGender] = subject.gender;
            iGender++;
        } else {
            puts("Error in the file");
        }
        fgets(line, LINE_SIZE, dataSubjectFile);
    }
    fclose(dataSubjectFile);

}

/**
 * . Open the file trainSet.csv and testSet.csv and if error openning one file closing the other one 
 * 
 * \param trainSet pointer of pointer
 * \param testSet pointer of pointer
 * cf doc for fopen_s https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/fopen-s-wfopen-s?view=msvc-170#parameters
 * \return error code and the file pointer if no error
 */
void openSet(FILE** trainSet, FILE** testSet) {
    // fopen_s(trainSet, trainSet_NAME, "w");
    // if (*testSet == NULL) {                         
    //     exit(100);                                       Code version bloc 1
    // }
    // fopen_s(testSet, testSet_NAME, "w");
    
    // if (*trainSet == NULL) {
    //     exit(100);
    // }
    int code;
    code = fopen_s(trainSet, trainSet_NAME, "w");
    if (code != 0) {                         
        exit(100);
    }
    code = fopen_s(testSet, testSet_NAME, "w");
    if (code != 0) {
        exit(100);
    }
}

/**
 * .Creation of the data set from the 360 files
 * 
 * \param repositories char tab of the path of the repositories
 * \return error code
 */
void createDataSet(char repositories[][L_GPATH]) {
    int gender[NB_SUBJECT];
    initGender(gender);                    
    FILE* trainSet;
    FILE* testSet;
    openSet(&trainSet, &testSet);
    writeheaderLineCsv(trainSet);
    writeheaderLineCsv(testSet);

    int iPeopleTest = 1;
    int index = 0;
    int iRepo = 0;
    FILE* fiSub;
    while (iRepo < NB_REPOS) {
        int iPeopleRepo = 0;
        int iFile = 1;

        char path[PATH_SIZE] = { "A_DeviceMotion_data/" };             
        
        while (iFile <= NB_SUBJECT) {
            
            // add folder to path
            createPath(path, repositories[iRepo], iFile);

            // fopen_s(&fiSub, path, "r");
            // if (fiSub == NULL) {
            //     fclose(trainSet);  version b1
            //     fclose(testSet);
            //     exit(100);
            //}

            int err = fopen_s(&fiSub, path, "r");
            if (err != 0) {
                fclose(trainSet); 
                fclose(testSet);
                exit(100);
            }

            int motion = motionNumber(repositories[iRepo]);
            LineDataDataset dataMotion;
            dataMotion.motion = motion;
            dataMotion.gender = gender[iFile-1];

            dataMotion.index = index;
            char line[LINE_SIZE];
            fgets(line, LINE_SIZE, fiSub); // top line no data just title

            fgets(line, LINE_SIZE, fiSub);

            LineDataSub lineSub;                           

            int nbVal = sscanf_s(line, "%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf", &lineSub.time, &lineSub.attitudeRoll, &lineSub.attitudePitch, &lineSub.attitudeYaw, &lineSub.gravityX, &lineSub.gravityY, &lineSub.gravityZ, &lineSub.rotationRateX, &lineSub.rotationRateY, &lineSub.rotationRateZ, &lineSub.userAccelerationX, &lineSub.userAccelerationY, &lineSub.userAccelerationZ);
            int iVacc = 0;

            while (!(feof(fiSub)) && lineSub.time < VACC_SIZE && nbVal == 13) {
                iVacc++;
                dataMotion.vacc[lineSub.time] = sqrt((lineSub.userAccelerationX * lineSub.userAccelerationX) + (lineSub.userAccelerationY * lineSub.userAccelerationY) + (lineSub.userAccelerationZ * lineSub.userAccelerationZ));
                fgets(line, LINE_SIZE, fiSub);
                nbVal = sscanf_s(line, "%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf", &lineSub.time, &lineSub.attitudeRoll, &lineSub.attitudePitch, &lineSub.attitudeYaw, &lineSub.gravityX, &lineSub.gravityY, &lineSub.gravityZ, &lineSub.rotationRateX, &lineSub.rotationRateY, &lineSub.rotationRateZ, &lineSub.userAccelerationX, &lineSub.userAccelerationY, &lineSub.userAccelerationZ);
            }
            //on arrete la boucle si pas 13 val dans une des lignes
            // while (!(feof(fiSub)) && lineSub.time < VACC_SIZE && nbVal == 13) {
            //     iVacc++;
            //     dataMotion.vacc[lineSub.time] = sqrt((lineSub.userAccelerationX * lineSub.userAccelerationX) + (lineSub.userAccelerationY * lineSub.userAccelerationY) + (lineSub.userAccelerationZ * lineSub.userAccelerationZ));
            //     fgets(line, LINE_SIZE, fiSub);
            //     nbVal = sscanf_s(line, "%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf", &lineSub.time, &lineSub.attitudeRoll, &lineSub.attitudePitch, &lineSub.attitudeYaw, &lineSub.gravityX, &lineSub.gravityY, &lineSub.gravityZ, &lineSub.rotationRateX, &lineSub.rotationRateY, &lineSub.rotationRateZ, &lineSub.userAccelerationX, &lineSub.userAccelerationY, &lineSub.userAccelerationZ);
            // }

            //si on a pas 13 val on ne prend pas la ligne
            // while (!(feof(fiSub)) && lineSub.time < VACC_SIZE) {
            //     if(nbVal == 13) {
            //     iVacc++;
            //     dataMotion.vacc[lineSub.time] = sqrt((lineSub.userAccelerationX * lineSub.userAccelerationX) + (lineSub.userAccelerationY * lineSub.userAccelerationY) + (lineSub.userAccelerationZ * lineSub.userAccelerationZ));
            //     }
            //     fgets(line, LINE_SIZE, fiSub);
            //     nbVal = sscanf_s(line, "%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf", &lineSub.time, &lineSub.attitudeRoll, &lineSub.attitudePitch, &lineSub.attitudeYaw, &lineSub.gravityX, &lineSub.gravityY, &lineSub.gravityZ, &lineSub.rotationRateX, &lineSub.rotationRateY, &lineSub.rotationRateZ, &lineSub.userAccelerationX, &lineSub.userAccelerationY, &lineSub.userAccelerationZ);
            // }

            for(iVacc; iVacc < VACC_SIZE; iVacc++) {
                dataMotion.vacc[iVacc] = -1;
            }

            if (iPeopleRepo < 2) {
                if (iPeopleTest == iFile) {
                    writeLineCsv(dataMotion, testSet);
                    if (iPeopleTest == 24) {
                        iPeopleTest = 1;
                    } else {
                        iPeopleTest++;
                    }
                    iPeopleRepo++;
                } else {
                    writeLineCsv(dataMotion, trainSet);
                }
            } else {
                writeLineCsv(dataMotion, trainSet);
            }

            fclose(fiSub);
            iFile++;
            index++;
            strcpy_s(path, PATH_SIZE, "A_DeviceMotion_data/");
        }
        iRepo++;
    }
    fclose(trainSet);
    fclose(testSet);
}


/**
 * @brief Create a Path object
 * 
 * @param path path of the file
 * @param repositories list of repositories
 * @param iFile index of the file
 * 
 * @return the path of the file
 */
void createPath(char path[], char repositories[], int iFile) {
    strcat_s(path, PATH_SIZE, repositories);
    char iFileStr[3];
    strcat_s(path, PATH_SIZE, "sub_");
    sprintf_s(iFileStr, 3, "%d", iFile);
    strcat_s(path, PATH_SIZE, iFileStr);
    strcat_s(path, PATH_SIZE, ".csv");
}


/**
 * @brief Convert motion name to number
 * 
 * @param motion motion name
 * @return int motion number or -1 if error
 */
int motionNumber(char motion[]) {
    char motionShort[4];
    snprintf(motionShort, 4, "%s", motion);
    if (strcmp(motionShort, "dws") == 0)
        return 1;
    if (strcmp(motionShort, "jog") == 0)
        return 2;
    if (strcmp(motionShort, "ups") == 0)
        return 3;
    if (strcmp(motionShort, "sit") == 0)
        return 4;
    if (strcmp(motionShort, "std") == 0)
        return 5;
    if (strcmp(motionShort, "wlk") == 0)
        return 6;
    return -1;
}

void writeheaderLineCsv(FILE* file) {
    fprintf(file, "index,motion,gender,vacc\n");
}

/**
 * @brief write a line in the csv file
 * 
 * @param lineDataSet the struct to write
 * @param file the pointer to the file  where to write
 * 
 */
void writeLineCsv(LineDataDataset lineDataSet, FILE * file) {
    fprintf(file, "%d,%d,%d", lineDataSet.index, lineDataSet.motion, lineDataSet.gender);
    for (int i = 0; i < VACC_SIZE; i+=10)
        fprintf(file, ",%f,%f,%f,%f,%f,%lf,%f,%f,%f,%f", lineDataSet.vacc[i],lineDataSet.vacc[i+1],lineDataSet.vacc[i+1],lineDataSet.vacc[i+3],lineDataSet.vacc[i+4],
        lineDataSet.vacc[i+5],lineDataSet.vacc[i+6],lineDataSet.vacc[i+7],lineDataSet.vacc[i+8],lineDataSet.vacc[i+9]);
    fprintf(file, "\n");
}
