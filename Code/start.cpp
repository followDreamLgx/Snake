# include "start.h"

START::START(LPDIRECT3DDEVICE9 device):MESHXFILE(),CAMERA(device),
	_time_cut(5),
	_time_lock_almost(1000),
	_time_one_almost(10000),
	_time_two_almost(2000),
	_time_three_almost(2600),
	_scale_text_date(1500),
	_angle_text(120 / 180)
{

}
START::~START()
{

}
//初始化
void START::init(wchar_t *name_snake,wchar_t**name_text,int start,int prompt,
					const D3DXVECTOR3 &position,const D3DXVECTOR3 &lookat)
{
	init_snake(name_snake);
	init_camera(position,lookat);
	init_text(name_text,start,prompt);
	
	_init_position = position;
	_init_target = lookat;
	//初始化时间
	_time_one = _time_one_almost;
	_time_two = _time_two_almost;
	_time_three = _time_three_almost;
	_time_lock = 0;
	_isrender_start_text = false;
	_isrender_prompt_text = false;
}
void START::init_snake(wchar_t *name_snake)
{
	MESHXFILE::init(name_snake,device);
	D3DXMATRIX ro,tran,scale;
//	D3DXMatrixRotationAxis(&ro
	D3DXMatrixTranslation(&tran,0,2000,0);			//注意修改时需要修改提示文字的包装球的初始位置
	D3DXMatrixScaling(&scale,20,20,20);
	D3DXMatrixIdentity(&_world_snake);
	_world_snake = _world_snake * scale * tran;
}
void START::init_camera(const D3DXVECTOR3 &position,const D3DXVECTOR3 &lookat)
{
	set_camera(position,lookat);
}
void START::init_text(wchar_t **name_text,int num_start,int num_prompt)
{
	_num_start = num_start;			//记录字符串数量
	_num_prompt = num_prompt;

	D3DXMatrixScaling(&_scale_text,_scale_text_date / 4,_scale_text_date,_scale_text_date);
	D3DXMatrixRotationAxis(&_rotation_text,&D3DXVECTOR3(0,1,0),_angle_text);

	HDC hdc = CreateCompatibleDC( 0 );		//创建设备环境，hdc为windows设置环境的句柄
    HFONT hFont;
    HFONT hFontOld;

    LOGFONT lf;
	ZeroMemory(&lf, sizeof(LOGFONT));		//描述字体信息
	lf.lfHeight         = 200;				//
	lf.lfWidth          = 200;				//
	lf.lfEscapement     = 0;        
	lf.lfOrientation    = 0;     
	lf.lfWeight         = 1000;				// boldness, range 0(light) - 1000(bold)
	lf.lfItalic         = false;				//斜体
	lf.lfUnderline      = false;				//下划线
	lf.lfStrikeOut      = false;    
	lf.lfCharSet        = DEFAULT_CHARSET;
	lf.lfOutPrecision   = 0;              
	lf.lfClipPrecision  = 0;          
	lf.lfQuality        = 0;           
	lf.lfPitchAndFamily = 0;    
	
	wcscpy(lf.lfFaceName,L"宋体");	//字体

	//创建字体
    hFont = CreateFontIndirect(&lf);
    hFontOld = (HFONT)SelectObject(hdc, hFont); //将字体选入设备环境，并记录原字体

	//创建字体网格
	_text_start = new TEXT_STRUCT[num_start];
	_text_prompt = new TEXT_STRUCT[num_prompt];
	for(int i = 0;i < num_start + num_prompt;i++)
	{
		if(i < num_start)
		{
			create_text(&_text_start[i],name_text[i],hdc,i);							//创建开始游戏网格以及包装球
		}
		else 
		{	//i对应着名称的后半部分
			create_text(&_text_prompt[i - num_start],name_text[i],hdc,i - num_start);	//创建提示字体网格以及包装球		
		}
	}

    SelectObject(hdc, hFontOld);			//选入原字体
    DeleteObject( hFont );					//释放字体资源
    DeleteDC( hdc );						//释放设备环境

	D3DXCreateTextureFromFileA(_device,
				"陆雪琪(修改).jpg",			//从pMtrBuffer得到起始地址
				&_textures);				//存储纹理
}
//字体
void START::create_text(TEXT_STRUCT *text,wchar_t *name_text,HDC &hdc,int num)
{
	D3DXCreateText(_device, hdc,name_text,				//创建字体网格
		0.001,0.01, &(text->_text), 0, 0);
	text->_sphere._radius = 800;						//指定半径
	text->_sphere._center = D3DXVECTOR3(-1000,1400,0) + (num + 1) * D3DXVECTOR3(0,1500,0);			//设置包装盒的世界位置，拾取用
	D3DXCreateSphere(_device,text->_sphere._radius,20,20,&(text->_sphere_mesh),NULL);	//创建包装球
}
void START::update()
{
	float time_ro = -0.062;
	float time_tran = 1;
	if(_time_lock > 0)					//停下来擦看物体
	{	_time_lock -= _time_cut;	}
	else if(_time_one > 0)				//摄像机旋转阶段
	{
		_time_one -= _time_cut;
		RotationY(time_ro * _time_cut);				//传入的数为角度
		TranslationLook(time_tran * _time_cut);
	}
	else if(_time_two > 0)				//从第一个人物过度到贪吃蛇阶段
	{
		_time_two -= _time_cut;
		TranslationZ(time_tran * _time_cut);
	}
	else if(_time_three > 0)			//从贪吃蛇过度到第二个人物阶段
	{
		_time_three -= _time_cut;
		TranslationZ(time_tran * _time_cut);
	}
	else
	{	//摄像机遍历了所有物体，从远方观察紫禁之巅，并准备绘制提示字体
		set_camera(D3DXVECTOR3(-3475,3500,-3475),D3DXVECTOR3(0,3000,0));
		_isrender_start_text = true;
	}
	if(_time_lock == 0)					//保证没有在停止时调用
	{
		if(_time_one == 0)
		{
			_time_lock = _time_lock_almost;
			_time_one -= _time_cut;
		}
		else if(_time_two == 0)
		{
			_time_lock = _time_lock_almost;
			_time_two -= _time_cut;			
		}
		else if(_time_three == 0)		//摄像机到达某一个点停下来观察
		{
			_time_lock = _time_lock_almost;
			_time_three -= _time_cut;	
		}				
	}
}
void START::render()
{
	if(_isrender_start_text == false)			//摄像机还处于运动阶段
		update();
	else if(_isrender_prompt_text == true)		//需要绕开第二阶段，避免第二阶段进入
		render_text(_text_prompt,_num_prompt);
	else if(_isrender_start_text = true)		//
		render_text(_text_start,_num_start);
	View();
	_device->SetTransform(D3DTS_WORLD,&_world_snake);		//绘制蛇
	MESHXFILE::DrawMesh();
}
//绘制提示字符
void START::render_text(TEXT_STRUCT *text_struct,int num)
{
	D3DXMATRIX tran,world;
	for(int i = 0;i < num;i++)
	{
		D3DXMatrixTranslation(&tran,text_struct[i]._sphere._center.x,
									text_struct[i]._sphere._center.y,
									text_struct[i]._sphere._center.z);
		 world = _scale_text * _rotation_text * tran;
		_device->SetTransform(D3DTS_WORLD,&world);	
		_device->SetTexture(0,_textures);
		text_struct[i]._text->DrawSubset(0);					//文字
	}	
	//_device->SetRenderState(D3DRS_ALPHABLENDENABLE,true);		//设置alpha融合
	//_device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	//_device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	//for(int i = 0;i < num;i++)
	//{
	//	D3DXMatrixTranslation(&tran,text_struct[i]._sphere._center.x,
	//								text_struct[i]._sphere._center.y,
	//								text_struct[i]._sphere._center.z);
	//	//测试的时候是测试实际球的大小，所以没办法通过缩放来测试，平移矩阵就是球的变换矩阵
	//	_device->SetTransform(D3DTS_WORLD,&tran);
	//	text_struct->_sphere_mesh->DrawSubset(0);				//包装start
	//}
	//_device->SetRenderState(D3DRS_ALPHABLENDENABLE,false);
}

//测试射线是否与开始按钮相交
bool START::test(int x,int y)
{
	RAY ray;
	ray = CalcPickingRay(x,y);
	if(RaySphereIntTest(&ray,&(_text_start->_sphere)) == true)
		return true;
return false; 
}
//判断游戏阶段的函数
int START::stage()
{
	if(_isrender_prompt_text == true)
		return 3;							//游戏提示
	else if(_isrender_start_text == true)
		return 2;							//出现开始按钮
	else 
		return 1;							//摄像机处于运动状态
}
//设置函数进入游戏提示阶段
void START::set_prompt()
{
	_isrender_prompt_text = true;
	_isrender_start_text = false;
}
//将屏幕上的点转换为世界坐标系的射线
RAY START::CalcPickingRay(int x, int y)
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
bool START::RaySphereIntTest(RAY* ray,BOUNDINGSPHERE* sphere)
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
void START::reset()
{
	set_camera(_init_position,_init_target);
	//初始化时间
	_time_one = _time_one_almost;
	_time_two = _time_two_almost;
	_time_three = _time_three_almost;
	_time_lock = 0;
	_isrender_start_text = false;
	_isrender_prompt_text = false;
}

void START::begin_game()
{
	set_camera(D3DXVECTOR3(-3475,3500,-3475),D3DXVECTOR3(0,3000,0));
	_isrender_start_text = true;
	_time_one = 0;
	_time_two = 0;
	_time_three = 0;
	_time_lock = 0;
}