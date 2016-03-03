# ifndef _ALLOCATEHIERARCHY_H
# include "head.h"
//-----------------------------------------------------------------------------
// Desc: �̳���DXDXFRAME�ṹ�Ľṹ
//-----------------------------------------------------------------------------
struct D3DXFRAME_DERIVED: public D3DXFRAME
{
    D3DXMATRIXA16 CombinedTransformationMatrix;
};

//-----------------------------------------------------------------------------
// Desc: �̳���D3DXMESHCONTAINER�ṹ�Ľṹ
//-----------------------------------------------------------------------------
struct D3DXMESHCONTAINER_DERIVED: public D3DXMESHCONTAINER
{
    LPDIRECT3DTEXTURE9*  ppTextures;            //��������
    LPD3DXMESH           pOrigMesh;             //ԭʼ����
	DWORD                NumInfl;               //ÿ����������ܶ��ٹ�����Ӱ��
    DWORD                NumAttributeGroups;    //����������,������������
    LPD3DXBUFFER         pBoneCombinationBuf;   //������ϱ�
    D3DXMATRIX**         ppBoneMatrixPtrs;      //��Ź�������ϱ任����
    D3DXMATRIX*          pBoneOffsetMatrices;   //��Ź����ĳ�ʼ�任����
	DWORD                NumPaletteEntries;     //������������
	bool                 UseSoftwareVP;         //��ʶ�Ƿ�ʹ��������㴦��
};


//-----------------------------------------------------------------------------
// Desc: ����������.X�ļ����ؿ�ܲ�κ�����ģ������
//-----------------------------------------------------------------------------
class CAllocateHierarchy: public ID3DXAllocateHierarchy
{
private:
	HRESULT GenerateSkinnedMesh(D3DXMESHCONTAINER_DERIVED *pMeshContainer);
	HRESULT AllocateName( LPCSTR Name, LPSTR *pNewName );
	void    RemovePathFromFileName(LPSTR fullPath, LPWSTR fileName);
	wchar_t						*name_x;
public:
	STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame);
	STDMETHOD(CreateMeshContainer)( THIS_ LPCSTR              Name, 
                                    CONST D3DXMESHDATA*       pMeshData,
                                    CONST D3DXMATERIAL*       pMaterials, 
                                    CONST D3DXEFFECTINSTANCE* pEffectInstances, 
                                    DWORD                     NumMaterials, 
                                    CONST DWORD *             pAdjacency, 
                                    LPD3DXSKININFO pSkinInfo, 
                                    LPD3DXMESHCONTAINER *ppNewMeshContainer);    
    STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);
    STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase);

	friend class CSkinMesh;
};
# endif