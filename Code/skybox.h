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
	MESHXFILE				*_fruit_mesh;		//ˮ����x�ļ���ͷָ�룬��Ӧ������������������
	const int				_fruit_style;		//ˮ���ܵ�����
	FRUIT_NODE				*_fruit_head;				//��¼ˮ����ͷ������β
	int						_fruit_num;
	const float				_scale_fruit;		//ˮ���ķ�������
	const BOUNDINGBOX		_map;				//��ͼ�Ĵ�С
	const float				_distance;
	const int				_product_time;		//����ˮ����Ҫ��ʱ��
	const int				_product_cut;
	int						_product;			//��¼�Ƿ���������ˮ��
	bool					_begin_game;
public:
	SKYBOX_DERIVED(LPDIRECT3DDEVICE9 device,const BOUNDINGBOX &map_box,int fruit_num);
	~SKYBOX_DERIVED();
	void init(wchar_t **name_sky,wchar_t *name_map,wchar_t **name_fruit,const BOUNDINGBOX &box);	//
	void init(wchar_t *name_map);			//��ʼ��С��ͼ
	void inti(wchar_t **name_fruit);		//��ʼ��ˮ����������ˮ��
	void add();								//���ˮ��
	void reset(FRUIT_STRUCT*);				//��������ˮ�������ԣ����ˮ��ʱ�������������ˮ��
	void update();							//����ˮ������û�õ�ˮ��ȥ��
	void render();
	void render_map();
	void render_fruit();
	bool game_end();
	bool game_begin();
	friend void test();
};

# endif