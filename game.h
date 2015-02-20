
#ifndef _GAME_H_
#define _GAME_H_

#include <d3d9.h>

#define FIELD_SIZE 8
#define HALF_FIELD_SIZE FIELD_SIZE/2
#define TOTAL_COUNT FIELD_SIZE*FIELD_SIZE
#define COLOR_COUNT 5
#define START_COUNT 4
#define APPEAR_COUNT 3
#define DETONATE_COUNT 5

D3DCOLORVALUE GetColorByIndex(WORD idx);

struct TCell
{
    BOOL free;
    BOOL isNew;
    BOOL selected;
    BOOL detonating;
    WORD colorIndex;
};

struct TBallInfo
{
    WORD cellId;
    D3DCOLORVALUE color;
};

class TGame {
private:
    BOOL gameOver;
    TCell *cells;
    WORD *path;
    WORD pathLen;
    LONG score;
    void ClearField();
    WORD GetSelected();
    WORD GetNeighbours(WORD cellId, WORD *pNeighbours);
    BOOL CheckPipeDetonate(WORD *pPipeCells);
public:
    TGame();
    ~TGame();
    void New();
    BOOL CreateBalls(WORD count);
    void Select(WORD cellId);
    BOOL TryMove(WORD targetCellId);
    BOOL DetonateTest();
    WORD GetNewBallList(TBallInfo **ppNewList);
    WORD GetLastMovePath(WORD **ppMovePath);
    WORD GetDetonateList(WORD **ppDetonateList);
    LONG GetScore();
    BOOL IsGameOver();
};

#endif
