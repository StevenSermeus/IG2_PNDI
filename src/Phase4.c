#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "Phase2.h"

#define NB_TESTS 30
#define T_LINE 100000
#define NB_VACC 600
#define NB_MOTION 6


struct model {
	int numMotion;
	double vaccAvg[NB_VACC];
	double vaccStd[NB_VACC];
	double globalAverage;
};
typedef struct model Model;


struct testLine {
	int index;
	int numMotion;
	int gender;
	double vacc[NB_VACC];
};
typedef struct testLine TestLine;


int guessMotion(Model models[], double vaccTest[]);
void fillModel(Model model[]);
void fillClasses(int realClasses[], int estimatedClasses[], Model model[]);


void fillModel(Model model[]) {
	FILE* file;
	int openOK = fopen_s(&file, "model.csv", "r");
	if (openOK != 0) {
		printf("Error opening file\n");
		exit(100);
	}

	char line[T_LINE];
	char* token;
	char* nextToken = NULL;

	fgets(line, T_LINE, file);
	while (!feof(file)) {
		// averages
		token = strtok_s(line, ",", &nextToken);
		int motion = atoi(token) - 1;
		model[motion].numMotion = atoi(token);

		int iVacc = 0;
		while (!(feof(file)) && iVacc < NB_VACC) {
			token = strtok_s(NULL, ",", &nextToken);
			model[motion].vaccAvg[iVacc] = atof(token);
			iVacc++;
		}

		// standart deviations
		iVacc = 0;
		fgets(line, T_LINE, file);
		token = strtok_s(line, ",", &nextToken);
		while (!(feof(file)) && iVacc < NB_VACC) {
			token = strtok_s(NULL, ",", &nextToken);
			model[motion].vaccStd[iVacc] = atof(token);
			iVacc++;
		}

		// global average
		fgets(line, T_LINE, file);
		token = strtok_s(line, ",", &nextToken);
		token = strtok_s(line, ",", &nextToken);
		model[motion].globalAverage = atof(token);

		fgets(line, T_LINE, file);
	}

	fclose(file);
}

int guessMotion(Model models[], double vaccTest[]) {
	double vaccAvg[NB_MOTION] = { 0,0,0,0,0,0 };
	double vaccStd[NB_MOTION] = { 0,0,0,0,0,0 };
	double globalAvgTest[NB_MOTION] = { 0,0,0,0,0,0 };

	for (int iMotion = 0; iMotion < NB_MOTION; iMotion++) {
		for (int iVacc = 0; iVacc < NB_VACC; iVacc++) {
			vaccAvg[iMotion] += pow((vaccTest[iVacc] - models[iMotion].vaccAvg[iVacc]), 2);
			vaccStd[iMotion] += pow(sqrt(pow(vaccTest[iVacc] - models[iMotion].vaccAvg[iVacc], 2)) - models[iMotion].vaccStd[iMotion], 2);
			globalAvgTest[iMotion] += vaccTest[iVacc];
		}
		vaccAvg[iMotion] = sqrt(vaccAvg[iMotion]);
		vaccStd[iMotion] = sqrt(vaccStd[iMotion]);
		globalAvgTest[iMotion] = fabs(globalAvgTest[iMotion] / 600 - models[iMotion].globalAverage);
	}

	int guess[NB_MOTION] = { 0,0,0,0,0,0 };
	//get the lowest value in vaccAvg and vaccStd
	int minIndexAvg = 0;
	int minIndexStd = 0;
	int minIndexGlobal = 0;

	for (int iMotion = 1; iMotion < NB_MOTION; iMotion++) {
		if (vaccAvg[iMotion] < vaccAvg[minIndexAvg]) {
			minIndexAvg = iMotion;
		}
		if (vaccStd[iMotion] < vaccStd[minIndexStd]) {
			minIndexStd = iMotion;
		}
		if (globalAvgTest[iMotion] < globalAvgTest[minIndexGlobal]) {
			minIndexGlobal = iMotion;
		}
	}

	guess[minIndexAvg]++;
	guess[minIndexStd]++;
	guess[minIndexGlobal]++;

	//print guess
	for (int iMotion = 0; iMotion < NB_MOTION; iMotion++) {
		if (guess[iMotion] >= 2) {
			return iMotion + 1;
		}
	}
	return minIndexAvg + 1;
}

void fillClasses(int realClasses[], int estimatedClasses[], Model model[]) {
	FILE* testSet;
	int openOK = fopen_s(&testSet, "testSet.csv", "r");
	if (openOK != 0) {
		printf("Error opening file\n");
		exit(100);
	}

	char line[T_LINE];
	char* token;
	char* nextToken = NULL;

	fgets(line, T_LINE, testSet);				// header

	int iTest = 0;
	fgets(line, T_LINE, testSet);
	token = strtok_s(line, ",", &nextToken);	// index
	token = strtok_s(NULL, ",", &nextToken);	// numMotion
	realClasses[iTest] = atoi(token);
	token = strtok_s(NULL, ",", &nextToken);	// gender
	while (!feof(testSet) && token != NULL) {
		double vacc[NB_VACC];
		int i = 0;
		token = strtok_s(NULL, ",", &nextToken);
		while (!feof(testSet) && i < NB_VACC && token != NULL) {
			vacc[i] = atof(token);
			token = strtok_s(NULL, ",", &nextToken);
			i++;
		}
		estimatedClasses[iTest] = guessMotion(model, vacc);

		fgets(line, T_LINE, testSet);
		if (!feof(testSet)) {
			iTest++;
			token = strtok_s(line, ",", &nextToken);	// index
			token = strtok_s(NULL, ",", &nextToken);	// numMotion
			realClasses[iTest] = atoi(token);
			token = strtok_s(NULL, ",", &nextToken);	// gender
		}
	}
	iTest++;
	displayConfusionMatrix(realClasses, estimatedClasses, iTest);
	displayAccuracy(realClasses, estimatedClasses, iTest);
	displayResultsByClass(realClasses, estimatedClasses, iTest);
	fclose(testSet);
}

void main(void) {
	Model model[NB_MOTION];
	fillModel(model);

	int realClasses[NB_TESTS];
	int estimatedClasses[NB_TESTS];

	fillClasses(realClasses, estimatedClasses, model);
}
