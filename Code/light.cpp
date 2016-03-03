# include "global_value.h"
# include "definition.h"



//设置了光源，没有设置顶点法线，

void light(int swi)
{
	g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE,TRUE);			//开启见面反射
	D3DLIGHT9 light;
	ZeroMemory(&light,sizeof(light));
	D3DXVECTOR3 X(100.0,0.0,0.0),_X(-100.0,0.0,0.0);
	D3DXVECTOR3 Y(0.0,100.0,0.0),_Y(0.0,-100.0,0.0);
	D3DXVECTOR3 Z(0.0,0.0,100.0),_Z(0.0,0.0,-100.0);
	if(swi <= 3)
	{
		if(swi == 1)														
		{
			light.Type = D3DLIGHT_POINT;									//开启点光源
			light.Ambient = D3DXCOLOR(0.8f,0.8f,0.8f,1.0f);					//镜面反射光强度
			light.Diffuse = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);					//漫反射光强度	
			light.Specular = D3DXCOLOR(0.3f,0.3f,0.3f,1.0f);				//镜面反射光强度
			light.Position = 3 * X + Z;										//光源位置
			light.Attenuation0 = 0.10f;										//衰减系数
			light.Attenuation1 = 0.0f;										
			light.Attenuation2 = 0.0f;
			light.Range = 300000.0f;											//光照范围
		}
		else if(swi == 2)													
		{
			light.Type =	D3DLIGHT_DIRECTIONAL;							//开启方向光,没有衰减系数，没有光源位置，也没有光照范围
			light.Ambient = D3DXCOLOR(0.8f,0.8f,0.8f,1.0f);					//镜面反射光强度
			light.Diffuse = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);					//漫反射光强度	
			light.Specular = D3DXCOLOR(0.3f,0.3f,0.3f,1.0f);				//镜面反射光强度
			light.Direction = -3 * X - 3 * Z;
		}
		else if(swi == 3)													//开启聚光灯
		{
			light.Type =	D3DLIGHT_SPOT;									//开启点光源
			light.Ambient = D3DXCOLOR(0.8f,0.8f,0.8f,1.0f);					//镜面反射光强度
			light.Diffuse = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);					//漫反射光强度	
			light.Specular = D3DXCOLOR(0.3f,0.3f,0.3f,1.0f);				//镜面反射光强度
			light.Position = -Y * 3;												//光源位置
			light.Direction = Y;											//光源方向
			light.Attenuation0 = 1.0f;										//衰减系数
			light.Attenuation1 = 0.0f;										
			light.Attenuation2 = 0.0f;
			light.Range = 300000.0f;										//光照范围
			light.Falloff = 0.1f;											//辐射系数
			light.Phi = D3DX_PI / 2.0f;										//外锥体大小
			light.Theta = D3DX_PI / 4.0f;									//内椎体大小
		}
		g_pd3dDevice->SetRenderState(D3DRS_LIGHTING,true);					//开启光照渲染模式
		g_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS,true);			//
		g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE,true);			//
		g_pd3dDevice->SetLight(0,&light);									//设置光源
		g_pd3dDevice->LightEnable(0,true);									//启用光源
		g_pd3dDevice->SetRenderState(D3DRS_AMBIENT,D3DCOLOR_XRGB(50,50,50));
	}
}