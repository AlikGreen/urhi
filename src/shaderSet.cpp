#include "shaderSet.h"

#include <algorithm>
#include <unordered_set>
#include <utility>

namespace urhi
{
    ShaderSet::ShaderSet(std::vector<ShaderEntryPoint> stages)
        : m_stages(std::move(stages)) { }

    const ShaderEntryPoint* ShaderSet::find(const ShaderStage stage) const
    {
        for(auto& ep : m_stages)
        {
            if(ep.stage == stage)
                return &ep;
        }

        return nullptr;
    }

    const ShaderEntryPoint* ShaderSet::findByName(const std::string_view entryPoint) const
    {
        for(auto& ep : m_stages)
        {
            if(ep.name == entryPoint)
                return &ep;
        }

        return nullptr;
    }

    ShaderSet ShaderSet::filter(std::initializer_list<ShaderStage> stages) const
    {
        std::vector<ShaderEntryPoint> filtered;

        for (const auto& ep : m_stages)
        {
            if (std::find(stages.begin(), stages.end(), ep.stage) != stages.end())
                filtered.push_back(ep);
        }

        return ShaderSet(std::move(filtered));
    }

    bool ShaderSet::contains(const ShaderStage stage) const
    {
        for(auto& ep : m_stages)
        {
            if(ep.stage == stage)
                return true;
        }

        return false;
    }

    bool ShaderSet::isValid() const
    {
        std::unordered_set<ShaderStage> seen;
        for (const auto& ep : m_stages)
        {
            if (!seen.insert(ep.stage).second)
                return false;
        }

        if (seen.contains(ShaderStage::Compute))
            return seen.size() == 1;

        return seen.contains(ShaderStage::Vertex) && seen.contains(ShaderStage::Fragment);
    }

    const std::vector<ShaderEntryPoint> & ShaderSet::stages() const
    {
        return m_stages;
    }
}
