#include <Windows.h>
#include <string>
#include <fstream>

using namespace std;

// Processes messages procedure
#define BACKGROUND_COLOR COLOR_WINDOW

constexpr auto WINDOW_NAME = L"Laba2";
constexpr auto PATH_TO_FILE = "D:\\Work\\ÎÑèÑÏ\\×àñòü 2\\Laba2\\InputFile.txt";
constexpr auto N = 100;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Data list (from input file)
string dataList[N];

// Size parameters
int windowWidth = 0;
int windowHeight = 0;
int panelHeight = 0;
int tableHeight = 0;

// Tables size
int rowsNum = 6;
int columnsNum = 3;

// Controls
HWND hEdtColumns;
HWND hEdtRows;
HWND hBtn;

// Get data in data list
void GetDataFromFile()
{
    ifstream file(PATH_TO_FILE);

    int i = 0;
    while (i < rowsNum * columnsNum && getline(file, dataList[i])) {
        i++;
    }
        
    file.close();
}

// Convert from string to LPWSTR
LPWSTR ConvertToLPWSTR(const std::string& convertString)
{
    // Add '0' in the string end
    LPWSTR result = new wchar_t[convertString.size() + 1];
    copy(convertString.begin(), convertString.end(), result);
    result[convertString.size()] = 0;

    return result;
}

// Draw table with data from input file
void DrawTable(HWND hWnd)
{
    PAINTSTRUCT paint;
    RECT rect;

    HDC hdc = BeginPaint(hWnd, &paint);

    tableHeight = 0;
    int columnWidth = windowWidth / columnsNum;

    // Draw first horizontal line
    MoveToEx(hdc, 0, 50, NULL);
    LineTo(hdc, windowWidth, 50);

    for (int i = 0; i < rowsNum; i++) {
        int maxHeight = 0;

        for (int j = 0; j < columnsNum; j++) {
            // Skip empty elemnts
            if (dataList[columnsNum * i + j] == "") {
                continue;
            }

            // Set parameters for drawing
            SetTextColor(hdc, RGB(0, 0, 255));
            SetRect(
                &rect, 
                j * columnWidth + 1, 
                tableHeight + 51,
                (j + 1) * columnWidth - 1, 
                windowHeight
            );

            // Draw text
            int height = DrawText(
                hdc, 
                ConvertToLPWSTR(dataList[columnsNum * i + j]),
                strlen(dataList[columnsNum * i + j].c_str()),
                &rect, 
                DT_WORDBREAK | DT_EDITCONTROL
            );

            // Save maximum clumn height
            if (height > maxHeight) {
                maxHeight = height;
            }
        }

        // Calculate current table height
        tableHeight += maxHeight;

        // Draw horizontal lines
        MoveToEx(hdc, 0, tableHeight + 50, NULL);
        LineTo(hdc, windowWidth, tableHeight + 50);
    }

    for (int i = 1; i < columnsNum; i++)
    {
        // Draw verticals lines
        MoveToEx(hdc, i * columnWidth, 50, NULL);
        LineTo(hdc, i * columnWidth, tableHeight + 50);
    }

    EndPaint(hWnd, &paint);
}

// Register a new window class
ATOM RegisterWindowClass(HINSTANCE hInstance)
{
    WNDCLASSEX windowClassEx;

    // Initialize window class fields
    windowClassEx.cbSize = sizeof(WNDCLASSEX);
    windowClassEx.hInstance = hInstance;
    windowClassEx.lpfnWndProc = WndProc;
    windowClassEx.cbClsExtra = 0;
    windowClassEx.cbWndExtra = 0;
    windowClassEx.lpszClassName = WINDOW_NAME;
    windowClassEx.style = CS_HREDRAW | CS_VREDRAW;
    windowClassEx.hIcon = LoadIcon(0, IDI_WINLOGO);
    windowClassEx.hCursor = LoadCursor(0, IDC_ARROW);
    windowClassEx.hbrBackground = CreateSolidBrush(GetSysColor(BACKGROUND_COLOR));
    windowClassEx.lpszMenuName = 0;
    windowClassEx.hIconSm = 0;

    // Register new window class
    return RegisterClassEx(& windowClassEx);
}

// Try initialize a window class instance
BOOL InitWindowInstance(HINSTANCE hInstance, int showMode)
{
    HWND hWnd;

    // Create window with user parameters
    hWnd = CreateWindow(
        WINDOW_NAME,
        WINDOW_NAME,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    // Check result
    if (!hWnd) {
        return FALSE;
    }

    // Show window
    ShowWindow(hWnd, showMode);

    return TRUE;
}

// Create control elements
BOOL CreateControls(HWND hWnd, LPARAM lParam)
{
    HINSTANCE hInst = ((LPCREATESTRUCT)lParam)->hInstance;

    // Create Edit for columns
    hEdtColumns = CreateWindow(L"edit", L"3", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT,
        10, 10, 50, 20, hWnd, 0, hInst, NULL);
    ShowWindow(hEdtColumns, SW_SHOWNORMAL);

    // Create Edit for rows
    hEdtRows = CreateWindow(L"edit", L"6", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT,
        80, 10, 50, 20, hWnd, 0, hInst, NULL);
    ShowWindow(hEdtRows, SW_SHOWNORMAL);

    // Create Button
    hBtn = CreateWindow(L"button", L"Calculate", WS_CHILD | WS_VISIBLE | WS_BORDER,
        150, 9, 120, 23, hWnd, 0, hInst, NULL);
    ShowWindow(hBtn, SW_SHOWNORMAL);

    return TRUE;
}

// Processes messages procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    // On window create
    case WM_CREATE:
        CreateControls(hWnd, lParam);
        GetDataFromFile();
        DrawTable(hWnd);
        break;
    // On window redraw
    case WM_PAINT:
    {
        DrawTable(hWnd);
        break;
    }
    // On window size change
    case WM_SIZE:
        // Get top panel height
        RECT clientRect, windowRect;
        GetClientRect(hWnd, &clientRect);
        GetWindowRect(hWnd, &windowRect);
        panelHeight = windowRect.bottom - windowRect.top - clientRect.bottom;

        // Get window height and width
        windowWidth = LOWORD(lParam);
        windowHeight = HIWORD(lParam);
        break;
    // Set minimum window height
    case WM_GETMINMAXINFO:
    {
        ((LPMINMAXINFO)lParam)->ptMinTrackSize.y = tableHeight + panelHeight + 50;
        ((LPMINMAXINFO)lParam)->ptMinTrackSize.x = 300;
        break;
    }
    // On button click
    case WM_COMMAND:
        if (lParam == (LPARAM)hBtn) {
            // Get input values
            TCHAR rows[20] = { 0 }, columns[20] = { 0 };
            GetWindowText(hEdtColumns, columns, 20);
            GetWindowText(hEdtRows, rows, 20);

            // Check input values and set them
            int tmpRows = _wtoi(rows), tmpColumns = _wtoi(columns);
            if (tmpRows > 0 && tmpRows <= 40 && tmpColumns > 0 && tmpColumns <= 40) {
                rowsNum = tmpRows;
                columnsNum = tmpColumns;
                InvalidateRect(hWnd, NULL, TRUE);
            }
        }
        break;
    // On window close
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    // Other messages
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

// Main function
int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR cmdLine, _In_ int showMode)
{
    MSG msg;

    // Register new window class
    RegisterWindowClass(hInstance);

    // Try initialize window class instance
    if (!InitWindowInstance(hInstance, showMode))
        return FALSE;

    // Main message processing cycle
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}