#ifndef QUADTREE_H
#define QUADTREE_H

#include <stdio.h>

enum { CHEIO, PARCIAL };

struct Quad {
    unsigned int id;
    float x, y;              // canto superior esquerdo da região
    float width, height;     // largura e altura da região
    int status;              // CHEIO ou PARCIAL
    unsigned char color[3];  // cor média da região
    struct Quad* NW;            // ponteiros para os filhos, se houver
    struct Quad* NE;
    struct Quad* SW;
    struct Quad* SE;
};

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} RGB;

typedef struct {
    int width, height;
    RGB *img;
} Img;

typedef struct Quad QuadNode;

QuadNode* geraQuadtree(Img* img, float minDetail);
QuadNode* newNode(int x, int y, int width, int height);

void clearTree(QuadNode* n);
void drawTree(QuadNode* raiz);
void drawNode(QuadNode* n);
void writeTree(QuadNode* raiz);
void writeNode(FILE* fp, QuadNode* n);
void toggleBorder();

#endif // QUADTREE_H
