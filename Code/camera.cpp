# include "camera.h"


//构造函数
CAMERA::CAMERA(LPDIRECT3DDEVICE9 _device)
{
	D3DXVECTOR3 y(0,1,0);
	//设置默认摄像机位置与观察点
	position = D3DXVECTOR3(1958,2357,-3741);
	target = D3DXVECTOR3(0,0,0);
	//设置摄像机的三个方向分量
	look = target - position;
	D3DXVec3Normalize(&look,&look);
	D3DXVec3Cross(&right,&y,&look);
	D3DXVec3Normalize(&right,&right);
	D3DXVec3Cross(&up,&look,&right);
	D3DXVec3Normalize(&up,&up);
	//设置取景变换
	device = _device;
	View();
	//设置投影变换
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH(&matProj,D3DX_PI / 2.0f,1.0f,1.0f,500000.0f);
	device->SetTransform(D3DTS_PROJECTION,&matProj);
}
//设置摄像机位置以及观察点，选择性调用
void CAMERA::set_camera(const D3DXVECTOR3 &position_base,const D3DXVECTOR3 &lookat_base)
{
	position = position_base;
	target = lookat_base;
	//重新设置摄像机的位置分量
	look = target - position;
	D3DXVec3Normalize(&look,&look);
	D3DXVec3Cross(&right,&D3DXVECTOR3(0,1,0),&look);
	D3DXVec3Normalize(&right,&right);
	D3DXVec3Cross(&up,&look,&right);
	D3DXVec3Normalize(&up,&up);
}
//沿着三个分量平移
VOID CAMERA::TranslationLook(FLOAT units)
{
	position += look * units;
	target += look * units;
return ;
}
VOID CAMERA::TranslationUp(FLOAT units)
{
	position += up * units;
	target += up * units;
return ;
}
VOID CAMERA::TranslationRight(FLOAT units)
{
	position += right * units;
	target += right * units;
return ;
}

//沿着三个分量旋转
VOID CAMERA::RotationLook(FLOAT angles)
{
	angles = angles / 180 * D3DX_PI;
	D3DXMATRIX mid;
	D3DXMatrixRotationAxis(&mid,&look,angles);
	D3DXVec3TransformCoord(&right,&right,&mid);
	D3DXVec3TransformCoord(&up,&up,&mid);
//经过查阅，其他书籍并没有改变观察点的位置，即没有以下代码-------------------------------------------
//	D3DXVec3TransformCoord(&target,&target,&mid);
}
VOID CAMERA::RotationUp(FLOAT angles)
{
	angles = angles / 180 * D3DX_PI;
	D3DXMATRIX mid;
	D3DXMatrixRotationAxis(&mid,&up,angles);
	D3DXVec3TransformCoord(&right,&right,&mid);
	D3DXVec3TransformCoord(&look,&look,&mid);
//经过查阅，其他书籍并没有改变观察点的位置，即没有以下代码----------------------------------------------
//	D3DXVec3TransformCoord(&target,&target,&mid);
}
VOID CAMERA::RotationRight(FLOAT angles)
{
	angles = angles / 180 * D3DX_PI;
	D3DXMATRIX mid;
	D3DXMatrixRotationAxis(&mid,&right,angles);
	D3DXVec3TransformCoord(&look,&look,&mid);
	D3DXVec3TransformCoord(&up,&up,&mid);
//经过查阅，其他书籍并没有改变观察点的位置，即没有以下代码----------------------------------------------
//	D3DXVec3TransformCoord(&target,&target,&mid);
}
//沿着坐标轴平移
VOID  CAMERA::TranslationX(FLOAT units)
{
	position += D3DXVECTOR3(1,0,0) * units;
	target   += D3DXVECTOR3(1,0,0) * units;
}
VOID  CAMERA::TranslationY(FLOAT units)
{
	position += D3DXVECTOR3(0,1,0) * units;
	target   += D3DXVECTOR3(0,1,0) * units;
}
VOID  CAMERA::TranslationZ(FLOAT units)
{
	position += D3DXVECTOR3(0,0,1) * units;
	target   += D3DXVECTOR3(0,0,1) * units;
}
//沿着某个数轴绕着物体旋转
VOID CAMERA::RotationTargetRight(FLOAT angles)
{
	angles = angles / 180 * D3DX_PI;
	D3DXMATRIX mid;
	D3DXMatrixRotationAxis(&mid,&right,angles);
	//旋转三个方向分量
	D3DXVec3TransformCoord(&up,&up,&mid);
	D3DXVec3TransformCoord(&look,&look,&mid);
	//改变摄像机的位置
	D3DXVec3TransformCoord(&position,&position,&mid);
}
VOID CAMERA::RotationTargetUp(FLOAT angles)
{
	angles = angles / 180 * D3DX_PI;
	D3DXMATRIX mid;
	D3DXMatrixRotationAxis(&mid,&up,angles);
	//旋转三个方向分量
	D3DXVec3TransformCoord(&right,&right,&mid);
	D3DXVec3TransformCoord(&look,&look,&mid);
	//改变摄像机的位置
	D3DXVec3TransformCoord(&position,&position,&mid);
}
VOID CAMERA::RotationTargetLook(FLOAT angles)
{
	angles = angles / 180 * D3DX_PI;
	D3DXMATRIX mid;
	D3DXMatrixRotationAxis(&mid,&look,angles);
	//旋转三个方向分量
	D3DXVec3TransformCoord(&up,&up,&mid);
	D3DXVec3TransformCoord(&right,&right,&mid);
	//改变摄像机的位置
	D3DXVec3TransformCoord(&position,&position,&mid);
}
//绕着坐标轴旋转
VOID CAMERA::RotationX(FLOAT angles)
{
	angles = angles / 180 * D3DX_PI;
	D3DXMATRIX mid,po;
	D3DXMatrixRotationAxis(&mid,&D3DXVECTOR3(1,0,0),angles);
	D3DXMatrixRotationX(&po,angles);
	//旋转摄像机的三个分量
	D3DXVec3TransformNormal(&look,&look,&mid);
	D3DXVec3TransformNormal(&right,&right,&mid);
	D3DXVec3TransformNormal(&up,&up,&mid);
	D3DXVec3Normalize(&right,&right);
	D3DXVec3Normalize(&right,&right);
	D3DXVec3Normalize(&right,&right);
	//旋转摄像机的位置
	D3DXVec3TransformNormal(&position,&position,&po);
}
VOID CAMERA::RotationY(FLOAT angles)
{
	angles = angles / 180 * D3DX_PI;
	D3DXMATRIX mid,po;
	D3DXMatrixRotationAxis(&mid,&D3DXVECTOR3(0,1,0),angles);
	D3DXMatrixRotationY(&po,angles);
	//旋转摄像机的三个分量
	D3DXVec3TransformNormal(&look,&look,&mid);
	D3DXVec3TransformNormal(&right,&right,&mid);
	D3DXVec3TransformNormal(&up,&up,&mid);
	D3DXVec3Normalize(&right,&right);
	D3DXVec3Normalize(&right,&right);
	D3DXVec3Normalize(&right,&right);
	//旋转摄像机的位置
	D3DXVec3TransformNormal(&position,&position,&po);
}
VOID CAMERA::RotationZ(FLOAT angles)
{
	angles = angles / 180 * D3DX_PI;
	D3DXMATRIX mid,po;
	D3DXMatrixRotationAxis(&mid,&D3DXVECTOR3(0,0,1),angles);
	D3DXMatrixRotationZ(&po,angles);
	//旋转摄像机的三个分量
	D3DXVec3TransformNormal(&look,&look,&mid);
	D3DXVec3TransformNormal(&right,&right,&mid);
	D3DXVec3TransformNormal(&up,&up,&mid);
	D3DXVec3Normalize(&right,&right);
	D3DXVec3Normalize(&right,&right);
	D3DXVec3Normalize(&right,&right);
	//旋转摄像机的位置
	D3DXVec3TransformNormal(&position,&position,&po);
}
//取景变换
VOID CAMERA::View()
{
	//要是用不了估计就是你的问题了
    // 创建出取景变换矩阵
	//依次写出取景变换矩阵的第一行,各个分量的的x坐标
	
    view._11 = right.x;												// Rx
    view._12 = up.x;											// Ux
    view._13 = look.x;											// Lx
    view._14 = 0.0f;
	//依次写出取景变换矩阵的第二行，各个分量的y坐标
    view._21 = right.y;											// Ry
    view._22 = up.y;											// Uy
    view._23 = look.y;											// Ly
    view._24 = 0.0f;
	//依次写出取景变换矩阵的第三行，各个分量的z坐标
    view._31 = right.z;											// Rz
    view._32 = up.z;											// Uz
    view._33 = look.z;											// Lz
    view._34 = 0.0f;
	//依次写出取景变换矩阵的第四行
    view._41 = -D3DXVec3Dot(&right, &position);					// -P*R	最后一行的参数根据D3DXMatrixLookAtLH的计算方式得到
    view._42 = -D3DXVec3Dot(&up, &position);					// -P*U
    view._43 = -D3DXVec3Dot(&look, &position);					// -P*L
    view._44 = 1.0f;											//注意最后一个不能为0		

	device->SetTransform(D3DTS_VIEW,&view);
}
D3DXMATRIX &CAMERA::GetView()
{
return view;
}
//获取摄像机位置
D3DXVECTOR3 CAMERA::Position()
{
return position;
}
//获取摄像机的look分量
D3DXVECTOR3 CAMERA::Look()
{
return look;
}