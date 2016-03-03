#ifndef _CAMERA_H
# define _CAMERA_H

# include "head.h"

class CAMERA
{
protected:
	D3DXVECTOR3 position;					//�������λ��
	D3DXVECTOR3 look;						//�۲����
	D3DXVECTOR3 right;						//�ҷ���
	D3DXVECTOR3 up;							//�Ϸ���
	D3DXVECTOR3 target;						//Ŀ��۲��
	D3DXMATRIX  proj;						//ͶӰ�任		projector��ͶӰ�ǣ�
	D3DXMATRIX  view;						//ȡ���任
	LPDIRECT3DDEVICE9 device;				//COM�ӿ�

public:
	//���캯��
	CAMERA(LPDIRECT3DDEVICE9 _device);
	//���������λ���Լ��۲�㣬ѡ���Ե���
	void set_camera(const D3DXVECTOR3 &position,const D3DXVECTOR3 &lookat);
	//������������ƽ��
	VOID TranslationLook(FLOAT units);
	VOID TranslationUp(FLOAT units);
	VOID TranslationRight(FLOAT units);
	//��������������ƽ��
	VOID TranslationX(FLOAT units);
	VOID TranslationY(FLOAT units);
	VOID TranslationZ(FLOAT units);
	//��������������ת
	VOID RotationLook(FLOAT angles);
	VOID RotationUp(FLOAT angles);
	VOID RotationRight(FLOAT angles);
	//����������ת
	VOID RotationTargetLook(FLOAT angles);
	VOID RotationTargetRight(FLOAT angles);
	VOID RotationTargetUp(FLOAT angles);
	//������������ת
	VOID RotationX(FLOAT angles);
	VOID RotationY(FLOAT angles);
	VOID RotationZ(FLOAT angles);
	//����ȡ���任
	D3DXMATRIX &GetView();
	VOID View();
	VOID View(D3DXMATRIX &viewport);		//Ϊʵ��
	//����ͶӰ�任
	//��������
	//��ȡ�����λ���Լ�����
	D3DXVECTOR3 Position();
	D3DXVECTOR3 Look();
};

#endif