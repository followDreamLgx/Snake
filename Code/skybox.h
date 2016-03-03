# ifndef _SKYBOX_H
# define _SKYBOX_H

# include "head.h"
# include "Meshxfile.h"
# define FRUIT_NUM   5

extern void test();	
class  SKYBOX
{
protected:
	LPDIRECT3DDEVICE9		_device;
	LPDIRECT3DVERTEXBUFFER9	_vertexbuf;
	LPDIRECT3DINDEXBUFFER9	_indexbuf;
	LPDIRECT3DTEXTURE9		_texture[6];
public:
	SKYBOX(LPDIRECT3DDEVICE9 );
	VOID InitSkyBox(wchar_t **name,const BOUNDINGBOX &box);
	VOID render();
};

struct FRUIT_NODE
{
	FRUIT_STRUCT *_head;
	FRUIT_STRUCT *_end;
	struct FRUIT_NODE *_next;
};
class SKYBOX_DERIVED:public SKYBOX
{
protected:
	LPDIRECT3DVERTEXBUFFER9	_vertexbuf_map;
	LPDIRECT3DTEXTURE9		_texture_map;
	MESHXFILE				*_fruit_mesh;		//水果的x文件的头指针，对应的数量由类型数决定
	const int				_fruit_style;		//水果总的类型
	FRUIT_NODE				*_fruit_head;				//记录水果的头结点与结尾
	int						_fruit_num;
	const float				_scale_fruit;		//水果的放缩比例
	const BOUNDINGBOX		_map;				//地图的大小
	const float				_distance;
	const int				_product_time;		//生产水果需要的时间
	const int				_product_cut;
	int						_product;			//记录是否重新生成水果
	bool					_begin_game;
public:
	SKYBOX_DERIVED(LPDIRECT3DDEVICE9 device,const BOUNDINGBOX &map_box,int fruit_num);
	~SKYBOX_DERIVED();
	void init(wchar_t **name_sky,wchar_t *name_map,wchar_t **name_fruit,const BOUNDINGBOX &box);	//
	void init(wchar_t *name_map);			//初始化小地图
	void inti(wchar_t **name_fruit);		//初始化水果，并增添水果
	void add();								//添加水果
	void reset(FRUIT_STRUCT*);				//重新设置水果的属性，添加水果时，用于随机设置水果
	void update();							//更新水果，将没用的水果去除
	void render();
	void render_map();
	void render_fruit();
	bool game_end();
	bool game_begin();
	friend void test();
};

# endif