# ifndef _ATTACK_H
# define _ATTACK_H

# include "SkinMesh.h"
# include "head.h"
# define JUGG 1
# define SUQI 2


struct PERSON
{
	PERSON_STRUCT *_head;
	PERSON_STRUCT *_end;
	struct PERSON *_next;
};
struct PRODUCT
{
	int			_product;
	bool		_prepare;
	D3DXMATRIX	_scale;
	CSkinMesh	*_skin;
};
extern void test();
class ATTACK
{
protected:
	LPDIRECT3DDEVICE9		_device;
	PRODUCT					*_jugg;
	PRODUCT					*_suqi;
	PERSON					*_person;
	const int				_life_time;
	const int				_life_cut;
	char					*_ani_set_jugg[6];
public:
	ATTACK(LPDIRECT3DDEVICE9 );
	~ATTACK();
	void init(wchar_t **);
	void add(const D3DXVECTOR3 &position,int style);
	void update();
	void render(const D3DXMATRIX &world_base); 
	friend class SELECT_SYSTEM;
	friend void test();
};


struct SELECT
{
	CSkinMesh	*_skin;
	BOUNDINGSPHERE _sphere_im;
	D3DXMATRIX	_tran;
	D3DXMATRIX  _scale;
	D3DXMATRIX  _rotation;
	ID3DXMesh	*_sphere;
	bool		_prepare;
	int			_product;
};
class SELECT_SYSTEM
{
protected:
	SELECT	*_jugg;
	SELECT	*_suqi;
	const int	_product_time;
	const int	_product_cut;
	const BOUNDINGBOX	_box;
	LPD3DXFONT	_text;
	D3DMATERIAL9 _material;		//网格的材质信息
	LPDIRECT3DDEVICE9 _device;
	RECT rect;						//用于获取主窗口矩形
	char		*_ani_set_jugg[6];
public:
	SELECT_SYSTEM(LPDIRECT3DDEVICE9 device,const BOUNDINGBOX &box);
	~SELECT_SYSTEM();
	void init(wchar_t*name_jugg,wchar_t*name_suqi);
	void init_suqi(wchar_t*name_suqi);
	void init_jugg(wchar_t*name_jugg);
	void init_text();
	bool test(float x,float y);
	bool add_person(float x,float y,const D3DXMATRIX world,ATTACK *);
	RAY CalcPickingRay(int x, int y);
	void render();
	void update();
};

# endif