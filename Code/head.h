# ifndef _HEAD_H
# define _HEAD_H

# include <windows.h>						//初始化window时添加
# include <d3d9.h>							//初始化direct3D时添加
# include <d3dx9.h>
# include <fstream>							//地形类中用到
using namespace std;						//地形类中用到ifstream

//D3DXCOLOR颜色值
const D3DXCOLOR      WHITE( D3DCOLOR_XRGB(255, 255, 255) );
const D3DXCOLOR      BLACK( D3DCOLOR_XRGB(  0,   0,   0) );
const D3DXCOLOR        RED( D3DCOLOR_XRGB(255,   0,   0) );
const D3DXCOLOR      GREEN( D3DCOLOR_XRGB(  0, 255,   0) );
const D3DXCOLOR       BLUE( D3DCOLOR_XRGB(  0,   0, 255) );
const D3DXCOLOR     YELLOW( D3DCOLOR_XRGB(255, 255,   0) );
const D3DXCOLOR       CYAN( D3DCOLOR_XRGB(  0, 255, 255) );
const D3DXCOLOR    MAGENTA( D3DCOLOR_XRGB(255,   0, 255) );

//四面体灵活顶点格式
struct CUSTOMVERTEX
{
	D3DXVECTOR3 _position;
	D3DXVECTOR3 _n;
	FLOAT _u1,_v1;				//使用了纹理坐标后需要改变宏定义

	CUSTOMVERTEX(D3DXVECTOR3 position,D3DXVECTOR3 n,
					FLOAT u1,FLOAT v1):
						_position(position),
						_n(n),
						_u1(u1),_v1(v1)
	{}
};
# define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
//地形,天空盒的灵活顶点格式
//广告牌也是这种
struct TERRAINVERTEX
{
	FLOAT _x,_y,_z;
	FLOAT _u,_v;
	TERRAINVERTEX(FLOAT x,FLOAT y,FLOAT z,FLOAT u,FLOAT v):
					_x(x),_y(y),_z(z),_u(u),_v(v)
					{}
};

# define D3DFVF_TERRAINVERTEX (D3DFVF_XYZ |  D3DFVF_TEX1)


//枚举变量
enum NUM{A = 0,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z};

struct BOUNDINGBOX
{
public:
	D3DXVECTOR3 _min;
	D3DXVECTOR3 _max;

public:
	BOUNDINGBOX(D3DXVECTOR3&);
	bool IsPointInside(D3DXVECTOR3 &);				//传入物体的地址
};
struct BOUNDINGSPHERE
{
	D3DXVECTOR3 _center;
	float _radius;
};
//-------------------------------------
//粒子的属性
//-------------------------------------
struct ATTRIBUTE
{
	D3DXVECTOR3 _position;
	D3DXVECTOR3 _velocity;		//粒子的速度
	D3DXVECTOR3	_acceleration;	//粒子的加速度
	float		_life_time;		//粒子存活的周期
	float		_age;			//粒子的当前年龄
	D3DXCOLOR		_color;
	D3DXCOLOR		_color_fade;	//粒子颜色如何随时间减弱
	bool		_alive;			//记录粒子是否存活
};

//粒子系统顶点结构
struct PARTICLE
{
	D3DXVECTOR3 _position;
	D3DCOLOR _color;
	static const DWORD FVF;
};
//射线
struct RAY
{
	D3DXVECTOR3 _origin;
	D3DXVECTOR3 _direction;
};

//-----------------------------贪吃蛇-------------------------------//
struct PERSON_STRUCT
{
	D3DXVECTOR3	_position;
	D3DXVECTOR3 _look;
	int			_life;
	int			_style;
	struct PERSON_STRUCT *_next;
};
//水果相关结构体
struct FRUIT_STRUCT
{
	bool _alive;
	int _style;
	D3DXVECTOR3 _position;
	struct FRUIT_STRUCT *_next;
};
//-----------------------------------------------------------------------//
//将float存储的内容原封不动转换为DWORD
DWORD FtoDw(float);
//产生一个在规定范围的随机数
float GetRandomFloat(float lowbound,float highbound);
//产生一个在规定范围的随机向量
void GetRandomVector(D3DXVECTOR3 *out,D3DXVECTOR3 *min,D3DXVECTOR3 *max);

# endif