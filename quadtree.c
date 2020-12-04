#include "quadtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>     /* OpenGL functions */
#endif

unsigned int first = 1;
char desenhaBorda = 1;

float calculateLevelDetailOfRegion(Img* pic, QuadNode* region, int minDetail);
QuadNode* newNode(int x, int y, int width, int height);
QuadNode* geraQuadTreeAux(QuadNode* root, int minDetail, Img* pic);
QuadNode* geraQuadtree(Img* pic, float minDetail);
void drawTree(QuadNode* raiz);

QuadNode* newNode(int x, int y, int width, int height)
{
    QuadNode* n = malloc(sizeof(QuadNode));
    n->x = x;
    n->y = y;
    n->width = width;
    n->height = height;
    n->NW = n->NE = n->SW = n->SE = NULL;
    n->color[0] = n->color[1] = n->color[2] = 0;
    n->id = first++;
    return n;
}

QuadNode* geraQuadTreeAux(QuadNode* root, int minDetail, Img* pic){
    calculateLevelDetailOfRegion(pic, root,minDetail);
    
    if(root->status == PARCIAL){
        root->NW = geraQuadTreeAux(newNode(root->x,root->y,root->width/2,root->height/2), minDetail, pic);
        root->NE = geraQuadTreeAux(newNode(root->x + root->width/2,root->y,root->width/2,root->height/2), minDetail, pic);
        root->SW = geraQuadTreeAux(newNode(root->x,root->y + root->height/2,root->width/2,root->height/2), minDetail, pic);
        root->SE = geraQuadTreeAux(newNode(root->x + root->width/2,root->y + root->height/2,root->width/2,root->height/2), minDetail, pic);
    }
    return root;
}

QuadNode* geraQuadtree(Img* pic, float minDetail)
{   
    // Converte o vetor RGB para uma MATRIZ que pode acessada por pixels[linha][coluna]
    //RGB (*pixels)[pic->width] = (RGB(*)[pic->width]) pic->img;

    // Veja como acessar os primeiros 10 pixels da imagem, por exemplo:
    //int i;
    //for(i=0; i<10; i++)
    //    printf("%02X %02X %02X\n",pixels[0][i].r,pixels[1][i].g,pixels[2][i].b);

    QuadNode* root = newNode(0,0,pic->width,pic->height);
    calculateLevelDetailOfRegion(pic, root,minDetail);
    printf("%d\n", root->status);
    printf("%d\n", root->color[0]);
    printf("%d\n", root->color[1]);
    printf("%d\n", root->color[2]);
    if( root->status == PARCIAL){
        root->NW = geraQuadTreeAux(newNode(root->x,root->y,root->width/2,root->height/2), minDetail, pic);
        root->NE = geraQuadTreeAux(newNode(root->x + root->width/2,root->y,root->width/2,root->height/2), minDetail, pic);
        root->SW = geraQuadTreeAux(newNode(root->x,root->y + root->height/2,root->width/2,root->height/2), minDetail, pic);
        root->SE = geraQuadTreeAux(newNode(root->x + root->width/2,root->y + root->height/2,root->width/2,root->height/2), minDetail, pic);
    }



// COMENTE a linha abaixo quando seu algoritmo ja estiver funcionando
// Caso contrario, ele ira gerar uma arvore de teste com 3 nodos

//#define DEMO
#ifdef DEMO

    /************************************************************/
    /* Teste: criando uma raiz e dois nodos a mais              */
    /************************************************************/

     int width = pic->width;
     int height = pic->height;

     QuadNode* raiz = newNode(0,0,width,height);
     raiz->status = PARCIAL;
     raiz->color[0] = 0;
     raiz->color[1] = 0;
     raiz->color[2] = 255;

     //NE
     QuadNode* ne = newNode(raiz->width/2,0,raiz->width/2,raiz->height/2);
     ne->status = PARCIAL;
     ne->color[0] = 0;
     ne->color[1] = 0;
     ne->color[2] = 255;

    // // Aponta da raiz para o nodo nw
     raiz->NE = ne;

     QuadNode* ne2 = newNode(width/2+width/4,0,width/4,height/4);
     ne2->status = CHEIO;
     ne2->color[0] = 255;
     ne2->color[1] = 0;
     ne2->color[2] = 0;

     // Aponta do nodo nw para o nodo nw2
     ne->NE = ne2;

#endif
    // Finalmente, retorna a raiz da árvore
    drawTree(root);
    return root;
}

float calculateLevelDetailOfRegion(Img* pic, QuadNode* region, int minDetail){
    RGB (*pixels)[pic->width] = (RGB(*)[pic->width]) pic->img;

    double diferenca = 0;
    double meR = 0;
    double meG = 0;
    double meB = 0;
    for(int i = region->y; i < region->y + region->height; i++){
        for(int j = region->x; j < region->x + region->width; j++){
            meR += pixels[i][j].r;
            meG += pixels[i][j].g;
            meB += pixels[i][j].b;
        }
    }

    int area = region->height * region->width;

    region->color[0] = (int) meR/(area);
    region->color[1] = (int) meG/(area);
    region->color[2] = (int) meB/(area);

    for(int i = region->y; i < region->y + region->height; i++){
        for(int j = region->x; j < region->x + region->width; j++){
            diferenca += sqrt(pow(pixels[i][j].r - region->color[0],2) + pow(pixels[i][j].g - region->color[1],2) + pow(pixels[i][j].b - region->color[2],2));
        }
    }

    diferenca = diferenca/(area);
    region-> status = (diferenca <= minDetail) ? CHEIO : PARCIAL;
    //printf("%f\n",diferenca);
    //printf("CHEIO %d\n",CHEIO);
    //printf("PARCIAL %d\n",PARCIAL);

    //printf("%d\n",region->color[0]);
    //printf("%d\n",region->color[1]);
    //printf("%d\n",region->color[2]);
    return diferenca;
}

// Limpa a memória ocupada pela árvore
void clearTree(QuadNode* n)
{
    if(n == NULL) return;
    if(n->status == PARCIAL)
    {
        clearTree(n->NE);
        clearTree(n->NW);
        clearTree(n->SE);
        clearTree(n->SW);
    }
    //printf("Liberando... %d - %.2f %.2f %.2f %.2f\n", n->status, n->x, n->y, n->width, n->height);
    free(n);
}

// Ativa/desativa o desenho das bordas de cada região
void toggleBorder() {
    desenhaBorda = !desenhaBorda;
    printf("Desenhando borda: %s\n", desenhaBorda ? "SIM" : "NÃO");
}

// Desenha toda a quadtree
void drawTree(QuadNode* raiz) {
    if(raiz != NULL)
        drawNode(raiz);
}

// Grava a árvore no formato do Graphviz
void writeTree(QuadNode* raiz) {
    FILE* fp = fopen("quad.dot", "w");
    fprintf(fp, "digraph quadtree {\n");
    if (raiz != NULL)
        writeNode(fp, raiz);
    fprintf(fp, "}\n");
    fclose(fp);
    printf("\nFim!\n");
}

void writeNode(FILE* fp, QuadNode* n)
{
    if(n == NULL) return;

    if(n->NE != NULL) fprintf(fp, "%d -> %d;\n", n->id, n->NE->id);
    if(n->NW != NULL) fprintf(fp, "%d -> %d;\n", n->id, n->NW->id);
    if(n->SE != NULL) fprintf(fp, "%d -> %d;\n", n->id, n->SE->id);
    if(n->SW != NULL) fprintf(fp, "%d -> %d;\n", n->id, n->SW->id);
    writeNode(fp, n->NE);
    writeNode(fp, n->NW);
    writeNode(fp, n->SE);
    writeNode(fp, n->SW);
}

// Desenha todos os nodos da quadtree, recursivamente
void drawNode(QuadNode* n)
{
    if(n == NULL) return;

    glLineWidth(0.1);

    if(n->status == CHEIO) {
        glBegin(GL_QUADS);
        glColor3ubv(n->color);
        glVertex2f(n->x, n->y);
        glVertex2f(n->x+n->width-1, n->y);
        glVertex2f(n->x+n->width-1, n->y+n->height-1);
        glVertex2f(n->x, n->y+n->height-1);
        glEnd();
    }

    else if(n->status == PARCIAL)
    {
        if(desenhaBorda) {
            glBegin(GL_LINE_LOOP);
            glColor3ubv(n->color);
            glVertex2f(n->x, n->y);
            glVertex2f(n->x+n->width-1, n->y);
            glVertex2f(n->x+n->width-1, n->y+n->height-1);
            glVertex2f(n->x, n->y+n->height-1);
            glEnd();
        }
        drawNode(n->NE);
        drawNode(n->NW);
        drawNode(n->SE);
        drawNode(n->SW);
    }
    // Nodos vazios não precisam ser desenhados... nem armazenados!
}

