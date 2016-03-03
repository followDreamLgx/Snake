# include "billboard.h"

//��ʼ������
BILLBOARD::BILLBOARD(LPDIRECT3DDEVICE9 device)
{
	_device = device;
	_texture = NULL;
}

//����ǰ����һ�ε��ô˺�����
//�������¼�������
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
	//��������
	D3DXCreateTextureFromFile(_device,name,&_texture);
}

//���ƹ���
void BILLBOARD::draw(D3DXMATRIX &view,D3DXMATRIX &world)
{
	//����ȡ���任���������㲢���칫������
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
	//��ʽ����
	_device->SetRenderState(D3DRS_ALPHABLENDENABLE,true);				//��Alpha�ں�
	_device->SetTexture(0,_texture);
	_device->SetStreamSource(0,_vertex,0,sizeof(TERRAINVERTEX));
	_device->SetFVF(D3DFVF_TERRAINVERTEX);
	_device->SetRenderState(D3DRS_LIGHTING,false);
	_device->DrawPrimitive(D3DPT_TRIANGLELIST,0,2);
	_device->SetRenderState(D3DRS_LIGHTING,true);
	_device->SetRenderState(D3DRS_ALPHABLENDENABLE,false);				//�ر�Alpha�ں�
}