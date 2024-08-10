#include "Client.h"

#include "Base/Util/DebugHandler.h"

#include <entt/entt.hpp>
#include <tracy/Tracy.hpp>

namespace Network
{
    Client::Client(asio::io_context& ioContext, std::shared_ptr<asio::ip::tcp::resolver>& resolver) : _asioContext(ioContext), _resolver(resolver)
    {
        _readBuffer = Bytebuffer::Borrow<DEFAULT_BUFFER_SIZE>();
    }

    void Client::Stop()
    {
        if (!IsConnected())
            return;

        try
        {
            _socket->shutdown(asio::ip::tcp::socket::shutdown_both);
            _socket->close();
        }
        catch (const std::exception& e)
        {
            NC_LOG_ERROR("Network::Client : Stop Exception ({0})", e.what());
        }

        _socket = nullptr;
        _readBuffer->Reset();

        _nextBufferID = 0;
        _activeBuffers.clear();

        Network::SocketMessageEvent messageEvent;
        while (_messageEvents.try_dequeue(messageEvent)) {}
    }

    bool Client::Connect(const char* address, u16 port)
    {
        if (IsConnected())
        {
            NC_LOG_ERROR("Network::Client : Connect Failed (Already connected)");
            return false;
        }

        if (!address)
        {
            NC_LOG_ERROR("Network::Client : Connect Failed (Address passed as nullptr)");
            return false;
        }

        asio::ip::tcp::resolver::results_type endpoints = _resolver->resolve(address, std::to_string(port));
        if (endpoints.empty())
        {
            NC_LOG_ERROR("Network::Client : Connect Failed (Failed to resolve endpoint)");
            return false;
        }

        auto endpoint = (*endpoints).endpoint();

        asio::error_code ec;
        auto socket = std::make_shared<asio::ip::tcp::socket>(_asioContext);
        socket->connect(endpoint, ec);

        if (ec)
        {
            NC_LOG_ERROR("Network::Client : Connect Failed ({0})", ec.message());
            return false;
        }

        _socket = std::move(socket);
        _socket->non_blocking(true);

        ReadMessageHeader();
        NC_LOG_INFO("Network::Client : Connected to {0}:{1}", endpoint.address().to_string(), endpoint.port());
        return true;
    }
    void Client::Send(std::shared_ptr<Bytebuffer>& buffer)
    {
        if (!IsConnected())
        {
            return;
        }

        const BufferID bufferID = _nextBufferID++;
        _activeBuffers[bufferID] = buffer;

        asio::async_write(*_socket, asio::buffer(buffer->GetDataPointer(), buffer->writtenData), [this, bufferID](std::error_code ec, std::size_t length)
        {
            if (ec)
            {
                NC_LOG_ERROR("Network::Client : Send Failed ({0})", ec.message());
                Stop();
            }
        
            ClearBuffer(bufferID);
        });
    }

    void Client::ReadMessageHeader()
    {
        asio::async_read(*_socket, asio::buffer(_readBuffer->GetDataPointer(), sizeof(MessageHeader)), [this](std::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                auto* header = reinterpret_cast<MessageHeader*>(_readBuffer->GetDataPointer());
                _readBuffer->SkipWrite(length);
        
                if (header->size > DEFAULT_BUFFER_SIZE - sizeof(MessageHeader))
                {
                    NC_LOG_ERROR("Network::Client : ReadHeader Failed (Message Size Too Large)");
                    Stop();
                    return;
                }
        
                if (header->size > 0)
                {
                    ReadMessageBody();
                }
                else
                {
                    std::shared_ptr<Bytebuffer> messageBuffer = Bytebuffer::Borrow<sizeof(MessageHeader)>();
                    {
                        messageBuffer->Put(*header);
                    }
        
                    // Emit Message Event
                    EnqueueMessage(messageBuffer);
        
                    // Reset Read Buffer
                    _readBuffer->Reset();
        
                    ReadMessageHeader();
                }
            }
            else
            {
                NC_LOG_ERROR("Network::Client : ReadHeader Failed ({0})", ec.message());
                Stop();
            }
        });
    }

    void Client::ReadMessageBody()
    {
        NC_ASSERT(_readBuffer && _readBuffer->writtenData == sizeof(MessageHeader), "Network::ServerSession : ReadMessageBody Failed (Invalid Buffer)");

        u32 sizeToRead = reinterpret_cast<MessageHeader*>(_readBuffer->GetDataPointer())->size;
        asio::async_read(*_socket, asio::buffer(_readBuffer->GetWritePointer(), sizeToRead), [this](std::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                auto* header = reinterpret_cast<MessageHeader*>(_readBuffer->GetDataPointer());
                _readBuffer->SkipRead(sizeof(MessageHeader));
        
                std::shared_ptr<Bytebuffer> messageBuffer = Bytebuffer::Borrow<DEFAULT_BUFFER_SIZE>();
                {
                    messageBuffer->Put(*header);
        
                    // Payload
                    {
                        std::memcpy(messageBuffer->GetWritePointer(), _readBuffer->GetReadPointer(), header->size);
                        messageBuffer->SkipWrite(header->size);
                    }
                }
        
                // Emit Message Event
                EnqueueMessage(messageBuffer);
        
                // Reset Read Buffer
                _readBuffer->Reset();
        
                ReadMessageHeader();
            }
            else
            {
                NC_LOG_ERROR("Network::Client : ReadBody Failed ({0})", ec.message());
                Stop();
            }
        });
    }

    void Client::EnqueueMessage(std::shared_ptr<Bytebuffer>& buffer)
    {
        SocketMessageEvent messageEvent;
        messageEvent.socketID = SOCKET_ID_INVALID;
        messageEvent.message.buffer = std::move(buffer);
        _messageEvents.enqueue(messageEvent);
    }

    void Client::ClearBuffer(BufferID bufferID)
    {
        _activeBuffers.erase(bufferID);
    }
}