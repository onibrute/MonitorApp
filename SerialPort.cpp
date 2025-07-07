#include "pch.h"                   
#include "SerialPort.h"           
#include <winsock2.h>              
#include <windows.h>
#include <cstdint>
#include <vector>
#include <string>


SerialPort::SerialPort()
    : m_hComm(INVALID_HANDLE_VALUE)
{
}

SerialPort::~SerialPort()
{
    Close();
}

bool SerialPort::Open(const std::string& portName, int baudRate)
{
    Close();
    std::string fullName = "\\\\.\\" + portName;

    m_hComm = CreateFileA(
        fullName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        0, 
        nullptr);

    if (m_hComm == INVALID_HANDLE_VALUE)
    {
        DWORD err = GetLastError();
        char msg[256];
        sprintf_s(msg, "Eroare CreateFileA la deschiderea portului %s (err=%lu)", fullName.c_str(), err);
        MessageBoxA(NULL, msg, "SerialPort::Open", MB_OK | MB_ICONERROR);
        return false;
    }

    DCB dcb = {};
    dcb.DCBlength = sizeof(dcb);
    if (!GetCommState(m_hComm, &dcb))
    {
        MessageBoxA(NULL, "GetCommState a eșuat.", "SerialPort::Open", MB_OK | MB_ICONERROR);
        Close();
        return false;
    }

    dcb.BaudRate = baudRate;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    if (!SetCommState(m_hComm, &dcb))
    {
        MessageBoxA(NULL, "SetCommState a eșuat.", "SerialPort::Open", MB_OK | MB_ICONERROR);
        Close();
        return false;
    }

    COMMTIMEOUTS to = {};
    to.ReadIntervalTimeout = 20;
    to.ReadTotalTimeoutMultiplier = 2;
    to.ReadTotalTimeoutConstant = 300; 

    SetCommTimeouts(m_hComm, &to);

    return true;
}



void SerialPort::Close()
{
    if (m_hComm != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hComm);
        m_hComm = INVALID_HANDLE_VALUE;
    }
}

bool SerialPort::Write(const std::vector<uint8_t>& data)
{
    if (!IsOpen()) return false;

    OVERLAPPED ov = {};
    ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    DWORD written = 0;
    BOOL result = WriteFile(m_hComm, data.data(), (DWORD)data.size(), &written, &ov);

    if (!result)
    {
        if (GetLastError() == ERROR_IO_PENDING)
        {
        
            DWORD wait = WaitForSingleObject(ov.hEvent, 200);
            if (wait == WAIT_OBJECT_0)
                GetOverlappedResult(m_hComm, &ov, &written, FALSE);
            else
                written = 0; 
        }
        else
        {
            written = 0;
        }
    }

    CloseHandle(ov.hEvent);
    return written == data.size();
}


bool SerialPort::Read(std::vector<uint8_t>& buffer, size_t size, unsigned int timeoutMs)
{
    if (!IsOpen()) return false;

    buffer.resize(size);
    DWORD read = 0;

    COMMTIMEOUTS to = {};
    to.ReadIntervalTimeout = 1;                  
    to.ReadTotalTimeoutConstant = timeoutMs;     
    to.ReadTotalTimeoutMultiplier = 1;          
    SetCommTimeouts(m_hComm, &to);

    BOOL ok = ReadFile(m_hComm, buffer.data(), (DWORD)size, &read, nullptr);

    if (!ok || read != size)
    {
        buffer.clear();
        return false;
    }

    return true;
}

