#pragma once
#include <string_view>
#include <unordered_map>
#include <grl/hash.h>

namespace urhi
{
class NameRegistry
{
public:
    static std::string getName(uint32_t nameHash);


    template <std::size_t N>
    static consteval uint32_t getHash(const char (&str)[N])
    {
        const uint32_t hash = grl::Hash::fnv1a32(str);

        static const bool init = [&str, hash]()
        {
            registerName(hash, str);
            return true;
        }();
        (void)init;

        return hash;
    }

    static uint32_t getHash(const std::string_view name)
    {
        const uint32_t hash = grl::Hash::fnv1a32(name);
        registerName(hash, name);
        return hash;
    }
private:
    static void registerName(const uint32_t hash, std::string_view name)
    {
        m_nameMap.try_emplace(hash, name);
    }

    static std::unordered_map<uint32_t, std::string> m_nameMap;
};
}
