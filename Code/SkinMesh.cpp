//=============================================================================
// SkinMesh.cpp: ��Ƥ����ģ�����ʵ��
//=============================================================================

#include "SkinMesh.h"

//--------------------------------------------------------------------
// Desc: ���캯������������  Construction/Destruction
//--------------------------------------------------------------------
CSkinMesh::CSkinMesh()
{
	m_bPlayAnim         = true;
	m_pd3dDevice        = NULL;
    m_pAnimController   = NULL;
    m_pFrameRoot        = NULL;
    m_pBoneMatrices     = NULL;
	m_pAnimationset		= NULL;
    m_NumBoneMatricesMax = 0;

	m_pAlloc = new CAllocateHierarchy();
}


//-----------------------------------------------------------------------------
// Desc: ���캯������������ 
//-----------------------------------------------------------------------------
CSkinMesh::~CSkinMesh()
{
	D3DXFrameDestroy(m_pFrameRoot, m_pAlloc);
    if(m_pAnimController)
		m_pAnimController->Release();
	delete m_pAlloc;
}
 

//-----------------------------------------------------------------------------
// Desc:������������Ƥ����ģ��
//-----------------------------------------------------------------------------
HRESULT CSkinMesh::OnCreate(LPDIRECT3DDEVICE9 pD3DDevice, WCHAR *strFileName)
{
	HRESULT hr;
	m_pd3dDevice = pD3DDevice;
	hr = LoadFromXFile(strFileName);
	if(FAILED(hr))
		return hr;
	return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: ���ļ�������Ƥ����ģ��
//-----------------------------------------------------------------------------
HRESULT CSkinMesh::LoadFromXFile(WCHAR *strFileName)
{
    HRESULT hr;
	//��¼x�ļ��������ڴ�������·����ע�����x�ļ����¼�ļ�����˳��
	m_pAlloc->name_x = new wchar_t[wcslen(strFileName) + 10];
	wcscpy(m_pAlloc->name_x,strFileName);
	//��.X�ļ����ز�ο�ܺͶ�������
	if(FAILED(D3DXLoadMeshHierarchyFromX(strFileName, D3DXMESH_MANAGED, m_pd3dDevice, 
		                            m_pAlloc, NULL, &m_pFrameRoot, &m_pAnimController)))
		MessageBox(0,L"SkinMesh.cpp -- LoadFromXFile error����1",0,0);
	
	//����������ܵ���ϱ任����
    if(FAILED(SetupBoneMatrixPointers(m_pFrameRoot)))
		MessageBox(0,L"SkinMesh.cpp -- LoadFromXFile error����2",0,0);  
	
	//�����ܶ���ı߽���
    hr = D3DXFrameCalculateBoundingSphere(m_pFrameRoot, &m_vObjectCenter, &m_fObjectRadius);
	if (FAILED(hr))
        return hr;

	return S_OK;
}


//--------------------------------------------------------------------------
// Desc: ����LoadFromXFile�е��á������Ӻ���SetupBoneMatrixPointersOnMesh()
//       ���úø������(ʵ�����Ǹ�������)����ϱ任����
// ע��: ��������ʵ��û�м����������������ϱ任����ֻ��Ϊÿ�����󿪱�����Ӧ
//       �Ĵ洢�ռ䣬�����ļ������ں���CSkinMesh::UpdateFrameMatrices()����ɵġ�
//---------------------------------------------------------------------------
HRESULT CSkinMesh::SetupBoneMatrixPointers(LPD3DXFRAME pFrame)		//�ò����״ε��ö�Ӧ��ʵ��Ϊ�����
{
    HRESULT hr;

	//ֻ�е��β�Ϊ����ܵ�ʱ��Ż���ô˺���******************************����
    if (pFrame->pMeshContainer != NULL)
    {
        hr = SetupBoneMatrixPointersOnMesh(pFrame->pMeshContainer);  //�����Ӻ���
        if (FAILED(hr))
            return hr;
    }

    if (pFrame->pFrameSibling != NULL)
    {
        hr = SetupBoneMatrixPointers(pFrame->pFrameSibling);   //�ݹ�
        if (FAILED(hr))
            return hr;
    }

    if (pFrame->pFrameFirstChild != NULL)
    {
        hr = SetupBoneMatrixPointers(pFrame->pFrameFirstChild);  //�ݹ�
        if (FAILED(hr))
            return hr;
    }

    return S_OK;
}

//����*************
//-----------------------------------------------------------------------------
// Desc: ����SetupBoneMatrixPointers()�б����ã�����ÿ����������ϱ任����
//-----------------------------------------------------------------------------
HRESULT CSkinMesh::SetupBoneMatrixPointersOnMesh(LPD3DXMESHCONTAINER pMeshContainerBase)	//ͨ������ܵ�������ô˺���
																		//�����ڱ������еĲ���ֻ�������й�
{
    UINT iBone, cBones;  // cBones��ʾ����������iBone��ʾѭ������
    D3DXFRAME_DERIVED *pFrame;

	//��ǿ��תΪ��չ��
    D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;

    // ֻ����Ƥ����ģ�Ͳ��й�������
    if (pMeshContainer->pSkinInfo != NULL)
    {
		//�õ���������
        cBones = pMeshContainer->pSkinInfo->GetNumBones();

		//����洢��������Ŀռ�,ע������ָ��Ŀռ�
        pMeshContainer->ppBoneMatrixPtrs = new D3DXMATRIX*[cBones];     
		if (pMeshContainer->ppBoneMatrixPtrs == NULL)
            return E_OUTOFMEMORY;

        for (iBone = 0; iBone < cBones; iBone++)
        {
			//ͨ��ѭ��������������ܣ�Ѱ�����й���
            pFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pFrameRoot, pMeshContainer->pSkinInfo->GetBoneName(iBone));
            if (pFrame == NULL)
                return E_FAIL;

			//����ܵ���ϱ任����ֵ����Ӧ�Ĺ����ĸ��ϱ任����
            pMeshContainer->ppBoneMatrixPtrs[iBone] = &pFrame->CombinedTransformationMatrix;
		}
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: ���¿�ܲ����ƿ��
//       (1)��m_pAnimController->AdvanceTime()����ʱ�䣬m_pAnimController��
//          ��LPD3DXANIMATIONCONTROLLER��һ������
//       (2)�ú���CSkinMesh::UpdateFrameMatrices()���¿��
//       (3)�ú���CSkinMesh::DrawFrame()���ƿ��
//-----------------------------------------------------------------------------
HRESULT CSkinMesh::Render(D3DXMATRIXA16* matWorld,double fElapsedAppTime)
{
    if( 0.0f == fElapsedAppTime ) 
        return S_OK;

	if (m_bPlayAnim && m_pAnimController != NULL)		//����ʱ�����ã�������ʱ���
		m_pAnimController->AdvanceTime(fElapsedAppTime, NULL );

	UpdateFrameMatrices(m_pFrameRoot, matWorld);		//��������������ܣ������Ӻ���
	DrawFrame(m_pFrameRoot);							//����������Ƥ�����������Ӻ���
	
	return S_OK;
}


/*-----------------------------------------------------------------------------
// Desc:���������������ϱ任����
1. ���븸�����ָ�룬��������任���󣨸�����������任����Ϊ��紫�������任���󣬼���һ�ε���ʱ��
	�����˸��������������任���������Ϊ���ĸ����Ϊ�������任��
2. ͨ����ǰ�ĳ�ʼ�任�����븸��ı任����õ���ϱ任����
3. �Ƚ��ֵܹ��������������
4. �ٴ�С�ֵܵĶ��ӿ�ʼ���ظ����Լ������ֵܵľ���任��
5. С�ֵ���ɺ��ٴӴ�����С��翪ʼ�ظ����ĸ�������ֱ��ȫ�����
//---------------------------------------------------------------------------*/
VOID CSkinMesh::UpdateFrameMatrices(LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix)
{
    D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;

	//����������任����������ϱ任�������ʼ�任�������γɵ�ǰ��������ϱ任����
    if (pParentMatrix != NULL)
        D3DXMatrixMultiply(&pFrame->CombinedTransformationMatrix, &pFrame->TransformationMatrix, pParentMatrix);
    else
        pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix;
	//�ֵܵݹ�
    if (pFrame->pFrameSibling != NULL)
    {
        UpdateFrameMatrices(pFrame->pFrameSibling, pParentMatrix);
    }
	//���ӵݹ�
    if (pFrame->pFrameFirstChild != NULL)
    {
        UpdateFrameMatrices(pFrame->pFrameFirstChild, &pFrame->CombinedTransformationMatrix);
    }
}


//-----------------------------------------------------------------------------
// Desc: ���ƿ��.
//       ����CSkinMesh::DrawMeshContainer()����һ��LPD3DXMESHCONTAINER����
//       �ı���pMeshContainer.Ȼ��ݹ����ͬһ����ܺ���һ����ܡ�
//-----------------------------------------------------------------------------
VOID CSkinMesh::DrawFrame(LPD3DXFRAME pFrame)
{
    LPD3DXMESHCONTAINER pMeshContainer;
	//��ȡ��ǰ������Ӧ������
    pMeshContainer = pFrame->pMeshContainer;
    while (pMeshContainer != NULL)
    {	
        DrawMeshContainer(pMeshContainer, pFrame);				//���Ƶ�ǰ������������
        pMeshContainer = pMeshContainer->pNextMeshContainer;	//��ȡ��һ��������
    }
	
    if (pFrame->pFrameSibling != NULL)
    {
        DrawFrame(pFrame->pFrameSibling);
    }
	
    if (pFrame->pFrameFirstChild != NULL)
    {
        DrawFrame(pFrame->pFrameFirstChild);
    }
}


//-----------------------------------------------------------------------------
// Name: DrawMeshContainer()
// Desc: Called to render a mesh in the hierarchy
//-----------------------------------------------------------------------------
VOID CSkinMesh::DrawMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase)
{
	//��ȡ��Ƥ��Ϣ
	D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;
	//��ȡ������Ϣ
	D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
	UINT iMaterial;
	UINT iAttrib;
	LPD3DXBONECOMBINATION pBoneComb;

	UINT iMatrixIndex;
	UINT iPaletteEntry;
	D3DXMATRIXA16 matTemp;

	if (pMeshContainer->pSkinInfo != NULL) //�������Ƥ����
	{
		//����Ƿ�ʹ�����������

		//�����ǰӲ����֧��, ��ʹ��������㴦��
		if (pMeshContainer->UseSoftwareVP)
			m_pd3dDevice->SetSoftwareVertexProcessing(true);

		//��������������
		if (pMeshContainer->NumInfl)
			m_pd3dDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, true);
		
		//����Ӱ�쵱ǰ����ģ�Ͷ���Ĺ�������, ������Ҫʹ�õĻ�Ͼ�����������
		//�Ѿ������Ż���ԭ������Բ鿴���Բ���
		if (pMeshContainer->NumInfl == 1)			//D3DVBF_OWEIGHTS = 256
			m_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_0WEIGHTS);
		else 
			m_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, pMeshContainer->NumInfl - 1);

		//�����������Ⱦ������Ⱦ
		//��ȡ��ϱ任�������������
		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
		for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)		//����������
		{
			//���û�Ͼ���NumPalleteEntriesΪ����������
			//��ѭ����ȡһ����������ѭ���������������ĸ�������任����
			for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
			{
				iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
				if (iMatrixIndex != UINT_MAX)
				{
					D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], 
						                 pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex] );
					m_pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( iPaletteEntry ), &matTemp );
				}
			}

			//���ò��ʺ�����
			m_pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D );
			m_pd3dDevice->SetTexture( 0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId] );
			
			//��Ⱦ������ģ��
			pMeshContainer->MeshData.pMesh->DrawSubset( iAttrib );
		}

		//�ָ�������״̬
		//��һ����Ҫ����������ϵͳ��Ĭ�����ã�����Ҳû��
		m_pd3dDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
		m_pd3dDevice->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);

		//�ָ����㴦��ģʽ
		if (pMeshContainer->UseSoftwareVP)
			m_pd3dDevice->SetSoftwareVertexProcessing(false);
	} 
	else  // ���ֻ����ͨ��������Ӳ��ʺ�ͻ�������
	{
		m_pd3dDevice->SetTransform(D3DTS_WORLD, &pFrame->CombinedTransformationMatrix);
		for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
		{
			m_pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[iMaterial].MatD3D );
			m_pd3dDevice->SetTexture( 0, pMeshContainer->ppTextures[iMaterial] );
			pMeshContainer->MeshData.pMesh->DrawSubset(iMaterial);
		}
	}
}
//----------------------------------------------------------------------------
//Desc:���ö���
//---------------------------------------------------------------------------
void CSkinMesh::set_animation(char *name)
{
	static DWORD new_track = 1,old_track = 0;
	float currtime = 0;
	//���ض�����
	m_pAnimController->GetAnimationSetByName(name,&m_pAnimationset);
	new_track = old_track == 0 ? 1 :0;	
	m_pAnimController->SetTrackAnimationSet(new_track,m_pAnimationset);

	m_pAnimController->UnkeyAllTrackEvents(old_track);		//���Trackͨ���������¼�
	m_pAnimController->UnkeyAllTrackEvents(new_track);
	m_pAnimController->KeyTrackEnable(old_track,false,currtime + 0.25);	//�ر�ԭͨ��
	m_pAnimController->SetTrackEnable(new_track,true);					//�����µ�ͨ��
	//�ı�����ͨ��������
	m_pAnimController->KeyTrackSpeed(old_track,0.0f,currtime,0.25f,D3DXTRANSITION_LINEAR);
	m_pAnimController->KeyTrackSpeed(new_track,1.0f,currtime,0.25f,D3DXTRANSITION_LINEAR);
	//�ı�����ͨ����Ȩ��
	m_pAnimController->KeyTrackWeight(old_track,0.0f,currtime,0.25f,D3DXTRANSITION_LINEAR);	
	m_pAnimController->KeyTrackWeight(new_track,1.0f,currtime,0.25f,D3DXTRANSITION_LINEAR);
	//�޸Ĺ����־
	old_track = new_track;
}

//-----------------------------------------------------------------------------
// Desc: �ͷ���Ƥ����ģ��
//-----------------------------------------------------------------------------
HRESULT CSkinMesh::OnDestory()
{
	delete this;
	return S_OK;
}
