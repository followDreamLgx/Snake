# include "attack.h"
# define X_SUQI			0
# define Y_SUQI			2000
# define Z_SUQI			_box._min.z
# define RADIUS_SUQI	800
# define SCALE_SUQI		20

# define X_JUGG			-200
# define Y_JUGG			2000
# define Z_JUGG			_box._max.z
# define RADIUS_JUGG	600
# define SCALE_JUGG		6


bool RaySphereIntTest(RAY* ray,BOUNDINGSPHERE* sphere);
void TransformRay(RAY* ray, D3DXMATRIX* T);
bool IntersectTriangle(const D3DXVECTOR3& orig, const D3DXVECTOR3& dir,
    D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2,
    float* t);

ATTACK::ATTACK(LPDIRECT3DDEVICE9 device):
	_life_time(1200),
	_life_cut(1)
{	
	_device	= device;
	_jugg = new PRODUCT();
	_suqi = new PRODUCT();
	_jugg->_skin = _suqi->_skin = NULL;

	_person = new PERSON();				
	_person->_end = _person->_head = NULL;
	_person->_next = NULL;
}
ATTACK::~ATTACK()
{}
void ATTACK::init(wchar_t **name_person)
{	
	_jugg->_skin = new CSkinMesh();
	_suqi->_skin = new CSkinMesh();
	_jugg->_skin->OnCreate(_device,name_person[0]);
	_suqi->_skin->OnCreate(_device,name_person[1]);
	_jugg->_product = _suqi->_product = 0;
	_jugg->_prepare = _suqi->_prepare = false;
	D3DXMATRIX ro;
	//剑圣的初始变换矩阵
	D3DXMatrixScaling(&_jugg->_scale,SCALE_JUGG,SCALE_JUGG,SCALE_JUGG);
	D3DXMatrixRotationAxis(&ro,&D3DXVECTOR3(0,0,1),0);
	_jugg->_scale *= ro;
	char *a[6]= {{"stay"},{"walk"},{"attack"},{"behit"},
	{"dieing"},{"dead"}};
	for(int i = 0;i < 6;i++)
		_ani_set_jugg[i] = a[i];
	_jugg->_skin->set_animation(_ani_set_jugg[2]);
	//苏琪的初始变换矩阵
	D3DXMatrixScaling(&_suqi->_scale,SCALE_SUQI,SCALE_SUQI,SCALE_SUQI);
}
void ATTACK::add(const D3DXVECTOR3 &position,int style)
{
	PERSON_STRUCT *p_new;
	if(_person->_head == NULL)		//增添到头结点
		_person->_head = _person->_end = p_new = new PERSON_STRUCT();
	else //补充到最后节点
		_person->_end = _person->_end->_next = p_new = new PERSON_STRUCT();

	p_new->_life = _life_time;
	p_new->_look = D3DXVECTOR3(0,0,-1);
	p_new->_position = position;
	p_new->_style = style;
	p_new->_next = NULL;
}
void ATTACK::update()
{
	PERSON_STRUCT *p = _person->_head,*before;
	for(before = p;p != NULL;p = p->_next)
	{
		if(p->_life > 0)
		{	
			p->_life -= _life_cut;
		}
		else
		{
			if(p == _person->_head && p == _person->_end)	//只有一个节点
			{												//删除后直接退出
				_person->_head = _person->_end = NULL;	
				delete p;
				p = NULL;
				return ;
			}
			else if(p == _person->_head)			//多个节点，删除头结点
			{
				_person->_head = p->_next;
				delete p;
				p = _person->_head;
			}
			else if(p == _person->_end)
			{
				_person->_end = before;			//删除最后一个节点后可以直接退出
				_person->_end = NULL;
				delete p;				
				return ;					
			}
		}
		before = p;
	}
}
void ATTACK::render(const D3DXMATRIX &world_base)
{
	update();
	//这里的矩阵类型与平时使用的类型不同
	D3DXMATRIXA16 world = (D3DXMATRIXA16)world_base;
	D3DXMATRIXA16 tran;
	PERSON_STRUCT *p = _person->_head;
	float time = 0.01;

	for(;p != NULL;p = p->_next)
	{
		/*D3DXMatrixIdentity(&world);*/
		world = world_base;
		D3DXMatrixTranslation(&tran,p->_position.x,p->_position.y,p->_position.z);
		if(p->_style == JUGG)
		{
			world = world * _jugg->_scale * tran;
			_jugg->_skin->Render(&world,time);
		}
		else if(p->_style == SUQI)
		{
			world = world * _suqi->_scale * tran;
			_suqi->_skin->Render(&world,time);
		}
	}
}

//------------------------------------------
//选择系统
//------------------------------------------
SELECT_SYSTEM::SELECT_SYSTEM(LPDIRECT3DDEVICE9 device,const BOUNDINGBOX &box):
	_product_time(800),
	_product_cut(1),
	_device(device),
	_box(box)
{ 
	_jugg = new SELECT();
	_jugg->_skin = new CSkinMesh();
	_suqi = new SELECT();
	_suqi->_skin = new CSkinMesh();
}
SELECT_SYSTEM::~SELECT_SYSTEM()
{ }
//--------------------------------------------------
//初始化及相关函数
//--------------------------------------------------
void SELECT_SYSTEM::init(wchar_t*name_jugg,wchar_t*name_suqi)
{ 
	init_text();
	init_suqi(name_suqi);
	init_jugg(name_jugg);
	ZeroMemory(&_material,sizeof(D3DMATERIAL9));
	_material.Diffuse = _material.Ambient = D3DXCOLOR(0.5,0.5,0.5,0.5f);
	_material.Emissive = D3DXCOLOR(0.5,0.5,0.5,0.5f);
	_material.Specular = D3DXCOLOR(0.5,0.5,0.5,0.5f);
	_material.Power = 0.5;
}

void SELECT_SYSTEM::init_suqi(wchar_t*name_suqi)
{
	//创建苏琪
	//注意，修改了坐标需要同时修改球的坐标
	_suqi->_skin->OnCreate(_device,name_suqi);
	_suqi->_sphere_im._center = D3DXVECTOR3(X_SUQI,Y_SUQI,Z_SUQI);
	_suqi->_sphere_im._radius = RADIUS_SUQI;
	_suqi->_prepare = false;
	_suqi->_product = 0;
	D3DXMatrixScaling(&(_suqi->_scale),SCALE_SUQI,SCALE_SUQI,SCALE_SUQI);
	D3DXMatrixRotationAxis(&(_suqi->_rotation),&D3DXVECTOR3(0,0,1),0);
	D3DXMatrixTranslation(&(_suqi->_tran),X_SUQI,Y_SUQI,Z_SUQI);
	D3DXCreateSphere(_device,_suqi->_sphere_im._radius,20,20,&(_suqi->_sphere),0);
}
void SELECT_SYSTEM::init_jugg(wchar_t*name_jugg)
{
	//创建剑圣
	_jugg->_skin->OnCreate(_device,name_jugg);
	_jugg->_sphere_im._center= D3DXVECTOR3(X_JUGG,Y_JUGG,Z_JUGG);
	_jugg->_sphere_im._radius = RADIUS_JUGG;
	_jugg->_prepare = false;
	_jugg->_product = 0;
	D3DXMatrixRotationAxis(&(_jugg->_rotation),&D3DXVECTOR3(0,0,1),0);
	D3DXMatrixScaling(&(_jugg->_scale),SCALE_JUGG,SCALE_JUGG,SCALE_JUGG);
	D3DXMatrixTranslation(&(_jugg->_tran),X_JUGG,Y_JUGG,Z_JUGG);
	D3DXCreateSphere(_device,_jugg->_sphere_im._radius, 20,20,&(_jugg->_sphere),0);

	//剑圣的动作
	char *a[6]= {{"stay"},{"walk"},{"attack"},{"behit"},
	{"dieing"},{"dead"}};
	for(int i = 0;i < 6;i++)
		_ani_set_jugg[i] = a[i];
	_jugg->_skin->set_animation(_ani_set_jugg[2]);
}
void SELECT_SYSTEM::init_text()
{
	D3DXCreateFont(_device,						//P277     2d字体
	50,												//高度
	30,												//平均宽度
	1000,											//字体权重值
	0,												//过滤属性
	false,											//斜体
	DEFAULT_CHARSET,								//字符集，使用默认的字符集
	OUT_DEFAULT_PRECIS,								//输出文本的精度
	DEFAULT_QUALITY,								//字符的输出质量
	0,												//指定字体的索引，通常设为0
	L"隶书",										//字体，此为宋体
	&_text);										//UFO
	//GetClientRect(hWnd,&rect);						//获取主窗口矩形
}
//---------------------------------------------
//测试鼠标是否点击了人物的函数
//注意解决重复点击的问题，在外部解决
//---------------------------------------------
bool SELECT_SYSTEM::test(float x,float y)
{ 
	if(_jugg->_prepare == true || _suqi->_prepare == true)	//已经点击了其中一个人物
		return false;
	RAY ray;
	ray = CalcPickingRay(x,y);								//计算出射线
	if(RaySphereIntTest(&ray,&(_jugg->_sphere_im)) == true
		&& _jugg->_product <=0 )						//点击了剑圣
	{
		_jugg->_prepare = true;
		return true;
	}
	ray = CalcPickingRay(x,y);
	if(RaySphereIntTest(&ray,&(_suqi->_sphere_im)) == true
		&& _suqi->_product <= 0)		//点击了苏琪
	{
		_suqi->_prepare = true;
		return true;
	}
return false;
}
//注意了，此处的世界变换与地图的世界变换应该一致
bool SELECT_SYSTEM::add_person(float x,float y,const D3DXMATRIX world,ATTACK *attack)
{
	float t;
	RAY ray;
	ray = CalcPickingRay(x,y);                                        
	static D3DXVECTOR3	ver_A = D3DXVECTOR3(_box._min.x,0,_box._max.z),
						ver_B = D3DXVECTOR3(_box._max.x,0,_box._max.z),
						ver_C = D3DXVECTOR3(_box._max.x,0,_box._min.z),
						ver_D = D3DXVECTOR3(_box._min.x,0,_box._min.z);
	//判断射线是否与地图相交，假如相交，在相应点添加人物
	if(IntersectTriangle(ray._origin,ray._direction,ver_A,ver_B,ver_C,&t) == true	||
		IntersectTriangle(ray._origin,ray._direction,ver_A,ver_C,ver_D,&t) == true)
	{
		if(_jugg->_prepare == true)			//某一个人物已经生产好并且被点击
		{	
			attack->add(ray._origin + ray._direction * t,JUGG);
			_jugg->_product = _product_time;				//恢复生产状态
			_jugg->_prepare = false;						//恢复未选择状态
			return true;
		}
		else if(_suqi->_prepare == true)
		{
			attack->add(ray._origin + ray._direction * t,SUQI);
			_suqi->_product = _product_time;				//恢复生产状态
			_suqi->_prepare = false;						//恢复未选择状态
			return true;			
		}
	}
	::MessageBox(0,L"SELECT_SYSTEM::add_person - FAILED",0,0);
return false;
}
void SELECT_SYSTEM::render()
{ 
	update();
	D3DXMATRIXA16 world;
	float time = 0.005;
	//绘制剑圣
	world = _jugg->_scale * _jugg->_rotation * _jugg->_tran;
	_jugg->_skin->Render(&world,time);
	//绘制舒淇	
	world = _suqi->_scale * _suqi->_rotation * _suqi->_tran;
	_suqi->_skin->Render(&world,time);

	//融合准备
	_device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);					//打开alpha融合
	//四面体镜像与镜面的融合方程
	_device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCCOLOR);
	_device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCCOLOR);
	//绘制剑圣包装球
	_device->SetTransform(D3DTS_WORLD,&_jugg->_tran);
	if(_jugg->_product > 0)											//剑圣还在集气，注意包装球无须放缩变换
		_jugg->_sphere->DrawSubset(0);		

	_device->SetTransform(D3DTS_WORLD,&_suqi->_tran);
	if(_suqi->_product > 0)											//苏琪还在集气，注意包装球无须放缩变换
		_suqi->_sphere->DrawSubset(0);
	_device->SetRenderState(D3DRS_ALPHABLENDENABLE,false);					//关闭融合
}
void SELECT_SYSTEM::update()
{ 
	if(_jugg->_product > 0)
		_jugg->_product -= _product_cut;
	if(_suqi->_product > 0)
		_suqi->_product -= _product_cut;
}


//----------------------------
//龙书的几个测试函数，已经过修改.
//使用时应注意：结构体在head文件
//使用的前提是，投影变换矩阵与取景变换矩阵已经设置好
//----------------------------
RAY SELECT_SYSTEM::CalcPickingRay(int x, int y)
{
	float px = 0.0f;
	float py = 0.0f;

	D3DVIEWPORT9 vp;
	_device->GetViewport(&vp);

	D3DXMATRIX proj;
	_device->GetTransform(D3DTS_PROJECTION, &proj);

	px = ((( 2.0f*x) / vp.Width)  - 1.0f) / proj(0, 0);
	py = (((-2.0f*y) / vp.Height) + 1.0f) / proj(1, 1);

	RAY ray;
	ray._origin    = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	ray._direction = D3DXVECTOR3(px, py, 1.0f);
	//将射线平移至世界坐标系,详情可以参照矩阵的平移以及摄像机类的取景变换矩阵
	//下面的代码可以用下一个函数代替
	D3DXMATRIX view;
	_device->GetTransform(D3DTS_VIEW,&view);
	D3DXMatrixInverse(&view, NULL, &view);
	D3DXVec3TransformCoord(&ray._origin,&ray._origin,&view);
	D3DXVec3TransformNormal(&(ray._direction),&(ray._direction),&view);
	D3DXVec3Normalize(&(ray._direction),&(ray._direction));
	return ray;
}
void TransformRay(RAY* ray, D3DXMATRIX* T)
{
	// transform the ray's origin, w = 1.
	//转换ray的出发点
	D3DXVec3TransformCoord(
		&ray->_origin,
		&ray->_origin,
		T);

	// transform the ray's direction, w = 0.
	//转换ray的方向
	D3DXVec3TransformNormal(
		&ray->_direction,
		&ray->_direction,
		T);

	// normalize the direction
	//规范化方向向量
	D3DXVec3Normalize(&ray->_direction, &ray->_direction);
}

bool RaySphereIntTest(RAY* ray,BOUNDINGSPHERE* sphere)
{
	D3DXVECTOR3 v = ray->_origin - sphere->_center;

	float b = 2.0f * D3DXVec3Dot(&ray->_direction, &v);
	float c = D3DXVec3Dot(&v, &v) - (sphere->_radius * sphere->_radius);

	// find the discriminant
	float discriminant = (b * b) - (4.0f * c);

	// test for imaginary number
	if( discriminant < 0.0f )
		return false;

	discriminant = sqrtf(discriminant);

	float s0 = (-b + discriminant) / 2.0f;
	float s1 = (-b - discriminant) / 2.0f;

	// if a solution is >= 0, then we intersected the sphere
	if( s0 >= 0.0f || s1 >= 0.0f )
		return true;
	
	return false;
}

// Determine whether a ray intersect with a triangle
// Parameters
// orig: origin of the ray
// dir: direction of the ray
// v0, v1, v2: vertices of triangle
// t(out): weight of the intersection for the ray
// u(out), v(out): barycentric coordinate of intersection

bool IntersectTriangle(const D3DXVECTOR3& orig, const D3DXVECTOR3& dir,
    D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2,
    float* t)
{
    // E1
    D3DXVECTOR3 E1 = v1 - v0;
    // E2
    D3DXVECTOR3 E2 = v2 - v0;
    // P
    D3DXVECTOR3 P;
	D3DXVec3Cross(&P,&dir,&E2);
    // determinant
    float det = D3DXVec3Dot(&E1,&P);
    // keep det > 0, modify T accordingly
    D3DXVECTOR3 T;
    if( det >0 )
    {
        T = orig - v0;
    }
    else
    {
        T = v0 - orig;
        det = -det;
    }
    // If determinant is near zero, ray lies in plane of triangle
    if( det < 0.0001f )
        return false;
    // Q
    D3DXVECTOR3 Q;
	D3DXVec3Cross(&Q,&T,&E1);
    // Calculate t, scale parameters, ray intersects triangle
    *t = D3DXVec3Dot(&E2,&Q);
    float fInvDet = 1.0f / det;
    *t *= fInvDet;

return true;
}