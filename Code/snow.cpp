# include "snow.h"
# include "head.h"

//指定雪花区域与雪花数量，大小，缓存大小，片段大小			P244
SNOW::SNOW(BOUNDINGBOX *box,int num_particles):_box(*box)
{
	_size = 50.0f;							//雪花的大小
	_vertexbuf_size = 2048;					//粒子系统对GPU的可用大小
	_vertexbuf_offset = 0;					//顶点缓存的偏移量
	_vertexbuf_num = 512;					//每次填充顶点缓存顶点数
	for(int i = 0;i < num_particles;i++)	//根据所需数量添加雪花
		add_particle();
}

//本函数没错
//设置粒子属性						P245
//在指定的外界体内创建了具有随机xz坐标的雪花粒子，并将粒子的y坐标设为
//外接体高度值，赋予雪花一定向左下飘的速度
void SNOW::reset_particle(ATTRIBUTE *attribute)
{
	attribute->_alive = true;
	GetRandomVector(&attribute->_position,			//设置雪花飘落的速度大小以及方向
		&_box._min,
		&_box._max);

	attribute->_position.y = _box._max.y;
	attribute->_velocity.x = GetRandomFloat(0.0f,1.0f) * (-3.0f);
	attribute->_velocity.y = GetRandomFloat(0.0f,1.0f) * (-10.0f);
	attribute->_velocity.z = 0.0f;

	attribute->_color = WHITE;						//设定雪花的颜色（实际上雪花的颜色是纹理的颜色）
}
void SNOW::update(float time)				//对所有雪花进行更新，死了的雪花则将其复活
{
	list<ATTRIBUTE>::iterator i;
	for(i = _particles.begin();i != _particles.end();i++)
	{
		i->_position += i->_velocity * time;			//速度乘以时间等于位移
		if(_box.IsPointInside(i->_position) == false)
			reset_particle(&(*i));
	}
}


/*------------------------------------------------------------
fire system
焰火系统
-------------------------------------------------------------*/

FIRE::FIRE(D3DXVECTOR3 *origin,int num_particles)
{
	_origin				= *origin;
	_size				= 0.9f;
	_vertexbuf_size     = 2048;
	_vertexbuf_offset   = 0;   
	_vertexbuf_num		= 512; 
	for(int i = 0; i < num_particles; i++)
		add_particle();	
}

/*
1. 将粒子复活并将其置入粒子源处
2. 获取任意方向，大小在规定范围内的速度，并将其规范化，以便绘制出球形区域
3. 设置粒子的颜色为随机颜色
4. 设置粒子的生命周期为2s
*/
void FIRE::reset_particle(ATTRIBUTE *attribute)
{
	attribute->_alive  = true;
	attribute->_position = _origin;

	D3DXVECTOR3 min = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
	D3DXVECTOR3 max = D3DXVECTOR3( 1.0f,  1.0f,  1.0f);
	//获取任意方向的速度
	GetRandomVector(
		&attribute->_velocity,
		&min,
		&max);

	//将速度规范化才能绘制出球形
	D3DXVec3Normalize(
		&attribute->_velocity,
		&attribute->_velocity);

	attribute->_velocity *= 100.0f;
	//将粒子设置为一个随机的颜色
	attribute->_color = D3DXCOLOR(
		GetRandomFloat(0.0f, 1.0f),
		GetRandomFloat(0.0f, 1.0f),
		GetRandomFloat(0.0f, 1.0f),
		1.0f);

	attribute->_age      = 0.0f;
	attribute->_life_time = 2.0f; // lives for 2 seconds
}
/* 更新所有的粒子
1. 逐个访问所有粒子
2. 假如粒子是存活的，根据时间移动粒子
3. 假如粒子已经超过了生命期，杀死粒子
*/
void FIRE::update(float time)
{
	list<ATTRIBUTE>::iterator i;
	for(i = _particles.begin(); i != _particles.end(); i++)
	{
		// 只更新存活的粒子only update living particles
		if( i->_alive )
		{
			i->_position += i->_velocity * time;
			i->_age += time;
			if(i->_age > i->_life_time) // 杀死生命结束的粒子kill 
				i->_alive = false;
		}
	}
}
void FIRE::prepare_render()
{
	//父类中prepare_render函数
	POINTSYSTEM::prepare_render();
	_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);		//目标因子与源因子像素相加
    _device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	// read, but don't write particles to z-buffer
	//假如写入了深度缓存，当粒子接近摄像机的时候，很多场景都会被抹杀
	_device->SetRenderState(D3DRS_ZWRITEENABLE, false);
}
void FIRE::post_render()
{
	POINTSYSTEM::post_render();
	_device->SetRenderState(D3DRS_ZWRITEENABLE, true);
}

/**------------------------------------------------------
粒子抢
--------------------------------------------------------*/
GUN::GUN(CAMERA* camera)
{
	_camera				= camera;
	_size				= 8.0f;
	_vertexbuf_size     = 2048;
	_vertexbuf_offset   = 0;  
	_vertexbuf_num		= 512; 
}
/* 
1.设置粒子枪的位置
2.设置粒子枪的速度
3.设置粒子枪的颜色与生命
*/
void GUN::reset_particle(ATTRIBUTE* attribute)
{
	attribute->_alive  = true;
	D3DXVECTOR3 cameraPos;
	cameraPos = _camera->Position();
	D3DXVECTOR3 cameraDir;
	cameraDir = _camera->Look();
	// change to camera position
	attribute->_position = cameraPos;
	attribute->_position.y -= 10.0f;	// 粒子的位置比摄像机的位置略低	slightly below camera
										// 这样看起来才像我们带着一把枪	so its like we're carrying a gun
	// travels in the direction the camera is looking
	attribute->_velocity = cameraDir * 100.0f;
	// green
	attribute->_color = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	attribute->_age      = 0.0f; 
	attribute->_life_time = 1.0f; // lives for 1 seconds
}

/*更新所有粒子
1.遍历整个粒子容器，更新所有粒子的位置以及生命
2.若粒子已经超过生命期，将其杀死
3.将已经死亡的所有粒子进行更新
*/
void GUN::update(float time)
{
	list<ATTRIBUTE>::iterator i;
	for(i = _particles.begin(); i != _particles.end(); i++)
	{
		i->_position += i->_velocity * time;
		i->_age += time;

		if(i->_age > i->_life_time) // kill 
			i->_alive = false;
	}
	remove_dead_particles();
}