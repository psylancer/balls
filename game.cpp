#include "game.h"

D3DCOLORVALUE GetColorByIndex(WORD idx)
{
    //по индексу можно получить значение цвета для Direct3D
    D3DCOLORVALUE mat;
    ZeroMemory(&mat,sizeof(D3DCOLORVALUE));
    mat.a=1.0f;
    switch(idx)
    {
        case 0: { mat.r=1.0f; mat.g=0.0f; mat.b=0.0f; break; }
        case 1: { mat.r=0.0f; mat.g=1.0f; mat.b=0.0f; break; }
        case 2: { mat.r=0.0f; mat.g=0.0f; mat.b=1.0f; break; }
        case 3: { mat.r=1.0f; mat.g=0.0f; mat.b=1.0f; break; }
        case 4: { mat.r=1.0f; mat.g=1.0f; mat.b=0.0f; break; }
        case 5: { mat.r=0.0f; mat.g=1.0f; mat.b=1.0f; break; }
    }
    return mat;
}

void TGame::ClearField()
{
    //очистка всех ячеек
    ZeroMemory(cells,TOTAL_COUNT*sizeof(TCell));
    for(WORD i=0;i<TOTAL_COUNT;i++)
    {
        cells[i].free=TRUE;
    }
}

WORD TGame::GetSelected()
{
    //получение номера выбранного шара
    for(WORD i=0;i<TOTAL_COUNT;i++)
    {
        if(cells[i].selected)
        {
            return i;
        }
    }
    return 0xffff;
}

WORD TGame::GetNeighbours(WORD cellId, WORD *pNeighbours)
{
    //функция определяет соседние ячейки для заданной - их может быть от 2 до 4
    //в зависимости от того, где находится данная ячейка
    WORD count=0;
    if(cellId>=FIELD_SIZE)
    {
        pNeighbours[count++]=cellId-FIELD_SIZE;
    }
    if(cellId<TOTAL_COUNT-FIELD_SIZE)
    {
        pNeighbours[count++]=cellId+FIELD_SIZE;
    }
    if(cellId%FIELD_SIZE>0)
    {
        pNeighbours[count++]=cellId-1;
    }
    if((cellId+1)%FIELD_SIZE)
    {
        pNeighbours[count++]=cellId+1;
    }
    return count;
}

BOOL TGame::CheckPipeDetonate(WORD *pPipeCells)
{
    //функция проверяет, являются ли все ячейки в списке занятыми шарами одного цвета
    if(cells[pPipeCells[0]].free)
    {
        return FALSE;
    }
    WORD first=cells[pPipeCells[0]].colorIndex;
    BOOL success=TRUE;
    for(WORD i=1;i<DETONATE_COUNT;i++)
    {
        success=success&&(!cells[pPipeCells[i]].free)&&(cells[pPipeCells[i]].colorIndex==first);
    }
    if(success)
    {
        for(WORD i=0;i<DETONATE_COUNT;i++)
        {
            cells[pPipeCells[i]].detonating=TRUE;
        }
    }
    return success;
}

TGame::TGame()
{
    pathLen=0;
    score=0;
    gameOver=FALSE;
    path=NULL;
    cells=new TCell[TOTAL_COUNT];
    ClearField();
}

TGame::~TGame()
{
    if(path!=NULL)
    {
        delete[] path;
        path=NULL;
    }
    if(cells!=NULL)
    {
        delete[] cells;
        cells=NULL;
    }
}

void TGame::New()
{
    pathLen=0;
    score=0;
    gameOver=FALSE;
    if(path!=NULL)
    {
        delete[] path;
        path=NULL;
    }
    //очищаем поле
    ClearField();
    //создаем начальное кол-во шаров
    CreateBalls(START_COUNT);
}

BOOL TGame::CreateBalls(WORD count)
{
    WORD *freeCellList,freeCellCount=0;
    //сначала получаем список всех свободных ячеек
    freeCellList=new WORD[TOTAL_COUNT];
    for(WORD i=0;i<TOTAL_COUNT;i++)
    {
        if(cells[i].free)
        {
            freeCellList[freeCellCount++]=i;
        }
    }
    if(freeCellCount<count)
    {
        //если свободных ячеек недостаточно, то игра окончена
        gameOver=TRUE;
        return FALSE;
    }
    //выбираем нужное число свободных ячеек
    WORD id;
    for(WORD i=0;i<count;i++)
    {
        id=(freeCellCount-1)*rand()/RAND_MAX;
        cells[freeCellList[id]].colorIndex=(COLOR_COUNT-1)*rand()/RAND_MAX;
        cells[freeCellList[id]].detonating=FALSE;
        cells[freeCellList[id]].free=FALSE;
        cells[freeCellList[id]].isNew=TRUE;
        cells[freeCellList[id]].selected=FALSE;
        //удаляем полученный номер из списка свободных
        for(WORD j=id+1;j<freeCellCount;j++)
        {
            freeCellList[j-1]=freeCellList[j];
        }
        freeCellCount--;
    }
    delete[] freeCellList;
    return TRUE;
}

void TGame::Select(WORD cellId)
{
    //сначала снимаем выделение со всех шаров, т.к. объект game не хранит отдельно номер выделенного шара
    for(WORD i=0;i<TOTAL_COUNT;i++)
    {
        cells[i].selected=FALSE;
    }
    //если запрошенная ячейка не свободна, то помечаем активной
    if(!cells[cellId].free)
    {
        cells[cellId].selected=TRUE;
    }
}

BOOL TGame::TryMove(WORD targetCellId)
{
    //попытка перемещения в заданную ячейку
    WORD *weight,*currentSet,*goodNeighbours,*allNeighbours,
         currentCount=1,goodNeighbourCount,allNeighbourCount,
         distance=0,
         selectedId,cellId;
    if(!cells[targetCellId].free)
    {
        return FALSE;
    }
    weight=new WORD[TOTAL_COUNT];
    //алгоритм Дейкстры - вычисление матрицы весов.
    //Инициализируем свободные ячейки значением TOTAL_COUNT, а занятые 0xffff
    for(WORD i=0;i<TOTAL_COUNT;i++)
    {
        weight[i]=cells[i].free ? TOTAL_COUNT : 0xffff;
    }
    //текущий набор ячеек
    currentSet=new WORD[TOTAL_COUNT];
    //набор "хороших" соседей - поиск будет продолжен по ним
    goodNeighbours=new WORD[TOTAL_COUNT];
    //все соседи текущей ячейки (от 2 до 4)
    allNeighbours=new WORD[4];
    //начальная ячейка
    selectedId=GetSelected();
    //инициализируем текущей набор ячеек только одной, т.е. текущей
    currentSet[0]=selectedId;
    weight[currentSet[0]]=0;
    BOOL finished=FALSE;
    do
    {
        distance++; //счетчик расстояния
        goodNeighbourCount=0; //"хорошие" соседи - это свободные соседние ячейки, помеченные бОльшим расстоянием, чем оно реально может быть
        //проходим по всем текущим ячейкам
        for(WORD i=0;(i<currentCount);i++)
        {
            //получаем всех соседей одной из текущих ячеек
            allNeighbourCount=GetNeighbours(currentSet[i],allNeighbours); //все 2, 3 или 4 соседа
            for(WORD j=0;j<allNeighbourCount;j++)
            {
                //если соседняя ячейка свободна и ее вес больше текущего счетчика состояния, то обновляем ее и запоминаем, как хорошего соседа
                if((weight[allNeighbours[j]]!=0xffff)&&(weight[allNeighbours[j]]>distance))
                {
                    weight[allNeighbours[j]]=distance;
                    goodNeighbours[goodNeighbourCount++]=allNeighbours[j];
                    if(allNeighbours[j]==targetCellId)
                    {
                        //если эта ячейка целевая, то дальнейшее заполнение матрицы весов нецелесообразно
                        finished=TRUE;
                    }
                }
            }
        }
        //переносим хороших соседей в текущее множество ячеек для следующей итерации
        memcpy(currentSet,goodNeighbours,goodNeighbourCount*sizeof(WORD));
        currentCount=goodNeighbourCount;
    }while((goodNeighbourCount>0)&&!finished); //цикл выполняется, пока не будет достигнута целевая ячейка, либо пока не будет полностью заполнена матрица весов
    //если путь существует, то вес целевой ячейки изменится
    if(weight[targetCellId]!=TOTAL_COUNT)
    {
        //строим путь - для этого проходим по ячейкам в обратном направлении,
        //то есть от целевой в текущую
        pathLen=distance;
        path=new WORD[pathLen];
        cellId=targetCellId;
        path[pathLen-1]=cellId;
        for(WORD i=1;i<pathLen;i++)
        {
            //получаем всех соседей ячейки (начиная с целевой)
            allNeighbourCount=GetNeighbours(cellId,allNeighbours);
            for(WORD j=0;j<allNeighbourCount;j++)
            {
                //с каждым шагом вес должен уменьшаться на единицу - это гарантировано матрицей весов
                if(weight[allNeighbours[j]]==pathLen-i)
                {
                    cellId=allNeighbours[j];
                    path[pathLen-i-1]=cellId;
                    break;
                }
            }
        }
        //освобождаем выделенную ячейку и переносим шар в целевую
        ZeroMemory(&cells[targetCellId],sizeof(TCell));
        cells[targetCellId].colorIndex=cells[selectedId].colorIndex;
        ZeroMemory(&cells[selectedId],sizeof(TCell));
        cells[selectedId].free=TRUE;
    }
    delete[] weight;
    delete[] currentSet;
    delete[] goodNeighbours;
    delete[] allNeighbours;
    return (pathLen>0);
}

BOOL TGame::DetonateTest()
{
    //тест на взрыв
    WORD detonates=0;
    WORD *pipe;
    pipe=new WORD[DETONATE_COUNT];
    //пробуем все возможные горизонтальные варианты
    for(WORD i=0;i<FIELD_SIZE;i++)
    {
        //j соответствует начальному вертикальному ряду
        for(WORD j=0;j<=FIELD_SIZE-DETONATE_COUNT;j++)
        {
            for(WORD k=0;k<DETONATE_COUNT;k++)
            {
                pipe[k]=i*FIELD_SIZE+j+k;
            }
            //отправляем на проверку цвета
            if(CheckPipeDetonate(pipe))
            {
                detonates++;
            }
        }
    }
    //пробуем все возможные вертикальные варианты
    for(WORD i=0;i<FIELD_SIZE;i++)
    {
        //j соответствует начальному горизонтальному ряду
        for(WORD j=0;j<=FIELD_SIZE-DETONATE_COUNT;j++)
        {
            for(WORD k=0;k<DETONATE_COUNT;k++)
            {
                pipe[k]=(j+k)*FIELD_SIZE+i;
            }
            if(CheckPipeDetonate(pipe))
            {
                detonates++;
            }
        }
    }
    //пробуем все прямые диагонали
    for(WORD i=0;i<=FIELD_SIZE-DETONATE_COUNT;i++)
    {
        for(WORD j=0;j<=FIELD_SIZE-DETONATE_COUNT;j++)
        {
            for(WORD k=0;k<DETONATE_COUNT;k++)
            {
                pipe[k]=(i+k)*FIELD_SIZE+j+k;
            }
            if(CheckPipeDetonate(pipe))
            {
                detonates++;
            }
        }
    }
    //пробуем все обратные диагонали
    for(WORD i=DETONATE_COUNT-1;i<FIELD_SIZE;i++)
    {
        for(WORD j=0;j<=FIELD_SIZE-DETONATE_COUNT;j++)
        {
            for(WORD k=0;k<DETONATE_COUNT;k++)
            {
                pipe[k]=(i-k)*FIELD_SIZE+j+k;
            }
            if(CheckPipeDetonate(pipe))
            {
                detonates++;
            }
        }
    }
    delete[] pipe;
    return (detonates>0);
}

WORD TGame::GetNewBallList(TBallInfo **ppNewList)
{
    //функция возвращает список недавно созданных шаров,
    //после чего они уже считаются старыми (флаг isNew сбрасывается)
    WORD newBallCount=0,currentId=0;
    //сначала считаем количество
    for(WORD i=0;i<TOTAL_COUNT;i++)
    {
        if(cells[i].isNew)
        {
            newBallCount++;
        }
    }
    if(newBallCount==0)
    {
        return 0;
    }
    *ppNewList=new TBallInfo[newBallCount];
    for(WORD i=0;i<TOTAL_COUNT;i++)
    {
        if(cells[i].isNew)
        {
            //пишем в список ID и цвет d3dcolorvalue
            cells[i].isNew=FALSE;
            (*ppNewList)[currentId].cellId=i;
            (*ppNewList)[currentId].color=GetColorByIndex(cells[i].colorIndex);
            currentId++;
        }
    }
    return newBallCount;
}

WORD TGame::GetLastMovePath(WORD **ppMovePath)
{
    //функция возвращает путь последнего перемещения шара
    if(path==NULL)
    {
        return 0;
    }
    *ppMovePath=new WORD[pathLen];
    memcpy(*ppMovePath,path,pathLen*sizeof(WORD));
    //после этого удаляем путь
    delete[] path;
    path=NULL;
    WORD tmp=pathLen;
    pathLen=0;
    return tmp;
}

WORD TGame::GetDetonateList(WORD **ppDetonateList)
{
    //функция возвращает список взрывающихся шаров, после чего
    //удаляет их
    WORD detonateCount=0,currentId=0;
    for(WORD i=0;i<TOTAL_COUNT;i++)
    {
        if(cells[i].detonating)
        {
            detonateCount++;
        }
    }
    if(detonateCount==0)
    {
        return 0;
    }
    *ppDetonateList=new WORD[detonateCount];
    for(WORD i=0;i<TOTAL_COUNT;i++)
    {
        if(cells[i].detonating)
        {
            (*ppDetonateList)[currentId]=i;
            ZeroMemory(&cells[i],sizeof(TCell));
            cells[i].free=TRUE;
            currentId++;
        }
    }
    //увеличиваем очки пропорционально числу взорванных шаров
    score+=detonateCount*100;
    return detonateCount;
}

LONG TGame::GetScore()
{
    return score;
}

BOOL TGame::IsGameOver()
{
    return gameOver;
}
