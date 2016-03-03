# include "head.h"
const DWORD PARTICLE::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
BOUNDINGBOX::BOUNDINGBOX(D3DXVECTOR3 &MAX)
{
	_max.x = MAX.x;
	_max.y = MAX.y;
	_max.z = MAX.z;

	_min.x = -MAX.x;
	_min.y = -MAX.y;
	_min.z = -MAX.z;
}
bool BOUNDINGBOX::IsPointInside(D3DXVECTOR3 &position)				//传入物体的地址
{
	if( position.x >= _min.x && position.y >= _min.y && position.z >= _min.z &&
		position.x <= _max.x && position.y <= _max.y && position.z <= _max.z )
		return true;
	else
		return false;
}
//将float存储的内容原封不动转换为DWORD
DWORD FtoDw(float f)
{
	return *((DWORD*)&f);
}
//产生一个在规定范围的随机数
//使用之前需要用srand()函数播下种子
float GetRandomFloat(float lowbound,float highbound)
{
	if(lowbound >= highbound)		//小的数比大的数还大，尼玛。。。
		return lowbound;

	float f = (rand() % 10000) * 0.0001f;
return (f * (highbound - lowbound)) + lowbound;
}
//产生一个在规定范围的随机向量
void GetRandomVector(D3DXVECTOR3 *out,D3DXVECTOR3 *min,D3DXVECTOR3 *max)
{
	out->x = GetRandomFloat(min->x,max->x);
	out->y = GetRandomFloat(min->y,max->y);
	out->z = GetRandomFloat(min->z,max->z);
}
