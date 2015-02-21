
#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <d3dx9.h>
#include <time.h>

#define CELL_SIZE 25.0f
#define CELL_BIAS 2.0f
#define BALL_R 10.0f
#define BALL_STRIPS 16
#define BALL_SECTORS 16
#define ROT_SPEED 0.01f
#define CAMERA_MIN_Y CELL_SIZE
#define CAMERA_MIN_ORIGIN CELL_SIZE
#define CAMERA_MAXDISTANCE 2*CELL_SIZE*FIELD_SIZE
#define CAMERA_MINDISTANCE CELL_SIZE*2
#define CAMERA_ZOOM_SPEED 5.0f
#define ANIMATE_JUMP_LEN 1.0f
#define ANIMATE_MOVE_LEN 0.1f
#define ANIMATE_APPEAR_LEN 0.5f
#define PSYSTEM_PERBALL 100
#define PSYSTEM_VELOCITY 15.0f
#define PSYSTEM_LIFETIME 1.5f
#define GEOMETRY_FVF D3DFVF_XYZ|D3DFVF_NORMAL
#define PARTICLE_FVF D3DFVF_XYZ|D3DFVF_DIFFUSE

void Terminate(LPCSTR msg);
DWORD Float2Dword(FLOAT val);

struct TBallInfo
{
    WORD cellId;
    D3DCOLORVALUE color;
};

D3DCOLORVALUE GetColorByIndex(WORD idx);

struct TCell
{
    BOOL free;
    BOOL isNew;
    BOOL selected;
    BOOL detonating;
    WORD colorIndex;
};

struct TVertex {
    FLOAT x,y,z;
    FLOAT nx,ny,nz;
};

struct TParticle {
    FLOAT x,y,z;
    D3DCOLOR color;
};

struct TBall {
    D3DCOLORVALUE color;
    BOOL free;
    BOOL jumping;
    BOOL appearing;
    BOOL moving;
};

struct TRay {
    D3DXVECTOR3 orig;
    D3DXVECTOR3 dir;
};

typedef enum _MAT_DESC
{
    MAT_DESC_AMBIENT = 0,
    MAT_DESC_DIFFUSE = 1,
    MAT_DESC_SPECULAR = 2,
    MAT_DESC_EMISSIVE = 3,
} MAT_DESC;

class TAtom {
private:
    LPDIRECT3DDEVICE9 pDevice;
    LPDIRECT3DVERTEXBUFFER9 pVb;
    LPDIRECT3DINDEXBUFFER9 pIb;
    WORD vertexCount;
    WORD indexCount;
    WORD primitiveCount;
    D3DPRIMITIVETYPE primitiveType;
public:
    TAtom(LPDIRECT3DDEVICE9 pDevice);
    ~TAtom();
    void Init(WORD vertexCount, WORD indexCount, WORD primitiveCount, D3DPRIMITIVETYPE primitiveType);
    void SetVertices(TVertex *pVertices);
    void SetIndices(WORD *pIndices);
    void Render();
};

class TGeometry {
private:
    LPDIRECT3DDEVICE9 pDevice;
    D3DMATERIAL9 mat;
    TAtom **atoms;
    WORD atomCount;
public:
    TGeometry(LPDIRECT3DDEVICE9 pDevice);
    ~TGeometry();
    void Init(WORD atomCount);
    void SetMaterialColor3f(MAT_DESC desc, FLOAT r, FLOAT g, FLOAT b);
    void SetMaterialColorValue(MAT_DESC desc, D3DCOLORVALUE color);
    void SetMaterialPower(FLOAT power);
    TAtom* GetAtom(WORD atomId);
    void Render();
};

class TAnimate {
private:
    LPDIRECT3DDEVICE9 pDevice;
    D3DXMATRIX *frames;
    WORD frameCount;
    FLOAT timeLength;
    FLOAT position;
    BOOL active;
    BOOL once;
public:
    TAnimate(LPDIRECT3DDEVICE9 pDevice);
    ~TAnimate();
    BOOL IsActive();
    void Init(WORD frameCount, FLOAT timeLength, BOOL once);
    void Reset(); //sets to 0 frame
    void Start();
    void SetFrame(WORD frameId, D3DXMATRIX frame);
    void UpdatePosition(clock_t deltaTime);
    void ApplyTransform();
};

class TParticleSystem {
private:
    LPDIRECT3DDEVICE9 pDevice;
    LPDIRECT3DVERTEXBUFFER9 pVb;
    WORD particleCount;
    FLOAT currentLife;
    TParticle *parts;
    D3DXVECTOR3 *dirs;
    BOOL active;
public:
    TParticleSystem(LPDIRECT3DDEVICE9 pDevice);
    ~TParticleSystem();
    void Init(TBallInfo *ballList, WORD count);
    BOOL IsActive();
    void Update(clock_t deltaTime);
    void Render();
};

class TEngine {
private:
    HWND hWindow;
    RECT WinRect;
    D3DXVECTOR3 CameraPos;
    LPDIRECT3D9 pD3d;
    LPDIRECT3DDEVICE9 pDevice;
    LPDIRECT3DTEXTURE9 pTex;
    LPD3DXFONT pFont;
    D3DPRESENT_PARAMETERS settings;
    clock_t currentTime;
    TGeometry *cellGeometry;
    TGeometry *ballGeometry;
    TParticleSystem *psystem;
    TBall *balls;
    TAnimate *jumpAnimation;
    TAnimate *moveAnimation;
    TAnimate *appearAnimation;
    LONG score;
    void InitD3d();
    void InitGeometry();
    void InitAnimation();
    void DrawPlatform();
    void DrawBalls();
    void UpdateView();
public:
    TEngine(HWND hWindow);
    ~TEngine();
    void AppearBalls(TBallInfo *ballInfo, WORD count);
    void MoveBall(WORD *path, WORD pathLen);
    void DetonateBalls(WORD *detonateList, WORD count);
    BOOL IsSelected();
    BOOL IsMoving();
    BOOL IsAppearing();
    BOOL IsDetonating();
    void OnResetGame();
    WORD OnClick(WORD x, WORD y, BOOL *IsCell); //if something selected returns cell id; ball id to select otherwise
    void OnRotateY(INT offset);
    void OnRotateX(INT offset);
    void OnZoom(INT zoom);
    void OnResize();
    void OnUpdateScore(LONG score);
    void Render();
};

#endif
