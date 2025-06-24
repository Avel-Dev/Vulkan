#include "ShaderUtils.h"
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace CHIKU
{
	namespace Utils
	{
        std::string GetSPIRVTypeName(const SpvReflectBlockVariable& member) 
        {
            const SpvReflectTypeDescription* type = member.type_description;
            if (!type) return "(unknown)";

            const auto flags = type->type_flags;

            // Matrix
            if (flags & SPV_REFLECT_TYPE_FLAG_MATRIX) 
            {
                return "mat" + std::to_string(type->traits.numeric.matrix.column_count);
            }

            // Vector
            if (flags & SPV_REFLECT_TYPE_FLAG_VECTOR) 
            {
                std::string base = "vec";
                if (flags & SPV_REFLECT_TYPE_FLAG_INT)  base = "ivec";
                if (flags & SPV_REFLECT_TYPE_FLAG_BOOL) base = "bvec";
                return base + std::to_string(type->traits.numeric.vector.component_count);
            }

            // Scalar
            if (flags & SPV_REFLECT_TYPE_FLAG_FLOAT) return "float";
            if (flags & SPV_REFLECT_TYPE_FLAG_INT)   return "int";
            if (flags & SPV_REFLECT_TYPE_FLAG_BOOL)  return "bool";

            return "(unknown)";
        }

        std::vector<uint32_t> LoadSPIRV(const std::string& path) 
        {
            std::ifstream file(path, std::ios::binary | std::ios::ate);
            size_t size = file.tellg();
            file.seekg(0);
            std::vector<uint32_t> spirv(size / sizeof(uint32_t));
            file.read(reinterpret_cast<char*>(spirv.data()), size);

            SpvReflectShaderModule module;
            SpvReflectResult result = spvReflectCreateShaderModule(spirv.size() * sizeof(uint32_t), spirv.data(), &module);
            if (result != SPV_REFLECT_RESULT_SUCCESS) 
            {
                throw std::runtime_error("Failed to reflect shader");
            }

            uint32_t varCount = 0;
            spvReflectEnumerateInputVariables(&module, &varCount, nullptr);
            std::vector<SpvReflectInterfaceVariable*> inputVars(varCount);
            spvReflectEnumerateInputVariables(&module, &varCount, inputVars.data());

            for (auto* var : inputVars) 
            {
                if (var->decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN) continue; // skip gl_VertexID etc.

                std::cout << "Vertex Input - Location: " << var->location
                    << ", Name: " << var->name
                    << ", Format: " << var->format << "\n";
            }

            uint32_t setCount = 0;
            spvReflectEnumerateDescriptorSets(&module, &setCount, nullptr);
            std::vector<SpvReflectDescriptorSet*> sets(setCount);
            spvReflectEnumerateDescriptorSets(&module, &setCount, sets.data());

            for (const SpvReflectDescriptorSet* set : sets) 
            {
                std::cout << "Set " << set->set << ":\n";
                for (uint32_t i = 0; i < set->binding_count; ++i) 
                {
                    const SpvReflectDescriptorBinding* binding = set->bindings[i];

                    std::cout << "  Binding " << binding->binding << ": "
                        << "Name = " << binding->name
                        << ", Type = " << binding->descriptor_type
                        << ", Count = " << binding->count << "\n";

                    // Now reflect the structure of the UBO (if it's a uniform buffer)
                    if (binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER) 
                    {
                        const SpvReflectBlockVariable& block = binding->block;
                        for (uint32_t j = 0; j < block.member_count; ++j) 
                        {
                            const SpvReflectBlockVariable& member = block.members[j];

                            std::cout << "    Member: " << member.name
                                << ", Offset: " << member.offset
                                << ", Size: " << member.size
                                << ", Type: ";

                            if (member.type_description->type_name)
                            {
                                std::cout << member.type_description->type_name;
                            }
                            else 
                            {
								std::cout << GetSPIRVTypeName(member);
                            }

                            std::cout << "\n";
                        }
                    }
                }
            }

			return spirv;
        }
	}
}