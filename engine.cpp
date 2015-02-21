
#include "engine.h"
#include "game.h"
#include <tchar.h>

#define __DIE__(msg) MessageBox(NULL,msg,_T("Error"),0); \
                       PostQuitMessage(0);

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

DWORD Float2Dword(FLOAT val)
{
    return *((DWORD*)&val);
}

TAtom::TAtom(LPDIRECT3DDEVICE9 pDevice)
{
    this->pDevice=pDevice;
    vertexCount=0;
    indexCount=0;
    primitiveCount=0;
    primitiveType=D3DPT_TRIANGLELIST;
    pVb=NULL;
    pIb=NULL;
}

TAtom::~TAtom()
{
    if(pVb!=NULL)
    {
        pVb->Release();
    }
    if(pIb!=NULL)
    {
        pIb->Release();
    }
}

void TAtom::Init(WORD vertexCount, WORD indexCount, WORD primitiveCount, D3DPRIMITIVETYPE primitiveType)
{
    this->vertexCount=vertexCount;
    this->indexCount=indexCount;
    this->primitiveCount=primitiveCount;
    this->primitiveType=primitiveType;
    if(pVb!=NULL)
    {
        pVb->Release();
    }
    if(pIb!=NULL)
    {
        pIb->Release();
    }
    if(FAILED(pDevice->CreateVertexBuffer(vertexCount*sizeof(TVertex),D3DUSAGE_WRITEONLY,GEOMETRY_FVF,D3DPOOL_DEFAULT,&pVb,NULL)))
    {
        __DIE__(_T("IDirect3DDevice9::CreateVertexBuffer failed."));
    }
    if(FAILED(pDevice->CreateIndexBuffer(indexCount*sizeof(WORD),D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_DEFAULT,&pIb,NULL)))
    {
        __DIE__(_T("IDirect3DDevice9::CreateIndexBuffer failed."));
    }
}

void TAtom::SetVertices(TVertex *pVertices)
{
    TVertex *tmp;
    if(FAILED(pVb->Lock(0,vertexCount*sizeof(TVertex),(VOID**)&tmp,D3DLOCK_DISCARD)))
    {
        __DIE__(_T("IDirect3DVertexBuffer9::Lock failed."));
    }
    memcpy(tmp,pVertices,vertexCount*sizeof(TVertex));
    pVb->Unlock();
}

void TAtom::SetIndices(WORD *pIndices)
{
    WORD *tmp;
    if(FAILED(pIb->Lock(0,indexCount*sizeof(WORD),(VOID**)&tmp,D3DLOCK_DISCARD)))
    {
        __DIE__(_T("IDirect3DIndexBuffer9::Lock failed."));
    }
    memcpy(tmp,pIndices,indexCount*sizeof(WORD));
    pIb->Unlock();
}

void TAtom::Render()
{
    pDevice->SetStreamSource(0,pVb,0,sizeof(TVertex));
    pDevice->SetFVF(GEOMETRY_FVF);
    pDevice->SetIndices(pIb);
    pDevice->DrawIndexedPrimitive(primitiveType,0,0,vertexCount,0,primitiveCount);
}

TGeometry::TGeometry(LPDIRECT3DDEVICE9 pDevice)
{
    this->pDevice=pDevice;
    atomCount=0;
    atoms=NULL;
    ZeroMemory(&mat,sizeof(D3DMATERIAL9));
    mat.Ambient.a=1.0f;
    mat.Diffuse.a=1.0f;
    mat.Emissive.a=1.0f;
    mat.Specular.a=1.0f;
    mat.Power=1.0f;
}

TGeometry::~TGeometry()
{
    if(atomCount>0)
    {
        for(WORD i=0;i<atomCount;i++)
        {
            delete atoms[i];
        }
        delete[] atoms;
    }
}

void TGeometry::Init(WORD atomCount)
{
    if(this->atomCount>0)
    {
        for(WORD i=0;i<this->atomCount;i++)
        {
            delete atoms[i];
        }
        delete[] atoms;
    }
    this->atomCount=atomCount;
    atoms=new TAtom*[atomCount];
    for(WORD i=0;i<atomCount;i++)
    {
        atoms[i]=new TAtom(pDevice);
    }
}

void TGeometry::SetMaterialColor3f(MAT_DESC desc, FLOAT r, FLOAT g, FLOAT b)
{
    switch(desc)
    {
    case MAT_DESC_AMBIENT:
        {
            mat.Ambient.r=r;
            mat.Ambient.g=g;
            mat.Ambient.b=b;
            break;
        }
    case MAT_DESC_DIFFUSE:
        {
            mat.Diffuse.r=r;
            mat.Diffuse.g=g;
            mat.Diffuse.b=b;
            break;
        }
    case MAT_DESC_EMISSIVE:
        {
            mat.Emissive.r=r;
            mat.Emissive.g=g;
            mat.Emissive.b=b;
            break;
        }
    case MAT_DESC_SPECULAR:
        {
            mat.Specular.r=r;
            mat.Specular.g=g;
            mat.Specular.b=b;
            break;
        }
    }
}

void TGeometry::SetMaterialColorValue(MAT_DESC desc, D3DCOLORVALUE color)
{
    switch(desc)
    {
    case MAT_DESC_AMBIENT:
        {
            mat.Ambient=color;
            break;
        }
    case MAT_DESC_DIFFUSE:
        {
            mat.Diffuse=color;
            break;
        }
    case MAT_DESC_EMISSIVE:
        {
            mat.Emissive=color;
            break;
        }
    case MAT_DESC_SPECULAR:
        {
            mat.Specular=color;
            break;
        }
    }
}

void TGeometry::SetMaterialPower(FLOAT power)
{
    mat.Power=power;
}

TAtom* TGeometry::GetAtom(WORD atomId)
{
    if((atomCount==0)||(atomId>atomCount-1))
    {
        return NULL;
    }
    return atoms[atomId];
}

void TGeometry::Render()
{
    if(atomCount>0)
    {
        pDevice->SetMaterial(&mat);
        for(WORD i=0;i<atomCount;i++)
        {
            atoms[i]->Render();
        }
    }
}

TAnimate::TAnimate(LPDIRECT3DDEVICE9 pDevice)
{
    this->pDevice=pDevice;
    frames=NULL;
    frameCount=0;
    position=0.0f;
    timeLength=0.0f;
    active=FALSE;
    once=TRUE;
}

TAnimate::~TAnimate()
{
    if(frameCount>0)
    {
        delete[] frames;
    }
}

BOOL TAnimate::IsActive()
{
    return active;
}

void TAnimate::Init(WORD frameCount, FLOAT timeLength, BOOL once)
{
    if(this->frameCount>0)
    {
        delete[] frames;
    }
    this->frameCount=frameCount;
    this->once=once;
    frames=new D3DXMATRIX[frameCount];
    this->timeLength=timeLength;
    position=0.0f;
}

void TAnimate::Reset()
{
    position=0.0f;
    active=FALSE;
}

void TAnimate::Start()
{
    position=0.0f;
    active=TRUE;
}

void TAnimate::SetFrame(WORD frameId, D3DXMATRIX frame)
{
    if((frameCount==0)||(frameId>frameCount-1))
    {
        return;
    }
    frames[frameId]=frame;
}

void TAnimate::UpdatePosition(clock_t deltaTime)
{
    //position меняется в диапазоне 0..1
    position+=deltaTime/(timeLength*1000.0f);
    if(position>1.0f)
    {
        position=0.0f;
        if(once)
        {
            active=FALSE;
        }
    }
}

void TAnimate::ApplyTransform()
{
    D3DXMATRIX mCurrent;
    pDevice->GetTransform(D3DTS_WORLD,&mCurrent);
    FLOAT progress=position*(frameCount-1);
    //находим ближайший слева "кадр" анимации
    WORD nearest=floor(progress);
    //t - параметр для линейной интерполяции двух матриц
    FLOAT t=progress-nearest;
    if(nearest>frameCount-1)
    {
        nearest=0;
    }
    else if(nearest==frameCount-1)
    {
        //если кадр последний, то ничего не интерполируем
        D3DXMatrixMultiply(&mCurrent,&(frames[0]),&mCurrent);
    }
    else
    {
        //умножаем мировую матрицу на интерполированную матрицу анимации
        D3DXMATRIX tmp=frames[nearest]+t*(frames[nearest+1]-frames[nearest]);
        D3DXMatrixMultiply(&mCurrent,&tmp,&mCurrent);
    }
    pDevice->SetTransform(D3DTS_WORLD,&mCurrent);
}

TParticleSystem::TParticleSystem(LPDIRECT3DDEVICE9 pDevice)
{
    this->pDevice=pDevice;
    pVb=NULL;
    parts=NULL;
    particleCount=0;
    currentLife=0.0f;
    dirs=NULL;
    active=FALSE;
}

TParticleSystem::~TParticleSystem()
{
    if(pVb!=NULL)
    {
        pVb->Release();
    }
    if(dirs!=NULL)
    {
        delete[] dirs;
    }
    if(parts!=NULL)
    {
        delete[] parts;
    }
}

void TParticleSystem::Init(TBallInfo *ballList, WORD count)
{
    particleCount=count*PSYSTEM_PERBALL;
    currentLife=0.0f;
    if(FAILED(pDevice->CreateVertexBuffer(particleCount*sizeof(TParticle),D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC|D3DUSAGE_POINTS,D3DFVF_XYZ|D3DFVF_DIFFUSE,D3DPOOL_DEFAULT,&pVb,NULL)))
    {
        __DIE__(_T("IDirect3DDevice::CreateVertexBuffer failed."));
    }
    parts=new TParticle[particleCount];
    dirs=new D3DXVECTOR3[particleCount];
    D3DXVECTOR3 offset;
    WORD idx;
    FLOAT x,z;
    for(WORD i=0;i<count;i++)
    {
        x=((ballList[i].cellId / FIELD_SIZE) - HALF_FIELD_SIZE + 0.5f) * CELL_SIZE;
        z=((ballList[i].cellId % FIELD_SIZE) - HALF_FIELD_SIZE + 0.5f) * CELL_SIZE;
        for(WORD j=0;j<PSYSTEM_PERBALL;j++)
        {
            //заполняем внутренности каждого шара случайно расположенными частицами
            idx=i*PSYSTEM_PERBALL+j;
            offset.x=BALL_R*((float)rand()/RAND_MAX-0.5f);
            offset.y=BALL_R*((float)rand()/RAND_MAX-0.5f);
            offset.z=BALL_R*((float)rand()/RAND_MAX-0.5f);
            //массив parts будет содержать абсолютные координаты частиц
            parts[idx].x=x+offset.x;
            parts[idx].y=BALL_R+offset.y;
            parts[idx].z=z+offset.z;
            //а массив dirs будет содержать векторы направления движения:
            //вектор скорости каждой частицы будет направлен из центра шара,
            //чтобы все разлеталось в разные стороны
            D3DXVec3Normalize(&dirs[idx],&offset);
            parts[idx].color=D3DCOLOR_COLORVALUE(ballList[i].color.r,ballList[i].color.g,ballList[i].color.b,ballList[i].color.a);
        }
    }
    TParticle *tmp;
    if(FAILED(pVb->Lock(0,particleCount*sizeof(TParticle),(VOID**)&tmp,D3DLOCK_DISCARD)))
    {
        __DIE__(_T("IDirect3DVertexBuffer9::Lock failed."));
    }
    memcpy(tmp,parts,particleCount*sizeof(TParticle));
    pVb->Unlock();
    active=TRUE;
}

BOOL TParticleSystem::IsActive()
{
    return active;
}

void TParticleSystem::Update(clock_t deltaTime)
{
    //сначала обновляем текущее время жизни системы частиц
    FLOAT t=(float)deltaTime/1000.0f;
    currentLife+=t;
    //затем вычисляем изменение вектора положения
    t*=PSYSTEM_VELOCITY;
    if(currentLife>PSYSTEM_LIFETIME)
    {
        //завершаем работу системы частиц
        active=FALSE;
        pVb->Release();
        pVb=NULL;
        particleCount=0;
        delete[] dirs;
        dirs=NULL;
        delete[] parts;
        parts=NULL;
        return;
    }
    //обновляем позиции частиц согласно направлениям их движения
    for(WORD i=0;i<particleCount;i++)
    {
        parts[i].x+=dirs[i].x*t;
        parts[i].y+=dirs[i].y*t;
        parts[i].z+=dirs[i].z*t;
    }
    TParticle *tmp;
    if(FAILED(pVb->Lock(0,particleCount*sizeof(TParticle),(VOID**)&tmp,D3DLOCK_DISCARD)))
    {
        __DIE__(_T("IDirect3DVertexBuffer9::Lock failed."));
    }
    //поскольку буфер у нас динамический, просто перезаписываем его
    memcpy(tmp,parts,particleCount*sizeof(TParticle));
    pVb->Unlock();
}

void TParticleSystem::Render()
{
    //частицы рендерим с выключенным освещением
    pDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
    pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE,TRUE);
    pDevice->SetRenderState(D3DRS_POINTSCALEENABLE,TRUE);
    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
    pDevice->SetStreamSource(0,pVb,0,sizeof(TParticle));
    pDevice->SetFVF(PARTICLE_FVF);
    pDevice->DrawPrimitive(D3DPT_POINTLIST,0,particleCount);
    pDevice->SetRenderState(D3DRS_LIGHTING,TRUE);
    pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE,FALSE);
    pDevice->SetRenderState(D3DRS_POINTSCALEENABLE,FALSE);
    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
}

void TEngine::InitD3d()
{
    if((pD3d=Direct3DCreate9(D3D_SDK_VERSION))==NULL)
    {
        __DIE__(_T("Direct3DCreate9 failed."));
    }
    D3DCAPS9 caps;
    D3DDISPLAYMODE mode;
    //из devcaps проверяем только HWTRANSFORMANDLIGHT
    pD3d->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&caps);
    pD3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&mode);
    ZeroMemory(&settings,sizeof(D3DPRESENT_PARAMETERS));
    settings.AutoDepthStencilFormat=D3DFMT_D24S8;
    settings.BackBufferCount=1;
    //несмотря на полноэкранный режим, берем текущие настройки дисплея
    settings.BackBufferFormat=mode.Format;
    settings.BackBufferHeight=mode.Height;
    settings.BackBufferWidth=mode.Width;
    settings.EnableAutoDepthStencil=TRUE;
    settings.hDeviceWindow=hWindow;
    settings.SwapEffect=D3DSWAPEFFECT_DISCARD;
    //settings.FullScreen_RefreshRateInHz=mode.RefreshRate;
    settings.Windowed=TRUE;
    if(FAILED(pD3d->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWindow,(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) ? D3DCREATE_HARDWARE_VERTEXPROCESSING : D3DCREATE_SOFTWARE_VERTEXPROCESSING,&settings,&pDevice)))
    {
        __DIE__(_T("IDirect3D9::CreateDevice failed."));
    }
    D3DLIGHT9 light;
    ZeroMemory(&light,sizeof(D3DLIGHT9));
    //источник света будет расположен над центром платформы на высоте 100 единиц
    light.Type=D3DLIGHT_POINT;
    light.Position=D3DXVECTOR3(0.0f,100.0f,0.0f);
    light.Attenuation0=1.0f;
    light.Specular.a=1.0f; light.Specular.r=1.0f; light.Specular.g=1.0f; light.Specular.b=1.0f;
    light.Range=500.0f;
    pDevice->SetRenderState(D3DRS_LIGHTING,TRUE);
    pDevice->SetLight(0,&light);
    pDevice->LightEnable(0,TRUE);
    pDevice->SetRenderState(D3DRS_AMBIENT,D3DCOLOR_XRGB(50,50,50));
    pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_CCW);
    pDevice->SetRenderState(D3DRS_SPECULARENABLE,TRUE);
    pDevice->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);
    //остальные настройки рендера нужны только для системы частиц
    pDevice->SetRenderState(D3DRS_POINTSIZE,Float2Dword(5.0f));
    pDevice->SetRenderState(D3DRS_POINTSIZE_MIN,Float2Dword(1.0f));
    pDevice->SetRenderState(D3DRS_POINTSIZE_MAX,Float2Dword(10.0f));
    pDevice->SetRenderState(D3DRS_POINTSCALE_A,Float2Dword(0.0f));
    pDevice->SetRenderState(D3DRS_POINTSCALE_B,Float2Dword(1.0f));
    pDevice->SetRenderState(D3DRS_POINTSCALE_C,Float2Dword(0.0f));
    pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
    pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
    pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
    pDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
    D3DXCreateTextureFromFile(pDevice,_T("tex.tga"),&pTex);
}

void TEngine::InitGeometry()
{
    //инициализации геометрии одной ячейки
    cellGeometry=new TGeometry(pDevice);
    cellGeometry->Init(1);
    cellGeometry->SetMaterialColor3f(MAT_DESC_AMBIENT,1.0f,0.0f,0.0f);
    cellGeometry->SetMaterialColor3f(MAT_DESC_SPECULAR,1.0f,0.0f,0.0f);
    cellGeometry->SetMaterialPower(10.0f);
    TAtom *atom=cellGeometry->GetAtom(0);
    atom->Init(20,30,10,D3DPT_TRIANGLELIST);
    TVertex *vertices;
    FLOAT tmp=1/sqrt(2.0);
    vertices=new TVertex[20];
    ZeroMemory(vertices,20*sizeof(TVertex));
    vertices[0].x=0.0f; vertices[0].y=-CELL_BIAS; vertices[0].z=0.0f;
    vertices[0].nx=-tmp; vertices[0].ny=tmp; vertices[0].nz=0.0f;
    vertices[1].x=0.0f; vertices[1].y=-CELL_BIAS; vertices[1].z=CELL_SIZE;
    vertices[1].nx=-tmp; vertices[1].ny=tmp; vertices[1].nz=0.0f;
    vertices[2].x=CELL_BIAS; vertices[2].y=0.0f; vertices[2].z=CELL_SIZE-CELL_BIAS;
    vertices[2].nx=-tmp; vertices[2].ny=tmp; vertices[2].nz=0.0f;
    vertices[3].x=CELL_BIAS; vertices[3].y=0.0f; vertices[3].z=CELL_BIAS;
    vertices[3].nx=-tmp; vertices[3].ny=tmp; vertices[3].nz=0.0f;

    vertices[4].x=0.0f; vertices[4].y=-CELL_BIAS; vertices[4].z=CELL_SIZE;
    vertices[4].nx=0.0f; vertices[4].ny=tmp; vertices[4].nz=tmp;
    vertices[5].x=CELL_SIZE; vertices[5].y=-CELL_BIAS; vertices[5].z=CELL_SIZE;
    vertices[5].nx=0.0f; vertices[5].ny=tmp; vertices[5].nz=tmp;
    vertices[6].x=CELL_SIZE-CELL_BIAS; vertices[6].y=0.0f; vertices[6].z=CELL_SIZE-CELL_BIAS;
    vertices[6].nx=0.0f; vertices[6].ny=tmp; vertices[6].nz=tmp;
    vertices[7].x=CELL_BIAS; vertices[7].y=0.0f; vertices[7].z=CELL_SIZE-CELL_BIAS;
    vertices[7].nx=0.0f; vertices[7].ny=tmp; vertices[7].nz=tmp;

    vertices[8].x=CELL_SIZE; vertices[8].y=-CELL_BIAS; vertices[8].z=CELL_SIZE;
    vertices[8].nx=tmp; vertices[8].ny=tmp; vertices[8].nz=0.0f;
    vertices[9].x=CELL_SIZE; vertices[9].y=-CELL_BIAS; vertices[9].z=0.0f;
    vertices[9].nx=tmp; vertices[9].ny=tmp; vertices[9].nz=0.0f;
    vertices[10].x=CELL_SIZE-CELL_BIAS; vertices[10].y=0.0f; vertices[10].z=CELL_BIAS;
    vertices[10].nx=tmp; vertices[10].ny=tmp; vertices[10].nz=0.0f;
    vertices[11].x=CELL_SIZE-CELL_BIAS; vertices[11].y=0.0f; vertices[11].z=CELL_SIZE-CELL_BIAS;
    vertices[11].nx=tmp; vertices[11].ny=tmp; vertices[11].nz=0.0f;

    vertices[12].x=CELL_SIZE; vertices[12].y=-CELL_BIAS; vertices[12].z=0.0f;
    vertices[12].nx=0.0f; vertices[12].ny=tmp; vertices[12].nz=-tmp;
    vertices[13].x=0.0f; vertices[13].y=-CELL_BIAS; vertices[13].z=0.0f;
    vertices[13].nx=tmp; vertices[13].ny=tmp; vertices[13].nz=0.0f;
    vertices[14].x=CELL_BIAS; vertices[14].y=0.0f; vertices[14].z=CELL_BIAS;
    vertices[14].nx=tmp; vertices[14].ny=tmp; vertices[14].nz=0.0f;
    vertices[15].x=CELL_SIZE-CELL_BIAS; vertices[15].y=0.0f; vertices[15].z=CELL_BIAS;
    vertices[15].nx=tmp; vertices[15].ny=tmp; vertices[15].nz=0.0f;

    vertices[16].x=CELL_BIAS; vertices[16].z=CELL_BIAS; vertices[16].ny=1.0f;
    vertices[17].x=CELL_BIAS; vertices[17].z=CELL_SIZE-CELL_BIAS; vertices[17].ny=1.0f;
    vertices[18].x=CELL_SIZE-CELL_BIAS; vertices[18].z=CELL_SIZE-CELL_BIAS; vertices[18].ny=1.0f;
    vertices[19].x=CELL_SIZE-CELL_BIAS; vertices[19].z=CELL_BIAS; vertices[19].ny=1.0f;
    atom->SetVertices(vertices);
    delete[] vertices;

    WORD *indices,idx;
    indices=new WORD[30];
    for(WORD i=0;i<5;i++)
    {
        idx=i*6;
        indices[idx]=i*4;
        indices[idx+1]=i*4+1;
        indices[idx+2]=i*4+2;
        indices[idx+3]=i*4;
        indices[idx+4]=i*4+2;
        indices[idx+5]=i*4+3;
    }
    atom->SetIndices(indices);
    delete[] indices;

    //геометрия шара
    ballGeometry=new TGeometry(pDevice);
    ballGeometry->Init(2+BALL_STRIPS);
    ballGeometry->SetMaterialPower(15.0f);
    //верхний "зонтик"
    atom=ballGeometry->GetAtom(0);
    atom->Init(BALL_SECTORS+1,BALL_SECTORS+2,BALL_SECTORS,D3DPT_TRIANGLEFAN);
    vertices=new TVertex[BALL_SECTORS+1];
    vertices[0].x=0.0f; vertices[0].y=BALL_R; vertices[0].z=0.0f;
    vertices[0].nx=0.0f; vertices[0].ny=1.0f; vertices[0].nz=0.0f;
    FLOAT stripHeight=2*BALL_R/(BALL_STRIPS+1),
          y=BALL_R-stripHeight/2,
          r=sqrt(BALL_R*BALL_R-y*y),
          arg;
    for(WORD i=1;i<BALL_SECTORS+1;i++)
    {
        arg=2*D3DX_PI*(i-1)/BALL_SECTORS;
        vertices[i].x=-r*cos(arg); vertices[i].y=y; vertices[i].z=r*sin(arg);
        vertices[i].nx=vertices[i].x/BALL_R; vertices[i].ny=vertices[i].y/BALL_R; vertices[i].nz=vertices[i].z/BALL_R;
    }
    atom->SetVertices(vertices);
    indices=new WORD[BALL_SECTORS+2];
    for(WORD i=0;i<BALL_SECTORS+1;i++)
    {
        indices[i]=i;
    }
    indices[BALL_SECTORS+1]=1;
    atom->SetIndices(indices);
    //нижний "зонтик"
    atom=ballGeometry->GetAtom(1);
    atom->Init(BALL_SECTORS+1,BALL_SECTORS+2,BALL_SECTORS,D3DPT_TRIANGLEFAN);
    vertices[0].x=0.0f; vertices[0].y=-BALL_R; vertices[0].z=0.0f;
    vertices[0].nx=0.0f; vertices[0].ny=-1.0f; vertices[0].nz=0.0f;
    y=-y;
    for(WORD i=1;i<BALL_SECTORS+1;i++)
    {
        arg=2*D3DX_PI*(i-1)/BALL_SECTORS;
        vertices[i].x=-r*cos(arg); vertices[i].y=y; vertices[i].z=-r*sin(arg);
        vertices[i].nx=vertices[i].x/BALL_R; vertices[i].ny=vertices[i].y/BALL_R; vertices[i].nz=vertices[i].z/BALL_R;
    }
    atom->SetVertices(vertices);
    atom->SetIndices(indices);
    delete[] vertices;
    delete[] indices;
    //полоски (strips)
    vertices=new TVertex[BALL_SECTORS*2];
    indices=new WORD[(BALL_SECTORS+1)*2];
    FLOAT next_r,next_y;
    for(WORD i=0;i<BALL_STRIPS;i++)
    {
        atom=ballGeometry->GetAtom(i+2);
        atom->Init(BALL_SECTORS*2,(BALL_SECTORS+1)*2,BALL_SECTORS*2,D3DPT_TRIANGLESTRIP);
        y=-BALL_R+stripHeight*(0.5f+i);
        next_y=-BALL_R+stripHeight*(1.5f+i);
        r=sqrt(BALL_R*BALL_R-y*y);
        next_r=sqrt(BALL_R*BALL_R-next_y*next_y);
        for(WORD j=0;j<BALL_SECTORS;j++)
        {
            arg=2*D3DX_PI*j/BALL_SECTORS;
            vertices[j*2].x=-r*cos(arg); vertices[j*2].y=y; vertices[j*2].z=-r*sin(arg);
            vertices[j*2].nx=vertices[j*2].x/BALL_R; vertices[j*2].ny=vertices[j*2].y/BALL_R; vertices[j*2].nz=vertices[j*2].z/BALL_R;
            vertices[j*2+1].x=-next_r*cos(arg); vertices[j*2+1].y=next_y; vertices[j*2+1].z=-next_r*sin(arg);
            vertices[j*2+1].nx=vertices[j*2+1].x/BALL_R; vertices[j*2+1].ny=vertices[j*2+1].y/BALL_R; vertices[j*2+1].nz=vertices[j*2+1].z/BALL_R;
        }
        atom->SetVertices(vertices);
        for(WORD j=0;j<BALL_SECTORS*2;j++)
        {
            indices[j]=j;
        }
        indices[BALL_SECTORS*2]=0;
        indices[BALL_SECTORS*2+1]=1;
        atom->SetIndices(indices);
    }
    delete[] vertices;
    delete[] indices;
}

void TEngine::InitAnimation()
{
    //анимация прыжка
    jumpAnimation=new TAnimate(pDevice);
    jumpAnimation->Init(5,ANIMATE_JUMP_LEN,FALSE);
    D3DXMATRIX tmp;
    D3DXMatrixIdentity(&tmp);
    jumpAnimation->SetFrame(0,tmp);
    jumpAnimation->SetFrame(2,tmp);
    jumpAnimation->SetFrame(4,tmp);
    D3DXMatrixTranslation(&tmp,0.0f,BALL_R,0.0f);
    jumpAnimation->SetFrame(1,tmp);
    D3DXMATRIX mScale;
    D3DXMatrixScaling(&mScale,1.0f,0.75f,1.0f);
    D3DXMatrixTranslation(&tmp,0.0f,-BALL_R*0.45,0.0f);
    D3DXMatrixMultiply(&tmp,&tmp,&mScale);
    jumpAnimation->SetFrame(3,tmp);
    //анимация появления
    appearAnimation=new TAnimate(pDevice);
    appearAnimation->Init(2,ANIMATE_APPEAR_LEN,TRUE);
    D3DXMatrixScaling(&mScale,0.2f,0.2f,0.2f);
    D3DXMatrixIdentity(&tmp);
    appearAnimation->SetFrame(0,mScale);
    appearAnimation->SetFrame(1,tmp);
    //анимация движения - просто заготовка, кадры записываются по факту
    moveAnimation=new TAnimate(pDevice);
}

void TEngine::DrawPlatform()
{
    D3DXMATRIX mWorld,mCurrent;
    pDevice->GetTransform(D3DTS_WORLD,&mCurrent);
    //проходя по всем ячейкам, рисуем платформу
    for(WORD i=0;i<FIELD_SIZE;i++)
    {
        for(WORD j=0;j<FIELD_SIZE;j++)
        {
            D3DXMatrixTranslation(&mWorld,((float)i-HALF_FIELD_SIZE)*CELL_SIZE,0.0f,((float)j-HALF_FIELD_SIZE)*CELL_SIZE);
            pDevice->SetTransform(D3DTS_WORLD,&mWorld);
            cellGeometry->Render();
        }
    }
    pDevice->SetTransform(D3DTS_WORLD,&mCurrent);
}

void TEngine::DrawBalls()
{
    D3DXMATRIX mWorld,mCurrent;
    pDevice->GetTransform(D3DTS_WORLD,&mCurrent);
    WORD row,col;
    for(WORD i=0;i<TOTAL_COUNT;i++)
    {
        if(balls[i].free)
        {
            continue;
        }
        row=i / FIELD_SIZE;
        col=i % FIELD_SIZE;
        //сначала переносим начало координат в нужную ячейку
        D3DXMatrixTranslation(&mWorld,(row-HALF_FIELD_SIZE+0.5)*CELL_SIZE,BALL_R,(col-HALF_FIELD_SIZE+0.5)*CELL_SIZE);
        pDevice->SetTransform(D3DTS_WORLD,&mWorld);
        ballGeometry->SetMaterialColorValue(MAT_DESC_AMBIENT,balls[i].color);
        ballGeometry->SetMaterialColorValue(MAT_DESC_SPECULAR,balls[i].color);
        if(balls[i].jumping)
        {
            //если шар прыгающий, то пытаемся применить трансформацию прыжка
            if(!jumpAnimation->IsActive())
            {
                balls[i].jumping=FALSE;
            }
            else
            {
                jumpAnimation->ApplyTransform();
            }
        }
        else if(balls[i].appearing)
        {
            //если шар появляющийся, то пытаемся применить трансформацию появления
            if(!appearAnimation->IsActive())
            {
                balls[i].appearing=FALSE;
            }
            else
            {
                appearAnimation->ApplyTransform();
            }
        }
        else if(balls[i].moving)
        {
            //если шар в движении, то пытаемся применить трансформацию перемещения
            if(!moveAnimation->IsActive())
            {
                balls[i].moving=FALSE;
            }
            else
            {
                //предварительно возращаем начало координат в центр платформы!
                D3DXMatrixIdentity(&mWorld);
                pDevice->SetTransform(D3DTS_WORLD,&mWorld);
                moveAnimation->ApplyTransform();
            }
        }
        ballGeometry->Render();
    }
    pDevice->SetTransform(D3DTS_WORLD,&mCurrent);
}

void TEngine::UpdateView()
{
    D3DXMATRIX mView;
    D3DXMatrixLookAtLH(&mView,&CameraPos,new D3DXVECTOR3(0.0f,0.0f,0.0f),new D3DXVECTOR3(0.0f,1.0f,0.0f));
    pDevice->SetTransform(D3DTS_VIEW,&mView);
}

TEngine::TEngine(HWND hWindow)
{
    this->hWindow=hWindow;
    score=0;
    currentTime=clock();
    balls=new TBall[TOTAL_COUNT];
    ZeroMemory(balls,TOTAL_COUNT*sizeof(TBall));
    for(WORD i=0;i<TOTAL_COUNT;i++)
    {
        balls[i].free=TRUE;
    }
    InitD3d();
    InitGeometry();
    InitAnimation();
    psystem=new TParticleSystem(pDevice);
    CameraPos=D3DXVECTOR3(-CELL_SIZE*HALF_FIELD_SIZE,CELL_SIZE*2,-CELL_SIZE*HALF_FIELD_SIZE);
    D3DXCreateFont(pDevice,40,25,1,0,FALSE,DEFAULT_CHARSET,OUT_TT_ONLY_PRECIS,0,DEFAULT_PITCH,_T("Arial"),&pFont);
    OnResize();
    UpdateView();
}

TEngine::~TEngine()
{
    if(pD3d!=NULL)
    {
        pD3d->Release();
    }
    if(pDevice!=NULL)
    {
        pDevice->Release();
    }
    if(pTex!=NULL)
    {
        pTex->Release();
    }
}

void TEngine::OnResetGame()
{
    score=0;
    ZeroMemory(balls,TOTAL_COUNT);
    for(WORD i=0;i<TOTAL_COUNT;i++)
    {
        balls[i].free=TRUE;
    }
    jumpAnimation->Reset();
    appearAnimation->Reset();
}

void TEngine::AppearBalls(TBallInfo *info, WORD count)
{
    for(WORD i=0;i<count;i++)
    {
        balls[info[i].cellId].free=FALSE;
        balls[info[i].cellId].appearing=TRUE;
        balls[info[i].cellId].color=info[i].color;
        balls[info[i].cellId].jumping=FALSE;
        balls[info[i].cellId].moving=FALSE;
    }
    appearAnimation->Start();
}

void TEngine::MoveBall(WORD *path, WORD pathLen)
{
    //массив path содержит номера ячеек, кроме начальной
    moveAnimation->Init(pathLen+1,pathLen*ANIMATE_MOVE_LEN,TRUE);
    D3DXMATRIX tmp;
    WORD selectedId;
    //сначала находим выбранный шар
    for(WORD i=0;i<TOTAL_COUNT;i++)
    {
        if(!balls[i].free && balls[i].jumping)
        {
            //логически переносим шар в целевую ячейку и ставим флаг moving=TRUE
            ZeroMemory(&balls[path[pathLen-1]],sizeof(TBall));
            balls[path[pathLen-1]].color=balls[i].color;
            balls[path[pathLen-1]].moving=TRUE;
            ZeroMemory(&balls[i],sizeof(TBall));
            balls[i].free=TRUE;
            jumpAnimation->Reset();
            selectedId=i;
            break;
        }
    }
    //запись кадров анимации перемещения
    WORD row=selectedId / FIELD_SIZE,
         col=selectedId % FIELD_SIZE;
    //начальное положение
    D3DXMatrixTranslation(&tmp,(row-HALF_FIELD_SIZE+0.5)*CELL_SIZE,BALL_R,(col-HALF_FIELD_SIZE+0.5)*CELL_SIZE);
    moveAnimation->SetFrame(0,tmp);
    for(WORD i=0;i<pathLen;i++)
    {
        //остальные положения вычисляем, исходя из содержимого массива path
        row=path[i] / FIELD_SIZE;
        col=path[i] % FIELD_SIZE;
        D3DXMatrixTranslation(&tmp,(row-HALF_FIELD_SIZE+0.5)*CELL_SIZE,BALL_R,(col-HALF_FIELD_SIZE+0.5)*CELL_SIZE);
        moveAnimation->SetFrame(i+1,tmp);
    }
    moveAnimation->Start();
}

void TEngine::DetonateBalls(WORD *detonateList, WORD count)
{
    TBallInfo *ballList=new TBallInfo[count];
    for(WORD i=0;i<count;i++)
    {
        //освобождаем ячейки от шаров и заполняем массив ballList для инициализации системы частиц
        ballList[i].cellId=detonateList[i];
        ballList[i].color=balls[detonateList[i]].color;
        ZeroMemory(&balls[detonateList[i]],sizeof(TBall));
        balls[detonateList[i]].free=TRUE;
    }
    psystem->Init(ballList,count);
    delete[] ballList;
}

BOOL TEngine::IsSelected()
{
    return jumpAnimation->IsActive(); //если активна анимация прыжка, значит какой-то шар выбран
}

BOOL TEngine::IsMoving()
{
    return moveAnimation->IsActive(); //если активна анимация движения, значит какой-то шар перемещается
}

BOOL TEngine::IsAppearing()
{
    return appearAnimation->IsActive(); //если активна анимация появления, значит какой-то шар появляется
}

BOOL TEngine::IsDetonating()
{
    return psystem->IsActive(); //если активна система частиц, значит анимируется взрыв
}

WORD TEngine::OnClick(WORD x, WORD y, BOOL *IsCell)
{
    //преобразуем координаты клика в луч в мировом пространстве
    D3DXMATRIX mat;
    pDevice->GetTransform(D3DTS_PROJECTION,&mat);
    FLOAT tx=(((2.0f*x)/((float)WinRect.right-WinRect.left))-1.0f)/mat(0,0),
          ty=(((-2.0f*y)/((float)WinRect.bottom-WinRect.top))+1.0f)/mat(1,1);
    TRay ray;
    //в пространстве вида начало и направление луча задаются так:
    ray.orig=D3DXVECTOR3(0.0f,0.0f,0.0f);
    ray.dir=D3DXVECTOR3(tx,ty,1.0f);
    pDevice->GetTransform(D3DTS_VIEW,&mat);
    D3DXMatrixInverse(&mat,0,&mat);
    //преобразуем в мировое пространство, умножая на инвертированную видовую матрицу.
    //Начало луча преобразуется, как точка, а направление - как нормаль!
    D3DXVec3TransformCoord(&ray.orig,&ray.orig,&mat);
    D3DXVec3TransformNormal(&ray.dir,&ray.dir,&mat);
    //сначала пытаемся найти пересечение с любым шаром
    //для этого нужно решить квадратное уравнение (задача пересечения луча и сферы)
    D3DXVECTOR3 ballCenter,tmp;
    WORD row,col,minBallId=TOTAL_COUNT;
    FLOAT a,b,c,discrime,t,minDistance=-1;
    a=D3DXVec3Dot(&ray.dir,&ray.dir);
    for(WORD i=0;i<TOTAL_COUNT;i++)
    {
        if(balls[i].free)
        {
            continue;
        }
        row=i / FIELD_SIZE;
        col=i % FIELD_SIZE;
        ballCenter=D3DXVECTOR3((row-HALF_FIELD_SIZE+0.5)*CELL_SIZE,BALL_R,(col-HALF_FIELD_SIZE+0.5)*CELL_SIZE);
        tmp=ballCenter-ray.orig;
        b=2*D3DXVec3Dot(&ray.dir,&tmp);
        c=D3DXVec3Dot(&tmp,&tmp)-BALL_R*BALL_R;
        discrime=b*b-4*a*c;
        if(discrime>0)
        {
            //есть пересечение!
            //но их может быть несколько, поэтому оцениваем расстояние от наблюдателя до шара
            b=sqrt(D3DXVec3Dot(&tmp,&tmp));
            if((minDistance==-1)||(b<minDistance))
            {
                minDistance=b;
                minBallId=i;
            }
        }
    }
    if(minBallId<TOTAL_COUNT)
    {
        //выбран шар - возможно, что до этого тоже был выбран шар, поэтому
        //снимаем выбор со всего множества
        for(WORD i=0;i<TOTAL_COUNT;i++)
        {
            balls[i].jumping=FALSE;
        }
        //выбираем новый шар
        balls[minBallId].jumping=TRUE;
        jumpAnimation->Start();
        *IsCell=FALSE;
        return minBallId;
    }
    //иначе, если уже что-то выбрано, пытаемся найти пересечение луча с ячейкой
    if(IsSelected())
    {
        //intersection будет содержать координаты x и z пересечения луча с плоскостью y=0
        t=-ray.orig.y/ray.dir.y;
        D3DXVECTOR3 intersection=ray.orig+t*ray.dir;
        //если x и z соответствуют какой-либо ячейке, то возвращаем ее ID
        if( (intersection.x>-CELL_SIZE*HALF_FIELD_SIZE)&&(intersection.x<CELL_SIZE*HALF_FIELD_SIZE) &&
            (intersection.z>-CELL_SIZE*HALF_FIELD_SIZE)&&(intersection.z<CELL_SIZE*HALF_FIELD_SIZE))
        {
            //row and column
            WORD row=floor(intersection.x/CELL_SIZE+HALF_FIELD_SIZE),
                 col=floor(intersection.z/CELL_SIZE+HALF_FIELD_SIZE);
            *IsCell=TRUE;
            return (row*FIELD_SIZE+col);
        }
    }
    return TOTAL_COUNT;
}

void TEngine::OnRotateX(INT offset)
{
    D3DXMATRIX mRot;
    //вращение камеры вокруг мировой оси Y
    D3DXMatrixRotationY(&mRot,ROT_SPEED*offset);
    D3DXVec3TransformCoord(&CameraPos,&CameraPos,&mRot);
    UpdateView();
}

void TEngine::OnRotateY(INT offset)
{
    D3DXMATRIX mRot;
    D3DXVECTOR3 vAxis,vTmp;
    //вращение камеры вокруг оси, лежащей в плоскости Y=0
    //и перпендекулярной направлению взгляда, - таким образом,
    //камера всегда направлена в начало координат
    D3DXVec3Cross(&vAxis,&CameraPos,new D3DXVECTOR3(0.0f,1.0f,0.0f));
    D3DXMatrixRotationAxis(&mRot,&vAxis,ROT_SPEED*offset);
    D3DXVec3TransformCoord(&vTmp,&CameraPos,&mRot);
    if((vTmp.y>CAMERA_MIN_Y)&&(sqrt(vTmp.x*vTmp.x+vTmp.z*vTmp.z)>CAMERA_MIN_ORIGIN))
    {
        CameraPos=vTmp;
        UpdateView();
    }
}

void TEngine::OnZoom(INT zoom)
{
    D3DXMATRIX mScale;
    D3DXVECTOR3 vTmp;
    FLOAT fraction=1.0f-CAMERA_ZOOM_SPEED*zoom/D3DXVec3Length(&CameraPos);
    D3DXMatrixScaling(&mScale,fraction,fraction,fraction);
    D3DXVec3TransformCoord(&vTmp,&CameraPos,&mScale);
    FLOAT len=D3DXVec3Length(&vTmp);
    if((len>CAMERA_MINDISTANCE)&&(len<CAMERA_MAXDISTANCE))
    {
        CameraPos=vTmp;
        if(CameraPos.y<CAMERA_MIN_Y)
        {
            CameraPos.y=CAMERA_MIN_Y;
        }
        UpdateView();
    }
}

void TEngine::OnResize()
{
    GetWindowRect(hWindow,&WinRect);
    D3DXMATRIX mProj;
    //обновляем матрицу проекции, чтобы не появлялось искажений пропорций
    D3DXMatrixPerspectiveFovLH(&mProj,D3DX_PI/4,(WinRect.right-WinRect.left)/((float)WinRect.bottom-WinRect.top),1.0f,1000.0f);
    pDevice->SetTransform(D3DTS_PROJECTION,&mProj);
}

void TEngine::OnUpdateScore(LONG score)
{
    this->score=score;
}

void TEngine::Render()
{
    //вычисляем, сколько миллисекунд прошло с момента отрисовки предыдущего кадра
    clock_t elapsed=clock(), deltaTime=elapsed-currentTime;
    currentTime=elapsed;
    //обновляем позиции анимаций, если они активны
    if(jumpAnimation->IsActive())
    {
        jumpAnimation->UpdatePosition(deltaTime);
    }
    if(appearAnimation->IsActive())
    {
        appearAnimation->UpdatePosition(deltaTime);
    }
    if(moveAnimation->IsActive())
    {
        moveAnimation->UpdatePosition(deltaTime);
    }
    pDevice->Clear(0,NULL,D3DCLEAR_STENCIL|D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB(0,0,0),1.0f,0);
    pDevice->BeginScene();
    //рисуем платформу
    DrawPlatform();
    //рисуем шары
    DrawBalls();
    //если активна система частиц, то обновляем положения частиц и рендерим их с текстурой
    if(psystem->IsActive())
    {
        pDevice->SetTexture(0,pTex);
        psystem->Update(deltaTime);
        psystem->Render();
        pDevice->SetTexture(0,0);
    }
    //вывод заработанных очков
    char buf[255]="Score: ",tmp[255];
    itoa(score,tmp,10);
    strcat(buf,tmp);
    RECT fontRect;
    fontRect.left=0;
    fontRect.right=GetSystemMetrics(SM_CXSCREEN);
    fontRect.top=0;
    fontRect.bottom=40;
    pFont->DrawText(NULL,_T(buf),-1,&fontRect,DT_CENTER,D3DCOLOR_XRGB(0,255,255));
    pDevice->EndScene();
    pDevice->Present(NULL,NULL,NULL,NULL);
}
