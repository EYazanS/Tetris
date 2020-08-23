#include<Windows.h>
#include<iostream>
#include <chrono>
#include <thread>
#include <vector>

void CreateAssets();
int Rotate(int pieceX, int pieceY, int degree);
void DrawField(wchar_t* screen);
bool DoesPieceIft(int pieceId, int rotation, int poxX, int posY);

using namespace std;

wstring Pieces[7];

int FieldWidth = 12, FieldHeight = 18;
int ScreenWidth = 120, ScreenHeight = 30;
unsigned char* Field = nullptr;
bool Keys[4];

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

	int screenBufferSize = ScreenWidth * ScreenHeight;

	wchar_t* screen = new wchar_t[screenBufferSize];

	for (int i = 0; i < screenBufferSize; i++)
		screen[i] = L' ';

	HANDLE consoleHandle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, NULL, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	SetConsoleActiveScreenBuffer(consoleHandle);

	DWORD bytesWritten = 0;

	bool isRunning = true;

	int currentPiece = 0;
	int currentRotation = 0;
	int currentX = (FieldWidth / 2) - 2;
	int currentY = 0;

	bool rotateButtonHeld = false;

	int gameSpeed = 20;
	int speedCount = 0;
	bool forceDown = false;
	vector<int> lines;
	int score = 0;

	while (isRunning)
	{
		// Game Timing
		this_thread::sleep_for(50ms);
		speedCount++;
		forceDown = (speedCount == gameSpeed);

		// Input
		for (size_t keyIndex = 0; keyIndex < 4; keyIndex++)
		{
			// Right arrow virtual key code // Left arrow virtual key code // Down arrow virtual key code // Z virtual key code key
			Keys[keyIndex] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[keyIndex]))) != 0;
		}

		// LOGIC

		// Can move right?
		currentX += (Keys[0] && DoesPieceIft(currentPiece, currentRotation, currentX + 1, currentY));

		// Can move left?
		currentX -= (Keys[1] && DoesPieceIft(currentPiece, currentRotation, currentX - 1, currentY));

		// Can move down?
		currentY += Keys[2] && DoesPieceIft(currentPiece, currentRotation, currentX, currentY + 1);

		if (Keys[3])
		{
			currentRotation += !rotateButtonHeld && DoesPieceIft(currentPiece, currentRotation + 1, currentX, currentY) ? 1 : 0;
			rotateButtonHeld = true;
		}
		else
		{
			rotateButtonHeld = false;
		}

		if (forceDown)
		{
			if (DoesPieceIft(currentPiece, currentRotation, currentX, currentY + 1))
				currentY++;
			else
			{
				// Lock the current piece into the field,
				for (size_t pieceX = 0; pieceX < 4; pieceX++)
					for (size_t pieceY = 0; pieceY < 4; pieceY++)
						if (Pieces[currentPiece][Rotate(pieceX, pieceY, currentRotation)] == L'X')
							Field[(currentY + pieceY) * FieldWidth + (currentX + pieceX)] = currentPiece + 1;

				// Check have we created full horizontal line
				for (size_t pieceY = 0; pieceY < 4; pieceY++)
					if (currentY + pieceY < FieldHeight - 1)
					{
						bool isLine = true;
						for (size_t pieceX = 1; pieceX < FieldWidth - 1; pieceX++)
							isLine &= (Field[(currentY + pieceY) * FieldWidth + pieceX]) != 0;

						if (isLine)
						{
							// Set line to equal symbol
							for (size_t pieceX = 0; pieceX < FieldWidth; pieceX++)
								Field[(currentY + pieceY) * FieldWidth + pieceX] = 8;

							lines.push_back(currentY + pieceY);
						}
					}

				score += 25;

				if ((!lines.empty()))
					score += (1 << lines.size()) * 100;

				// choose the next piece
				currentPiece = 0;
				currentRotation = 0;
				currentX = FieldWidth / 2;
				currentY = 0;
				currentPiece = rand() % 7;

				// If we cant fit next piece then its game over
				isRunning = DoesPieceIft(currentPiece, currentRotation, currentX, currentY);
			}

			speedCount = 0;
		}

		// Draw Field
		DrawField(screen);

		// Draw current piece
		for (size_t pieceX = 0; pieceX < 4; pieceX++)
		{
			for (size_t pieceY = 0; pieceY < 4; pieceY++)
			{
				if (Pieces[currentPiece][Rotate(pieceX, pieceY, currentRotation)] == L'X')
				{
					screen[(currentY + pieceY + 2) * ScreenWidth + (currentX + pieceX + 2)] = currentPiece + 65;
				}
			}
		}

		// Draw score
		swprintf_s(&screen[2 * ScreenWidth + FieldWidth + 6], 16, L"Score: %8d", score);

		// Draw finish lines
		if (!lines.empty())
		{
			// Display frame
			WriteConsoleOutputCharacter(consoleHandle, screen, screenBufferSize, { 0, 0 }, &bytesWritten);
			this_thread::sleep_for(400ms);

			for (auto& line : lines)
			{
				for (size_t pieceX = 1; pieceX < FieldWidth - 1; pieceX++)
				{
					for (size_t pieceY = line; pieceY > 0; pieceY--)
						Field[pieceY * FieldWidth + pieceX] = Field[(pieceY - 1) * FieldWidth + pieceX];

					Field[pieceX] = 0;
				}
			}

			lines.clear();
		}

		WriteConsoleOutputCharacter(consoleHandle, screen, screenBufferSize, { 0, 0 }, &bytesWritten);
	}

	CloseHandle(consoleHandle);

	cout << "Game over!! :(, your score: " << score << endl;

	return 0;
}

void DrawField(wchar_t* screen)
{
	for (int x = 0; x < FieldWidth; x++)
		for (int y = 0; y < FieldHeight; y++)
			screen[(y + 2) * ScreenWidth + (x + 2)] = L" ABCDEFG=#"[Field[y * FieldWidth + x]];
}

bool DoesPieceIft(int pieceId, int rotation, int posX, int posY)
{
	for (size_t x = 0; x < 4; x++)
		for (size_t y = 0; y < 4; y++)
		{
			// Get piece index
			int pieceIndex = Rotate(x, y, rotation);

			// Get index into field
			int fieldIndex = (posY + y) * FieldWidth + (posX + x);

			if (posX + x >= 0 && posX + x < FieldWidth)
			{
				if (posY + y >= 0 && posY + y < FieldHeight)
				{
					if (Pieces[pieceId][pieceIndex] == L'X' && Field[fieldIndex] != 0)
					{
						return false;
					}
				}
			}
		}

	return true;
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
		case 3: return 3 - pieceY + (4 * pieceX);		// 270 Degrees
		default: return 0;
	}
}