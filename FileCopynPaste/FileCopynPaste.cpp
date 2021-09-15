// FileCopynPaste.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <WinUser.h>
#include <tchar.h>
#include <stdio.h>
#include <wincon.h>
#include <thread>
#include <atlstr.h>

#define CTRL_SHIFT_C 101
#define CTRL_SHIFT_V 102

void keyEventProc(KEY_EVENT_RECORD ker, HANDLE handle, std::string* input);
void initWindow();

int main(int argc, char** argv)
{
    HANDLE stdIn, stdOut;
    //DWORD numRead, i;
    //INPUT_RECORD irInBuf[128];
    int counter = 0;
    stdIn = GetStdHandle(STD_INPUT_HANDLE);
    stdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    const HWND hWnd = GetConsoleWindow();

    if ((stdIn == INVALID_HANDLE_VALUE) || (stdOut == INVALID_HANDLE_VALUE))
    {
        return 1;
    }

    if (!SetConsoleTitle("File Copy 'n Paste"))
    {
        _tprintf(TEXT("SetConsoleTitle failed (%d)\n"), GetLastError());
        return 1;
    }

    initWindow();    

    MSG msg = { 0 };

    if (argc <= 1) 
    {
        std::cout << "\nArgument Count too small: " << argc;
        return 1;
    }

    std::cout << "\nArgument Count: " << argc << "\n" << argv[1];
    std::string argument = argv[1];
    std::string filename;   

    if (argument == "/f")
    {
        filename = argv[2];
        std::cout << "\nFilename is: " << filename;
    }

    while (GetMessage(&msg, NULL, 0, 0) != 0)
    {
        if (msg.message == WM_HOTKEY)
        {
            std::cout << "\nWM_HOTKEY received";
            std::cout << msg.wParam;

            switch (msg.wParam)
            {
            case CTRL_SHIFT_C: 
                std::cout << "\nCopy from clipboard";
                if (IsClipboardFormatAvailable(CF_TEXT))
                {
                    if (OpenClipboard(hWnd))
                    {
                        HGLOBAL   hglb;
                        LPTSTR    lptstr;
                        std::string clipboardData;

                        hglb = GetClipboardData(CF_TEXT);
                        if (hglb != NULL)
                        {
                            lptstr = (LPTSTR) GlobalLock(hglb);
                            if (lptstr != NULL)
                            {
                                std::ofstream myfile;
                                clipboardData = lptstr;                                                      
                                std::cout << "\n" << clipboardData;
                                GlobalUnlock(hglb);

                                myfile.open(filename);
                                myfile << clipboardData;
                                myfile.close();
                            }
                        }
                        CloseClipboard();
                    }
                }
                break;
            case CTRL_SHIFT_V:
                std::string clipboardData;
                std::string data;
                std::ifstream myfile;

                std::cout << "\nPaste from file";
                
                myfile.open(filename);
                while (std::getline(myfile, data)) 
                {
                    clipboardData += data;
                }
                myfile.close();
                std::cout << "\nData from file: " << clipboardData;

                if (OpenClipboard(hWnd))
                {
                    HGLOBAL   hglb;
                    LPTSTR    lptstr;

                    EmptyClipboard();

                    hglb = GlobalAlloc(GMEM_MOVEABLE,
                        (clipboardData.length() + 1) * sizeof(TCHAR));
                    if (hglb == NULL)
                    {
                        CloseClipboard();
                        break;
                    }

                    lptstr = (LPTSTR)GlobalLock(hglb);
                    if (lptstr != NULL)
                    {                        
                        memcpy(lptstr, clipboardData.c_str(), clipboardData.length());
                        GlobalUnlock(hglb);
                        SetClipboardData(CF_TEXT, hglb);
                    }

                    CloseClipboard();
                }

                break;
            }
        }
    }
    
}

void initWindow()
{   
    
    if (!RegisterHotKey(NULL, CTRL_SHIFT_C, MOD_CONTROL | MOD_SHIFT, 0x43))
    {
        std::cout << "Failed to register Ctrl+Shift+C: " << GetLastError() << "\n";
    } 
    else
    {
        std::cout << "Listening " << "\nCtrl+Shift+C for Copy";
    }

    if (!RegisterHotKey(NULL, CTRL_SHIFT_V, MOD_CONTROL | MOD_SHIFT, 0x56))
    {
        std::cout << "Failed to register Ctrl+Shift+V: " << GetLastError() << "\n";
    }
    else
    {
        std::cout << "\nCtrl+Shift+V for Paste";
    }
}


