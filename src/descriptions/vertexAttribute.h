#pragma once
#include <cstdint>
#include <typeinfo>

namespace NRHI
{
    struct VertexAttribute
    {
        uint32_t location;
        uint32_t slot;
        uint32_t size;
        const std::type_info* type;

        VertexAttribute(const uint32_t loc, const uint32_t slot, const uint32_t size, const std::type_info& t_info) : location(loc), slot(slot), size(size), type(&t_info) { }
    };
}
