#pragma once
#include "descriptions/shaderEntryPoint.h"

namespace urhi
{
class ShaderSet
{
public:
    ShaderSet() = default;
    explicit ShaderSet(std::vector<ShaderEntryPoint> stages);

    [[nodiscard]] const ShaderEntryPoint* find(ShaderStage stage) const;
    [[nodiscard]] const ShaderEntryPoint* findByName(std::string_view entryPoint) const;
    [[nodiscard]] ShaderSet filter(std::initializer_list<ShaderStage> stages) const;
    [[nodiscard]] bool contains(ShaderStage stage) const;
    [[nodiscard]] bool isValid() const;

    [[nodiscard]] const std::vector<ShaderEntryPoint>& stages() const;
private:
    std::vector<ShaderEntryPoint> m_stages;
};
}
