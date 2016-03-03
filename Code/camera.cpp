# include "camera.h"


//���캯��
CAMERA::CAMERA(LPDIRECT3DDEVICE9 _device)
{
	D3DXVECTOR3 y(0,1,0);
	//����Ĭ�������λ����۲��
	position = D3DXVECTOR3(1958,2357,-3741);
	target = D3DXVECTOR3(0,0,0);
	//����������������������
	look = target - position;
	D3DXVec3Normalize(&look,&look);
	D3DXVec3Cross(&right,&y,&look);
	D3DXVec3Normalize(&right,&right);
	D3DXVec3Cross(&up,&look,&right);
	D3DXVec3Normalize(&up,&up);
	//����ȡ���任
	device = _device;
	View();
	//����ͶӰ�任
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH(&matProj,D3DX_PI / 2.0f,1.0f,1.0f,500000.0f);
	device->SetTransform(D3DTS_PROJECTION,&matProj);
}
//���������λ���Լ��۲�㣬ѡ���Ե���
void CAMERA::set_camera(const D3DXVECTOR3 &position_base,const D3DXVECTOR3 &lookat_base)
{
	position = position_base;
	target = lookat_base;
	//���������������λ�÷���
	look = target - position;
	D3DXVec3Normalize(&look,&look);
	D3DXVec3Cross(&right,&D3DXVECTOR3(0,1,0),&look);
	D3DXVec3Normalize(&right,&right);
	D3DXVec3Cross(&up,&look,&right);
	D3DXVec3Normalize(&up,&up);
}
//������������ƽ��
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

//��������������ת
VOID CAMERA::RotationLook(FLOAT angles)
{
	angles = angles / 180 * D3DX_PI;
	D3DXMATRIX mid;
	D3DXMatrixRotationAxis(&mid,&look,angles);
	D3DXVec3TransformCoord(&right,&right,&mid);
	D3DXVec3TransformCoord(&up,&up,&mid);
//�������ģ������鼮��û�иı�۲���λ�ã���û�����´���-------------------------------------------
//	D3DXVec3TransformCoord(&target,&target,&mid);
}
VOID CAMERA::RotationUp(FLOAT angles)
{
	angles = angles / 180 * D3DX_PI;
	D3DXMATRIX mid;
	D3DXMatrixRotationAxis(&mid,&up,angles);
	D3DXVec3TransformCoord(&right,&right,&mid);
	D3DXVec3TransformCoord(&look,&look,&mid);
//�������ģ������鼮��û�иı�۲���λ�ã���û�����´���----------------------------------------------
//	D3DXVec3TransformCoord(&target,&target,&mid);
}
VOID CAMERA::RotationRight(FLOAT angles)
{
	angles = angles / 180 * D3DX_PI;
	D3DXMATRIX mid;
	D3DXMatrixRotationAxis(&mid,&right,angles);
	D3DXVec3TransformCoord(&look,&look,&mid);
	D3DXVec3TransformCoord(&up,&up,&mid);
//�������ģ������鼮��û�иı�۲���λ�ã���û�����´���----------------------------------------------
//	D3DXVec3TransformCoord(&target,&target,&mid);
}
//����������ƽ��
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
//����ĳ����������������ת
VOID CAMERA::RotationTargetRight(FLOAT angles)
{
	angles = angles / 180 * D3DX_PI;
	D3DXMATRIX mid;
	D3DXMatrixRotationAxis(&mid,&right,angles);
	//��ת�����������
	D3DXVec3TransformCoord(&up,&up,&mid);
	D3DXVec3TransformCoord(&look,&look,&mid);
	//�ı��������λ��
	D3DXVec3TransformCoord(&position,&position,&mid);
}
VOID CAMERA::RotationTargetUp(FLOAT angles)
{
	angles = angles / 180 * D3DX_PI;
	D3DXMATRIX mid;
	D3DXMatrixRotationAxis(&mid,&up,angles);
	//��ת�����������
	D3DXVec3TransformCoord(&right,&right,&mid);
	D3DXVec3TransformCoord(&look,&look,&mid);
	//�ı��������λ��
	D3DXVec3TransformCoord(&position,&position,&mid);
}
VOID CAMERA::RotationTargetLook(FLOAT angles)
{
	angles = angles / 180 * D3DX_PI;
	D3DXMATRIX mid;
	D3DXMatrixRotationAxis(&mid,&look,angles);
	//��ת�����������
	D3DXVec3TransformCoord(&up,&up,&mid);
	D3DXVec3TransformCoord(&right,&right,&mid);
	//�ı��������λ��
	D3DXVec3TransformCoord(&position,&position,&mid);
}
//������������ת
VOID CAMERA::RotationX(FLOAT angles)
{
	angles = angles / 180 * D3DX_PI;
	D3DXMATRIX mid,po;
	D3DXMatrixRotationAxis(&mid,&D3DXVECTOR3(1,0,0),angles);
	D3DXMatrixRotationX(&po,angles);
	//��ת���������������
	D3DXVec3TransformNormal(&look,&look,&mid);
	D3DXVec3TransformNormal(&right,&right,&mid);
	D3DXVec3TransformNormal(&up,&up,&mid);
	D3DXVec3Normalize(&right,&right);
	D3DXVec3Normalize(&right,&right);
	D3DXVec3Normalize(&right,&right);
	//��ת�������λ��
	D3DXVec3TransformNormal(&position,&position,&po);
}
VOID CAMERA::RotationY(FLOAT angles)
{
	angles = angles / 180 * D3DX_PI;
	D3DXMATRIX mid,po;
	D3DXMatrixRotationAxis(&mid,&D3DXVECTOR3(0,1,0),angles);
	D3DXMatrixRotationY(&po,angles);
	//��ת���������������
	D3DXVec3TransformNormal(&look,&look,&mid);
	D3DXVec3TransformNormal(&right,&right,&mid);
	D3DXVec3TransformNormal(&up,&up,&mid);
	D3DXVec3Normalize(&right,&right);
	D3DXVec3Normalize(&right,&right);
	D3DXVec3Normalize(&right,&right);
	//��ת�������λ��
	D3DXVec3TransformNormal(&position,&position,&po);
}
VOID CAMERA::RotationZ(FLOAT angles)
{
	angles = angles / 180 * D3DX_PI;
	D3DXMATRIX mid,po;
	D3DXMatrixRotationAxis(&mid,&D3DXVECTOR3(0,0,1),angles);
	D3DXMatrixRotationZ(&po,angles);
	//��ת���������������
	D3DXVec3TransformNormal(&look,&look,&mid);
	D3DXVec3TransformNormal(&right,&right,&mid);
	D3DXVec3TransformNormal(&up,&up,&mid);
	D3DXVec3Normalize(&right,&right);
	D3DXVec3Normalize(&right,&right);
	D3DXVec3Normalize(&right,&right);
	//��ת�������λ��
	D3DXVec3TransformNormal(&position,&position,&po);
}
//ȡ���任
VOID CAMERA::View()
{
	//Ҫ���ò��˹��ƾ������������
    // ������ȡ���任����
	//����д��ȡ���任����ĵ�һ��,���������ĵ�x����
	
    view._11 = right.x;												// Rx
    view._12 = up.x;											// Ux
    view._13 = look.x;											// Lx
    view._14 = 0.0f;
	//����д��ȡ���任����ĵڶ��У�����������y����
    view._21 = right.y;											// Ry
    view._22 = up.y;											// Uy
    view._23 = look.y;											// Ly
    view._24 = 0.0f;
	//����д��ȡ���任����ĵ����У�����������z����
    view._31 = right.z;											// Rz
    view._32 = up.z;											// Uz
    view._33 = look.z;											// Lz
    view._34 = 0.0f;
	//����д��ȡ���任����ĵ�����
    view._41 = -D3DXVec3Dot(&right, &position);					// -P*R	���һ�еĲ�������D3DXMatrixLookAtLH�ļ��㷽ʽ�õ�
    view._42 = -D3DXVec3Dot(&up, &position);					// -P*U
    view._43 = -D3DXVec3Dot(&look, &position);					// -P*L
    view._44 = 1.0f;											//ע�����һ������Ϊ0		

	device->SetTransform(D3DTS_VIEW,&view);
}
D3DXMATRIX &CAMERA::GetView()
{
return view;
}
//��ȡ�����λ��
D3DXVECTOR3 CAMERA::Position()
{
return position;
}
//��ȡ�������look����
D3DXVECTOR3 CAMERA::Look()
{
return look;
}