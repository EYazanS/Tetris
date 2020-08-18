#include<Windows.h>
#include<iostream>

void CreateAssets();
int Rotate(int pieceX, int pieceY, int degree);
void DrawField(wchar_t* screen);

using namespace std;

wstring Pieces[7];

int FieldWidth = 12, FieldHeight = 18;
int screenWidth = 120, screenHeight = 30;
unsigned char* Field = nullptr;

int main()
{
	CreateAssets();

	Field = new unsigned char[FieldWidth * FieldHeight];

	for (int x = 0; x < FieldWidth; x++)
		for (int y = 0; y < FieldHeight; y++)
		{
			bool isLeftWall = x == 0;
			bool isRightWall = x == (FieldWidth - 1);
			bool isBottomWall = y == (FieldHeight - 1);
			int value = (isLeftWall || isRightWall || isBottomWall) ? 9 : 0;
			Field[y * FieldWidth + x] = value;
		}

	int screenBufferSize = screenWidth * screenHeight;

	wchar_t* screen = new wchar_t[screenBufferSize];

	for (int i = 0; i < screenBufferSize; i++)
		screen[i] = L' ';

	HANDLE consoleHandle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, NULL, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	SetConsoleActiveScreenBuffer(consoleHandle);

	DWORD bytesWritten = 0;

	bool isRunning = true;

	while (isRunning)
	{
		// Draw Field
		DrawField(screen);

		// Display frame
		WriteConsoleOutputCharacter(consoleHandle, screen, screenBufferSize, { 0, 0 }, &bytesWritten);
	}

	return 0;
}

void DrawField(wchar_t* screen)
{
	for (int x = 0; x < FieldWidth; x++)
		for (int y = 0; y < FieldHeight; y++)
			screen[(y + 2) * screenWidth + (x + 2)] = L" ABCDEFG=#"[Field[y * FieldWidth + x]];
}

void CreateAssets()
{
	Pieces[0].append(L"..X.");
	Pieces[0].append(L"..X.");
	Pieces[0].append(L"..X.");
	Pieces[0].append(L"..X.");

	Pieces[1].append(L".XX.");
	Pieces[1].append(L"..X.");
	Pieces[1].append(L"..X.");
	Pieces[1].append(L"....");

	Pieces[2].append(L".XX.");
	Pieces[2].append(L".X..");
	Pieces[2].append(L".X..");
	Pieces[2].append(L"....");

	Pieces[3].append(L"....");
	Pieces[3].append(L".XX.");
	Pieces[3].append(L".XX.");
	Pieces[3].append(L"....");

	Pieces[4].append(L".X..");
	Pieces[4].append(L".XX.");
	Pieces[4].append(L".X..");
	Pieces[4].append(L"....");

	Pieces[5].append(L".X..");
	Pieces[5].append(L".XX.");
	Pieces[5].append(L"..X.");
	Pieces[5].append(L"....");

	Pieces[6].append(L"..X.");
	Pieces[6].append(L".XX.");
	Pieces[6].append(L".X..");
	Pieces[6].append(L"....");
}

int Rotate(int pieceX, int pieceY, int degree)
{
	switch (degree % 4)
	{
		case 0: return pieceY * 4 + pieceX;				// 0 Degrees
		case 1: return 12 + pieceY - (4 * pieceX);		// 90 Degrees
		case 2: return 15 - (pieceY * 4) - pieceX;		// 180 Degrees
		case 3: return 3 - pieceY + (4 + pieceX);		// 270 Degrees
		default: return 0;
	}
}