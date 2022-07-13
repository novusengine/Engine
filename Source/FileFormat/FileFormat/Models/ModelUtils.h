#pragma once
#include <memory>

struct Model;
class Bytebuffer;

namespace ModelUtils
{
	size_t GetModelSerializedSize(const Model& model);
	bool Serialize(const Model& model, std::shared_ptr<Bytebuffer>& buffer);
}