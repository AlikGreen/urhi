#pragma once

#include <atomic>
#include <slang.h>
#include <vector>

namespace urhi::slang
{
static bool uuidEquals(const SlangUUID& a, const SlangUUID& b)
{
    return std::memcmp(&a, &b, sizeof(SlangUUID)) == 0;
}


class MemoryBlob final : public ::slang::IBlob
{
public:
    explicit MemoryBlob(const std::vector<uint8_t>& data)
        : m_data(data)
    {}

    SlangResult queryInterface(SlangUUID const& uuid, void** outObject) override
    {
        if (!outObject) return SLANG_E_INVALID_ARG;
        *outObject = nullptr;

        const SlangUUID& blobId = ::slang::IBlob::getTypeGuid();
        const SlangUUID& unkId  = ISlangUnknown::getTypeGuid();

        if (uuidEquals(uuid, blobId) || uuidEquals(uuid, unkId))
        {
            *outObject = static_cast<::slang::IBlob*>(this);
            addRef();
            return SLANG_OK;
        }
        return SLANG_E_NO_INTERFACE;
    }

    uint32_t addRef() override  { return ++m_refCount; }
    uint32_t release() override
    {
        uint32_t count = --m_refCount;
        if (count == 0) delete this;
        return count;
    }

    // IBlob
    void const* getBufferPointer() override { return m_data.data(); }
    size_t      getBufferSize()    override { return m_data.size(); }

private:
    std::atomic<uint32_t> m_refCount{1};
    std::vector<uint8_t>  m_data;
};
}
