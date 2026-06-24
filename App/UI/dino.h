#ifndef __DINO_H
#define __DINO_H
typedef struct Object_Position{
	uint8_t minX,minY,maxX,maxY;
}Object_Position;
void Score_Tick(void);
void Show_Groud(void);
void Show_Barrier(void);
void Show_Cloud(void);
void Show_Dino(void);
int DinoGame_Slide(void);
int isColliding(struct Object_Position *a,struct Object_Position *b);
void DinoGame_Pos_Init(void);
void Dino_Jump(void);
#endif
