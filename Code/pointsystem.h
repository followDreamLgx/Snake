# ifndef _POINTSYSTEM_H
# define _POINTSYSTEM_H
# include "head.h"
# include <list>


//------------------------------------------------------------
//粒子系统的父类
//------------------------------------------------------------
class POINTSYSTEM
{
protected:	
	LPDIRECT3DDEVICE9		_device;			//
	D3DXVECTOR3				_origin;			//粒子源 
	

	float					_add_rate;			//粒子产生的速度
	float					_size;				//粒子的大小
	LPDIRECT3DTEXTURE9		_texture;			//粒子的纹理
	LPDIRECT3DVERTEXBUFFER9	_vertex;			//
	list<ATTRIBUTE>			_particles;			//注意需要头文件list，粒子的容器
	int						_max_particles;		//同时存在的粒子最多的数目
	//顶点缓存的数据
	DWORD					_vertexbuf_size;	//顶点缓存的大小
	DWORD					_vertexbuf_offset;	//顶点缓存的起始偏移量
	DWORD					_vertexbuf_num;		//每次填充到顶点缓存的顶点数量

public:
	POINTSYSTEM();
	virtual ~POINTSYSTEM();

	virtual bool init(LPDIRECT3DDEVICE9 device,wchar_t *text_name);			//没有重载
	virtual void reset();			//重新设定系统中每个粒子的属性P236
	virtual void reset_particle(ATTRIBUTE *attribute) = 0;		//
	virtual void add_particle();
	//
	virtual void update(float time) = 0;
	//
	virtual void prepare_render();
	virtual void render();
	virtual void post_render();

	bool is_empty();
	bool is_dead();

protected:
	virtual void remove_dead_particles();
};

# endif