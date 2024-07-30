#pragma once
#include "Define.h"

#include "Base/Types.h"
#include "Base/Memory/SharedPool.h"

// Forward Declarations for Windows/Linux
#ifdef _WIN32
typedef unsigned __int64 UINT_PTR;
typedef UINT_PTR SOCKET;
#elif __linux__
typedef int SOCKET;
#endif

class Bytebuffer;

namespace Network
{
    class Socket
    {
    public:
        enum class Type
        {
            NONE,
            CLIENT,
            SERVER
        };

        enum class Mode
        {
            NONE,
            TCP,
            UDP
        };

        enum class Result
        {
            SUCCESS,
            ERROR_GENERIC,
            ERROR_BIND_MODE_CLIENT,
            ERROR_CONNECT_MODE_SERVER,
            ERROR_WSA_UNINITIALIZED,
            ERROR_NETWORK_SERVICE_DOWN,
            ERROR_UNSUPPORTED_ADDRESS_FAMILY,
            ERROR_WSA_INPROGRESS,
            ERROR_SOCKET_DESCRIPTOR_UNAVAILABLE,
            ERROR_UNSPECIFIED_ADDRESS_FAMILY,
            ERROR_INVALID_PROVIDER_VERSION,
            ERROR_INVALID_PROCEDURE_TABLE,
            ERROR_MISSING_BUFFER_SPACE,
            ERROR_UNSUPPORTED_PROTOCOL,
            ERROR_INCORRECT_PROTOCOL,
            ERROR_PROVIDER_UNINITIALIZED,
            ERROR_UNSUPPORTED_SOCKET_TYPE,
            ERROR_SOCKET_ACCESS_FORBIDDEN,
            ERROR_ADDRESS_IN_USE,
            ERROR_INVALID_LOCAL_ADDRESS,
            ERROR_NOT_SOCKET,
            ERROR_WOULD_BLOCK,
            ERROR_SOCKET_IS_LISTENING,
            ERROR_INVALID_SOCKET_FOR_LISTEN,
            ERROR_SOCKET_CONNECTION_ABORTED,
            ERROR_SOCKET_CONNECTION_RESET,
            ERROR_SOCKET_CONNECTION_REFUSED,
            ERROR_NOT_IMPLEMENTED,
            ERROR_SERVER_UNINITIALIZED,
            ERROR_SERVER_ALREADY_STOPPED,
            ERROR_CLIENT_UNINITIALIZED,
            ERROR_CLIENT_ALREADY_INITIALIZED,
            ERROR_CLIENT_UNCONNECTED,
            ERROR_CLIENT_ALREADY_CONNECTED,
            ERROR_CLIENT_FAILED_TO_CLOSE,
            ERROR_CLIENT_FAILED_TO_RESOLVE_HOSTNAME,
            ERROR_CLIENT_FAILED_TO_CONVERT_HOSTNAME_TO_ADDRESS,
        };

        Socket();
        ~Socket();

        static Socket::Result Create(SOCKET socket, Type type, Mode mode, std::shared_ptr<Socket>& out);
        static Socket::Result Create(Type type, Mode mode, std::shared_ptr<Socket>& out);

        Result Connect(u32 host, u16 port);
        Result Connect(const char* hostName, u16 port);
        Result Connect(std::string& hostName, u16 port);
        Result Bind(std::string& hostName, u16 port);
        Result Accept(std::shared_ptr<Socket>& connection);
        Result Send(void* data, size_t size);
        Result Read();
        Result Read(void* buffer, size_t size);

        const ConnectionInfo& GetConnectionInfo() { return _connectInfo; }

        Result Close();

        Result SetBlockingState(bool state);
        Result SetNoDelayState(bool state);
        Result SetReceiveBufferSize(size_t size);
        Result SetSendBufferSize(size_t size);

        std::shared_ptr<Bytebuffer>& GetReadBuffer() { return _readBuffer; }

    private:
        Result CreateSocket();
        Result GetResultFromError(int error);
        static bool IsWinsockInitialized();

    public:
        Type GetType() { return _type; }
        Mode GetMode() { return _mode; }
    private:
        SOCKET _socket;

        Type _type = Type::NONE;
        Mode _mode = Mode::NONE;

        ConnectionInfo _connectInfo;

        std::shared_ptr<Bytebuffer> _readBuffer;
        std::shared_ptr<Bytebuffer> _sendBuffer;

        static SharedPool<Socket> _pool;
    };
}