# include "snake.h"

# define XMIN_SNAKE _box._min.x
# define XMAX_SNAKE _box._max.x
# define Y_SNAKE 200
# define ZMIN_SNAKE _box._min.z
# define ZMAX_SNAKE _box._max.z
# define SCALE_SNAKE 5	
# define RO_SNAKE	0

SNAKE::SNAKE(const BOUNDINGBOX &box):
	_box(box),				//蛇活动的范围
	_product_time(2000),	//产生新的蛇所需要的时间
	_distance(300),		//碰撞检测距离
	_distance_snake(200),	//同一条蛇不同节点的距离
	_go_time(500),			//前进时间
	_go_time_cut(1),
	_velocity(200),
	_update_time(50),
	_update_cut(1)
{
	_snake_big = new SNAKE_BIG();		//
	_snake_big->_head = _snake_big->_end = NULL;
	_snake_big->_next = NULL;
	_snake_mid = NULL;
}
SNAKE::~SNAKE()
{

}
//--------------------------------------
//初始化蛇类的x文件
//--------------------------------------
void SNAKE::init(LPDIRECT3DDEVICE9 device,wchar_t *name_snake)
{
	_product = _product_time;
	D3DXMatrixScaling(&_scale,SCALE_SNAKE,SCALE_SNAKE,SCALE_SNAKE);
	MESHXFILE::init(name_snake,device);
	for(int i = 0;i < 2;i++)			//生成两条蛇
		add();
}
//------------------------------------------
//生成新的蛇头
//------------------------------------------
void SNAKE::add()						
{
	SNAKE_STRUCT *head;
	D3DXVECTOR3 Z(0,0,1),X(1,0,0);
	head = new SNAKE_STRUCT();
	head->_ishead = true;				//头
	head->_alive = true;				//存活
	head->_child = false;				//怀孕
	head->_go_time = _go_time;			//继续前进时间
	head->_before = head->_next = NULL;					
	head->_update = _update_time;

	if(GetRandomFloat(0,2) < 1)			//上方产生新的蛇
	{	head->_position.z = ZMAX_SNAKE - 2 * _distance;
		head->_position.x = GetRandomFloat(XMIN_SNAKE + 2 * _distance,XMAX_SNAKE - 2 * _distance);
		head->_position.y = Y_SNAKE;
		head->_look = -Z;
		head->_right = -X;
		/*********修改旋转分量*********/
		D3DXMatrixRotationAxis(&(head->_rotation),&D3DXVECTOR3(0,1,0),0);
	}
	else								//右侧产生新的蛇
	{	head->_position.x = XMAX_SNAKE - 2 * _distance;
		head->_position.z = GetRandomFloat(ZMIN_SNAKE + 2 * _distance,ZMAX_SNAKE - 2 * _distance);
		head->_position.y = Y_SNAKE;
		head->_look = -X;
		head->_right = Z;
		/*********修改旋转分量*********/
		D3DXMatrixRotationAxis(&(head->_rotation),&D3DXVECTOR3(0,1,0),D3DX_PI / 2);
	}
	if(_snake_mid == NULL)		//程序刚刚开始，还没有蛇
	{
		_snake_big->_head = _snake_big->_end = _snake_mid = new SNAKE_MIDDLE();
		_snake_big->_next = NULL;
		_snake_mid->_before = NULL;						//第一个节点之前没有节点
		_snake_mid->_next = NULL;
		_snake_mid->_head = _snake_mid->_end = head;
	}
	else						//在已有蛇的基础上产生新的蛇
	{	
		_snake_big->_end->_next = new SNAKE_MIDDLE();
		_snake_big->_end->_next->_before = _snake_big->_end;
		_snake_big->_end = _snake_big->_end->_next;
		_snake_big->_end->_head = _snake_big->_end->_end = head;
		_snake_big->_end->_next = NULL;
	}
}
//-----------------------------------------
//增加蛇的长度
//蛇的头标志是否增加长度，在蛇尾处添加蛇节点
//-----------------------------------------
void SNAKE::add(SNAKE_STRUCT *p,SNAKE_MIDDLE *mid)		//形参为蛇的最后一个节点
{
	mid->_end = p->_next = new SNAKE_STRUCT();
	p->_next->_before = p;						//前后节点相连
	p->_next->_next = NULL;						//封尾
	p->_next->_ishead = false;					//不是头部
	p->_next->_alive = true;					//存活
	p->_next->_look = p->_look;					//行走方向
	p->_next->_right = p->_right;				//右侧
	p->_next->_position = p->_position - p->_look * _distance_snake;	//位置
	p->_next->_rotation = p->_rotation;			//视觉效果
	p->_next->_child = mid->_head->_child = false;		//将诞生新的节点改为false
}
void SNAKE::update()
{
	_product -= _go_time_cut;
	if(_product <= 0)
	{	add();
		_product = _product_time;
	}
	if(_snake_mid != NULL)								//有蛇存在
	{
		SNAKE_MIDDLE *p_mid = _snake_mid,*p_next;
		for( ; p_mid != NULL ;)							//还有蛇存在是进行循环
		{
			p_next = p_mid->_next;						//记录当前节点的下一个节点，删除蛇的时候需要
			SNAKE_STRUCT *p = p_mid->_head;

			p->_go_time -= _go_time_cut;

			if(p->_alive == false)						//蛇已经死亡
			{	delete_snake(&p_mid);					//删除整条蛇
				p_mid = p_next;
				continue;
			}

			if(p->_child == true)						//判断蛇头是否增加长度的
				add(p_mid->_end,p_mid);					//是，在蛇尾增加长度

			if(isneed_turn(p) == true)					//判断蛇是否需要拐弯
				if(turn(p) == false)
					continue;							//蛇需要转弯，但是没能成功转弯，等待转弯的机会
			
			p->_update -= _update_cut;					//为了防止蛇行走过快，设置此变量
			if(p->_update <= 0)
			{
				if(p->_go_time <= 0)						//减少行走时间
					p->_need_turn = true;
				go_ahead(p_mid->_end);						//更新整条蛇
				p->_update = _update_time;
			}
			p_mid = p_mid->_next;						//查看下一条蛇
		}
	}
}

//---------------------------------------
//将蛇往前推移
//---------------------------------------
void SNAKE::go_ahead(SNAKE_STRUCT *end)
{
	if(end->_before == NULL)			//到达蛇头
	{
		end->_position += end->_look * _velocity;
		return ;
	}
	else
	{
		end->_position += end->_look * _velocity;
		//判断是否需要调整节点的属性信息,即蛇是否到达拐弯点
		if(D3DXVec3Length(&(end->_look - end->_before->_look)) > 
			D3DXVec3Length(&(end->_look)))
		{
			end->_look = end->_before->_look;
			end->_rotation = end->_before->_rotation;
		}
		go_ahead(end->_before);		//递归一下
	}
}
//---------------------------------------
//删除已经死亡的蛇
//---------------------------------------
void SNAKE::delete_snake(SNAKE_MIDDLE **mid)	//注意改变的量关系到三个链表
{
	SNAKE_STRUCT *p = (*mid)->_head,*p1;
	for(;p != NULL;)							//删除整条蛇的节点
	{
		p1 = p->_next;
		delete p;
		p = p1;
	}
	(*mid)->_head = NULL;

	if((*mid)->_next == NULL && (*mid)->_before == NULL)		//只有一条蛇
	{
		_snake_mid = _snake_big->_head = _snake_big->_end = NULL;
		delete *mid;
		*mid = NULL;
		return ;
	}
	else if((*mid)->_before == NULL)							//第一条蛇
	{
		_snake_big->_head = _snake_mid = (*mid)->_next;
		_snake_big->_head->_before = NULL;						//封头
		delete *mid;
		*mid = NULL;
		return ;
	}
	else if((*mid)->_next == NULL)								//最后一条蛇
	{
		_snake_big->_end = (*mid)->_before;
		_snake_big->_end->_next = NULL;							//封尾
		delete *mid;
		*mid = NULL;
		return ;
	}
	else														//普通的蛇
	{
		(*mid)->_before->_next = (*mid)->_next;
		(*mid)->_next->_before = (*mid)->_before;
		delete *mid;
		*mid = NULL;	
		return ;
	}
}
//----------------------------------------
//判断蛇是否与物体相撞
bool SNAKE::isneed_turn(SNAKE_STRUCT *head)			//传入头结点
{
	if(head->_need_turn == true)					//在上一次需要拐弯时，没有拐弯成功
		return true;
	//若蛇与四周的墙壁即将相碰
	if(((head->_position + head->_look * _distance).x > XMAX_SNAKE) || 
		((head->_position +  head->_look * _distance).x < XMIN_SNAKE) ||
		((head->_position +  head->_look * _distance).z > ZMAX_SNAKE) ||
		((head->_position +  head->_look * _distance).z < ZMIN_SNAKE) )
		return true;
	if(ishit_snake(head) == true)					//若与其它蛇相撞，直接返回true
		return true;
return false;
}
//-----------------------------------------
//判断蛇之间是否碰撞
//-----------------------------------------
bool SNAKE::ishit_snake(SNAKE_STRUCT *head)			//传入头结点
{
	//待完善
return false;
}
bool SNAKE::turn(SNAKE_STRUCT *head)					//传入头结点
{
	float a;
	int b;
	while((a = GetRandomFloat(-1,1)) == 0);
	if(a < 0) b = -2;					//判断旋转90或者-90度
	else if(a > 0) b = 2;
	D3DXMATRIX ro;
	D3DXMatrixRotationAxis(&ro,&(D3DXVECTOR3(0,1,0)),D3DX_PI / b);
	D3DXVec3TransformNormal(&(head->_look),&(head->_look),&ro);
	D3DXVec3Normalize(&(head->_look),&(head->_look));
	head->_rotation *= ro;
	head->_need_turn = false;
	if(isneed_turn(head) == true)				//旋转失败，蛇还是需要旋转
	{
		return false;
		head->_need_turn = true;
	}
	head->_go_time = _go_time;
return true;
}
//-----------------------------
//测试蛇类与人物，水果的关系
bool SNAKE::test(FRUIT_STRUCT *head_fruit,PERSON_STRUCT *head_people)
{
	test(head_people);
	test(head_fruit);
return true;					//---------------暂定返回值
}
# ifndef JUGG 1
# define JUGG 1
# endif
# ifndef SUQI 2
# define SUQI 2
# endif
//测试人物是否击杀到了贪吃蛇
bool SNAKE::test(PERSON_STRUCT *head_people)
{
	SNAKE_STRUCT *snake;
	SNAKE_MIDDLE *snake_mid;
	
	while(head_people != NULL)
	{
		snake_mid = _snake_mid;
		while(snake_mid != NULL)
		{
			snake = snake_mid->_head;
			while(snake != NULL)
			{
				//检测蛇与人的距离		
				if(head_people->_style == JUGG)
				{	
					if(D3DXVec3Length(&(snake->_position - head_people->_position))
						< 3 * _distance)		//蛇与人的距离靠近
					//蛇在剑圣之前会死亡，其后不会，检测部位是全身，死亡在蛇头标志
					if(D3DXVec3Dot(&(snake->_position - head_people->_position),
						&(head_people->_look)) > 0)
					{	
						snake_mid->_head->_alive = false;
						break;
					}
				}
				else if(head_people->_style == SUQI)
				{	
					if(D3DXVec3Length(&(snake->_position - head_people->_position))
						<  _distance)
					//蛇在苏琪附近会死亡,由于检测的部位是蛇全身，而死亡是在蛇头标志
					snake_mid->_head->_alive = false;
					break;
				}
				
				snake = snake->_next;					//查看蛇的下一个节点
			}
			snake_mid = snake_mid->_next;				//查看下一条蛇
		}
		head_people = head_people->_next;				//查看下一个人
	}
return true;				//----------------暂定返回值
}
//测试贪吃蛇是否吃到水果
bool SNAKE::test(FRUIT_STRUCT *head_fruit)
{
	SNAKE_MIDDLE *snake_mid;
	snake_mid = _snake_mid;
	while(head_fruit != NULL)				//遍历水果链表
	{
		snake_mid = _snake_mid;
		while(snake_mid != NULL)			//遍历贪吃蛇头部，因为只有头部可以吃水果
		{
			if(D3DXVec3Length(&(snake_mid->_head->_position - head_fruit->_position))
				< _distance)				//蛇头与水果距离足够
				if(D3DXVec3Dot(&(snake_mid->_head->_position - head_fruit->_position),
						&(snake_mid->_head->_look)) > 0)		//通过余弦判断方向
			{
				snake_mid->_head->_child = true;
				head_fruit->_alive = false;
			}
			snake_mid = snake_mid->_next;						//查看下一个蛇头
		}
		head_fruit = head_fruit->_next;							//查看下一个水果
	}
return true;						//----------------暂定返回值
}
void SNAKE::render()
{
	update();
	SNAKE_MIDDLE *p_mid;
	SNAKE_STRUCT *p_str;
	D3DXMATRIX	world,tran;
	for(p_mid = _snake_mid;p_mid != NULL;p_mid = p_mid->_next)
	{
		for(p_str = p_mid->_head;p_str != NULL;p_str = p_str->_next)
		{
			D3DXMatrixIdentity(&world);
			D3DXMatrixTranslation(&tran,p_str->_position.x,p_str->_position.y,p_str->_position.z);
			world = world * _scale * p_str->_rotation * tran;
			_device->SetTransform(D3DTS_WORLD,&world);
			MESHXFILE::DrawMesh();						//在不同的地点绘制出相同的蛇
		}
	}
}
