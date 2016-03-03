# ifndef _START_H
# define _START_H

# include <d3d9.h>							//初始化direct3D时添加
# include <d3dx9.h>
# include "Meshxfile.h"
# include "camera.h"

class START:public MESHXFILE,public CAMERA
{
	struct TEXT_STRUCT
	{
		LPD3DXMESH	_text;
		LPD3DXMESH	_sphere_mesh;
		BOUNDINGSPHERE _sphere;
	};
	D3DXMATRIX		_world_snake;
	D3DXMATRIX		_scale_text;
	D3DXMATRIX		_rotation_text;
	D3DXVECTOR3		_init_position;
	D3DXVECTOR3		_init_target;
	LPDIRECT3DTEXTURE9  _textures;			//文字的纹理信息
	const int		_time_cut;
	int				 _time_one;
	int				_time_two;
	int				_time_three;
	int				_time_lock;
	TEXT_STRUCT		*_text_start;
	TEXT_STRUCT		*_text_prompt;
	const int		_time_one_almost;
	const int		_time_two_almost;
	const int		_time_three_almost;
	const int		_time_lock_almost;
	const float		_scale_text_date;
	const float		_angle_text;
	bool			_isrender_start_text;
	bool			_isrender_prompt_text;
	int				_num_start;
	int				_num_prompt;
public:
	START(LPDIRECT3DDEVICE9);
	~START();
	void init(wchar_t *name_snake,wchar_t**name_text,int start,int prompt,
					const D3DXVECTOR3 &position,const D3DXVECTOR3 &lookat);
	void init_snake(wchar_t *);
	void init_camera(const D3DXVECTOR3 &position,const D3DXVECTOR3 &lookat);
	void init_text(wchar_t **name,int num_start,int num_promt);
	void create_text(TEXT_STRUCT *text,wchar_t *name_text,HDC &hdc,int num);
	void update();
	bool test(int,int y);
	int stage();
	void set_prompt();
	RAY CalcPickingRay(int x, int y);
	bool RaySphereIntTest(RAY* ray,BOUNDINGSPHERE* sphere);
	void render();
	void render_text(TEXT_STRUCT *,int);
	void reset();
	void begin_game();
};


# endif