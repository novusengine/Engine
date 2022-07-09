#include "Bytebuffer.h"

Bytebuffer::RuntimePool Bytebuffer::_runtimeByteBuffers;
SharedPool<Bytebuffer> Bytebuffer::_byteBuffer128;
SharedPool<Bytebuffer> Bytebuffer::_byteBuffer512;
SharedPool<Bytebuffer> Bytebuffer::_byteBuffer1024;
SharedPool<Bytebuffer> Bytebuffer::_byteBuffer4096;
SharedPool<Bytebuffer> Bytebuffer::_byteBuffer8192;
SharedPool<Bytebuffer> Bytebuffer::_byteBuffer16384;
SharedPool<Bytebuffer> Bytebuffer::_byteBuffer32768;
SharedPool<Bytebuffer> Bytebuffer::_byteBuffer65536;
SharedPool<Bytebuffer> Bytebuffer::_byteBuffer131072;
SharedPool<Bytebuffer> Bytebuffer::_byteBuffer262144;
SharedPool<Bytebuffer> Bytebuffer::_byteBuffer524288;
SharedPool<Bytebuffer> Bytebuffer::_byteBuffer1048576;
SharedPool<Bytebuffer> Bytebuffer::_byteBuffer8388608;
SharedPool<Bytebuffer> Bytebuffer::_byteBuffer209715200;

std::shared_ptr<Bytebuffer> Bytebuffer::BorrowRuntime(size_t size)
{
	std::shared_ptr<Bytebuffer> buffer = _runtimeByteBuffers.acquireOrCreate(size);
	buffer->Reset();

	return buffer;
}