#pragma once
#include <string>

class ComponentReader
{
    private:
        std::string componentType_;
        std::string modelName_;
    public:
        ComponentReader(std::string componentType, std::string modelName)
            : componentType_(std::move(componentType)), modelName_(std::move(modelName)){}

        virtual ~ComponentReader() = default;

        [[nodiscard]] auto componentType() const -> const std::string& { return componentType_; }
        [[nodiscard]] auto modelName() const -> const std::string& { return modelName_; }
        virtual void readModel();
};