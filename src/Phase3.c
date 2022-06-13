#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

#define T_LINE 100000
#define NB_VACC 600

struct line {
	int numMotion;
	double vacc[NB_VACC];
};
typedef struct line Line;

struct modelDatas {
	double sum;
	double sumSquare;
	int nbValues;
};
typedef struct modelDatas ModelDatas;

struct globalAverage {
	double totalSum;
	int nbValues;
};
typedef struct globalAverage GlobalAverage;


void resetDatas(ModelDatas modelDatas[]) {
	int i = 0;
	do {
		modelDatas[i].sum = 0;
		modelDatas[i].sumSquare = 0;
		modelDatas[i].nbValues = 0;
		i++;
	} while (i < NB_VACC);
}

GlobalAverage processAverage(ModelDatas modelDatas[], double average[]) {
	GlobalAverage globalAverage;
	globalAverage.nbValues = 0;
	globalAverage.totalSum = 0;
	int i = 0;
	do {
		average[i] = modelDatas[i].sum / modelDatas[i].nbValues;
		globalAverage.totalSum += modelDatas[i].sum;
		globalAverage.nbValues += modelDatas[i].nbValues;
		i++;
	} while (i < NB_VACC);
	return globalAverage;
}

void processStandartDeviation(ModelDatas modelDatas[], double standartDeviation[], double average[]) {
	int i = 0;
	do {
		standartDeviation[i] = sqrt(modelDatas[i].sumSquare / modelDatas[i].nbValues - average[i] * average[i]);
		i++;
	} while (i < NB_VACC);
}

double processGeneralAverage(GlobalAverage globalAverage) {
	return globalAverage.totalSum / globalAverage.nbValues;
}

void writeMvtModelCsv(FILE* modelFile, int numMotion, double average[], double standartDeviation[], double generalAverage) {
	fprintf(modelFile, "%d", numMotion);
	for (int i = 0; i < NB_VACC; i += 10)
		fprintf(modelFile, ",%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf", average[i], average[i + 1], average[i + 2], average[i + 3], average[i + 4], average[i + 5], average[i + 6], average[i + 7], average[i + 8], average[i + 9]);
	fprintf(modelFile, "\n");

	fprintf(modelFile, "%d", numMotion);
	for (int i = 0; i < NB_VACC; i += 10)
		fprintf(modelFile, ",%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf", standartDeviation[i], standartDeviation[i + 1], standartDeviation[i + 2], standartDeviation[i + 3], standartDeviation[i + 4], standartDeviation[i + 5], standartDeviation[i + 6], standartDeviation[i + 7], standartDeviation[i + 8], standartDeviation[i + 9]);
	fprintf(modelFile, "\n");

	fprintf(modelFile, "%d,%f\n", numMotion, generalAverage);
}

void modelCreation() {
	FILE* trainSet;
	fopen_s(&trainSet, "trainSet.csv", "r");
	
	FILE* model;
	fopen_s(&model, "model.csv", "w");

	if (trainSet == NULL) {
		printf("TrainSet not open");
	} else if (model == NULL) {
		printf("Model not open");
	} else {
		char line[T_LINE];
		
		fgets(line, T_LINE, trainSet);
		char* token;
		char* nextToken = NULL;

		//File
		while (!feof(trainSet)) {
			Line file;
			ModelDatas modelDatas[NB_VACC];
			resetDatas(modelDatas);

			token = strtok_s(line, ",", &nextToken); // Index
			token = strtok_s(NULL, ",", &nextToken); // Mouvment
			file.numMotion = atoi(token);
			token = strtok_s(NULL, ",", &nextToken); // Gender

			int numMotion = file.numMotion;

			//Line 
			while (!feof(trainSet) && file.numMotion == numMotion) {
				int iColum = 0;
				
				//Vacc
				token = strtok_s(NULL, ",", &nextToken);
				while (token != NULL && iColum < NB_VACC) {
					modelDatas[iColum].sum += atof(token);
					modelDatas[iColum].sumSquare += atof(token) * atof(token);
					modelDatas[iColum].nbValues++;
					token = strtok_s(NULL, ",", &nextToken);
					iColum++;
				}
				
				fgets(line, T_LINE, trainSet);
				if (!feof(trainSet)) {
					token = strtok_s(line, ",", &nextToken); // Index
					token = strtok_s(NULL, ",", &nextToken); // Mouvment
					file.numMotion = atoi(token);
					token = strtok_s(NULL, ",", &nextToken); // Gender
				}
			}

			double average[NB_VACC];
			double standardtDeviation[NB_VACC];
			GlobalAverage globalAverage = processAverage(modelDatas, average);
			processStandartDeviation(modelDatas, standardtDeviation, average);
			double generalAverage = processGeneralAverage(globalAverage);

			writeMvtModelCsv(model, numMotion, average, standardtDeviation, generalAverage);
			
			numMotion = file.numMotion;
			fgets(line, T_LINE, trainSet);
		}
	}
}

void main(void) {
	modelCreation();
}
