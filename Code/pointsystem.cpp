# include "pointsystem.h"
		
POINTSYSTEM::POINTSYSTEM()
{
	_device = 0;
	_vertex = 0;
	_texture = 0;
}	



bool POINTSYSTEM::init(LPDIRECT3DDEVICE9 device,wchar_t *text_name)
{
	_device = device;
	HRESULT hr = 0;
	hr = _device->CreateVertexBuffer(
		_vertexbuf_size * sizeof(PARTICLE),
		D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,	//此处不能使用托管内存池
		PARTICLE::FVF,
		D3DPOOL_DEFAULT,
		&_vertex,
		0);
	if(FAILED(hr))
	{
		::MessageBox(0, L"CreateVertexBuffer() - FAILED", L"PSystem", 0);
		return false;
	}
	hr = D3DXCreateTextureFromFile(
		device,
		text_name,
		&_texture);
	if(FAILED(hr))
	{
		MessageBox(0,L"D3DXCreateTextureFromFile() - FAILED",L"PSystem", 0);
		return false;
	}
	return true;
}
//已经过对比
//重新设定系统中每个粒子的属性			P236
void POINTSYSTEM::reset()
{
	list<ATTRIBUTE>::iterator i;			
	for(i = _particles.begin();i != _particles.end();i++)
		reset_particle(&(*i));				//为什么要先解引用再取地址
}

//已经过对比
//增加一个粒子							P236
//首先生成一个粒子
//在粒子进入列表前，调用reset_particle对粒子进行初始化
void POINTSYSTEM::add_particle()
{
	ATTRIBUTE attribute;
	reset_particle(&attribute);
	_particles.push_back(attribute);
}
//调用了派生类的虚函数
//void POINTSYSTEM::reset_particle(ATTRIBUTE *attribute)
//{}

//已经过对比
//用于绘制之前，进行必要的初始化绘制状态进行设置
//由于该方法的实现依赖于具体的系统，所以定义为虚函数
//默认定义如下
void POINTSYSTEM::prepare_render()
{
	_device->SetRenderState(D3DRS_LIGHTING,FALSE);				//默认关闭光源
	_device->SetRenderState(D3DRS_POINTSPRITEENABLE,true);		//点精灵的绘制状态，将整个纹理映射到点精灵P231
	_device->SetRenderState(D3DRS_POINTSCALEENABLE,true);		//点精灵的尺寸用观察坐标系的单位来度量	P231
	_device->SetRenderState(D3DRS_POINTSIZE,FtoDw(_size));		//点精灵的尺寸
	_device->SetRenderState(D3DRS_POINTSIZE_MIN,FtoDw(0.0f));	//点精灵可取的最小值

	_device->SetRenderState(D3DRS_POINTSCALE_A,FtoDw(0.0f));	//描述点精灵如何随距离改变尺寸
	_device->SetRenderState(D3DRS_POINTSCALE_B,FtoDw(0.0f));	//
	_device->SetRenderState(D3DRS_POINTSCALE_C,FtoDw(1.0f));	//
	//使用图片的纹理
	_device->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
	_device->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
	//设置alpha融合
	_device->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
	_device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	_device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
}
//已经过对比
//存储一个特定的粒子系统可能已设置好的任何绘制状态值，为虚函数
//默认实现如下
void POINTSYSTEM::post_render()
{
	_device->SetRenderState(D3DRS_LIGHTING,true);
	_device->SetRenderState(D3DRS_POINTSPRITEENABLE,false);		//点精灵的绘制状态，将整个纹理映射到点精灵P231
	_device->SetRenderState(D3DRS_POINTSCALEENABLE,false);		//点精灵的尺寸用观察坐标系的单位来度量	P231
	_device->SetRenderState(D3DRS_ALPHABLENDENABLE,false);
}
/*---------------------------------------------------------
1.当粒子容器不为空的时候进入绘制
2.绘制状态的设置
3.将顶点缓存锁定，并先设定偏移量为0
4.逐个填充顶点缓存，带顶点缓存达到片段的上限时进行绘制
5.从下一个片段开始锁定顶点缓存.若偏移量超过了缓存容量，
	则将偏移量移动到缓存起始位置
重复4,5操作直到将所有顶点填充进顶点缓存
6.判断最后一段缓存是否填充满，假如没有填满说明这段缓存没有绘制
	需要再绘制一次
7.将所有绘制状态复原
----------------------------------------------------------*/
void POINTSYSTEM::render()
{

	if( !_particles.empty() )			//这个empty函数是容器自带的
	{
		prepare_render();				//设置绘制状态
		D3DXMATRIX world;
		D3DXMatrixIdentity(&world);
		_device->SetTransform(D3DTS_WORLD,&world);

		_device->SetTexture(0,_texture);
		_device->SetFVF(PARTICLE::FVF);
		_device->SetStreamSource(0,_vertex,0,sizeof(PARTICLE));

		if(_vertexbuf_offset >= _vertexbuf_size)		//偏移量已经超出了顶点缓存区
			_vertexbuf_offset = 0;

		PARTICLE *pVertex = 0;
		_vertex->Lock(									//锁定顶点缓存
			_vertexbuf_offset * sizeof(PARTICLE),
			_vertexbuf_num * sizeof(PARTICLE),
			(void**)&pVertex,
			_vertexbuf_offset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		DWORD num_in_buf = 0;
		list<ATTRIBUTE>::iterator i;					//创建顶点容器的迭代器
		for(i = _particles.begin();i != _particles.end();i++)
		{
			if(i->_alive == true)
			{
				pVertex->_position = i->_position;
				pVertex->_color = (D3DCOLOR)i->_color;
				pVertex++;
				num_in_buf++;

				if(num_in_buf == _vertexbuf_num)		//片段中顶点已满，将其绘制
				{	
					_vertex->Unlock();
					_device->DrawPrimitive(D3DPT_POINTLIST,
						_vertexbuf_offset,
						_vertexbuf_num);
				
					_vertexbuf_offset += _vertexbuf_num;
					if(_vertexbuf_offset >= _vertexbuf_size)			//记录偏移量是否超过内存限定
						_vertexbuf_offset = 0;
					_vertex->Lock(
						_vertexbuf_offset * sizeof(PARTICLE),
						_vertexbuf_num * sizeof(PARTICLE),
						(void**)&pVertex,
						_vertexbuf_offset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);
					num_in_buf = 0;
				}
			}
		}
		_vertex->Unlock();
		if(num_in_buf != 0)						//将最后的一个片段绘制
			_device->DrawPrimitive(
			D3DPT_POINTLIST,
			_vertexbuf_offset,
			num_in_buf);
		_vertexbuf_offset += _vertexbuf_num;
		post_render();
	}
}

//用于杀死超出范围或者声明周期已经结束的粒子
void POINTSYSTEM::remove_dead_particles()
{
	//首先定义一个迭代器
	list<ATTRIBUTE>::iterator i;
	i = _particles.begin();
	while(i != _particles.end())			//遍历粒子容器
	{
		if(i->_alive == false)				//将粒子舍弃后，无须迭代。
			i = _particles.erase(i);		//erase函数会自动将迭代器后移一位
		else
			i++;
	}
}

//对所有粒子进行更新,基类无须定义此函数
//void POINTSYSTEM::update(float time)


//返回粒子源是否为空的信息
bool POINTSYSTEM::is_empty()
{
	return _particles.empty();
}
//判断粒子源中所有的粒子是否已经全部死亡
//当有一个粒子存活的是否即返回true
//当所有粒子死亡的时候返回false
bool POINTSYSTEM::is_dead()
{
	std::list<ATTRIBUTE>::iterator i;
	for(i = _particles.begin(); i != _particles.end(); i++)
	{
		// is there at least one living particle?  If yes,
		// the system is not dead.
		if( i->_alive )
			return false;
	}
	// no living particles found, the system must be dead.
	return true;
}
POINTSYSTEM::~POINTSYSTEM()
{
	if(_vertex)
		_vertex->Release();
	if(_texture)
		_texture->Release();
}