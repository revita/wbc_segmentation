#ifdef __llvm__
#pragma GCC diagnostic ignored "-Wdangling-else"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "util.h"
#include "imageProcessing.h"

void relabelComponents(MATRIX *rmap){
	int maxLabel = maxMatrixEntry(*rmap), currLabel;
	int *relabels = (int *)calloc(maxLabel+1,sizeof(int));
	long i, j;

	for (i = 0; i <= maxLabel; i++)
		relabels[i] = -1;
	currLabel = 1;
	for (i = 0 ; i < rmap->row; i++)
		for (j = 0; j < rmap->column; j++)
			if (rmap->data[i][j] > 0)
				if (relabels[ rmap->data[i][j] ] == -1)
					relabels[ rmap->data[i][j] ] = currLabel++;
	for (i = 0 ; i < rmap->row; i++)
		for (j = 0; j < rmap->column; j++)
			if (rmap->data[i][j] > 0)
				rmap->data[i][j] = relabels[ rmap->data[i][j] ];
	free(relabels);
}
void relabelComponentsWithSpecifiedBackground(MATRIX *rmap, int background, int startLabel){
	int maxLabel = maxMatrixEntry(*rmap);
	int *relabels = (int *)calloc(maxLabel+1,sizeof(int));
	int unlabeled = startLabel - 1;
	long i, j;

	for (i = 0; i <= maxLabel; i++)
		relabels[i] = unlabeled;
	for (i = 0 ; i < rmap->row; i++)
		for (j = 0; j < rmap->column; j++)
			if (rmap->data[i][j] != background)
				if (relabels[ rmap->data[i][j] ] == unlabeled)
					relabels[ rmap->data[i][j] ] = startLabel++;
	for (i = 0 ; i < rmap->row; i++)
		for (j = 0; j < rmap->column; j++)
			if (rmap->data[i][j] != background)
				rmap->data[i][j] = relabels[ rmap->data[i][j] ];
	free(relabels);
}
MATRIX fourConnectivity(MATRIX image){
	MATRIX visited = allocateMatrix(image.row,image.column);
	MATRIX regions = allocateMatrix(image.row,image.column);
	long *queueX = (long *)malloc(image.row * image.column * sizeof(long));
	long *queueY = (long *)malloc(image.row * image.column * sizeof(long));
	long i, j, x, y, queueStart, queueEnd;
	short label = 0;

	initializeMatrix(&visited,UNVISITED);
	initializeMatrix(&regions,BACKGROUND);
	for (i = 0; i < image.row; i++)
		for (j = 0; j < image.column; j++){
			if (image.data[i][j] == BACKGROUND)
				continue;
			if (visited.data[i][j] == VISITED)
				continue;

			label++;
			queueX[0] = i;		queueY[0] = j;
			queueStart = 0;		queueEnd = 1;
			visited.data[i][j] = VISITED;
				
			while (queueEnd > queueStart){
				x = queueX[queueStart];		y = queueY[queueStart];
				regions.data[x][y] = label;
				if (x-1 >= 0 && visited.data[x-1][y] != VISITED && image.data[x-1][y] != BACKGROUND){
					visited.data[x-1][y] = VISITED;
					queueX[queueEnd] = x-1;		queueY[queueEnd] = y;		queueEnd++;
				}
				if (x+1 < image.row && visited.data[x+1][y] != VISITED && image.data[x+1][y] != BACKGROUND){
					visited.data[x+1][y] = VISITED;
					queueX[queueEnd] = x+1;		queueY[queueEnd] = y;		queueEnd++;
				}
				if (y-1 >= 0 && visited.data[x][y-1] != VISITED && image.data[x][y-1] != BACKGROUND){
					visited.data[x][y-1] = VISITED;
					queueX[queueEnd] = x;		queueY[queueEnd] = y-1;		queueEnd++;
				}
				if (y+1 < image.column && visited.data[x][y+1] != VISITED && image.data[x][y+1] != BACKGROUND){
					visited.data[x][y+1] = VISITED;
					queueX[queueEnd] = x;		queueY[queueEnd] = y+1;		queueEnd++;
				}
				queueStart++;
			}
		}
	free(queueX);		free(queueY);
	freeMatrix(visited);
	return regions;
}
MATRIX eightConnectivity(MATRIX image){
	MATRIX visited = allocateMatrix(image.row,image.column);
	MATRIX regions = allocateMatrix(image.row,image.column);
	long *queueX = (long *)malloc(image.row * image.column * sizeof(long));
	long *queueY = (long *)malloc(image.row * image.column * sizeof(long));
	long i, j, x, y, queueStart, queueEnd;
	short label = 0;

	initializeMatrix(&visited,UNVISITED);
	initializeMatrix(&regions,BACKGROUND);
	for (i = 0; i < image.row; i++)
		for (j = 0; j < image.column; j++){
			if (image.data[i][j] == BACKGROUND)
				continue;
			if (visited.data[i][j] == VISITED)
				continue;

			label++;
			queueX[0] = i;		queueY[0] = j;
			queueStart = 0;		queueEnd = 1;
			visited.data[i][j] = VISITED;
				
			while (queueEnd > queueStart){
				x = queueX[queueStart];		y = queueY[queueStart];
				regions.data[x][y] = label;
				if ((x-1 >= 0) && 
					(visited.data[x-1][y] != VISITED) && (image.data[x-1][y] != BACKGROUND)){
					visited.data[x-1][y] = VISITED;
					queueX[queueEnd] = x-1;		queueY[queueEnd] = y;		queueEnd++;
				}
				if ((x+1 < image.row) && 
					(visited.data[x+1][y] != VISITED) && (image.data[x+1][y] != BACKGROUND)){
					visited.data[x+1][y] = VISITED;
					queueX[queueEnd] = x+1;		queueY[queueEnd] = y;		queueEnd++;
				}
				if ((y-1 >= 0) && 
					(visited.data[x][y-1] != VISITED) && (image.data[x][y-1] != BACKGROUND)){
					visited.data[x][y-1] = VISITED;
					queueX[queueEnd] = x;		queueY[queueEnd] = y-1;		queueEnd++;
				}
				if ((y+1 < image.column) && 
					(visited.data[x][y+1] != VISITED) && (image.data[x][y+1] != BACKGROUND)){
					visited.data[x][y+1] = VISITED;
					queueX[queueEnd] = x;		queueY[queueEnd] = y+1;		queueEnd++;
				}
				if ((x-1 >= 0) && (y-1 >= 0) &&
					(visited.data[x-1][y-1] != VISITED) && (image.data[x-1][y-1] != BACKGROUND)){
					visited.data[x-1][y-1] = VISITED;
					queueX[queueEnd] = x-1;		queueY[queueEnd] = y-1;		queueEnd++;
				}
				if ((x-1 >= 0) && (y+1 < image.column) &&
					(visited.data[x-1][y+1] != VISITED) && (image.data[x-1][y+1] != BACKGROUND)){
					visited.data[x-1][y+1] = VISITED;
					queueX[queueEnd] = x-1;		queueY[queueEnd] = y+1;		queueEnd++;
				}
				if ((x+1 < image.row) && (y-1 >= 0) &&
					(visited.data[x+1][y-1] != VISITED) && (image.data[x+1][y-1] != BACKGROUND)){
					visited.data[x+1][y-1] = VISITED;
					queueX[queueEnd] = x+1;		queueY[queueEnd] = y-1;		queueEnd++;
				}
				if ((x+1 < image.row) && (y+1 < image.column) &&
					(visited.data[x+1][y+1] != VISITED) && (image.data[x+1][y+1] != BACKGROUND)){
					visited.data[x+1][y+1] = VISITED;
					queueX[queueEnd] = x+1;		queueY[queueEnd] = y+1;		queueEnd++;
				}
				queueStart++;
			}
		}
	free(queueX);		free(queueY);
	freeMatrix(visited);
	return regions;
}
void eightConnectivityPartial(MATRIX marked, int id, BNDBOX bnd, MATRIX visited, 
							  int *queueX, int *queueY, MATRIX *reg, int *regNo){
	long i, j, x, y, queueStart, queueEnd;

	*regNo = 0;
	initializeMatrixPartial(&visited,UNVISITED,bnd.minx,bnd.maxx,bnd.miny,bnd.maxy);
	initializeMatrixPartial(reg,BACKGROUND,bnd.minx,bnd.maxx,bnd.miny,bnd.maxy);
	for (i = bnd.minx; i <= bnd.maxx; i++)
		for (j = bnd.miny; j <= bnd.maxy; j++){
			if (marked.data[i][j] != id)
				continue;
			if (visited.data[i][j] == VISITED)
				continue;

			(*regNo)++;
			queueX[0] = i;		queueY[0] = j;
			queueStart = 0;		queueEnd = 1;
			visited.data[i][j] = VISITED;
				
			while (queueEnd > queueStart){
				x = queueX[queueStart];		y = queueY[queueStart];
				reg->data[x][y] = *regNo;
				if ((x-1 >= bnd.minx) && 
					(visited.data[x-1][y] != VISITED) && (marked.data[x-1][y] == id)){
					visited.data[x-1][y] = VISITED;
					queueX[queueEnd] = x-1;		queueY[queueEnd] = y;		queueEnd++;
				}
				if ((x+1 <= bnd.maxx) && 
					(visited.data[x+1][y] != VISITED) && (marked.data[x+1][y] == id)){
					visited.data[x+1][y] = VISITED;
					queueX[queueEnd] = x+1;		queueY[queueEnd] = y;		queueEnd++;
				}
				if ((y-1 >= bnd.miny) && 
					(visited.data[x][y-1] != VISITED) && (marked.data[x][y-1] == id)){
					visited.data[x][y-1] = VISITED;
					queueX[queueEnd] = x;		queueY[queueEnd] = y-1;		queueEnd++;
				}
				if ((y+1 <= bnd.maxy) && 
					(visited.data[x][y+1] != VISITED) && (marked.data[x][y+1] == id)){
					visited.data[x][y+1] = VISITED;
					queueX[queueEnd] = x;		queueY[queueEnd] = y+1;		queueEnd++;
				}
				if ((x-1 >= bnd.minx) && (y-1 >= bnd.miny) &&
					(visited.data[x-1][y-1] != VISITED) && (marked.data[x-1][y-1] == id)){
					visited.data[x-1][y-1] = VISITED;
					queueX[queueEnd] = x-1;		queueY[queueEnd] = y-1;		queueEnd++;
				}
				if ((x-1 >= bnd.minx) && (y+1 <= bnd.maxy) &&
					(visited.data[x-1][y+1] != VISITED) && (marked.data[x-1][y+1] == id)){
					visited.data[x-1][y+1] = VISITED;
					queueX[queueEnd] = x-1;		queueY[queueEnd] = y+1;		queueEnd++;
				}
				if ((x+1 <= bnd.maxx) && (y-1 >= bnd.miny) &&
					(visited.data[x+1][y-1] != VISITED) && (marked.data[x+1][y-1] == id)){
					visited.data[x+1][y-1] = VISITED;
					queueX[queueEnd] = x+1;		queueY[queueEnd] = y-1;		queueEnd++;
				}
				if ((x+1 <= bnd.maxx) && (y+1 <= bnd.maxy) &&
					(visited.data[x+1][y+1] != VISITED) && (marked.data[x+1][y+1] == id)){
					visited.data[x+1][y+1] = VISITED;
					queueX[queueEnd] = x+1;		queueY[queueEnd] = y+1;		queueEnd++;
				}
				queueStart++;
			}
		}
}
int otsu(double *p, int no){
	double *omega	= (double *)calloc(no,sizeof(double));		// initialized with 0
	double *mu		= (double *)calloc(no,sizeof(double));		// initialized with 0
	double maxT, mt, sigma;
	int i, thr;

	omega[0] = mu[0] = p[0];
	for (i = 1; i < no; i++){
		omega[i]	= omega[i-1]	+ p[i];
		mu[i]		= mu[i-1]		+ p[i]*(i+1);
	}
	mt = mu[no-1];

	thr = -1;
	for (i = 0; i < no; i++){
		if (fabs( omega[i]*(1-omega[i]) ) < ZERO)
			sigma = 0.0;
		else
			sigma = SQUARE(mt*omega[i] - mu[i]) / (omega[i]*(1-omega[i]));
		if ((thr == -1) || (maxT < sigma)){
			thr = i;
			maxT = sigma;
		}
	}
	free(omega);
	free(mu);
	return thr;	
}
int otsuGrayImage(MATRIX M){
	int i, j, thr;
	double *p = (double *)calloc(256,sizeof(double));		// initialized with 0

	for (i = 0; i < M.row; i++)
		for (j = 0; j < M.column; j++)
			p[ M.data[i][j] ]++;
	for (i = 0; i < 256; i++)
		p[i] /= (M.row * M.column);
	thr = otsu(p,256);

	free(p);
	return thr;
}
int otsuGrayImageWithBackground(MATRIX M, int background){
	int maxNo = maxMatrixEntry(M);
	int total, i, j, thr;
	double *p = (double *)calloc(maxNo+1,sizeof(double)); // initialized with 0

	total = 0;
	for (i = 0; i < M.row; i++)
		for (j = 0; j < M.column; j++)
			if (M.data[i][j] != background){
				p[ M.data[i][j] ]++;
				total++;
			}
	for (i = 0; i <= maxNo; i++)
		p[i] /= total;
	thr = otsu(p,maxNo+1);
	free(p);
	return thr;
}
int otsuLocalGraySubimage(MATRIX sub, MATRIX localMap, int regId, 
					   int minx, int maxx, int miny, int maxy){
	int total, i, j, thr;
	double *p = (double *)calloc(256,sizeof(double)); // initialized with 0
	
	total = 0;
	for (i = minx; i <= maxx; i++)
		for (j = miny; j <= maxy; j++)
			if (localMap.data[i][j] == regId){
				p[ sub.data[i-minx][j-miny] ]++;
				total++;
			}
	for (i = 0; i < 256; i++)
		p[i] /= total;
	
	thr = otsu(p,256);
	
	free(p);
	return thr;
}
int otsuGraySubimage(MATRIX M, int minx, int maxx, int miny, int maxy){
	int total, i, j, thr;
	double *p = (double *)calloc(256,sizeof(double)); // initialized with 0
	
	total = 0;
	for (i = minx; i <= maxx; i++)
		for (j = miny; j <= maxy; j++){
			p[ M.data[i][j] ]++;
			total++;
		}
	for (i = 0; i < 256; i++)
		p[i] /= total;
	
	thr = otsu(p,256);
	
	free(p);
	return thr;
}
int otsuLocalGrayImage(MATRIX M, MATRIX localMap, int regId, 
					   int minx, int maxx, int miny, int maxy){
	int total, i, j, thr;
	double *p = (double *)calloc(256,sizeof(double)); // initialized with 0
	
	total = 0;
	for (i = minx; i <= maxx; i++)
		for (j = miny; j <= maxy; j++)
			if (localMap.data[i][j] == regId){
				p[ M.data[i][j] ]++;
				total++;
			}
	for (i = 0; i < 256; i++)
		p[i] /= total;
	
	thr = otsu(p,256);
	
	free(p);
	return thr;
}
MATRIX imthresh(MATRIX M, int thr){
	MATRIX R = allocateMatrix(M.row,M.column);
	int i, j;

	initializeMatrix(&R,0);
	for (i = 0; i < M.row; i++)
		for (j = 0; j < M.column; j++)
			if (M.data[i][j] >= thr)
				R.data[i][j] = 1;
	return R;
}
void imthresh2(MATRIX *M, int thr){
	int i, j;

	for (i = 0; i < M->row; i++)
		for (j = 0; j < M->column; j++)
			if (M->data[i][j] >= thr)
				M->data[i][j] = 1;
			else
				M->data[i][j] = 0;
}
void imsubthresh2(MATRIX *M, int thr, MATRIX mask, int no, BNDBOX bnd){
	int i, j;

	for (i = bnd.minx; i <= bnd.maxx; i++)
		for (j = bnd.miny; j <= bnd.maxy; j++)
			if (mask.data[i][j] == no){
				if (M->data[i][j] >= thr)
					M->data[i][j] = 1;
				else
					M->data[i][j] = 0;
			}
}
MATRIX findComponentBoundingBoxes(MATRIX C){ 
	int maxNo = maxMatrixEntry(C);
	MATRIX bndBox = allocateMatrix(maxNo+1,4); // minx, maxx, miny, maxy
	int i, j, no;

	initializeMatrix(&bndBox,-1);
	for (i = 0; i < C.row; i++)
		for (j = 0; j < C.column; j++)
			if (C.data[i][j]){
				no = C.data[i][j];
				if ((bndBox.data[no][0] == -1) || (bndBox.data[no][0] > i))
					bndBox.data[no][0] = i;
				if ((bndBox.data[no][1] == -1) || (bndBox.data[no][1] < i))
					bndBox.data[no][1] = i;
				if ((bndBox.data[no][2] == -1) || (bndBox.data[no][2] > j))
					bndBox.data[no][2] = j;
				if ((bndBox.data[no][3] == -1) || (bndBox.data[no][3] < j))
					bndBox.data[no][3] = j;
			}
	return bndBox;
}
MATRIX imfilter(MATRIX M, MATRIX h, int is_0_255){
	MATRIX R = allocateMatrix(M.row,M.column);
	int i, j, x, y, dx, dy;

	dx = h.row/2;		dy = h.column/2;
	for (i = 0; i < M.row; i++)
		for (j = 0; j < M.column; j++){
			R.data[i][j] = 0;
			for (x = -dx; x <= dx; x++)
				for (y = -dy; y <= dy; y++)
					if ((i+x >= 0) && (i+x < M.row) && (j+y >= 0) && (j+y < M.column))
						R.data[i][j] += h.data[dx+x][dy+y] * M.data[i+x][j+y];
			if (is_0_255 == YES){
				if (R.data[i][j] < 0)		R.data[i][j] = 0;
				if (R.data[i][j] > 255)		R.data[i][j] = 255;
			}
			else if (is_0_255 != NO)
				terminateProgram("imfilter: enter YES or NO");
		}
	return R;
}
MATRIX imfilterWithMask(MATRIX M, MATRIX h, MATRIX mask, int label, int is_0_255,
						int minx, int miny, int maxx, int maxy){
	MATRIX R = allocateMatrix(maxx - minx + 1,maxy - miny + 1);
	int i, j, x, y, dx, dy;

	dx = h.row/2;		dy = h.column/2;
	for (i = minx; i <= maxx; i++)
		for (j = miny; j <= maxy; j++){
			R.data[i-minx][j-miny] = 0;
			if (mask.data[i][j] == label){
				for (x = -dx; x <= dx; x++)
					for (y = -dy; y <= dy; y++)
						if ((i+x >= minx) && (i+x <= maxx) && (j+y >= miny) && 
							(j+y <= maxy) && (mask.data[i+x][j+y] == label))
							R.data[i-minx][j-miny] += h.data[dx+x][dy+y] * M.data[i+x][j+y];
				if (is_0_255 == YES){
					if (R.data[i-minx][j-miny] < 0)			R.data[i-minx][j-miny] = 0;
					if (R.data[i-minx][j-miny] > 255)		R.data[i-minx][j-miny] = 255;
				}
				else if (is_0_255 != NO)
					terminateProgram("imfilter: enter YES or NO");
			}
		}
	return R;
}
void fillInside(MATRIX *M, int label, int minX, int maxX, int minY, int maxY){
	int i, j;
	MATRIX visited = allocateMatrix(maxX-minX+1,maxY-minY+1);
	int *myQueueX = (int *)calloc(visited.row*visited.column,sizeof(int));
	int *myQueueY = (int *)calloc(visited.row*visited.column,sizeof(int));
	int startQ, endQ, currX, currY;
	
	initializeMatrix(&visited,0);
	for (i = 0; i < visited.row; i++){
		if (M->data[minX+i][minY] != label)		visited.data[i][0] = 2;
		if (M->data[minX+i][maxY] != label)		visited.data[i][visited.column-1] = 2;
	}
	for (j = 0; j < visited.column; j++){
		if (M->data[minX][minY+j] != label)		visited.data[0][j] = 2;
		if (M->data[maxX][minY+j] != label)		visited.data[visited.row-1][j] = 2;
	}
	for (i = 0; i < visited.row; i++)
		for (j = 0; j < visited.column; j++){
			if (visited.data[i][j] != 2)
				continue;
			startQ = 0;
			endQ = 1;
			myQueueX[0] = i;
			myQueueY[0] = j;
			visited.data[i][j] = 1;
			// four connectivity
			while (startQ < endQ){
				currX = myQueueX[startQ];
				currY = myQueueY[startQ];
				startQ++;
				if ((currX-1 >= 0) && (visited.data[currX-1][currY] != 1) && (M->data[minX+currX-1][minY+currY]) != label){
					myQueueX[endQ] = currX-1;
					myQueueY[endQ] = currY;
					endQ++;
					visited.data[currX-1][currY] = 1;
				}
				if ((currX+1 < visited.row) && (visited.data[currX+1][currY] != 1) && (M->data[minX+currX+1][minY+currY]) != label){
					myQueueX[endQ] = currX+1;
					myQueueY[endQ] = currY;
					endQ++;
					visited.data[currX+1][currY] = 1;
				}
				if ((currY-1 >= 0) && (visited.data[currX][currY-1] != 1) && (M->data[minX+currX][minY+currY-1]) != label){
					myQueueX[endQ] = currX;
					myQueueY[endQ] = currY-1;
					endQ++;
					visited.data[currX][currY-1] = 1;
				}
				if ((currY+1 < visited.column) && (visited.data[currX][currY+1] != 1) && (M->data[minX+currX][minY+currY+1]) != label){
					myQueueX[endQ] = currX;
					myQueueY[endQ] = currY+1;
					endQ++;
					visited.data[currX][currY+1] = 1;
				}
			}
		}
	for (i = 0; i < visited.row; i++)
		for (j = 0; j < visited.column; j++)
			if (visited.data[i][j] == 0)
				M->data[minX+i][minY+j] = label;
	freeMatrix(visited);
	free(myQueueX);
	free(myQueueY);
}
void markLineBetweenTwoPoints(MATRIX *M, int label, int x1, int y1, int x2, int y2){
	int minx, maxx, miny, maxy, x, y, dimx, dimy;
	double m, c;

	if (x1 < x2)	{ minx = x1;		maxx = x2; }
	else			{ minx = x2;		maxx = x1; }
	if (y1 < y2)	{ miny = y1;		maxy = y2; }
	else			{ miny = y2;		maxy = y1; }			
	dimx = M->row;	dimy = M->column;

	if (x1 == x2){
		for (y = miny; y <= maxy; y++)
			M->data[x1][y] = label;
		return;
	}
	if (y1 == y2){
		for (x = minx; x <= maxx; x++)
			M->data[x][y1] = label;
		return;
	}
	m = (double)(y1-y2) / (x1-x2);
	c = y1 - m*x1;
	for (x = minx; x <= maxx; x++){
		y = (int)(m*x + c);
		if (y >= dimy)	y = dimy - 1;		if (y < 0)		y = 0;
		M->data[x][y] = label;
		if (y+1 < dimy)						M->data[x][y+1] = label;
	}
	for (y = miny; y <= maxy; y++){
		x = (int)((y-c) / m);
		if (x >= dimx)	x = dimx - 1;		if (x < 0)		x = 0;
		M->data[x][y] = label;
		if (x+1 < dimx)						M->data[x+1][y] = label;
	}
}
void bwerode2 (MATRIX *M, MATRIX S){
	MATRIX R = allocateMatrix(M->row,M->column);
	long i, j, k1, k2;
	int msize = S.row / 2;

	if (S.row % 2 == 0)		// if even, exit
		terminateProgram("\nError: Size of the structural element should be odd\n\n");

	initializeMatrix(&R,1);
	for (i = 0; i < M->row; i++)
		for (j = 0; j < M->column; j++){
			int found = 0;
			if ((M->data[i][j]) != 0 && (M->data[i][j]) != 1)
				terminateProgram("\nError: Only BW images can be eroded\n\n");

			for (k1 = -msize; k1 <= msize; k1++){
				for (k2 = -msize; k2 <= msize; k2++){
					int x = i + k1; 
					int y = j + k2;
					if ((x < 0) || (x >= M->row) || (y < 0) || (y >= M->column))
						continue;
					if (S.data[k1 + msize][k2 + msize] && (M->data[x][y] == 0)){
						R.data[i][j] = 0;
						found = 1;
						break;
					}
				}
				if (found)
					break;
			}
		}
	for (i = 0; i < M->row; i++)
		for (j = 0; j < M->column; j++)
			M->data[i][j] = R.data[i][j];
	freeMatrix(R);
}
void bwsuberode2 (MATRIX *M, MATRIX S, int minx, int maxx, int miny, int maxy){
	MATRIX R = allocateMatrix(maxx-minx+1,maxy-miny+1);
	long i, j, k1, k2;
	int msize = S.row / 2;

	if (S.row % 2 == 0)		// if even, exit
		terminateProgram("\nError: Size of the structural element should be odd\n\n");

	initializeMatrix(&R,1);
	for (i = 0; i < R.row; i++)
		for (j = 0; j < R.column; j++){
			int found = 0;

			if ((M->data[i+minx][j+miny]) != 0 && (M->data[i+minx][j+miny]) != 1)
				terminateProgram("\nError: Only BW images can be eroded\n\n");

			for (k1 = -msize; k1 <= msize; k1++){
				for (k2 = -msize; k2 <= msize; k2++){
					int x = i + k1; 
					int y = j + k2;
					
					if ((x < 0) || (x >= R.row) || (y < 0) || (y >= R.column))
						continue;
					if (S.data[k1 + msize][k2 + msize] && (M->data[x+minx][y+miny] == 0)){
						R.data[i][j] = 0;
						found = 1;
						break;
					}
				}
				if (found)
					break;
			}
		}
	for (i = 0; i < R.row; i++)
		for (j = 0; j < R.column; j++)
			M->data[i+minx][j+miny] = R.data[i][j];
	freeMatrix(R);
}
void bnderode2(MATRIX *M, MATRIX original, MATRIX S){
	MATRIX R = allocateMatrix(M->row,M->column);
	long i, j, k1, k2;
	int msize = S.row / 2;

	if (S.row % 2 == 0)		// if even, exit
		terminateProgram("\nError: Size of the structural element should be odd\n\n");

	initializeMatrix(&R,1);
	for (i = 0; i < M->row; i++)
		for (j = 0; j < M->column; j++){
			int found = 0;
			if ((M->data[i][j]) != 0 && (M->data[i][j]) != 1)
				terminateProgram("\nError: Only BW images can be eroded\n\n");

			for (k1 = -msize; k1 <= msize; k1++){
				for (k2 = -msize; k2 <= msize; k2++){
					int x = i + k1; 
					int y = j + k2;
					if ((x < 0) || (x >= M->row) || (y < 0) || (y >= M->column)){
						if (original.data[i][j] == 0){
							R.data[i][j] = 0;
							found = 1;
							break;
						}
						continue;
					}
					if (S.data[k1 + msize][k2 + msize] && (M->data[x][y] == 0)){
						R.data[i][j] = 0;
						found = 1;
						break;
					}
				}
				if (found)
					break;
			}
		}
	for (i = 0; i < M->row; i++)
		for (j = 0; j < M->column; j++)
			M->data[i][j] = R.data[i][j];
	freeMatrix(R);
}
void bwdilate2(MATRIX *M, MATRIX S){
	MATRIX R = allocateMatrix(M->row,M->column);
	long i, j, k1, k2;
	int msize = S.row / 2;

	if (S.row % 2 == 0)		// if even, exit
		terminateProgram("\nError: Size of the structural element should be odd\n\n");

	initializeMatrix(&R,0);
	for (i = 0; i < M->row; i++)
		for (j = 0; j < M->column; j++){
			int found = 0;
			if ((M->data[i][j]) != 0 && (M->data[i][j]) != 1)
				terminateProgram("\nError: Only BW images can be dilated\n\n");

			for (k1 = -msize; k1 <= msize; k1++){
				for (k2 = -msize; k2 <= msize; k2++){
					int x = i + k1; 
					int y = j + k2;
					if ((x < 0) || (x >= M->row) || (y < 0) || (y >= M->column))
						continue;
					if (S.data[k1 + msize][k2 + msize] && (M->data[x][y] == 1)){
						R.data[i][j] = 1;
						found = 1;
						break;
					}
				}
				if (found)
					break;
			}
		}
	for (i = 0; i < M->row; i++)
		for (j = 0; j < M->column; j++)
			M->data[i][j] = R.data[i][j];
	freeMatrix(R);
}
void bwsubdilate2 (MATRIX *M, MATRIX S, int minx, int maxx, int miny, int maxy){
	MATRIX R = allocateMatrix(maxx-minx+1,maxy-miny+1);
	long i, j, k1, k2;
	int msize = S.row / 2;

	if (S.row % 2 == 0)		// if even, exit
		terminateProgram("\nError: Size of the structural element should be odd\n\n");

	initializeMatrix(&R,0);
	for (i = 0; i < R.row; i++)
		for (j = 0; j < R.column; j++){
			int found = 0;
			if ((M->data[i+minx][j+miny]) != 0 && (M->data[i+minx][j+miny]) != 1)
				terminateProgram("\nError: Only BW images can be dilated\n\n");

			for (k1 = -msize; k1 <= msize; k1++){
				for (k2 = -msize; k2 <= msize; k2++){
					int x = i + k1; 
					int y = j + k2;
					if ((x < 0) || (x >= R.row) || (y < 0) || (y >= R.column))
						continue;
					if (S.data[k1 + msize][k2 + msize] && (M->data[x+minx][y+miny] == 1)){
						R.data[i][j] = 1;
						found = 1;
						break;
					}
				}
				if (found)
					break;
			}
		}
	for (i = 0; i < R.row; i++)
		for (j = 0; j < R.column; j++)
			M->data[i+minx][j+miny] = R.data[i][j];
	freeMatrix(R);
}
MATRIX bwdilate(MATRIX M, MATRIX S){
	MATRIX R = allocateMatrix(M.row,M.column);
	long i, j, k1, k2;
	int msize = S.row / 2;

	if (S.row % 2 == 0)		// if even, exit
		terminateProgram("\nError: Size of the structural element should be odd\n\n");

	initializeMatrix(&R,0);
	for (i = 0; i < M.row; i++)
		for (j = 0; j < M.column; j++){
			int found = 0;
			if ((M.data[i][j]) != 0 && (M.data[i][j]) != 1)
				terminateProgram("\nError: Only BW images can be dilated\n\n");

			for (k1 = -msize; k1 <= msize; k1++){
				for (k2 = -msize; k2 <= msize; k2++){
					int x = i + k1; 
					int y = j + k2;
					if ((x < 0) || (x >= M.row) || (y < 0) || (y >= M.column))
						continue;
					if (S.data[k1 + msize][k2 + msize] && (M.data[x][y] == 1)){
						R.data[i][j] = 1;
						found = 1;
						break;
					}
				}
				if (found)
					break;
			}
		}
	return R;
}
void bwopen2 (MATRIX *M, MATRIX S){
	bwerode2(M,S);
	bwdilate2(M,S);
}
MATRIX createDiskStructuralElement8(){
	MATRIX M = allocateMatrix(15,15);

	initializeMatrix(&M,1);
	M.data[0][0]   = M.data[0][1]   = M.data[0][2]   = M.data[0][3]   = M.data[1][0]   = 0;
	M.data[1][1]   = M.data[1][2]   = M.data[2][0]   = M.data[2][1]   = M.data[3][0]   = 0;
	M.data[0][14]  = M.data[0][13]  = M.data[0][12]  = M.data[0][11]  = M.data[1][14]  = 0;
	M.data[1][13]  = M.data[1][12]  = M.data[2][14]  = M.data[2][13]  = M.data[3][14]  = 0;
	M.data[14][0]  = M.data[14][1]  = M.data[14][2]  = M.data[14][3]  = M.data[13][0]  = 0;
	M.data[13][1]  = M.data[13][2]  = M.data[12][0]  = M.data[12][1]  = M.data[11][0]  = 0;
	M.data[14][14] = M.data[14][13] = M.data[14][12] = M.data[14][11] = M.data[13][14] = 0;
	M.data[13][13] = M.data[13][12] = M.data[12][14] = M.data[12][13] = M.data[11][14] = 0;
	
	return M;
}
MATRIX createDiskStructuralElement6(){
	MATRIX M = allocateMatrix(11,11);

	initializeMatrix(&M,1);
	M.data[0][0]  = M.data[0][1]  = M.data[0][9]  = M.data[0][10]  = 0;
	M.data[1][0]  = M.data[1][10] = M.data[9][0]  = M.data[9][10]  = 0;
	M.data[10][0] = M.data[10][1] = M.data[10][9] = M.data[10][10] = 0;
	return M;
}
MATRIX createDiskStructuralElement5(){
	MATRIX M = allocateMatrix(9,9);

	initializeMatrix(&M,1);
	M.data[0][0] = M.data[0][1] = M.data[0][7] = M.data[0][8] = 0;
	M.data[1][0] = M.data[1][8] = M.data[7][0] = M.data[7][8] = 0;
	M.data[8][0] = M.data[8][1] = M.data[8][7] = M.data[8][8] = 0;
	return M;
}
MATRIX createDiskStructuralElement4(){
	MATRIX M = allocateMatrix(7,7);

	initializeMatrix(&M,1);
	M.data[0][0] = M.data[0][1] = M.data[1][0] = M.data[5][0] = M.data[6][0] = M.data[6][1] = 0;
	M.data[0][6] = M.data[0][5] = M.data[1][6] = M.data[5][6] = M.data[6][6] = M.data[6][5] = 0;
	return M;
}
MATRIX createDiskStructuralElement2(){
	MATRIX M = allocateMatrix(5,5);

	initializeMatrix(&M,1);
	M.data[0][0] = M.data[0][1] = M.data[0][3] = M.data[0][4] = 0;
	M.data[1][0] = M.data[1][4] = M.data[3][0] = M.data[3][4] = 0;
	M.data[4][0] = M.data[4][1] = M.data[4][3] = M.data[4][4] = 0;
	return M;
}
MATRIX createOctagonStructuralElement3(){
	MATRIX M = allocateMatrix(7,7);

	initializeMatrix(&M,1);
	M.data[0][0] = M.data[0][1] = M.data[0][5] = M.data[0][6] = 0;
	M.data[1][0] = M.data[1][6] = M.data[5][0] = M.data[5][6] = 0;
	M.data[6][0] = M.data[6][1] = M.data[6][5] = M.data[6][6] = 0;
	return M;
}
MATRIX createOctagonStructuralElement4(){
	MATRIX M = allocateMatrix(9,9);

	initializeMatrix(&M,1);
	M.data[0][0] = M.data[0][1] = M.data[0][7] = M.data[0][8] = 0;
	M.data[1][0] = M.data[1][8] = M.data[7][0] = M.data[7][8] = 0;
	M.data[8][0] = M.data[8][1] = M.data[8][7] = M.data[8][8] = 0;

	M.data[0][2] = M.data[0][6] = M.data[8][2] = M.data[8][6] = 0;
	M.data[1][1] = M.data[1][7] = M.data[7][1] = M.data[7][7] = 0;
	M.data[2][0] = M.data[2][8] = M.data[6][0] = M.data[6][8] = 0;
	return M;
}
MATRIX createOctagonStructuralElement6(){
	MATRIX M = allocateMatrix(13,13);
	int i;

	initializeMatrix(&M,1);
	for (i = 0; i < 4; i++)
		M.data[0][i] = M.data[0][12-i] = M.data[12][i] = M.data[12][12-i] = 0;
	for (i = 0; i < 3; i++)
		M.data[1][i] = M.data[1][12-i] = M.data[11][i] = M.data[11][12-i] = 0;
	for (i = 0; i < 2; i++)
		M.data[2][i] = M.data[2][12-i] = M.data[10][i] = M.data[10][12-i] = 0;
	M.data[3][0] = M.data[3][12] = M.data[9][0] = M.data[9][12] = 0;
	return M;
}
MATRIX createSquareStructuralElement(int a){
	MATRIX se = allocateMatrix(a,a);
	initializeMatrix(&se,1);
	return se;
}
void markRectangularRegions(MATRIX *reg, MATRIX bndbox){
	int i, j, minx, maxx, miny, maxy;

	initializeMatrix(reg,0);
	for (i = 1; i < bndbox.row; i++){
		minx =  bndbox.data[i][0];
		if (minx == -1)
			continue;
		maxx =  bndbox.data[i][1];
		miny =  bndbox.data[i][2];
		maxy =  bndbox.data[i][3];
		for (j = minx; j <= maxx; j++)
			reg->data[j][miny] = reg->data[j][maxy] = 1;
		for (j = miny; j <= maxy; j++)
			reg->data[minx][j] = reg->data[maxx][j] = 1;
		fillInside(reg,1,minx,maxx,miny,maxy);
	}
}
MATRIX findRegionBoundaries(MATRIX R, int background){
	MATRIX bnd = allocateMatrix(R.row,R.column);
	int i, j, rno;

	initializeMatrix(&bnd,0);
	for (i = 0; i < R.row; i++)
		for (j = 0; j < R.column; j++){
			if (R.data[i][j] == background)
				continue;
			if ((i-1 < 0) || (i+1 >= R.row) || (j-1 < 0) || (j+1 >= R.column)){
				bnd.data[i][j] = 1;
				continue;
			}

			rno = R.data[i][j];
			if (((R.data[i-1][j] != rno) && (bnd.data[i-1][j] == 0)) ||
				((R.data[i+1][j] != rno) && (bnd.data[i+1][j] == 0)) ||
				((R.data[i][j-1] != rno) && (bnd.data[i][j-1] == 0)) ||
				((R.data[i][j+1] != rno) && (bnd.data[i][j+1] == 0)))
				bnd.data[i][j] = 1;
		}
	return bnd;
}
void bwareaopen2(MATRIX *M, int areaThr){
	int maxNo = maxMatrixEntry(*M), i, j, *areas;
	MATRIX tmp;

	if (maxNo == 0)
		return;
	else if (maxNo == 1){
		tmp = fourConnectivity(*M);
		copyMatrix(M,tmp);
		freeMatrix(tmp);
	}
	maxNo  = maxMatrixEntry(*M);
	areas = (int *)calloc(maxNo+1,sizeof(int));
	for (i = 0; i < M->row; i++)
		for (j = 0; j < M->column; j++)
			if (M->data[i][j])
				areas[M->data[i][j]]++;

	for (i = 0; i < M->row; i++)
		for (j = 0; j < M->column; j++)
			if (M->data[i][j] && (areas[M->data[i][j]] < areaThr))
				M->data[i][j] = 0;
	free(areas);
	relabelComponents(M);
}
void bwextend2(MATRIX *M, int d){
	int i, j, k;

	for (k = 1; k <= d; k++){
		for (i = 0; i < M->row; i++)
			for (j = 0; j < M->column; j++)
				if (M->data[i][j] == k){
					if ((i-1 >= 0) && (M->data[i-1][j] == 0))			M->data[i-1][j] = k+1;
					if ((i+1 < M->row) && (M->data[i+1][j] == 0))		M->data[i+1][j] = k+1;
					if ((j-1 >= 0) && (M->data[i][j-1] == 0))			M->data[i][j-1] = k+1;
					if ((j+1 < M->column) && (M->data[i][j+1] == 0))	M->data[i][j+1] = k+1;
				}
	}
	for (i = 0; i < M->row; i++)
		for (j = 0; j < M->column; j++)
			if (M->data[i][j])
				M->data[i][j] = 1;
}
BNDBOX *calculateBoundingBoxes(MATRIX reg, int background){
	int regNo = maxMatrixEntry(reg), i, j, rno;
	BNDBOX *bnd = (BNDBOX *)calloc(regNo+1,sizeof(BNDBOX));

	for (i = 0; i <= regNo; i++)
		bnd[i].minx = bnd[i].maxx = bnd[i].miny = bnd[i].maxy = -1;

	for (i = 0; i < reg.row; i++)
		for (j = 0; j < reg.column; j++)
			if (reg.data[i][j] != background){
				rno = reg.data[i][j];
				if ((bnd[rno].minx == -1) || (i < bnd[rno].minx))		bnd[rno].minx = i;
				if ((bnd[rno].maxx == -1) || (i > bnd[rno].maxx))		bnd[rno].maxx = i;
				if ((bnd[rno].miny == -1) || (j < bnd[rno].miny))		bnd[rno].miny = j;
				if ((bnd[rno].maxy == -1) || (j > bnd[rno].maxy))		bnd[rno].maxy = j;
			}
	return bnd;
}
BNDBOX *calculateBoundingBoxesAreas(MATRIX reg, int background, int **areas){
	int regNo = maxMatrixEntry(reg), i, j, rno;
	BNDBOX *bnd = (BNDBOX *)calloc(regNo+1,sizeof(BNDBOX));
	
	*areas = (int *)calloc(regNo+1,sizeof(int));

	for (i = 0; i <= regNo; i++)
		bnd[i].minx = bnd[i].maxx = bnd[i].miny = bnd[i].maxy = -1;

	for (i = 0; i < reg.row; i++)
		for (j = 0; j < reg.column; j++)
			if (reg.data[i][j] != background){
				rno = reg.data[i][j];
				(*areas)[rno]++;
				if ((bnd[rno].minx == -1) || (i < bnd[rno].minx))		bnd[rno].minx = i;
				if ((bnd[rno].maxx == -1) || (i > bnd[rno].maxx))		bnd[rno].maxx = i;
				if ((bnd[rno].miny == -1) || (j < bnd[rno].miny))		bnd[rno].miny = j;
				if ((bnd[rno].maxy == -1) || (j > bnd[rno].maxy))		bnd[rno].maxy = j;
			}
	return bnd;
}
BNDBOX newBoundingBox(int minx, int maxx, int miny, int maxy){
	BNDBOX bnd;

	bnd.minx = minx;
	bnd.maxx = maxx;
	bnd.miny = miny;
	bnd.maxy = maxy;
	return bnd;
}
void extendBoundingBoxes(BNDBOX *bnd, int no, int offset, int dimx, int dimy){
	int k;

	for (k = 0; k <= no; k++){
		bnd[k].minx -= offset;		if (bnd[k].minx < 0)			bnd[k].minx = 0;
		bnd[k].maxx += offset;		if (bnd[k].maxx >= dimx)		bnd[k].maxx = dimx-1;
		bnd[k].miny -= offset;		if (bnd[k].miny < 0)			bnd[k].miny = 0;
		bnd[k].maxy += offset;		if (bnd[k].maxy >= dimy)		bnd[k].maxy = dimy-1;
	}
}
void createSobels(MATRIX *hhor, MATRIX *hver){
	*hhor = allocateMatrix(3,3);
	*hver = allocateMatrix(3,3);
	initializeMatrix(hhor,0);
	initializeMatrix(hver,0);

	hhor->data[0][0] = hhor->data[0][2] = 1;
	hver->data[0][0] = hver->data[2][0] = 1; 
	hhor->data[2][0] = hhor->data[2][2] = -1;
	hver->data[0][2] = hver->data[2][2] = -1;
	hhor->data[0][1] = hver->data[1][0] = 2;
	hhor->data[2][1] = hver->data[1][2] = -2;
}
MATRIX createCircularWindow(int thr){
	MATRIX win = allocateMatrix(2*thr+1,2*thr+1);
	int i, j;
	
	for (i = 0; i < win.row; i++)
		for (j = 0; j < win.column; j++)
			if (SQUARE(i-thr) + SQUARE(j-thr) <= SQUARE(thr))
				win.data[i][j] = 1;
			else
				win.data[i][j] = 0;
	
	return win;
}
CIRCLE_BND *createCircularBoundaries(int maxRadius){
	CIRCLE_BND *B = (CIRCLE_BND *)calloc(maxRadius,sizeof(CIRCLE_BND));
	MATRIX M = allocateMatrix(maxRadius,maxRadius);
	int i, j, k, cnt;

	initializeMatrix(&M,-1);
	
	M.data[0][0] = 0;
	B[0].N = 1;
	B[0].r = 0;
	B[0].x = (int *)calloc(1,sizeof(int));
	B[0].y = (int *)calloc(1,sizeof(int));
	B[0].x[0] = 0;
	B[0].y[0] = 0;

	for (k = 1; k < maxRadius; k++){
		B[k].r = k;
		B[k].N = 0;
		B[k].x = B[k].y = NULL;
		for (i = 0; i <= k; i++)
			for (j = 0; j <= k; j++){
				if (M.data[i][j] != -1)
					continue;
				if (i*i + j*j <= k*k){
					M.data[i][j] = k;
					B[k].N += 1;
				}
			}
		if (B[k].N == 0)
			continue;
		B[k].x = (int *)calloc(B[k].N,sizeof(int));
		B[k].y = (int *)calloc(B[k].N,sizeof(int));
		cnt = 0;
		for (i = 0; i <= k; i++)
			for (j = 0; j <= k; j++)
				if (M.data[i][j] == k){
					B[k].x[cnt] = i;
					B[k].y[cnt] = j;
					cnt++;
				}
	}
	//writeMatrixIntoFile(M,"temp",0);
	freeMatrix(M);
	return B;
}
void freeCircularBoundaries(CIRCLE_BND *B, int maxRadius){
	int i;

	for (i = 0; i < maxRadius; i++){
		free(B[i].x);
		free(B[i].y);
	}
	free(B);
}
void imfill2(MATRIX *L){
	int i, j, maxNo, *reached;
	MATRIX tmp;
	
	for (i = 0; i < L->row; i++)
		for (j = 0; j < L->column; j++)
			if (L->data[i][j])
				L->data[i][j] = 0;
			else
				L->data[i][j] = 1;
	tmp = fourConnectivity(*L);
	
	maxNo = maxMatrixEntry(tmp);
	reached = (int *)calloc(maxNo+1,sizeof(int));
	for (i = 0; i < tmp.row; i++)
		reached[ tmp.data[i][0] ] = reached[ tmp.data[i][tmp.column-1] ] = 1;
	for (i = 0; i < tmp.column; i++)
		reached[ tmp.data[0][i] ] = reached[ tmp.data[tmp.row-1][i] ] = 1;
	
	
	for (i = 0; i < L->row; i++)
		for (j = 0; j < L->column; j++)
			if (L->data[i][j] == 0)
				L->data[i][j] = 1;
			else if (reached[ tmp.data[i][j] ] == 0)
				L->data[i][j] = 1;
			else
				L->data[i][j] = 0;
}
double findNearestPts(const Pt *pts, int pts_size, int r, int c)
{
	int i;
	Pt n;
	long d, min_dist;

	return min_dist;
}
int numelNonzero(const MATRIX map){
	int i,j,numel = 0;
	for (i=0;i<map.row;i++)
		for (j=0;j<map.column;j++)
			if (map.data[i][j] != 0)
				numel++;
	return numel;
}
MATRIX bwdist2(const MATRIX bw){
	int num_wh = numelNonzero(bw), pts_index = 0, i, j;
	Pt *pts, *p;
	MATRIX distMap = allocateMatrix(bw.row, bw.column);
	
	pts = (Pt*) malloc(sizeof(Pt) * num_wh);

	for (i=0;i<bw.row;i++)
		for (j=0;j<bw.column;j++)
			if (bw.data[i][j] == 1)
			{
				pts[pts_index].r = i;
				pts[pts_index].c = j;
				pts[pts_index++].val = 0;
			}

	for (i=0;i<bw.row;i++)
		for (j=0;j<bw.column;j++)
			if (bw.data[i][j] == 0)
				distMap.data[i][j] = findNearestPts(pts, num_wh, i, j);
	free(pts);
	return distMap;
}
/* http: //liris.cnrs.fr/david.coeurjolly/Enseignement/DistanceTransform.pdf */
MATRIX bwdist_(const MATRIX* bw){
	int i,j,z,lastWh;
	long long t, mind;
	MATRIX tmp, dist;

	tmp = allocateMatrix(bw->row, bw->column);
	dist = allocateMatrix(bw->row, bw->column);

	for (i=0; i<bw->row; i++)
	{
		lastWh = -1;
		for (j=0; j<bw->column; j++)
		{
			if (bw->data[i][j] == 0)
			{
				if (lastWh == -1)
					tmp.data[i][j] = MAX_DIST;
				else
					tmp.data[i][j] = (lastWh - j) * (lastWh - j);
			}
			else
			{
				lastWh = j;
				tmp.data[i][j] = 0;
			}
		}

		lastWh = -1;
		for (j=bw->column-1; j>=0; j--)
		{
			if (bw->data[i][j] == 0)
			{
				if (lastWh != -1)
				{
					t = (lastWh - j) * (lastWh - j);
					if (t < tmp.data[i][j])
						tmp.data[i][j] = t;
				}
			}
			else
				lastWh = j;
		}
	}
	
	initializeMatrix(&dist, MAX_DIST);
	for (i=0; i<tmp.row; i++)
	{
		for (j=0; j<tmp.column; j++)
		{
			mind = MAX_DIST;
			for (z=0; z<tmp.row; z++)
			{
				t = (i - z) * (i - z) + tmp.data[z][j];
				if (t < mind)
				{
					mind = t;
				}
			}		
			dist.data[i][j] = mind;
		}
	}
	freeMatrix(tmp);
	return dist;
}

MATRIX orMatrix(const MATRIX *a, const MATRIX *b)
{
	int i,j;
	MATRIX c = allocateMatrix(a->row, a->column);
	initializeMatrix(&c, 0);
	for (i=0;i<a->row;i++)
	{
		for (j=0;j<a->column;j++)
		{
			if (a->data[i][j] > 0 || b->data[i][j] > 0)
				c.data[i][j] = 1;
		}
	}
	return c;
}

MATRIX findEdge(MATRIX L, int thickness)
{
	int dx, dy, i2, j2, k;
	MATRIX L2, L3, edge, kucuk;
	dx = L.row;
	dy = L.column;
    
	L2 = allocateMatrix(dx, dy);
	edge = allocateMatrix(dx, dy);
	initializeMatrix(&edge, 0);
	initializeMatrix(&L2, 0);

	if (thickness == 0)
	{
		copyMatrix(&L, edge);
        return edge;
	}
    else
	{
		for (i2=0;i2<dx;i2++)
            for (j2=0;j2<dy;j2++)
                if (L.data[i2][j2] > 0)
					if ((i2-1 >= 0) && (L.data[i2-1][j2] == 0))
						L2.data[i2][j2] = L.data[i2][j2];
					else if ((i2+1 < dx) && (L.data[i2+1][j2] == 0))
						L2.data[i2][j2] = L.data[i2][j2];
					else if ((j2-1 >= 0) && (L.data[i2][j2-1] == 0))
						L2.data[i2][j2] = L.data[i2][j2];
					else if ((j2+1 < dy) && (L.data[i2][j2+1] == 0))
						L2.data[i2][j2] = L.data[i2][j2];
	}
	copyMatrix(&edge, L2);
    if (thickness > 1)
	{
		kucuk = allocateMatrix(dx, dy);
		initializeMatrix(&kucuk, 0);
		copyMatrix(&kucuk, L2);
        for (k=1;k<=thickness;k++)
		{
			L3 = allocateMatrix(dx,dy);
			initializeMatrix(&L3, 0);
            for (i2=0;i2<dx;i2++)
                for (j2=0;j2<dy;j2++)
                    if (kucuk.data[i2][j2] > 0)
					{
						if ((i2-1 >= 0) && (L.data[i2-1][j2] > 0))
							L3.data[i2-1][j2] = 1;
						else if ((i2+1 < dx) && (L.data[i2+1][j2] > 0))
							L3.data[i2+1][j2] = 1;
						else if ((j2-1 >= 0) && (L.data[i2][j2-1] > 0))
							L3.data[i2][j2-1] = 1;
						else if ((j2+1 < dy) && (L.data[i2][j2+1] > 0))
							L3.data[i2][j2+1] = 1;
					}
			edge = orMatrix(&edge, &kucuk);
			copyMatrix(&kucuk, L3);
			freeMatrix(L3);
		}
		freeMatrix(kucuk);
	}
	freeMatrix(L2);
	return edge;
}