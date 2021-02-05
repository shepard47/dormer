#define COBJMACROS
#define CINTERFACE

#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

HINSTANCE inst;
WNDCLASSEX wc;
MSG e;
HWND win;
IDXGISwapChain *sc;
ID3D11Device *dev;
ID3D11DeviceContext *ctx;
ID3D11RenderTargetView *backbuf;
float col[] = { 0,0.2,0.4,1 };
ID3D11InputLayout *layout;
ID3D11VertexShader *vert;
ID3D11PixelShader *frag;
ID3D11Buffer *vbuf;

void
cleanup()
{
	IDXGISwapChain_Release(sc);
	ID3D11RenderTargetView_Release(backbuf);
	ID3D11Device_Release(dev);
	ID3D11DeviceContext_Release(ctx);
}

LRESULT CALLBACK
winproc(HWND win, UINT etype, WPARAM wp, LPARAM lp)
{
	switch(etype){
	case WM_CLOSE:
		cleanup();
		DestroyWindow(win);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
	default:
		return DefWindowProc(win, etype, wp, lp);
	}
}

void
initd3d(void)
{
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = win;
	scd.SampleDesc.Count = 4;
	scd.Windowed = 1;

	D3D11CreateDeviceAndSwapChain(0,D3D_DRIVER_TYPE_HARDWARE,0,0,0,0,D3D11_SDK_VERSION,&scd,&sc,&dev,0,&ctx);

	ID3D11Texture2D *pbackbuf;
	IDXGISwapChain_GetBuffer(sc, 0, &IID_ID3D11Texture2D, (void*)&pbackbuf);
	ID3D11Device_CreateRenderTargetView(dev, pbackbuf, 0, &backbuf);
	ID3D11Texture2D_Release(pbackbuf);
	ID3D11DeviceContext_OMSetRenderTargets(ctx, 1, &backbuf, 0);

	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = 800;
	viewport.Height = 600;
	ID3D11DeviceContext_RSSetViewports(ctx, 1, &viewport);
}

void
draw()
{
	ID3D11DeviceContext_ClearRenderTargetView(ctx, backbuf, col);
	ID3D11DeviceContext_VSSetShader(ctx, vert, 0, 0);
	ID3D11DeviceContext_PSSetShader(ctx, frag, 0, 0);

	unsigned int stride = sizeof(float) * 3;
	unsigned int offset = 0;
	ID3D11DeviceContext_IASetVertexBuffers(ctx, 0, 1, &vbuf, &stride, &offset);
	ID3D11DeviceContext_IASetPrimitiveTopology(ctx, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ID3D11DeviceContext_Draw(ctx, 3, 0);

	IDXGISwapChain_Present(sc, 0, 0);
}

void
mkprog(void)
{
	ID3D10Blob *vs, *fs;

	D3DReadFileToBlob(L"vert.cso", &vs);
	D3DReadFileToBlob(L"frag.cso", &fs);

	ID3D11Device_CreateVertexShader(dev, ID3D10Blob_GetBufferPointer(vs), ID3D10Blob_GetBufferSize(vs), 0, &vert);
	ID3D11Device_CreatePixelShader(dev, ID3D10Blob_GetBufferPointer(fs), ID3D10Blob_GetBufferSize(fs), 0, &frag);
	ID3D11DeviceContext_VSSetShader(ctx, vert, 0, 0);
	ID3D11DeviceContext_PSSetShader(ctx, frag, 0, 0);

	D3D11_INPUT_ELEMENT_DESC ied[] = {
		{"POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}/*,
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},*/
	};

	ID3D11Device_CreateInputLayout(dev, ied, 1, ID3D10Blob_GetBufferPointer(vs), ID3D10Blob_GetBufferSize(vs), &layout);
	ID3D11DeviceContext_IASetInputLayout(ctx, layout);
}

void
mktriangle()
{
	float data[] = {
	   0.0f,  0.5f,  0.0f,
	   0.5f, -0.5f,  0.0f,
	  -0.5f, -0.5f,  0.0f,
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(data);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ID3D11Device_CreateBuffer(dev, &bd, 0, &vbuf);

	D3D11_MAPPED_SUBRESOURCE ms;
	ID3D11DeviceContext_Map(ctx, vbuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	memcpy(ms.pData, data, sizeof(data));
	ID3D11DeviceContext_Unmap(ctx, vbuf, 0);
}

int
main()
{
	inst = GetModuleHandle(0);
	
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = winproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = inst;
	wc.hIcon = LoadIcon(0, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"direct";
	wc.cbSize = sizeof(WNDCLASSEX);
	RegisterClassEx(&wc);

	win = CreateWindowEx(WS_EX_CLIENTEDGE, L"direct", L"direct", WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, 0, 0, inst, 0);
	ShowWindow(win, SW_SHOW);
	UpdateWindow(win);

	initd3d();
	mkprog();
	mktriangle();

	while(1){
		if(PeekMessage(&e, NULL, 0, 0, PM_REMOVE)){
			TranslateMessage(&e);
			DispatchMessage(&e);
		}
		draw();
	}

	return e.wParam;
}
