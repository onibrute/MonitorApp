#pragma once
#include <string>
#include <vector>

// Interfaţă generică pentru port serial
class ISerialProtocol
{
public:
    virtual ~ISerialProtocol() {}

    // Deschide portul (ex. "COM3"), cu viteza dată
    virtual bool Open(const std::string& portName, int baudRate) = 0;
    virtual void Close() = 0;

    // Scrie un buffer de octeți pe port
    virtual bool Write(const std::vector<uint8_t>& data) = 0;

    // Citește exact 'size' octeți, cu timeout în ms
    virtual bool Read(std::vector<uint8_t>& buffer, size_t size, unsigned int timeoutMs) = 0;

    // Returnează true dacă portul este deschis
    virtual bool IsOpen() const = 0;
};
