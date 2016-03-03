//=============================================================================
// SkinMesh.h: 蒙皮网格模型类的定义
//=============================================================================

# ifndef _SKINMESH_H
# define _SKINMESH_H
#include "AllocateHierarchy.h"


class CSkinMesh
{
private:
	CAllocateHierarchy*         m_pAlloc;
	LPDIRECT3DDEVICE9			m_pd3dDevice;
	LPD3DXFRAME					m_pFrameRoot;
	D3DXMATRIXA16*              m_pBoneMatrices;
	UINT                        m_NumBoneMatricesMax;
	BOUNDINGSPHERE				sphere;
public:
	D3DXVECTOR3   m_vObjectCenter;
	float         m_fObjectRadius;
	BOOL		  m_bPlayAnim;
	LPD3DXANIMATIONCONTROLLER	m_pAnimController;		//动画控制相关变量
	LPD3DXANIMATIONSET			m_pAnimationset;			//
private:
	HRESULT SetupBoneMatrixPointers( LPD3DXFRAME pFrame );
	HRESULT SetupBoneMatrixPointersOnMesh( LPD3DXMESHCONTAINER pMeshContainerBase );

	HRESULT LoadFromXFile(WCHAR* strFileName);
	VOID    UpdateFrameMatrices( LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix ); 

	VOID DrawFrame(LPD3DXFRAME pFrame);
	VOID DrawMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase);


public:
	HRESULT OnCreate(LPDIRECT3DDEVICE9 pD3DDevice, WCHAR* strFileName);
	HRESULT Render(D3DXMATRIXA16* matWorld, double fElapsedAppTime);
	HRESULT OnDestory();
	void set_animation(char *);				//自定义函数，用于设置骨骼动画
public:
	CSkinMesh();
	virtual ~CSkinMesh();
};

# endif


