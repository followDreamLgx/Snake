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
	//前面
	pVertices[A] = TERRAINVERTEX(box._min.x,box._max.y,box._min.z,0,0);
	pVertices[B] = TERRAINVERTEX(box._max.x,box._max.y,box._min.z,1,0);
	pVertices[C] = TERRAINVERTEX(box._max.x,0,box._min.z,1,1);
	pVertices[D] = TERRAINVERTEX(box._min.x,0,box._min.z,0,1);
	//右面
	pVertices[E] = TERRAINVERTEX(box._max.x,box._max.y,box._min.z,0,0);
	pVertices[F] = TERRAINVERTEX(box._max.x,box._max.y,box._max.z,1,0);
	pVertices[G] = TERRAINVERTEX(box._max.x,0,box._max.z,1,1);
	pVertices[H] = TERRAINVERTEX(box._max.x,0,box._min.z,0,1);
	//背面
	pVertices[I] = TERRAINVERTEX(box._max.x,box._max.y,box._max.z,0,0);
	pVertices[J] = TERRAINVERTEX(box._min.x,box._max.y,box._max.z,1,0);
	pVertices[K] = TERRAINVERTEX(box._min.x,0,box._max.z,1,1);
	pVertices[L] = TERRAINVERTEX(box._max.x,0,box._max.z,0,1);
	//左面
	pVertices[M] = TERRAINVERTEX(box._min.x,box._max.y,box._max.z,0,0);
	pVertices[N] = TERRAINVERTEX(box._min.x,box._max.y,box._min.z,1,0);
	pVertices[O] = TERRAINVERTEX(box._min.x,0,box._min.z,1,1);
	pVertices[P] = TERRAINVERTEX(box._min.x,0,box._max.z,0,1);
	//顶面
	pVertices[Q] = TERRAINVERTEX(box._min.x,box._max.y,box._max.z,1,1);
	pVertices[R] = TERRAINVERTEX(box._max.x,box._max.y,box._max.z,0,1);
	pVertices[S] = TERRAINVERTEX(box._max.x,box._max.y,box._min.z,0,0);
	pVertices[T] = TERRAINVERTEX(box._min.x,box._max.y,box._min.z,1,0);
	//地面
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
	//填充索引缓存
	WORD *pIndices = NULL;
	_indexbuf->Lock(0,0,(void**)&pIndices,0);
	//第一重循环控制面的数量
	//第二重循环控制每个面的三角形数量
	//第三重循环控制每个三角形索引对应的顶点缓存
	for(int i = 0,num = 0;i < 23;i += 4)
		for(int j = 2;j > 0;j--)
		{
			pIndices[num++] = i;
			for(int k = 1;k >= 0;k--)
				pIndices[num++] = i + j + k;
		}
	_indexbuf->Unlock();
	//加载纹理
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
			0,			//起始顶点位置
			0,			//允许使用最小索引值
			24, 		//顶点数量	
			i * 6,		//起始索引位置
			2			//图元数量
			);
	}
}

//-----------------------------------------------------
//用于贪吃蛇的天空类，包含了小地图，水果，大天空盒
//-----------------------------------------------------
SKYBOX_DERIVED::SKYBOX_DERIVED(LPDIRECT3DDEVICE9 device,const BOUNDINGBOX &map_box,int fruit_num):
				SKYBOX(device),
				_fruit_style(fruit_num),			//水果总的类型
				_scale_fruit(30),					//水果的放缩比例,
				_map(map_box),						//地图的大小
			    _distance(100),
			    _product_time(500)					//生产水果需要的时间
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
//析构函数
//-------------------------------
SKYBOX_DERIVED::~SKYBOX_DERIVED()		
{}
//------------------------------
//初始化
//------------------------------
void SKYBOX_DERIVED::init(wchar_t **name_sky,wchar_t *name_map,wchar_t **name_fruit,
	const BOUNDINGBOX &sky_box)	//
{
	inti(name_fruit);
	SKYBOX::InitSkyBox(name_sky,sky_box);			//初始化天空盒
	init(name_map);									//初始化小地图
	_product = _product_time;
}
//--------------------------------------
//初始化水果并增添水果
//--------------------------------------
void SKYBOX_DERIVED::inti(wchar_t **name_fruit)
{
	//记录着整个水果链表的起始位置
	_fruit_head = new FRUIT_NODE();
	_fruit_head->_end = NULL;
	_fruit_head->_head = NULL;
	_fruit_head->_next = NULL;
	for(int i = 0;i < _fruit_style;i++)				//初始化水果的x文件
	{
		_fruit_mesh[i].init(name_fruit[i],_device);
		add();
		_fruit_num++;
	}
}
//--------------------------------------
//初始化地面贴图
//--------------------------------------
void SKYBOX_DERIVED::init(wchar_t *name_map)
{
	//注意，锁定的缓冲区指针是地形的指针
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
//添加水果
//---------------------------------
void SKYBOX_DERIVED::add()								//添加水果
{
	FRUIT_STRUCT *p1,*p = new FRUIT_STRUCT();
	p->_alive = true;
	p->_style = (int)GetRandomFloat(0,_fruit_style);
	reset(p);
	_fruit_num++;
}
//---------------------------------
//重新设置新的水果属性
//---------------------------------
void SKYBOX_DERIVED::reset(FRUIT_STRUCT *p)			//重新设置水果的属性，添加水果时，用于随机设置水果
{
	FRUIT_STRUCT *p1;
	p->_style = GetRandomFloat(0,_fruit_style);						//类型
	p->_position.x = GetRandomFloat(_map._min.x,_map._max.x) / 10;	//位置,地图很大，但是水果不能
	p->_position.y = _map._max.y;
	p->_position.z = GetRandomFloat(_map._min.z,_map._max.z) / 10;
	p->_next = NULL;											//封口
	if(_fruit_head->_head == NULL)								//若还没有水果
	{	
		_fruit_head->_head = _fruit_head->_end = p;				//head与end记录p
	
		return ;
	}
	//遍历整个链表，判断水果的位置是否重复
	for(p1 = _fruit_head->_head;p1 != NULL;p1 = p1->_next)
	{
		if(D3DXVec3Length(&(p->_position - p1->_position)) < _distance)
		{	
			p->_position.x = GetRandomFloat(_map._min.x,_map._max.x);
			p->_position.z = GetRandomFloat(_map._min.z,_map._max.z);
			p1 = _fruit_head->_head;					//存在相同位置的水果，重新遍历
		}
	}
	_fruit_head->_end = _fruit_head->_end->_next = p;	//将水果存入最后节点
}
//-----------------------------------
//更新所有水鬼，盒子与地面无须更新
//-----------------------------------
void SKYBOX_DERIVED::update()							//更新水果，将没用的水果去除
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
			if(_fruit_head->_head == _fruit_head->_end)	//链表中只有一个节点
			{
				delete _fruit_head->_head;
				_fruit_head->_head = _fruit_head->_end = NULL;
				break;
			}
			else if(_fruit_head->_head == p)			//删除第一个节点		
			{	
				_fruit_head->_head = p->_next;
				delete p;
				p = _fruit_head->_head;
			}
			else if(_fruit_head->_end == p)				//删除最后一个节点
			{
				_fruit_head->_end = before;
				before->_next = NULL;
				delete p;
				return;				//最后一个节点已经检查，直接退出
			}
			else										//删除普通节点
			{
				before->_next = p->_next;
				delete p;
				p = before->_next;
				continue;
			}
		}
		before = p;										//记录上一个节点
	}
}
void SKYBOX_DERIVED::render()
{
	update();
	D3DXMATRIX world;
	D3DXMatrixIdentity(&world);
	_device->SetTransform(D3DTS_WORLD,&world);
	SKYBOX::render();											//绘制天空盒
	//_device->SetRenderState(D3DRS_ALPHABLENDENABLE,true);		//开启ALPHA融合
	//_device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
	//_device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ZERO);
	render_map();												//绘制小地图
	
	render_fruit();												//绘制水果
}
//------------------------------------
//绘制小地图
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
//绘制水果
//------------------------------------
void SKYBOX_DERIVED::render_fruit()
{
	//D3DXMATRIX ro;		//旋转人物
	//D3DXMatrixRotationAxis(&ro,&D3DXVECTOR3(0,1,0),D3DX_PI / (-2));
	//_scale_fruit *= ro;
	FRUIT_STRUCT *p = _fruit_head->_head;
	D3DXMATRIX world,tran,scale;
	D3DXMATRIX scale_big;
	D3DXMatrixScaling(&scale_big,_scale_fruit / 6,_scale_fruit / 6,_scale_fruit / 6);
	D3DXMatrixScaling(&scale,_scale_fruit,_scale_fruit,_scale_fruit);
	for(;p != NULL;p = p->_next)						//遍历整个链表，绘制所有水果
	{
		D3DXMatrixIdentity(&world);
		D3DXMatrixTranslation(&tran,p->_position.x,p->_position.y,p->_position.z);
		if(p->_style == _fruit_style)					// 最后的苹果太大，需要缩小
			world = world * scale_big * tran;
		else
			world = world * scale * tran;
		_device->SetTransform(D3DTS_WORLD,&world);
		_fruit_mesh[p->_style].DrawMesh();				//绘制对应的水果
	}
}

//-----------------------------------
//判断是否游戏结束
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