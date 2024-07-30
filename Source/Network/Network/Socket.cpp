#include "Socket.h"

#include "Base/Memory/Bytebuffer.h"
#include "Base/Util/DebugHandler.h"

#ifdef _WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")
#elif __linux__
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <limits>

namespace Network
{
    SharedPool<Socket> Socket::_pool;

#if WIN32
    Socket::Socket() : _socket(NULL) { }
#else
    Socket::Socket() : _socket(0) { }
#endif

    Socket::~Socket() { }

    Socket::Result Socket::Create(SOCKET socket, Type type, Mode mode, std::shared_ptr<Socket>& out)
    {
#if WIN32
        if (!IsWinsockInitialized())
        {
            WSADATA data;
            i32 code = WSAStartup(MAKEWORD(2, 2), &data);
            if (code != 0)
            {
                NC_LOG_CRITICAL("[Network] Failed to initialize WinSock");
            }
        }
#endif

        assert(mode != Mode::NONE);

        out = _pool.acquireOrCreate();

        out->_type = type;
        out->_mode = mode;
        out->_socket = socket;

        if (type == Type::CLIENT)
        {
            out->_readBuffer = Bytebuffer::Borrow<DEFAULT_BUFFER_SIZE>();
            out->_sendBuffer = Bytebuffer::Borrow<DEFAULT_BUFFER_SIZE>();

            out->SetReceiveBufferSize(DEFAULT_BUFFER_SIZE);
            out->SetSendBufferSize(DEFAULT_BUFFER_SIZE);
        }

        return Socket::Result::SUCCESS;
    }

    Socket::Result Socket::Create(Type type, Mode mode, std::shared_ptr<Socket>& out)
    {
#if WIN32
        if (!IsWinsockInitialized())
        {
            WSADATA data;
            i32 code = WSAStartup(MAKEWORD(2, 2), &data);
            if (code != 0)
            {
                NC_LOG_CRITICAL("[Network] Failed to initialize WinSock");
            }
        }
#endif

        assert(mode != Mode::NONE);

        out = _pool.acquireOrCreate();
        out->_type = type;
        out->_mode = mode;

        Result result = out->CreateSocket();

        if (result == Result::SUCCESS && type == Type::CLIENT)
        {
            out->_readBuffer = Bytebuffer::Borrow<DEFAULT_BUFFER_SIZE>();
            out->_sendBuffer = Bytebuffer::Borrow<DEFAULT_BUFFER_SIZE>();

            out->SetReceiveBufferSize(DEFAULT_BUFFER_SIZE);
            out->SetSendBufferSize(DEFAULT_BUFFER_SIZE);
        }

        return result;
    }

    Socket::Result Socket::Connect(u32 host, u16 port)
    {
        if (_type != Type::CLIENT)
            return Result::ERROR_CONNECT_MODE_SERVER;

        Result result = Result::SUCCESS;
#ifdef _WIN32
        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = host;
        serverAddr.sin_port = htons(port);

        i32 code = connect(_socket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
        if (code == SOCKET_ERROR)
        {
            result = GetResultFromError(WSAGetLastError());
        }

        _connectInfo.ipAddr = serverAddr.sin_addr.S_un.S_addr;
        _connectInfo.ipAddrStr = std::string(inet_ntoa(serverAddr.sin_addr));
        _connectInfo.port = port;

#elif __linux__
        return Result::ERROR_NOT_IMPLEMENTED;
#endif

        return result;
    }

    Socket::Result Socket::Connect(const char* hostName, u16 port)
    {
        if (_type != Type::CLIENT)
            return Result::ERROR_CONNECT_MODE_SERVER;

        Result result = Result::SUCCESS;
        u32 host = 0;

#ifdef _WIN32
        hostent* localHost = gethostbyname(hostName);
        if (!localHost)
            return Result::ERROR_CLIENT_FAILED_TO_RESOLVE_HOSTNAME;

        char* localIP = inet_ntoa(*(struct in_addr*)*localHost->h_addr_list);
        if (!localIP)
            return Result::ERROR_CLIENT_FAILED_TO_CONVERT_HOSTNAME_TO_ADDRESS;

        host = inet_addr(localIP);
#endif
        return Connect(host, port);
    }

    Socket::Result Socket::Connect(std::string& hostName, u16 port)
    {
        return Connect(hostName.c_str(), port);
    }

    Socket::Result Socket::Bind(std::string& hostName, u16 port)
    {
        if (_type != Type::SERVER)
            return Result::ERROR_BIND_MODE_CLIENT;

#ifdef _WIN32
        hostent* localHost = gethostbyname(hostName.c_str());
        char* localIP = inet_ntoa(*(struct in_addr*)*localHost->h_addr_list);

        sockaddr_in serverAddr { };
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = inet_addr(localIP);
        serverAddr.sin_port = htons(port);

        i32 bindCode = bind(_socket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
        if (bindCode == SOCKET_ERROR)
        {
            return GetResultFromError(WSAGetLastError());
        }

        i32 listenCode = listen(_socket, std::numeric_limits<i32>().max());
        if (listenCode == SOCKET_ERROR)
        {
            return GetResultFromError(WSAGetLastError());
        }

        _connectInfo.ipAddr = serverAddr.sin_addr.S_un.S_addr;
        _connectInfo.ipAddrStr = std::string(inet_ntoa(serverAddr.sin_addr));
        _connectInfo.port = port;

#elif __linux__
        return Result::ERROR_NOT_IMPLEMENTED;
#endif

        return Result::SUCCESS;
    }

    Socket::Result Socket::Accept(std::shared_ptr<Socket>& connection)
    {
        if (_type != Type::SERVER)
            return Result::ERROR_BIND_MODE_CLIENT;

#ifdef _WIN32
        SOCKADDR_IN addr;
        i32 addrlen = sizeof(addr);

        SOCKET socket = accept(_socket, (SOCKADDR*)&addr, &addrlen);
        if (socket == INVALID_SOCKET)
        {
            return GetResultFromError(WSAGetLastError());
        }

        Socket::Result createResult = Socket::Create(socket, Socket::Type::CLIENT, _mode, connection);
        if (createResult != Socket::Result::SUCCESS)
        {
            return createResult;
        }

        connection->_connectInfo.ipAddr = addr.sin_addr.S_un.S_addr;
        connection->_connectInfo.ipAddrStr = std::string(inet_ntoa(addr.sin_addr));
        connection->_connectInfo.port = addr.sin_port;

#elif __linux__
        return Result::ERROR_NOT_IMPLEMENTED;
#endif

        return Result::SUCCESS;
    }

    Socket::Result Socket::Send(void* data, size_t size)
    {
        if (_type != Type::CLIENT)
            return Result::ERROR_BIND_MODE_CLIENT;

#ifdef _WIN32
        i32 code = 0;
        size_t dataToSend = size;

        if (_mode == Mode::TCP)
        {
            while (dataToSend > 0)
            {
                size_t offset = size - dataToSend;

                code = send(_socket, reinterpret_cast<char*>(data) + offset, static_cast<i32>(dataToSend), 0);
                if (code == SOCKET_ERROR)
                {
                    return GetResultFromError(WSAGetLastError());
                }

                dataToSend -= code;
            }
        }
        else if (_mode == Mode::UDP)
        {
            // Add UDP
            return Result::ERROR_NOT_IMPLEMENTED;
        }

#elif __linux__
        return Result::ERROR_NOT_IMPLEMENTED;
#endif

        return Result::SUCCESS;
    }

    Socket::Result Socket::Read()
    {
        return Read(_readBuffer->GetWritePointer(), _readBuffer->GetSpace());
    }

    Socket::Result Socket::Read(void* buffer, size_t size)
    {
        if (_type != Type::CLIENT)
            return Result::ERROR_BIND_MODE_CLIENT;

#ifdef _WIN32
        i32 code = 0;

        if (_mode == Mode::TCP)
        {
            code = recv(_socket, reinterpret_cast<char*>(buffer), static_cast<i32>(size), 0);
            if (code == SOCKET_ERROR)
            {
                i32 wsaError = WSAGetLastError();
                return GetResultFromError(wsaError);
            }

            if (code == 0)
            {
                return Result::ERROR_SOCKET_CONNECTION_ABORTED;
            }

            _readBuffer->SkipWrite(code);
        }
        else if (_mode == Mode::UDP)
        {
            // Add UDP
            return Result::ERROR_NOT_IMPLEMENTED;
        }

#elif __linux__
        return Result::ERROR_NOT_IMPLEMENTED;
#endif

        return Result::SUCCESS;
    }

    Socket::Result Socket::Close()
    {
#ifdef _WIN32
        i32 code = closesocket(_socket);
        if (code == SOCKET_ERROR)
        {
            return GetResultFromError(WSAGetLastError());
        }
#elif __linux__
        return Result::ERROR_NOT_IMPLEMENTED;
#endif

        return Result::SUCCESS;
    }

    Socket::Result Socket::SetBlockingState(bool state)
    {
#ifdef _WIN32
        u_long mode = !state;
        i32 code = ioctlsocket(_socket, FIONBIO, &mode);
        if (code == SOCKET_ERROR)
        {
            return GetResultFromError(WSAGetLastError());
        }
#elif __linux__
        return Result::ERROR_NOT_IMPLEMENTED;
#endif

        return Result::SUCCESS;
    }

    Socket::Result Socket::SetNoDelayState(bool state)
    {
#ifdef _WIN32
        i32 flags = state;
        i32 code = setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&flags, sizeof(flags));
        if (code == SOCKET_ERROR)
        {
            return GetResultFromError(WSAGetLastError());
        }
#elif __linux__
        return Result::ERROR_NOT_IMPLEMENTED;
#endif

        return Result::SUCCESS;
    }

    Socket::Result Socket::SetReceiveBufferSize(size_t size)
    {
#ifdef _WIN32
        i32 code = setsockopt(_socket, SOL_SOCKET, SO_RCVBUF, (char*)_readBuffer->GetDataPointer(), static_cast<i32>(size));
        if (code == SOCKET_ERROR)
        {
            return GetResultFromError(WSAGetLastError());
        }
#elif __linux__
        return Result::ERROR_NOT_IMPLEMENTED;
#endif

        return Result::SUCCESS;
    }

    Socket::Result Socket::SetSendBufferSize(size_t size)
    {
#ifdef _WIN32
        i32 code = setsockopt(_socket, SOL_SOCKET, SO_SNDBUF, (char*)_sendBuffer->GetDataPointer(), static_cast<i32>(size));
        if (code == SOCKET_ERROR)
        {
            return GetResultFromError(WSAGetLastError());
        }
#elif __linux__
        return Result::ERROR_NOT_IMPLEMENTED;
#endif

        return Result::SUCCESS;
    }

    Socket::Result Socket::CreateSocket()
    {
#ifdef _WIN32
        i32 mode = SOCK_STREAM;
        i32 protocol = IPPROTO_TCP;

        if (_mode == Mode::UDP)
        {
            mode = SOCK_DGRAM;
            protocol = IPPROTO_UDP;
        }

        _socket = socket(AF_INET, mode, protocol);
        if (_socket == INVALID_SOCKET)
        {
            return GetResultFromError(WSAGetLastError());
        }
#elif __linux__
        return Result::ERROR_NOT_IMPLEMENTED;
#endif

        return Result::SUCCESS;
    }

    Socket::Result Socket::GetResultFromError(i32 error)
    {
        Result result = Result::ERROR_GENERIC;

#ifdef _WIN32
        switch (error)
        {
        case WSANOTINITIALISED:
            result = Result::ERROR_WSA_UNINITIALIZED;
            break;
        case WSAENETDOWN:
            result = Result::ERROR_NETWORK_SERVICE_DOWN;
            break;
        case WSAEAFNOSUPPORT:
            result = Result::ERROR_UNSUPPORTED_ADDRESS_FAMILY;
            break;
        case WSAEINPROGRESS:
            result = Result::ERROR_WSA_INPROGRESS;
            break;
        case WSAEMFILE:
            result = Result::ERROR_SOCKET_DESCRIPTOR_UNAVAILABLE;
            break;
        case WSAEINVAL:
            result = Result::ERROR_UNSPECIFIED_ADDRESS_FAMILY;
            break;
        case WSAEINVALIDPROVIDER:
            result = Result::ERROR_INVALID_PROVIDER_VERSION;
            break;
        case WSAEINVALIDPROCTABLE:
            result = Result::ERROR_INVALID_PROCEDURE_TABLE;
            break;
        case WSAENOBUFS:
            result = Result::ERROR_MISSING_BUFFER_SPACE;
            break;
        case WSAEPROTONOSUPPORT:
            result = Result::ERROR_UNSUPPORTED_PROTOCOL;
            break;
        case WSAEPROTOTYPE:
            result = Result::ERROR_INCORRECT_PROTOCOL;
            break;
        case WSAEPROVIDERFAILEDINIT:
            result = Result::ERROR_PROVIDER_UNINITIALIZED;
            break;
        case WSAESOCKTNOSUPPORT:
            result = Result::ERROR_UNSUPPORTED_SOCKET_TYPE;
            break;
        case WSAEACCES:
            result = Result::ERROR_SOCKET_ACCESS_FORBIDDEN;
            break;
        case WSAEADDRINUSE:
            result = Result::ERROR_ADDRESS_IN_USE;
            break;
        case WSAEADDRNOTAVAIL:
            result = Result::ERROR_INVALID_LOCAL_ADDRESS;
            break;
        case WSAENOTSOCK:
            result = Result::ERROR_NOT_SOCKET;
            break;
        case WSAEWOULDBLOCK:
            result = Result::ERROR_WOULD_BLOCK;
            break;
        case WSAEISCONN:
            result = Result::ERROR_SOCKET_IS_LISTENING;
            break;
        case WSAEOPNOTSUPP:
            result = Result::ERROR_INVALID_SOCKET_FOR_LISTEN;
            break;
        case WSAECONNABORTED:
            result = Result::ERROR_SOCKET_CONNECTION_ABORTED;
            break;
        case WSAECONNRESET:
            result = Result::ERROR_SOCKET_CONNECTION_RESET;
            break;
        case WSAECONNREFUSED:
            result = Result::ERROR_SOCKET_CONNECTION_REFUSED;
            break;

        default:
            break;
        }
#elif __linux__
        return Result::ERROR_NOT_IMPLEMENTED;
#endif

        if (result == Result::ERROR_GENERIC)
        {
            NC_LOG_WARNING("[Networking] Error Code %u is unhandled", error);
        }

        return result;
    }

    bool Socket::IsWinsockInitialized()
    {
#ifdef WIN32
        SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (s == INVALID_SOCKET)
            return false;

        closesocket(s);
#endif

        return true;
    }
}
