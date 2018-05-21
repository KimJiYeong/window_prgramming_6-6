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
//전역 함수 모음----
static BOOL Start, Shape, SelTile,SelImage, Max;
static int size_X , size_Y, tile_X , tile_Y, select_tile, select_image, select_back;
static int Sh_count = 0;
HWND hWnd2;
static BOOL Sel;
//마우스 관련 x, y
static POINT mouse, move;
//-----------



int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow) //h의 의미? = 핸들
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hinst = hinstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //배경색
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW); //프로그램 내에서 보이는 커서
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION); //아이콘 
	WndClass.hInstance = hinstance; //현재 실행되고 있는 객체의 핸들
	WndClass.lpfnWndProc = WndProc; //프로시저함수의 이름 
	WndClass.lpszClassName = lpszClass; //윈도우 클래스 내용의 이름 
	WndClass.lpszMenuName = NULL; // 메뉴바 이름
	WndClass.style = CS_HREDRAW | CS_VREDRAW; //윈도우 출력 스타일
											  // 여기까지 WNDCLASS구조체의 변수들에 값을 대입

	RegisterClass(&WndClass); // 윈도우 클래스를 운영체제에 등록

	hWnd = CreateWindow(lpszClass, "Windows Program 1-1", WS_OVERLAPPEDWINDOW, 10, 10, 1000, 1000, NULL, (HMENU)NULL, hinstance, NULL);
	//CreateWindow(클래스 이름,타이틀 이름,윈도우 스타일, 윈도우 위치좌표x , y,윈도우 가로크기, 윈도우 세로크기, 부모 윈도우 핸들, 메뉴 핸들, 응용프로그램 인스턴스, 생성 윈도우 정보(NULL))
	ShowWindow(hWnd, nCmdShow); //윈도우의 화면 출력
	UpdateWindow(hWnd); //OS에 WM_PAINT메시지 전송
	while (GetMessage(&Message, NULL, 0, 0))
	{
		//윈도우 프로시저에서 PostQuitMessage()를 호출할때 종료됨.
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}//메시지루프를 돌림.
	return (int)Message.wParam;
}
//마우스로 
struct PutPos
{
	RECT re;
	POINT pt;
	int install = 7;
	BOOL Dreaw; // 그리는지 안그리는지 판단한다.
};

//윈도우 비트맵 전역함수
static BOOL DB_image;
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) //CALLBACK(윈도우 핸들, 처리해야 할 메시지의 값,입력장치의 값1,2)
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
	switch (iMessage) //메시지의 번호
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
		}//모덜리스 처리
		
		//비트맵 이미지 불러옵니다.
		//-타일 이미지를 불러온다.
		for (int i = 0; i < 6; i++) {
			install_tile[i] = LoadBitmap(g_hinst, MAKEINTRESOURCE(103+i));
		}
		//-이미지를 불러온다.
		for (int i = 0; i < 2; i++) {
			install_image[i] = LoadBitmap(g_hinst, MAKEINTRESOURCE(110+i));
		}
		//-배경이미지를 불러온다.
		for (int i = 0; i < 2; i++) {
			install_back[i] = LoadBitmap(g_hinst, MAKEINTRESOURCE(112 + i));
		}
		
		DB_image = FALSE;
		break;


	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		
		//그리는과 동시에 이 좌표값을 할당받는 사각형들도 그려주자~~
		if (Start) {
			//배경 이미지 삽입
			memdc = CreateCompatibleDC(hdc);
			SelectObject(memdc, install_back[select_back]);
			StretchBlt(hdc, 0, 0, size_X, size_Y, memdc, 0, 0, 200, 200, SRCCOPY);

		
			//근데 start일때만 그려져야 하는거 잊지 말자!
			//격자 칸 그리기==================
			for (int i = 0; i < tile_X; i++) {
				MoveToEx(hdc, (size_X / tile_X) * i, 0, NULL);
				LineTo(hdc, (size_X / tile_X) * i, size_Y);
			}//세로 줄
			for (int j = 0; j <tile_Y; j++) {
				MoveToEx(hdc, 0, (size_Y / tile_Y) * j, NULL);
				LineTo(hdc, size_X, (size_Y / tile_Y) * j);
			}//가로 줄
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
		

			//선택된걸 표시해준다.
			if (Sel) {
			//테스트용 사각형 출력
				
			//콤보박스
				

				//타일 이미지 삽입
				//memdc = CreateCompatibleDC(hdc);
				save[sel_x][sel_y].install = select_tile;
				SelectObject(memdc, install_tile[select_tile]);
				StretchBlt(hdc, put[sel_x][sel_y].re.left, put[sel_x][sel_y].re.top, (size_X/tile_X), (size_Y / tile_Y), memdc, 0, 0, 50, 50, SRCCOPY);
				//여기서 문제! 타일 올려놓고 저장하도록 만들어야해~~!!
				
				//save 된것만 되게 만들어보자~~!! 얏호 성공이다아
				for (int i = 0; i < tile_Y; i++) {
					for (int j = 0; j <tile_X; j++) {
						if (save[i][j].install != 7) {
							SelectObject(memdc, install_tile[save[i][j].install]);
							StretchBlt(hdc, save[i][j].re.left, save[i][j].re.top, (size_X / tile_X), (size_Y / tile_Y), memdc, 0, 0, 50, 50, SRCCOPY);
						}
					}
				}

					//타일 이미지 삽입
					/*memdc2 = CreateCompatibleDC(hdc);*/
			//자 아이디어를 생각해보자 으아악 
			//이게 만약 덮어지면

			 save_image[sel_x][sel_y].install = select_image;
			if (save_image[sel_x][sel_y].install == 0) { //0놓으려는 위치가 0이면
				if (!DB_image) {
						SelectObject(memdc, install_image[select_image]);
						TransparentBlt(hdc, image[sel_x][sel_y].re.left, image[sel_x][sel_y].re.top, (size_X / tile_X), (size_Y / tile_Y), memdc, 0, 0, 150, 150, RGB(0, 0, 255));
						//여기서 문제! 타일 올려놓고 저장하도록 만들어야해~~!!
					}
				}
			else {
				SelectObject(memdc, install_image[select_image]);
				TransparentBlt(hdc, image[sel_x][sel_y].re.left, image[sel_x][sel_y].re.top, (size_X / tile_X), (size_Y / tile_Y), memdc, 0, 0, 150, 150, RGB(0, 0, 255));

			}
				//분기를 나눠준다.



					//save 된것만 되게 만들어보자~~!! 얏호 성공이다아
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
	//			//만약 마우스 놓인곳과 mouse가 맞으면
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
	//마우스로 클릭하기 위해서 생각해 보자~~
	//선택됬다는걸 좌표로 표현해 줄까?? 그럼 빨간 테두리를 그려보자!
	//마우스로 클릭한 좌표를 받아온다.
		
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
		
					//만약 마우스 놓인곳과 mouse가 맞으면
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
	//case에서 정의되지 않은 메시지는 커널이 처리하도록 메시지 전달
}
//이것도 나름 윈도우 창
BOOL CALLBACK Dlg_1Proc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	HWND hButton;
	PAINTSTRUCT d_ps;
	HBRUSH hBrush, oldBrush;
	//타일 비트맵
	HBITMAP hTile1, hTile2, hTile3, hTile4, hTile5, hTile6;
	HWND hTile_Bt1, hTile_Bt2, hTile_Bt3, hTile_Bt4, hTile_Bt5, hTile_Bt6;
	//콤보박스
	static HWND hCombo;
	static char back[][10] = { "number_1","number_2" };
	//이미지 비트맵
	HBITMAP hImage1, hImage2;
	HWND hImage_Bt1, hImage_Bt2;

	static int posX, posY, plusX;
	switch (iMessage)
	{
	case WM_INITDIALOG:
		//윈도우의 create랑 같은 원리
		Start = FALSE;
		size_X = 0; tile_X = 0;
		size_Y = 0; tile_Y = 0;

		//버튼에 이미지 입히기
		//버튼----------------------
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

		//이미지
		hImage1 = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_CHAR1));
		hImage_Bt1 = GetDlgItem(hDlg, IDC_Image1);
		SendMessage(hImage_Bt1, BM_SETIMAGE, 0, (LPARAM)hImage1);

		hImage2 = LoadBitmap(g_hinst, MAKEINTRESOURCE(IDB_CHAR2));
		hImage_Bt2 = GetDlgItem(hDlg, IDC_Image2);
		SendMessage(hImage_Bt2, BM_SETIMAGE, 0, (LPARAM)hImage2);

		//콤보박스 목록
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
		
		//---------위에서 불러온 값으로 만든다.
		case IDC_Create:
			//사이즈랑 에딧에 저장된 값을 불러옵니다.
			//사이즈랑 타일의 갯수가 저장됩니다.!
			size_X= GetDlgItemInt(hDlg, IDC_Size_Edit, NULL, TRUE);
			size_Y = GetDlgItemInt(hDlg, IDC_Size_Edit2, NULL, TRUE);
			tile_X=GetDlgItemInt(hDlg, IDC_Tile_Edit, NULL, TRUE);
			tile_Y = GetDlgItemInt(hDlg, IDC_Tile_Edit2, NULL, TRUE);
			if (tile_X > 20 || size_X > 1001) {
				//타일의 갯수가 지나치게 많거나 사이즈를 크게 주면 입력이 안되게 설정한다.
			}
			else {
				WndProc(hDlg, wParam, InvalidateRect(NULL, NULL, TRUE), lParam);
				Start = TRUE;

			}
			break;

		//타일 선택을 판단해줍니다.
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

		//콤보
		case IDC_Background_combo:
			if (HIWORD(wParam) == CBN_SELCHANGE)
				select_back = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
			break;

		//시스템 메뉴
		case IDCANCEL:
			DestroyWindow(hDlg);
			hDlg = NULL;
			break;
		}

	}
	
	return 0;
}


