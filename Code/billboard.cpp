# include "billboard.h"

//初始化函数
BILLBOARD::BILLBOARD(LPDIRECT3DDEVICE9 device)
{
	_device = device;
	_texture = NULL;
}

//绘制前至少一次调用此函数。
//可以重新加载纹理
void BILLBOARD::Init(wchar_t *name)
{
	_device->CreateVertexBuffer(6 * sizeof(TERRAINVERTEX),
		0,
		D3DFVF_TERRAINVERTEX,
		D3DPOOL_MANAGED,
		&_vertex,
		0);
	TERRAINVERTEX *pVertices = NULL;
	_vertex->Lock(0,0,(void**)&pVertices,0);

	pVertices[C] = TERRAINVERTEX(-30,60,0,0,0);
	pVertices[B] = TERRAINVERTEX(-30,0,0,0,1);
	pVertices[A] = TERRAINVERTEX(30,0,0,1,1);

	pVertices[F] = TERRAINVERTEX(-30,60,0,0,0);
	pVertices[E] = TERRAINVERTEX(30,0,0,1,1);
	pVertices[D] = TERRAINVERTEX(30,60,0,1,0);
	_vertex->Unlock();
	//加载纹理
	D3DXCreateTextureFromFile(_device,name,&_texture);
}

//绘制广告板
void BILLBOARD::draw(D3DXMATRIX &view,D3DXMATRIX &world)
{
	//根据取景变换矩阵来计算并构造公告板矩阵
    D3DXMATRIX billboard;
    D3DXMatrixIdentity(&billboard);
    billboard._11 = view._11;
    billboard._13 = view._13;
    billboard._31 = view._31;
    billboard._33 = view._33;
    D3DXMatrixInverse(&billboard, NULL, &billboard);

	D3DXMATRIX matNPC;
    D3DXMatrixIdentity(&matNPC);
    matNPC =  matNPC * billboard * world;
    _device->SetTransform(D3DTS_WORLD,&matNPC);
	//正式绘制
	_device->SetRenderState(D3DRS_ALPHABLENDENABLE,true);				//打开Alpha融合
	_device->SetTexture(0,_texture);
	_device->SetStreamSource(0,_vertex,0,sizeof(TERRAINVERTEX));
	_device->SetFVF(D3DFVF_TERRAINVERTEX);
	_device->SetRenderState(D3DRS_LIGHTING,false);
	_device->DrawPrimitive(D3DPT_TRIANGLELIST,0,2);
	_device->SetRenderState(D3DRS_LIGHTING,true);
	_device->SetRenderState(D3DRS_ALPHABLENDENABLE,false);				//关闭Alpha融合
}