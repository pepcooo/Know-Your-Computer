#pragma once
#include <string>

class ComponentReader
{
protected:
    std::string componentType_;
    std::string modelName_;
public:
    ComponentReader(std::string componentType)
        : componentType_(std::move(componentType)), modelName_("Unknown"){}

    virtual ~ComponentReader() = default;

    [[nodiscard]] auto componentType() const -> const std::string& { return componentType_; }
    [[nodiscard]] auto modelName() const -> const std::string& { return modelName_; }
    virtual void readModel() = 0;
    virtual void printModel() const = 0;
};