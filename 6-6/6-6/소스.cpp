#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <atlImage.h>
#include <math.h>
#include"resource.h"
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hinst;
LPCTSTR lpszClass = TEXT("First");
BOOL CALLBACK Dlg_1Proc(HWND, UINT, WPARAM, LPARAM);
//���� �Լ� ����----
static BOOL Start, Shape, SelTile,SelImage, Max;
static int size_X , size_Y, tile_X , tile_Y, select_tile, select_image, select_back;
static int Sh_count = 0;
HWND hWnd2;
static BOOL Sel;
//���콺 ���� x, y
static POINT mouse, move;
//-----------



int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow) //h�� �ǹ�? = �ڵ�
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hinst = hinstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //����
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW); //���α׷� ������ ���̴� Ŀ��
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION); //������ 
	WndClass.hInstance = hinstance; //���� ����ǰ� �ִ� ��ü�� �ڵ�
	WndClass.lpfnWndProc = WndProc; //���ν����Լ��� �̸� 
	WndClass.lpszClassName = lpszClass; //������ Ŭ���� ������ �̸� 
	WndClass.lpszMenuName = NULL; // �޴��� �̸�
	WndClass.style = CS_HREDRAW | CS_VREDRAW; //������ ��� ��Ÿ��
											  // ������� WNDCLASS����ü�� �����鿡 ���� ����

	RegisterClass(&WndClass); // ������ Ŭ������ �ü���� ���

	hWnd = CreateWindow(lpszClass, "Windows Program 1-1", WS_OVERLAPPEDWINDOW, 10, 10, 1000, 1000, NULL, (HMENU)NULL, hinstance, NULL);
	//CreateWindow(Ŭ���� �̸�,Ÿ��Ʋ �̸�,������ ��Ÿ��, ������ ��ġ��ǥx , y,������ ����ũ��, ������ ����ũ��, �θ� ������ �ڵ�, �޴� �ڵ�, �������α׷� �ν��Ͻ�, ���� ������ ����(NULL))
	ShowWindow(hWnd, nCmdShow); //�������� ȭ�� ���
	UpdateWindow(hWnd); //OS�� WM_PAINT�޽��� ����
	while (GetMessage(&Message, NULL, 0, 0))
	{
		//������ ���ν������� PostQuitMessage()�� ȣ���Ҷ� �����.
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}//�޽��������� ����.
	return (int)Message.wParam;
}
//���콺�� 
struct PutPos
{
	RECT re;
	POINT pt;
	int install = 7;
	BOOL Dreaw; // �׸����� �ȱ׸����� �Ǵ��Ѵ�.
};

//������ ��Ʈ�� �����Լ�
static BOOL DB_image;
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) //CALLBACK(������ �ڵ�, ó���ؾ� �� �޽����� ��,�Է���ġ�� ��1,2)
{
	hWnd2 = hWnd;
	PAINTSTRUCT ps;
	HDC hdc , memdc , memdc2;
	static int id;
	HWND hButton;
	HWND hDlg = NULL;
	HBRUSH hBrush, oldBrush;
	static HBITMAP install_tile[6] , install_image[2] , install_back[2];
	static PutPos put[20][20];
	static PutPos save[20][20];

	static PutPos image[20][20];
	static PutPos save_image[20][20];
	
	static int sel_x, sel_y;
	switch (iMessage) //�޽����� ��ȣ
	{
	case WM_CREATE:
//		wnd_x = 600;
//		wnd_y = 600;
		Sel = FALSE;
		SelTile = FALSE;
		if (!IsWindow(hDlg))
		{
			hDlg = CreateDialog(g_hinst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, Dlg_1Proc);
			ShowWindow(hDlg, SW_SHOW);
		}//������� ó��
		
		//��Ʈ�� �̹��� �ҷ��ɴϴ�.
		//-Ÿ�� �̹����� �ҷ��´�.
		for (int i = 0; i < 6; i++) {
			install_tile[i] = LoadBitmap(g_hinst, MAKEINTRESOURCE(103+i));
		}
		//-�̹����� �ҷ��´�.
		for (int i = 0; i < 2; i++) {
			install_image[i] = LoadBitmap(g_hinst, MAKEINTRESOURCE(110+i));
		}
		//-����̹����� �ҷ��´�.
		for (int i = 0; i < 2; i++) {
			install_back[i] = LoadBitmap(g_hinst, MAKEINTRESOURCE(112 + i));
		}
		
		DB_image = FALSE;
		break;


	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		
		//�׸��°� ���ÿ� �� ��ǥ���� �Ҵ�޴� �簢���鵵 �׷�����~~
		if (Start) {
			//��� �̹��� ����
			memdc = CreateCompatibleDC(hdc);
			SelectObject(memdc, install_back[select_back]);
			StretchBlt(hdc, 0, 0, size_X, size_Y, memdc, 0, 0, 200, 200, SRCCOPY);

		
			//�ٵ� start�϶��� �׷����� �ϴ°� ���� ����!
			//���� ĭ �׸���==================
			for (int i = 0; i < tile_X; i++) {
				MoveToEx(hdc, (size_X / tile_X) * i, 0, NULL);
				LineTo(hdc, (size_X / tile_X) * i, size_Y);
			}//���� ��
			for (int j = 0; j <tile_Y; j++) {
				MoveToEx(hdc, 0, (size_Y / tile_Y) * j, NULL);
				LineTo(hdc, size_X, (size_Y / tile_Y) * j);
			}//���� ��
			 //==============================
			

			for (int i = 0; i < tile_Y; i++) {
			for (int j = 0; j <tile_X; j++) {	
				put[i][j].re.bottom = ((size_Y / tile_Y) * i + (size_Y / tile_Y));
				put[i][j].re.top = ((size_Y / tile_Y) * i);
				put[i][j].re.right = ((size_X / tile_X) * j + (size_X / tile_X));
				put[i][j].re.left = ((size_X / tile_X) * j);
				
				save[i][j].re.bottom = ((size_Y / tile_Y) * i + (size_Y / tile_Y));
				save[i][j].re.top = ((size_Y / tile_Y) * i);
				save[i][j].re.right = ((size_X / tile_X) * j + (size_X / tile_X));
				save[i][j].re.left = ((size_X / tile_X) * j);

				//----------------
				image[i][j].re.bottom = ((size_Y / tile_Y) * i + (size_Y / tile_Y));
				image[i][j].re.top = ((size_Y / tile_Y) * i);
				image[i][j].re.right = ((size_X / tile_X) * j + (size_X / tile_X));
				image[i][j].re.left = ((size_X / tile_X) * j);

				save_image[i][j].re.bottom = ((size_Y / tile_Y) * i + (size_Y / tile_Y));
				save_image[i][j].re.top = ((size_Y / tile_Y) * i);
				save_image[i][j].re.right = ((size_X / tile_X) * j + (size_X / tile_X));
				save_image[i][j].re.left = ((size_X / tile_X) * j);
		
			
			
			}}
			//
		

			//���õȰ� ǥ�����ش�.
			if (Sel) {
			//�׽�Ʈ�� �簢�� ���
				
			//�޺��ڽ�
				

				//Ÿ�� �̹��� ����
				//memdc = CreateCompatibleDC(hdc);
				save[sel_x][sel_y].install = select_tile;
				SelectObject(memdc, install_tile[select_tile]);
				StretchBlt(hdc, put[sel_x][sel_y].re.left, put[sel_x][sel_y].re.top, (size_X/tile_X), (size_Y / tile_Y), memdc, 0, 0, 50, 50, SRCCOPY);
				//���⼭ ����! Ÿ�� �÷����� �����ϵ��� ��������~~!!
				
				//save �Ȱ͸� �ǰ� ������~~!! ��ȣ �����̴پ�
				for (int i = 0; i < tile_Y; i++) {
					for (int j = 0; j <tile_X; j++) {
						if (save[i][j].install != 7) {
							SelectObject(memdc, install_tile[save[i][j].install]);
							StretchBlt(hdc, save[i][j].re.left, save[i][j].re.top, (size_X / tile_X), (size_Y / tile_Y), memdc, 0, 0, 50, 50, SRCCOPY);
						}
					}
				}

					//Ÿ�� �̹��� ����
					/*memdc2 = CreateCompatibleDC(hdc);*/
			//�� ���̵� �����غ��� ���ƾ� 
			//�̰� ���� ��������

			 save_image[sel_x][sel_y].install = select_image;
			if (save_image[sel_x][sel_y].install == 0) { //0�������� ��ġ�� 0�̸�
				if (!DB_image) {
						SelectObject(memdc, install_image[select_image]);
						TransparentBlt(hdc, image[sel_x][sel_y].re.left, image[sel_x][sel_y].re.top, (size_X / tile_X), (size_Y / tile_Y), memdc, 0, 0, 150, 150, RGB(0, 0, 255));
						//���⼭ ����! Ÿ�� �÷����� �����ϵ��� ��������~~!!
					}
				}
			else {
				SelectObject(memdc, install_image[select_image]);
				TransparentBlt(hdc, image[sel_x][sel_y].re.left, image[sel_x][sel_y].re.top, (size_X / tile_X), (size_Y / tile_Y), memdc, 0, 0, 150, 150, RGB(0, 0, 255));

			}
				//�б⸦ �����ش�.



					//save �Ȱ͸� �ǰ� ������~~!! ��ȣ �����̴پ�
					for (int i = 0; i < tile_Y; i++) {
						for (int j = 0; j <tile_X; j++) {
							if (save_image[i][j].install == 0 || save_image[i][j].install == 1) {
								SelectObject(memdc, install_image[save_image[i][j].install]);
								TransparentBlt(hdc, save_image[i][j].re.left, save_image[i][j].re.top, (size_X / tile_X), (size_Y / tile_Y), memdc, 0, 0, 150, 150, RGB(0, 0, 255));
							}
						}
					}
					
				
			}
			DeleteDC(memdc);

		}


		EndPaint(hWnd, &ps);
		break;
	case WM_COMMAND:
		
		break;
	//case WM_MOUSEMOVE:
	//	move.x = LOWORD(lParam);
	//	move.y = HIWORD(lParam);

	//	hdc = GetDC(hWnd);
	//	hBrush = CreateSolidBrush(RGB(255, 0, 0));
	//	oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

	//	for (int i = 0; i < tile; i++) {
	//		for (int j = 0; j <tile; j++) {
	//			//���� ���콺 ���ΰ��� mouse�� ������
	//			if (PtInRect(&put[i][j].re, move)) {
	//		
	//				Rectangle(hdc, put[i][j].re.left, put[i][j].re.top, put[i][j].re.right, put[i][j].re.bottom);
	//				SelectObject(hdc, oldBrush);
	//				DeleteObject(hBrush);

	//			}
	//			else {

	//			}
	//			
	//		}

	//	}
	//	
	//	ReleaseDC(hWnd, hdc);

	//	break;
	case WM_LBUTTONDOWN:
	//���콺�� Ŭ���ϱ� ���ؼ� ������ ����~~
	//���É�ٴ°� ��ǥ�� ǥ���� �ٱ�?? �׷� ���� �׵θ��� �׷�����!
	//���콺�� Ŭ���� ��ǥ�� �޾ƿ´�.
		
		mouse.x = LOWORD(lParam);
		mouse.y = HIWORD(lParam);
		/*
		
		   case WM_LBUTTONDOWN:
      inputX = LOWORD(lParam);
      inputY = HIWORD(lParam);
      
      for (int i = 0; i < tile; ++i) {
         for (int j = 0; j < tile; ++j) {
            if (inputX > j * (size / tile) && inputX < (j + 1) * (size / tile) && inputY > i * (size / tile) && inputY < (i + 1) * (size / tile)) {
               map[j][i].index = selectIndex;
               if(map[j][i].object != 1)
                  map[j][i].object = Object;
            }
         }
      }
      InvalidateRect(hWnd, NULL, TRUE);
      break;
		
		*/



		for (int i = 0; i < tile_Y; i++) {
			for (int j = 0; j < tile_X; j++) {
		
					//���� ���콺 ���ΰ��� mouse�� ������
					if (PtInRect(&put[i][j].re, mouse)) {
						if (DB_image) {
						sel_x = i;
						sel_y = j;
						Sel = TRUE;
						put[i][j].Dreaw = TRUE;


					}
					else {
						put[i][j].Dreaw = FALSE;
					}
				}

			}
		}
		InvalidateRect(hWnd, NULL, TRUE);

		break;
	
	
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
	//case���� ���ǵ��� ���� �޽����� Ŀ���� ó���ϵ��� �޽��� ����
}
//�̰͵� ���� ������ â
BOOL CALLBACK Dlg_1Proc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	HWND hButton;
	PAINTSTRUCT d_ps;
	HBRUSH hBrush, oldBrush;
	//Ÿ�� ��Ʈ��
	HBITMAP hTile1, hTile2, hTile3, hTile4, hTile5, hTile6;
	HWND hTile_Bt1, hTile_Bt2, hTile_Bt3, hTile_Bt4, hTile_Bt5, hTile_Bt6;
	//�޺��ڽ�
	static HWND hCombo;
	static char back[][10] = { "number_1","number_2" };
	//�̹��� ��Ʈ��
	HBITMAP hImage1, hImage2;
	HWND hImage_Bt1, hImage_Bt2;

	static int posX, posY, plusX;
	switch (iMessage)
	{
	case WM_INITDIALOG:
		//�������� create�� ���� ����
		Start = FALSE;
		size_X = 0; tile_X = 0;
		size_Y = 0; tile_Y = 0;

		//��ư�� �̹��� ������
		//��ư----------------------
		hTile1 = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_BITMAP1));
		hTile_Bt1 = GetDlgItem(hDlg, IDC_Tile1);
		SendMessage(hTile_Bt1, BM_SETIMAGE, 0, (LPARAM)hTile1);

		hTile2 = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_BITMAP2));
		hTile_Bt2 = GetDlgItem(hDlg, IDC_Tile2);
		SendMessage(hTile_Bt2, BM_SETIMAGE, 0, (LPARAM)hTile2);

		hTile3 = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_BITMAP3));
		hTile_Bt3 = GetDlgItem(hDlg, IDC_Tile3);
		SendMessage(hTile_Bt3, BM_SETIMAGE, 0, (LPARAM)hTile3);

		hTile4 = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_BITMAP4));
		hTile_Bt4 = GetDlgItem(hDlg, IDC_Tile4);
		SendMessage(hTile_Bt4, BM_SETIMAGE, 0, (LPARAM)hTile4);

		hTile5 = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_BITMAP5));
		hTile_Bt5 = GetDlgItem(hDlg, IDC_Tile5);
		SendMessage(hTile_Bt5, BM_SETIMAGE, 0, (LPARAM)hTile5);

		hTile6 = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_BITMAP6));
		hTile_Bt6 = GetDlgItem(hDlg, IDC_Tile6);
		SendMessage(hTile_Bt6, BM_SETIMAGE, 0, (LPARAM)hTile6);

		//�̹���
		hImage1 = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_CHAR1));
		hImage_Bt1 = GetDlgItem(hDlg, IDC_Image1);
		SendMessage(hImage_Bt1, BM_SETIMAGE, 0, (LPARAM)hImage1);

		hImage2 = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_CHAR2));
		hImage_Bt2 = GetDlgItem(hDlg, IDC_Image2);
		SendMessage(hImage_Bt2, BM_SETIMAGE, 0, (LPARAM)hImage2);

		//�޺��ڽ� ���
		hCombo = GetDlgItem(hDlg, IDC_Background_combo);
		//SendMessage(hCombo , CB_SETITEMHEIGH)
		for (int i = 0; i < 2; i++) {
			SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)back[i]);
		}
		//-------------------
		break;


	case WM_COMMAND:
		//hdc = GetDC(hDlg);
		switch (LOWORD(wParam)) {
		
		//---------������ �ҷ��� ������ �����.
		case IDC_Create:
			//������� ������ ����� ���� �ҷ��ɴϴ�.
			//������� Ÿ���� ������ ����˴ϴ�.!
			size_X= GetDlgItemInt(hDlg, IDC_Size_Edit, NULL, TRUE);
			size_Y = GetDlgItemInt(hDlg, IDC_Size_Edit2, NULL, TRUE);
			tile_X=GetDlgItemInt(hDlg, IDC_Tile_Edit, NULL, TRUE);
			tile_Y = GetDlgItemInt(hDlg, IDC_Tile_Edit2, NULL, TRUE);
			if (tile_X > 20 || size_X > 1001) {
				//Ÿ���� ������ ����ġ�� ���ų� ����� ũ�� �ָ� �Է��� �ȵǰ� �����Ѵ�.
			}
			else {
				WndProc(hDlg, wParam, InvalidateRect(NULL, NULL, TRUE), lParam);
				Start = TRUE;

			}
			break;

		//Ÿ�� ������ �Ǵ����ݴϴ�.
		case IDC_Tile1:
			select_tile = 0;
			SelImage = FALSE;

			break;
		case IDC_Tile2:
			select_tile = 1;
			SelImage = FALSE;

			break;
		case IDC_Tile3:
			select_tile = 2;
			SelImage = FALSE;

			break;
		case IDC_Tile4:
			select_tile = 3;
			SelImage = FALSE;

			break;
		case IDC_Tile5:
			select_tile = 4;
			SelImage = FALSE;

			break;
		case IDC_Tile6:
			select_tile = 5;
			SelImage = FALSE;

			break;
		case IDC_Image1:
			SelImage = TRUE;
			DB_image = TRUE;
			select_image = 1;
			break;
		case IDC_Image2:
			DB_image = FALSE;
			select_image = 0;

			break;

		//�޺�
		case IDC_Background_combo:
			if (HIWORD(wParam) == CBN_SELCHANGE)
				select_back = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
			break;

		//�ý��� �޴�
		case IDCANCEL:
			DestroyWindow(hDlg);
			hDlg = NULL;
			break;
		}

	}
	
	return 0;
}


