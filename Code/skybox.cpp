# include "skybox.h"

SKYBOX::SKYBOX(LPDIRECT3DDEVICE9 device)
{
	_device = device;
	_indexbuf	= NULL;
	_vertexbuf = NULL;
}

VOID SKYBOX::InitSkyBox(wchar_t **name,const BOUNDINGBOX &box)
{
	_device->CreateVertexBuffer(24 * sizeof(TERRAINVERTEX),
		0,
		D3DFVF_TERRAINVERTEX,
		D3DPOOL_MANAGED,
		&_vertexbuf,
		0);
	TERRAINVERTEX *pVertices = NULL;
	_vertexbuf->Lock(0,0,(void**)&pVertices,0);
	//ǰ��
	pVertices[A] = TERRAINVERTEX(box._min.x,box._max.y,box._min.z,0,0);
	pVertices[B] = TERRAINVERTEX(box._max.x,box._max.y,box._min.z,1,0);
	pVertices[C] = TERRAINVERTEX(box._max.x,0,box._min.z,1,1);
	pVertices[D] = TERRAINVERTEX(box._min.x,0,box._min.z,0,1);
	//����
	pVertices[E] = TERRAINVERTEX(box._max.x,box._max.y,box._min.z,0,0);
	pVertices[F] = TERRAINVERTEX(box._max.x,box._max.y,box._max.z,1,0);
	pVertices[G] = TERRAINVERTEX(box._max.x,0,box._max.z,1,1);
	pVertices[H] = TERRAINVERTEX(box._max.x,0,box._min.z,0,1);
	//����
	pVertices[I] = TERRAINVERTEX(box._max.x,box._max.y,box._max.z,0,0);
	pVertices[J] = TERRAINVERTEX(box._min.x,box._max.y,box._max.z,1,0);
	pVertices[K] = TERRAINVERTEX(box._min.x,0,box._max.z,1,1);
	pVertices[L] = TERRAINVERTEX(box._max.x,0,box._max.z,0,1);
	//����
	pVertices[M] = TERRAINVERTEX(box._min.x,box._max.y,box._max.z,0,0);
	pVertices[N] = TERRAINVERTEX(box._min.x,box._max.y,box._min.z,1,0);
	pVertices[O] = TERRAINVERTEX(box._min.x,0,box._min.z,1,1);
	pVertices[P] = TERRAINVERTEX(box._min.x,0,box._max.z,0,1);
	//����
	pVertices[Q] = TERRAINVERTEX(box._min.x,box._max.y,box._max.z,1,1);
	pVertices[R] = TERRAINVERTEX(box._max.x,box._max.y,box._max.z,0,1);
	pVertices[S] = TERRAINVERTEX(box._max.x,box._max.y,box._min.z,0,0);
	pVertices[T] = TERRAINVERTEX(box._min.x,box._max.y,box._min.z,1,0);
	//����
	pVertices[X] = TERRAINVERTEX(box._min.x,0,box._max.z,0,0);
	pVertices[W] = TERRAINVERTEX(box._max.x,0,box._max.z,0,1);
	pVertices[V] = TERRAINVERTEX(box._max.x,0,box._min.z,1,1);
	pVertices[U] = TERRAINVERTEX(box._min.x,0,box._min.z,1,0);

	_vertexbuf->Unlock();
	_device->CreateIndexBuffer(36 * sizeof(WORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&_indexbuf,
		0);
	//�����������
	WORD *pIndices = NULL;
	_indexbuf->Lock(0,0,(void**)&pIndices,0);
	//��һ��ѭ�������������
	//�ڶ���ѭ������ÿ���������������
	//������ѭ������ÿ��������������Ӧ�Ķ��㻺��
	for(int i = 0,num = 0;i < 23;i += 4)
		for(int j = 2;j > 0;j--)
		{
			pIndices[num++] = i;
			for(int k = 1;k >= 0;k--)
				pIndices[num++] = i + j + k;
		}
	_indexbuf->Unlock();
	//��������
	for(int i = 0;i < 6;i++)
		D3DXCreateTextureFromFile(_device,name[i],&_texture[i]);  
}

VOID SKYBOX::render()
{
	_device->SetStreamSource(0,_vertexbuf,0,sizeof(TERRAINVERTEX));
	_device->SetFVF(D3DFVF_TERRAINVERTEX);				
	_device->SetIndices(_indexbuf);		
	for(int i = 0;i < 6;i++)
	{
		_device->SetTexture(0,_texture[i]);
		_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
			0,			//��ʼ����λ��
			0,			//����ʹ����С����ֵ
			24, 		//��������	
			i * 6,		//��ʼ����λ��
			2			//ͼԪ����
			);
	}
}

//-----------------------------------------------------
//����̰���ߵ�����࣬������С��ͼ��ˮ��������պ�
//-----------------------------------------------------
SKYBOX_DERIVED::SKYBOX_DERIVED(LPDIRECT3DDEVICE9 device,const BOUNDINGBOX &map_box,int fruit_num):
				SKYBOX(device),
				_fruit_style(fruit_num),			//ˮ���ܵ�����
				_scale_fruit(30),					//ˮ���ķ�������,
				_map(map_box),						//��ͼ�Ĵ�С
			    _distance(100),
			    _product_time(500)					//����ˮ����Ҫ��ʱ��
				,_product_cut(1)
{
	_vertexbuf_map = NULL;
	_texture_map	 = NULL;
	_fruit_mesh = new MESHXFILE[_fruit_style];
	_fruit_head = NULL;
	_fruit_num = 0;
	_begin_game = false ;
}
//-------------------------------
//��������
//-------------------------------
SKYBOX_DERIVED::~SKYBOX_DERIVED()		
{}
//------------------------------
//��ʼ��
//------------------------------
void SKYBOX_DERIVED::init(wchar_t **name_sky,wchar_t *name_map,wchar_t **name_fruit,
	const BOUNDINGBOX &sky_box)	//
{
	inti(name_fruit);
	SKYBOX::InitSkyBox(name_sky,sky_box);			//��ʼ����պ�
	init(name_map);									//��ʼ��С��ͼ
	_product = _product_time;
}
//--------------------------------------
//��ʼ��ˮ��������ˮ��
//--------------------------------------
void SKYBOX_DERIVED::inti(wchar_t **name_fruit)
{
	//��¼������ˮ���������ʼλ��
	_fruit_head = new FRUIT_NODE();
	_fruit_head->_end = NULL;
	_fruit_head->_head = NULL;
	_fruit_head->_next = NULL;
	for(int i = 0;i < _fruit_style;i++)				//��ʼ��ˮ����x�ļ�
	{
		_fruit_mesh[i].init(name_fruit[i],_device);
		add();
		_fruit_num++;
	}
}
//--------------------------------------
//��ʼ��������ͼ
//--------------------------------------
void SKYBOX_DERIVED::init(wchar_t *name_map)
{
	//ע�⣬�����Ļ�����ָ���ǵ��ε�ָ��
	_device->CreateVertexBuffer(6 * sizeof(TERRAINVERTEX),
		0,
		D3DFVF_TERRAINVERTEX,
		D3DPOOL_MANAGED,
		&_vertexbuf_map,
		0);
	TERRAINVERTEX *pVertices = NULL;
	_vertexbuf_map->Lock(0,0,(void**)&pVertices,0);
	pVertices[A] = TERRAINVERTEX(_map._min.x,_map._max.y,_map._max.z,0,0);
	pVertices[B] = TERRAINVERTEX(_map._max.x,_map._max.y,_map._max.z,100,0);
	pVertices[C] = TERRAINVERTEX(_map._max.x,_map._max.y,_map._min.z,100,100);
	pVertices[D] = TERRAINVERTEX(_map._min.x,_map._max.y,_map._max.z,0,0);
	pVertices[E] = TERRAINVERTEX(_map._max.x,_map._max.y,_map._min.z,100,100);
	pVertices[F] = TERRAINVERTEX(_map._min.x,_map._max.y,_map._min.z,0,100);
	_vertexbuf_map->Unlock();
	D3DXCreateTextureFromFile(_device,name_map,&_texture_map); 
}
//---------------------------------
//���ˮ��
//---------------------------------
void SKYBOX_DERIVED::add()								//���ˮ��
{
	FRUIT_STRUCT *p1,*p = new FRUIT_STRUCT();
	p->_alive = true;
	p->_style = (int)GetRandomFloat(0,_fruit_style);
	reset(p);
	_fruit_num++;
}
//---------------------------------
//���������µ�ˮ������
//---------------------------------
void SKYBOX_DERIVED::reset(FRUIT_STRUCT *p)			//��������ˮ�������ԣ����ˮ��ʱ�������������ˮ��
{
	FRUIT_STRUCT *p1;
	p->_style = GetRandomFloat(0,_fruit_style);						//����
	p->_position.x = GetRandomFloat(_map._min.x,_map._max.x) / 10;	//λ��,��ͼ�ܴ󣬵���ˮ������
	p->_position.y = _map._max.y;
	p->_position.z = GetRandomFloat(_map._min.z,_map._max.z) / 10;
	p->_next = NULL;											//���
	if(_fruit_head->_head == NULL)								//����û��ˮ��
	{	
		_fruit_head->_head = _fruit_head->_end = p;				//head��end��¼p
	
		return ;
	}
	//�������������ж�ˮ����λ���Ƿ��ظ�
	for(p1 = _fruit_head->_head;p1 != NULL;p1 = p1->_next)
	{
		if(D3DXVec3Length(&(p->_position - p1->_position)) < _distance)
		{	
			p->_position.x = GetRandomFloat(_map._min.x,_map._max.x);
			p->_position.z = GetRandomFloat(_map._min.z,_map._max.z);
			p1 = _fruit_head->_head;					//������ͬλ�õ�ˮ�������±���
		}
	}
	_fruit_head->_end = _fruit_head->_end->_next = p;	//��ˮ���������ڵ�
}
//-----------------------------------
//��������ˮ������������������
//-----------------------------------
void SKYBOX_DERIVED::update()							//����ˮ������û�õ�ˮ��ȥ��
{
	FRUIT_STRUCT *p,*before;
	p = before = _fruit_head->_head;
	_product -= _product_cut;
	if(_product <= 0  && _begin_game == true)
	{	add();
		_product = _product_time;
	}
	for(;p != NULL;p = p->_next)
	{
		if(p->_alive == false)							
		{
			_fruit_num--;
			if(_fruit_head->_head == _fruit_head->_end)	//������ֻ��һ���ڵ�
			{
				delete _fruit_head->_head;
				_fruit_head->_head = _fruit_head->_end = NULL;
				break;
			}
			else if(_fruit_head->_head == p)			//ɾ����һ���ڵ�		
			{	
				_fruit_head->_head = p->_next;
				delete p;
				p = _fruit_head->_head;
			}
			else if(_fruit_head->_end == p)				//ɾ�����һ���ڵ�
			{
				_fruit_head->_end = before;
				before->_next = NULL;
				delete p;
				return;				//���һ���ڵ��Ѿ���飬ֱ���˳�
			}
			else										//ɾ����ͨ�ڵ�
			{
				before->_next = p->_next;
				delete p;
				p = before->_next;
				continue;
			}
		}
		before = p;										//��¼��һ���ڵ�
	}
}
void SKYBOX_DERIVED::render()
{
	update();
	D3DXMATRIX world;
	D3DXMatrixIdentity(&world);
	_device->SetTransform(D3DTS_WORLD,&world);
	SKYBOX::render();											//������պ�
	//_device->SetRenderState(D3DRS_ALPHABLENDENABLE,true);		//����ALPHA�ں�
	//_device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
	//_device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ZERO);
	render_map();												//����С��ͼ
	
	render_fruit();												//����ˮ��
}
//------------------------------------
//����С��ͼ
//------------------------------------
void SKYBOX_DERIVED::render_map()
{
	_device->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_MIRROR);
	_device->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_MIRROR);
	_device->SetStreamSource(0,_vertexbuf_map,0,sizeof(TERRAINVERTEX));
	_device->SetFVF(D3DFVF_TERRAINVERTEX);	
	_device->SetTexture(0,_texture_map);
	_device->DrawPrimitive(
			D3DPT_TRIANGLELIST,
			0,
			2);
	_device->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);
	_device->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);
}
//------------------------------------
//����ˮ��
//------------------------------------
void SKYBOX_DERIVED::render_fruit()
{
	//D3DXMATRIX ro;		//��ת����
	//D3DXMatrixRotationAxis(&ro,&D3DXVECTOR3(0,1,0),D3DX_PI / (-2));
	//_scale_fruit *= ro;
	FRUIT_STRUCT *p = _fruit_head->_head;
	D3DXMATRIX world,tran,scale;
	D3DXMATRIX scale_big;
	D3DXMatrixScaling(&scale_big,_scale_fruit / 6,_scale_fruit / 6,_scale_fruit / 6);
	D3DXMatrixScaling(&scale,_scale_fruit,_scale_fruit,_scale_fruit);
	for(;p != NULL;p = p->_next)						//��������������������ˮ��
	{
		D3DXMatrixIdentity(&world);
		D3DXMatrixTranslation(&tran,p->_position.x,p->_position.y,p->_position.z);
		if(p->_style == _fruit_style)					// ����ƻ��̫����Ҫ��С
			world = world * scale_big * tran;
		else
			world = world * scale * tran;
		_device->SetTransform(D3DTS_WORLD,&world);
		_fruit_mesh[p->_style].DrawMesh();				//���ƶ�Ӧ��ˮ��
	}
}

//-----------------------------------
//�ж��Ƿ���Ϸ����
//-----------------------------------
bool SKYBOX_DERIVED::game_end()
{
	if(_fruit_num <= 0)
		return true;
	else
		return false;
}

bool SKYBOX_DERIVED::game_begin()
{
	_begin_game = true;
return true;
}