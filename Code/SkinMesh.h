//=============================================================================
// SkinMesh.h: ��Ƥ����ģ����Ķ���
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
	LPD3DXANIMATIONCONTROLLER	m_pAnimController;		//����������ر���
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
	void set_animation(char *);				//�Զ��庯�����������ù�������
public:
	CSkinMesh();
	virtual ~CSkinMesh();
};

# endif


