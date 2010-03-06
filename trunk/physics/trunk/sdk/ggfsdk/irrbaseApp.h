#pragma once

namespace ggf
{
	namespace irr_base
	{
class IBaseApp 
{
public:
	irr::IrrlichtDevice *m_pDevice;

	HINSTANCE m_hInstance;
	std::string m_strClassName;
	HWND m_hWnd;
	LPDIRECT3D9             m_pD3D;//       = NULL; // Used to create the D3DDevice
	LPDIRECT3DDEVICE9       m_pd3dDevice;// = NULL; // Our rendering device
	LPDIRECT3DVERTEXBUFFER9 m_pVB;//        = NULL; // Buffer to hold Vertices

	virtual int Init(LPCWSTR lpclassName)=0;
	virtual bool Apply(float fElapsedTime)=0;
	virtual void Render(float fElapsedTime)=0;
	virtual void Close()=0;
};
	}
}