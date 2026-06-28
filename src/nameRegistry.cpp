#include "nameRegistry.h"

#include <string>

namespace urhi
{
    std::unordered_map<uint32_t, std::string> NameRegistry::m_nameMap{};

    std::string NameRegistry::getName(const uint32_t nameHash)
    {
        const auto it = m_nameMap.find(nameHash);
        if(it == m_nameMap.end()) return "";

        return it->second;
    }
}

